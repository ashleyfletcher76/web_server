#!/bin/bash

# Config files we need
URL_FILE_POST="data_post.txt"
URL_FILE_GET="data_get.txt"
OUTPUT_FILE_POST="siege_output_post.log"
OUTPUT_FILE_GET="siege_output_get.log"
URL="http://localhost:8080/empty.html"

if [ -z "$1" ]; then
	echo "Please provide a method (get or post) as arg"
	exit 1
fi

METHOD=$1

if [ "$METHOD" == "post" ]; then
	echo "Running post method..."
	siege -b -c255 -r5 "$URL POST < $URL_FILE_POST" --content-type "application/x-www-form-urlencoded" > "$OUTPUT_FILE_POST" 2>&1
elif [ "$METHOD" == "get" ]; then
	echo "Running get method..."
	siege -b -c255 -r5 "$URL" > "$OUTPUT_FILE_GET" 2>&1
else
	echo "Invalid method."
	exit 1
fi

# Now check the failed transactions

if [ $? -eq 0 ]; then
	echo "Siege test completed."
else
	echo "Siege test failed."
fi

