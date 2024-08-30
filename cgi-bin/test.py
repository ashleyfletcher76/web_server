from __future__ import print_function
import random

# Generate a random number between 0 and 1
# Content type must be declared first
print("Content-Type: text/html")
print()  # End of headers

# Start of HTML content
print("<html>")
print("<body>")
print("<p>Who is better at table tennis Masum or Ashley ?</p>")  # Use paragraph tags for proper HTML formatting
if random.random() < 0.5:
	print("<p>Ashley, of course!</p>")
else:
	print("<p>Try again!</p>")

# while True:
# 	print("<p>masum======Loooooooool no way</p>")
print("</body>")
print("</html>")

