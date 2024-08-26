#!/bin/bash

# Must write PID as parameter
PID=$1
OUTPUT_FILE="fd_log.txt"

> "$OUTPUT_FILE"

while true; do
	echo "$(date): $(lsof -p $PID | wc -l) open files" | tee -a "$OUTPUT_FILE"
	sleep 1
done
