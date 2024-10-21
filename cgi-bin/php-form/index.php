#!/usr/bin/env php

<?php
if ($_SERVER["REQUEST_METHOD"] == "GET") {
    $content = <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="index.css">
    <title>Form Submission</title>
</head>
<body style='text-align:center;'>
    <h1 style='color: green;'>CGI Form Example: </h1>

    <form action="./result.php" method="POST" enctype="application/x-www-form-urlencoded">
        <label for="name">Name:</label>
        <input type="text" id="name" name="name" required><br>
        <label for="age">Age:</label>
        <input type="number" id="age" name="age" required><br>
        <label for="email">Email:</label>
        <input type="email" id="email" name="email" required><br>
        <label for="feedback">Feedback:</label><br>
        <textarea id="feedback" name="feedback" rows="4" cols="50" placeholder="Your feedback here..."></textarea><br>
        <label for="country">Country:</label>
        <select id="country" name="country">
            <option value="france">France</option>
            <option value="usa">USA</option>
            <option value="uk">UK</option>
            <option value="canada">Canada</option>
            <option value="australia">Australia</option>
        </select><br>
        <input type="submit" value="Submit">
    </form>
</body>
</html>
HTML;

    print "HTTP/1.1 200 OK" . PHP_EOL;
    print "Content-Type: text/html" . PHP_EOL;
    print "Content-Length: " . strlen($content) . PHP_EOL;
    print "\r\n\r\n";

    print $content;
} else {
    $content = "Not Found";
    print $_SERVER["SERVER_PROTOCOL"] . " 404 Not Found" . PHP_EOL;
    print "Content-Type: text/plain" . PHP_EOL;
    print "Content-Length: " . strlen($content) . PHP_EOL;
    print "\r\n\r\n";

    print $content;
}


?>
