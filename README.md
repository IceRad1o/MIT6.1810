# 

![course](.\assets\course.JPG)

# Introduction

This is the repo for [MIT 6.1810: Operating System Engineering](https://pdos.csail.mit.edu/6.828/2023/schedule.html).

## Get Started

My env: wsl2(ubuntu 20.04) + vscode

follow https://pdos.csail.mit.edu/6.828/2023/tools.html to set up environment

```bash
sudo apt-get install git build-essential gdb-multiarch qemu-system-misc gcc-riscv64-linux-gnu binutils-riscv64-linux-gnu
```



## Solutions

* 每个Lab的代码的提交最后都合并成一个commit，使用code可以方便地code review，相当于在使用git diff，查看新增/修改了哪些内容。

| Lab                | Code(branch)                                                 | Lab Report |
| ------------------ | ------------------------------------------------------------ | ---------- |
| Lab Utilities      | [code](https://github.com/IceRad1o/6.1810-Fall2023/commit/1ac40f9b1fe44decddcc270e76ccb21b04d4a237) |            |
| Lab System calls   | [code](https://github.com/IceRad1o/6.1810-Fall2023/commit/934e143c995b5ec3647af634dfc2f2e8452f11da) |            |
| Lab Page Tables    | [code](https://github.com/IceRad1o/6.1810-Fall2023/commit/213909fcb7299e51d01f07f9af925a349325aa0a) |            |
| Lab Traps          | [code](https://github.com/IceRad1o/6.1810-Fall2023/commit/3cbdcf99a3d352878f3b8e5ffa0a1f77bff592ff) |            |
| Lab Copy on-write  | [code](https://github.com/IceRad1o/6.1810-Fall2023/commit/3677b24aee93bba41c4464a4e4dd39d5a1796161) |            |
| Lab Multithreading |                                                              |            |
| Lab network driver |                                                              |            |
| Lab Lock           |                                                              |            |
| Lab File system    |                                                              |            |



## Hints for labs

一些提升体验的tips

### debug

* 使用 gdb-multiarch 来进行调试，可以参考2023年Lab2 的 gdb部分

  - 一个窗口make qemu-gdb，另一个窗口gdb-multiarch开始调试

  - 使用layout src来split窗口

#### 代码提示/补全

  - 可以使用clangd 进行代码提示，并使用bear生成compile_commands.json，提升代码编写体验
    
    1. 安装clangd：apt install clangd：https://clangd.llvm.org/
    
    2. 在vscode中安装clangd插件(vim也是一样，安装clangd插件)
    
    3. 安装bear：sudo pacman -S bear
    4. 使用命令bear make qemu在代码目录下生成compile_commands.json
    
    然后clangd就会默认在后台工作，编写代码自动提示

#### 代码风格

* 可以使用clang-format，下载入一个.clang-format文件(或者直接从这个repo的main分支里获取)，放在代码的主目录下，vscode安装clang-format插件后，就可以一键风格化代码，可以自行调整为xv6的风格或者自己喜欢的c/c++ 风格