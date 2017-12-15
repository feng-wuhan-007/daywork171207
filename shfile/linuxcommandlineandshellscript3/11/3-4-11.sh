#!/bin/bash
testing=date
echo "The date and time are: " $testing
today=$(date +%y%m%d)
#today="171213"
ls /usr/bin -al > log.$today
