#!/bin/bash

### find location of this script ###
HERE="${BASH_SOURCE[0]}"
while [ -h "$HERE" ]; do
	DIR="$( cd -P "$( dirname "$HERE" )" && pwd )"
	HERE="$(readlink "$HERE")"
	[[ $HERE != /* ]] && HERE="$DIR/$HERE"
done
HERE="$( cd -P "$( dirname "$HERE" )" && pwd )"

### start simulation back-end ###
vvp -M$HERE/thrift-server -mthriftserver $HERE/computer.vpp
#for your convenience: gdb -args vvp -M`pwd`/thrift-server -mthriftserver `pwd`/computer.vpp

### start simulation front-end ###
#TODO

