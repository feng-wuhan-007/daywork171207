#!/bin/bash
myarray=(1 2 3 4 5)
arg1=$(echo ${myarray[*]})
echo The final result is $arg1
