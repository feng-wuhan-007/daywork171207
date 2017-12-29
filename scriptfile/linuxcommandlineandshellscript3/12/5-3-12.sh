#!/bin/bash
#testuser=fengmc
testuser=fshare
if grep $testuser /etc/passwd
then
	echo "The user $testuser exists on the system."
	ls -a /home/$testuser/.b*
elif ls -d /home/$testuser
then 
	echo "The user $testuser does not exists on this system."
	echo "However,$testuser has a directory."
else
	echo "The user $testuser does not exist on this system."
	echo "And, $testuser does not have a directory."
fi
