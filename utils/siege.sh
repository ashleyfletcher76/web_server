#!/bin/bash

# Config files we need
URL_FILE="test_urls.txt"
OUTPUT_FILE="siege_output.log"
FAILURE_LOG="siege_failures.log"

# Run Siege with our format
siege -c50 -t1M -f $URL_FILE -v > $OUTPUT_FILE

# Now check the failed transactions

if grep -q "error" $OUTPUT_FILE; then
	echo "Failures detected. Logging details..."
	# Extract the details
	grep "error" $OUTPUT_FILE > $FAILURE_LOG
	echo "Failure details logged in $FAILURE_LOG"
else
	echo "No failures detected."
fi

