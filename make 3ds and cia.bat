3dstool -cvtf romfs romfs.bin --romfs-dir romfs
makerom -f cci -o flappy.3ds -rsf gw_workaround.rsf -target d -exefslogo -elf flappy.elf -icon icon.bin -romfs romfs.bin
makerom -f cia -o flappy.cia -elf flappy.elf -rsf cia_workaround.rsf -icon icon.bin -exefslogo -target t -romfs romfs.bin
rename flappy.cia flappy3ds.cia
rename flappy.elf flappy3ds.elf
move flappy3ds.cia Compiled/
move flappy3ds.elf Compiled/
move flappy3ds.3dsx Compiled/
pause