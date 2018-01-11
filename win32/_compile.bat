@echo off

set TOOLCHAIN=D:\Soft\vs2015_toolchain\
set ARCH=x86

call "%TOOLCHAIN%\vcvarsall.bat" %ARCH%

cl checkvirt.c
