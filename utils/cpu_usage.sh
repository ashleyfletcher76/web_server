#!/bin/bash

if ! command -v top &> /dev/null; then
	echo "top could not be found."
	exit 1
fi

top -l 1 | grep "CPU usage" | awk '{print "User: " $3 ", System: " $5 ", Idle: " $7}'
