@echo off

@echo Cleaning up
if exist *.prg del /q *.prg
if exist *.sym del /q *.sym
if exist *.txt del /q *.txt
if exist ..\..\emulator\gtx16.prg del /q ..\..\emulator\gtx16.prg

@echo Assembling sources
..\..\tools\acme\acme.exe --cpu 65c02 -f cbm -r gtx16.txt -l gtx16.sym -o gtx16.prg gtx16.asm

if not exist gtx16.prg goto END

@echo Copy prg to emulator
if exist *.prg copy *.prg ..\..\emulator /y

@echo Run the program in the emulator
if exist ..\..\emulator\gtx16.prg ..\..\emulator\x16emu.exe -run -prg gtx16.prg

:END
