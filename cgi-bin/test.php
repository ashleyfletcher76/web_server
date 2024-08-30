<?php
echo "Content-Type: text/html\r\n\r\n";
echo "<html>";
echo "<body>";
echo "<p>Who is better at table tennis Masum or Ashley?</p>";

if (mt_rand(0, 1) < 0.5) {
    echo "<p>Ashley, of course!</p>";
} else {
    echo "<p>Try again!</p>";
}

// Infinite loop (if needed)
 while (true) {
     echo "<p>masum======</p>";
 }

echo "</body>";
echo "</html>";
?>

