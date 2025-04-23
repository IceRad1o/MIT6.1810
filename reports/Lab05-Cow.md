# Lab05 Cow

## Preparation

* [阅读] : xv6 book chapter4.6 ，chapter5

## Solutions

根据Lab的提示：

* uvmcopy应该把子进程的PTE都映射到父进程的物理页上，而不是像之前一样重新分配内存，把映射后的页标记为只读(去掉PTE_W标记)，增加一个PTE的标记**PTE_COW**，用于表示是cow机制的页。这里要注意的是**原来就是read only的页不需要做标记**

  ```c++
  int
  uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
  {
    pte_t *pte;
    uint64 pa, i;
    uint flags;
    for(i = 0; i < sz; i += PGSIZE){
      if((pte = walk(old, i, 0)) == 0)
        panic("uvmcopy: pte should exist");
      if((*pte & PTE_V) == 0)
        panic("uvmcopy: page not present");
      pa = PTE2PA(*pte);
      // dont change flags for read-only pages, make writable page to read-only
      if(*pte & PTE_W) {
         *pte = (*pte | PTE_C) & ~PTE_W;
      }
      flags = PTE_FLAGS(*pte);
      if(mappages(new, i, PGSIZE, pa, flags) != 0){
        kfree((void*)pa);
        goto err;
      }
      acquire(&ref_count_lock);
      page_refcount[pa / PGSIZE] += 1;
      release(&ref_count_lock);
    }
    return 0;
  
   err:
    uvmunmap(new, 0, i / PGSIZE, 1);
    return -1;
  }
  
  ```

* usertrap：应该针对写入的page fault处理，即做原来uvmcopy做的事情：**分配新的物理内存 -> 拷贝只读的物理页到新的内存中 -> 根据page fault传入的虚拟地址，找到PTE，修改PTE指向新的物理内存并且设置成可写(PTE_W)，去掉PTE_COW的标记**。有一些case需要直接退出并结束进程，所以加一个handler，否则针对每个异常情况可能还需要exit。

```c++
// trap.c中添加：
else if((r_scause() == 15 || r_scause() == 13)){
   if(cowhandler(p->pagetable, r_stval()) < 0) {
      setkilled(p);
}
     
// vm.c中添加一个cowhandler，函数声明需要补充在def.h中
int 
cowhandler(pagetable_t pagetable, uint64 va) {
  if(va >= MAXVA) {
    return -1;
  }
  
  pte_t* pte;
  pte = walk(pagetable, va, 0);
  if(pte == 0) {
    return -1;
  }
  if ((*pte & PTE_U) == 0 || (*pte & PTE_V) == 0 || (*pte & PTE_C) == 0) {
    return -1;
  }

  uint flags = PTE_FLAGS(*pte) | PTE_W;
  uint64 pa = PTE2PA(*pte);
  uint64 mem;
    
  if((mem = (uint64)kalloc()) == 0) {
    return -1;    
  }
  memmove((char*)mem, (char*)pa, PGSIZE);
  kfree((void*)pa);
  *pte = PA2PTE(mem) | flags | PTE_V;
  *pte &= ~PTE_C;
  return 0;
}
```

* copyout：原本函数是拷贝内存到某个虚拟地址，现在需要针对PTE_COW标记的页做和usertrap类似的事情

```c++
int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  while(len > 0){
    va0 = PGROUNDDOWN(dstva);
    if(va0 >= MAXVA || va0 == 0)
      return -1;
    pte = walk(pagetable, va0, 0);
    if(pte == 0 || (*pte & PTE_V) == 0 || (*pte & PTE_U) == 0) {
       return -1;
    }
    
    // handle cow pages
    if(*pte & PTE_C) {
      if(cowhandler(pagetable, va0) !=0) {
        return -1;
      }
    }
    pa0 = PTE2PA(*pte);
    n = PGSIZE - (dstva - va0);
    if(n > len)
      n = len;
    memmove((void *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  return 0;
}
```

* kalloc/kfree：由于cow的页可能被多个进程的PTE指向，所以需要一个**引用计数**ref count来保护，只有引用当前页的计数是0的时候，才能free掉，需要在kalloc和kfree中维护这个计数逻辑。
  * 这个ref count是针对每个物理页的，所以可以用一个固定长度的数组page_refcount表示，在启动kinit函数中有一个freerange是初始化每个物理页的函数，可以使用page_refcount[p / PGSIZE]来标记每个单独页。
  * 顺着上面的逻辑，在uvmcopy的时候ref count + 1，在usertrap和copyout中需要调用kfree或者uvmunmap，来 -1计数。

实现的时候需要注意：

* reference count也需要锁来保护，需要在kinit中使用initlock初始化

* 由于freerange中使用了kfree，会在启动的时候就把ref count - 1，所以可以在freerange中先做一个初始化，这样所有的page初始count = 0

  ```c++
  int page_refcount[PHYSTOP / PGSIZE];
  struct spinlock ref_count_lock;
  
  void
  kinit()
  {
    initlock(&kmem.lock, "kmem");
    initlock(&ref_count_lock, "ref_count_lock");
    freerange(end, (void*)PHYSTOP);
  }
  
  void
  freerange(void *pa_start, void *pa_end)
  {
    char *p;
    p = (char*)PGROUNDUP((uint64)pa_start);
    for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE){
      page_refcount[(uint64)p / PGSIZE] = 1;
      kfree(p);
    }
  }
  ```

* uvmunmap会在最后清空pte的值，需要解析flag的话需要在uvmunmap之前就做好。
* 一些边界条件的判断，比如虚拟地址MAXVA的判断。



错误：在trap封装cowhandler之前，直接在trap.c的else if中实现了逻辑，但是使用了exit代替了return -1的这行。

> test kernmem: scause 0x000000000000000d
> sepc=0x0000000080001f00 stval=0x0000000000000000

测试程序在测试是否能读取KENBASE的地址空间，trap的值是0xd(13)，也就是Load page fault。在测试的某个子进程尝试print *a的时候就会发生trap，对于测试程序来说，通过需要kernel kill掉这个子进程。正确是逻辑是 setkilled(p) -> if killed(p) -> exit 而不是直接 exit。



## Summary

* 了解riscv的汇编语法
* 通过Lab了解copy on write的机制，以及xv6一个trap的整体流程(**uservec -> usertrap -> usertrapret -> userret**)，从用户态到内核态的一些机制和变化？
  * 一些trap相关的寄存器比如
    * scause代表trap的原因；
    * stvec代表trap handler指令的地址，发生trap后，CPU从这个值开始执行
  * 因为trap开始的时候不切换页表，所以需要一个包含内核代码但是map在用户空间的页：trampoline
  * 使用trapframe来存下用户态的寄存器信息，方便后面来上下文切换
* 什么时候会发生page fault？
  * cow：copy on write
  * lazy allocation：延迟分配内存
  * demand paging：有些application很大，shell执行的时候要从磁盘读取开销很大，会造成命令行卡顿，os可以给它创建好PTE，但是标记成无效，需要的时候再读取
  * paging to disk：也就是页的换入换出，针对有时候虚拟内存比RAM更大
