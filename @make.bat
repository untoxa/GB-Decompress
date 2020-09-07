@echo off
@set PROJ=test_decomp
@set GBDK=..\..\gbdk\
@set GBDKLIB=%GBDK%lib\small\asxxxx\
@set OBJ=build\
@set SRC=example\
@set RES=%SRC%gfx\
@set TOOLS=tools\
@set DECOMP=decompress\ASM\


@set CFLAGS=-mgbz80 --fsigned-char --no-std-crt0 -I%GBDK%include -I%GBDK%include\asm -I%SRC%include -I%DECOMP%. -I%OBJ%. -c
@set CFLAGS=%CFLAGS% --max-allocs-per-node 50000

@set LNAMES=-g _shadow_OAM=0xC000 -g .STACK=0xE000 -g .refresh_OAM=0xFF80 -b _DATA=0xc0a0 -b _CODE=0x0200
@set LFLAGS=-n -m -j -w -i -k %GBDKLIB%\gbz80\ -l gbz80.lib -k %GBDKLIB%\gb\ -l gb.lib %LNAMES%
@set LFILES=%GBDKLIB%gb\crt0.o

@set ASMFLAGS=-plosgff -I%GBDKLIB%

@set BINFLAGS=-yt 0x1a -yo 4 -ya 4

@echo Cleanup...

@if exist %OBJ% rd /s/q %OBJ%
@if exist %PROJ%.gb del %PROJ%.gb
@if exist %PROJ%.sym del %PROJ%.sym
@if exist %PROJ%.map del %PROJ%.map
@if exist %PROJ%.noi del %PROJ%.noi
@if exist %PROJ%.cdb del %PROJ%.cdb

@if not exist %OBJ% mkdir %OBJ%

@echo COMPILING RESOURCES...

@for %%x in (
         picture_map.bin 
         picture_tiles.2bpp
       ) do (
         .\compress\comp.exe %RES%%%x %OBJ%%%x.rle
         %TOOLS%bin2c.exe %OBJ%%%x.rle %OBJ%%%x.c
         call :docompile %OBJ% %%x.c
       )

@echo COMPILING...

@for %%x in (
        rle_decompress.s
       ) do call :doassemble %DECOMP% %%x

@for %%x in (
        %PROJ%.c
       ) do call :docompile %SRC% %%x

@echo LINKING...
sdldgb %LFLAGS% %PROJ%.ihx %LFILES%
IF %ERRORLEVEL% NEQ 0 goto :error

@echo MAKING BIN...
makebin -Z %BINFLAGS% %PROJ%.ihx %PROJ%.gb

@echo DONE!
@goto :eof

:docompile
@echo %2
sdcc %CFLAGS% %1%2 -o %OBJ%%2.rel
IF %ERRORLEVEL% NEQ 0 goto :error
@set LFILES=%LFILES% %OBJ%%2.rel
goto :eof

:doassemble
@echo %2
sdasgb %ASMFLAGS% %OBJ%%2.rel %1%2
IF %ERRORLEVEL% NEQ 0 goto :error
@set LFILES=%LFILES% %OBJ%%2.rel
goto :eof

:error
@echo ERROR !
exit 0
