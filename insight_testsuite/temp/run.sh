#!/bin/bash
#
# Use this shell script to compile (if necessary) your code and then execute it. Below is an example of what might be found in this file if your program was written in Python
#
#python ./src/sessionization.py ./input/log.csv ./input/inactivity_period.txt ./output/sessionization.txt

g++ ./src/edgar.cpp -O3 -o edgar
./edgar ./input/log.csv ./input/inactivity_period.txt ./output/sessionization.txt
