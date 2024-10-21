#!/bin/bash

html_content="<html>
<head>
    <meta charset='UTF-8'>
    <title>CGI Response</title>
</head>
<body>
    <h1>Success</h1>
    <p>Hello from my <strong>shell script</strong></p>
</body>
</html>"

content_length=${#html_content}

echo "HTTP/1.1 200 OK"
echo "Content-Type: text/html"
echo "Content-Length: $content_length"
echo ""

echo "$html_content"
