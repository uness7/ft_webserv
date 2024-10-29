#!/bin/bash

html_content="<html>
<head>
    <meta charset='UTF-8'>
    <title>CGI Response</title>
</head>
<body>
	<h1>
		Hello world!
	</h1>
	<p>
		Our web server works with bash script as well! Isn't that coool :)
	</p>
</body>
</html>"

content_length=${#html_content}

echo "HTTP/1.1 200 OK"
echo "Content-Type: text/html"
echo "Content-Length: $content_length"
echo ""

echo "$html_content"
