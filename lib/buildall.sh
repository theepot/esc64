#!/bin/bash

for D in `find . -maxdepth 1 -type d`
do
	echo $D
	pushd $D
	cmake .
	make
	popd
done



