#!/bin/bash
testuser=fengmc
if grep $testuser /etc/passwd;then
	echo "This is my first command"
	echo "This is my second command"
	ls -a /home/$testuser/.b*
fi
