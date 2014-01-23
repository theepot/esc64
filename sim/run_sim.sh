#!/bin/bash
#TODO add an help option that shows all the valid arguments to the simulation
### find location of this script ###
HERE="${BASH_SOURCE[0]}"
while [ -h "$HERE" ]; do
	DIR="$( cd -P "$( dirname "$HERE" )" && pwd )"
	HERE="$(readlink "$HERE")"
	[[ $HERE != /* ]] && HERE="$DIR/$HERE"
done
HERE="$( cd -P "$( dirname "$HERE" )" && pwd )"

### start simulation back-end ###
pushd $HERE
vvp -M$HERE/native-extensions -mnative-extensions $HERE/computer.vvp $@
popd

#for your convenience: gdb -args vvp -M`pwd`/native-extensions -mnative-extensions `pwd`/computer.vvp

### start simulation front-end ###
#TODO

