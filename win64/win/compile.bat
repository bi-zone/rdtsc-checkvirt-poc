@echo off

cl /c checkvirt.c
ml64 helpers.asm
link checkvirt.obj helpers.obj /OUT:checkvirt.exe
