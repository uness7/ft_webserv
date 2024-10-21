<?php

$env = $_SERVER;

function print_header($status = "200 OK", $content_type = "text/html", $content_len = 0)
{
    echo $_SERVER["SERVER_PROTOCOL"] . " $status\r\n";
    echo "Content-Type: $content_type\r\n";
    echo "Content-Length:" . $content_len . "\r\n";
}


if ($env["REQUEST_METHOD"] == "POST") {
    if (isset($env["CONTENT_LENGTH"]) && (int)$env["CONTENT_LENGTH"] > 0) {
        $entityBody = stream_get_contents(STDIN);
        $form_data = [];
        parse_str($entityBody, $form_data);


        $name = isset($form_data['name']) ? $form_data['name'] : '';
        $age = isset($form_data['age']) ? $form_data['age'] : '';
        $email = isset($form_data['email']) ? $form_data['email'] : '';
        $feedback = isset($form_data['feedback']) ? $form_data['feedback'] : '';
        $country = isset($form_data['country']) ? $form_data['country'] : '';

        $content = <<<HTML
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="index.css">
    <title>Form Submission Response</title>
</head>
<body>
    <div class="container">
        <h1>Received Form Data:</h1>
        <p><strong>Name:</strong> $name</p>
        <p><strong>Age:</strong> $age</p>
        <p><strong>Email:</strong> $email</p>
        <p><strong>Feedback:</strong> $feedback</p>
        <p><strong>Country:</strong> $country</p>
    </div>
</body>
</html>
HTML;
        print_header("200 OK", "text/html", strlen($content));
    } else {
        $content = "<html lang='fr'><head><meta charset='UTF-8'><h1>Bad Request</h1></head><body>";
        print_header("400 Bad Request", "text/html", strlen($content));
    }
} else {
    $content = "<html lang='fr'><head><meta charset='UTF-8'><h1>Not Found</h1></head><body>";
    print_header("404 Not Found", "text/html", strlen($content));
}

echo "\r\n";

echo $content;
