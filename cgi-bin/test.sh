#!/bin/bash

echo "Content-Type: text/html"
echo ""
echo "<html>"
echo "<body>"
echo "<p>Who is better at table tennis Masum or Ashley?</p>"

if [ $((RANDOM % 2)) -eq 0 ]; then
	echo "<p>Ashley, of course!</p>"
else
	echo "<p>Try again!</p>"
fi

# Infinite loop
# while true; do
# 	echo "<p>masum======</p>"
# done

echo "</body>"
echo "</html>"
