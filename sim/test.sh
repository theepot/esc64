#!/bin/bash

for D in `find . -type d`
do
	if [[ $D == "." ]] ; then
		continue
	fi
	
	pushd $D
	make > /dev/null
	for T in `find *.test 2> /dev/null`
	do
		echo "###" $T "begin ###"
		./$T | grep -i error
		echo "###" $T "  end ###"
	done
	
	popd > /dev/null
	echo
done

