#! /bin/bash
if [ "$#" -eq 1 ]; then
	export MINI_ASSEMBLER_TARGET=$1
	make ram.lst
else
	echo 'usage: asm FILE'
fi
