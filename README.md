# 6.1810-Fall2023

This is the repo for [MIT 6.1810: Operating System Engineering Fall2023](https://pdos.csail.mit.edu/6.828/2023/schedule.html)

I use wsl2(ubuntu 20.04) + vscode as the basic develop environment.

You can get all solutions from here:

## Solutions

All solutions are 

| Lab                | Summary | Code(branch)                                                 |
| ------------------ | ------- | ------------------------------------------------------------ |
| Lab Utilities      |         | [code](https://github.com/IceRad1o/6.1810-Fall2023/commit/1ac40f9b1fe44decddcc270e76ccb21b04d4a237)<br />[branch](https://github.com/IceRad1o/6.1810-Fall2023/tree/util) |
| Lab System calls   |         | [code](https://github.com/IceRad1o/6.1810-Fall2023/commit/934e143c995b5ec3647af634dfc2f2e8452f11da) <br />[branch](https://github.com/IceRad1o/6.1810-Fall2023/tree/syscall) |
| Lab Page Tables    |         | [code](https://github.com/IceRad1o/6.1810-Fall2023/commit/213909fcb7299e51d01f07f9af925a349325aa0a)<br />[branch](https://github.com/IceRad1o/6.1810-Fall2023/tree/pgtbl) |
| Lab Traps          |         |                                                              |
| Lab Copy on-write  |         |                                                              |
| Lab Multithreading |         |                                                              |
| Lab network driver |         |                                                              |
| Lab Lock           |         |                                                              |
| Lab File system    |         |                                                              |
| Lab mmap           |         |                                                              |



## Hints for labs

一些提升体验的tips

### debug

* 使用 gdb-multiarch 来进行调试，可以参考2023年Lab2 的 gdb部分

  - 一个窗口make qemu-gdb，另一个窗口gdb-multiarch开始调试

  - 使用layout src来split窗口

#### 代码提示/补全

  - 可以使用clangd 进行代码提示，并使用bear生成compile_commands.json，提升代码编写体验
    - apt install clangd：https://clangd.llvm.org/
    - 在vscode中安装clangd插件(vim也是一样，安装clangd插件)
    - bear make qemu生成compile_commands.json
    
    然后clangd就会默认在后台工作，编写代码自动提示

#### 代码风格

* 可以使用clang-format，在repo目录下载入一个.clang-format文件(或者直接从repo的main分支里获取)，放在代码的主目录下，vscode安装clang-format插件后，就可以一键风格化代码，可以自行调整为xv6的风格或者自己喜欢的c/c++ 风格