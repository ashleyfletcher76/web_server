#!/bin/bash

# Config files we need
URL_FILE="data.txt"
OUTPUT_FILE="siege_output.log"
URL="http://localhost:8080/"

# Run Siege with our format
siege -c250 -r5 "$URL POST < $URL_FILE" --content-type "application/x-www-form-urlencoded" > "$OUTPUT_FILE" 2>&1

# Now check the failed transactions

if [ $? -eq 0 ]; then
	echo "Siege test completed."
else
	echo "Siege test failed."
fi

