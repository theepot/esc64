#!/bin/bash
find -iname "*.sch" -exec gschem -o {}.eps -s /usr/share/gEDA/scheme/print.scm -- {} \;
find -iname "*.sch.eps" -exec inkscape -z -l{}.svg {} \;
for file in *.sch.eps.svg ; do mv $file `echo $file | sed 's/\(.*\.\)sch.eps.svg/\1svg/'` ; done
rm *.sch.eps
