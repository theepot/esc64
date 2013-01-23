#!/bin/bash

BASE=$1
echo "$BASE"
split_verilog_memory 8 "$BASE"_5.lst 8 "$BASE"_4.lst 8 "$BASE"_3.lst 8 "$BASE"_2.lst 8 "$BASE"_1.lst 8 "$BASE"_0.lst <  $BASE.lst

