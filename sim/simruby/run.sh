#!/bin/bash

### find location of this script ###
HERE="${BASH_SOURCE[0]}"
while [ -h "$HERE" ]; do
	DIR="$( cd -P "$( dirname "$HERE" )" && pwd )"
	HERE="$(readlink "$HERE")"
	[[ $HERE != /* ]] && HERE="$DIR/$HERE"
done
HERE="$( cd -P "$( dirname "$HERE" )" && pwd )"
export PIET=$HERE

### run makefile ###
make -C $HERE > /dev/null || exit $?

### start irb ###
export RUBYLIB=$HERE/service:$RUBYLIB
irb -r $HERE/init.rb $MODS

