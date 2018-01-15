#!/bin/bash
#testiing string equality
testuser=rich
	echo $USER
if [ $USER = $testuser ];then
	echo  "Welcome $testuser"
fi
