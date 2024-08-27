#!/usr/bin/php
<?php
// Set content-type header to tell the browser the response is HTML
header('Content-Type: text/html');

// Output HTML content
echo "<html>";
echo "<head><title>Simple PHP CGI Script</title></head>";
echo "<body>";
echo "<h1>Hello, CGI World!</h1>";
echo "<p>This is a simple PHP script running via CGI.</p>";
echo "</body>";
echo "</html>";
?>
