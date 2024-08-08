#!/bin/bash

if [ -z "$1" ]; then
	echo "Usage: $0 <process_id>"
	exit 1
fi

PID=$1

echo "Checking open fd's for process ID $PID"
output=$(lsof -p $PID | awk '$5 == "IPv4" || $5 == "IPv6" {print "FD: " $4 " Type: " $5 " Protocol: " $8}')

if [ -z "$output" ]; then
	echo "No open IPv4 or IPv6 sockets found."
else
	echo "Open IPv4 and IPv6 file descriptors:"
	echo "$output"
fi
