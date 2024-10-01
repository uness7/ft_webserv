#!/usr/bin/env python3
import cgi


form = cgi.FieldStorage();
name = form.getvalue('name')
status = form.getvalue('status')
age = form.getvalue('age')
email = form.getvalue('email')
feedback = form.getvalue('feedback')
colors = form.getlist('colors')
country = form.getvalue('country')

###############################################################################
def print_header(status="200 OK", content_type="text/html"):
    print(f"HTTP/1.1 {status}");
    print(f"Content-Type: {content_type}");

def print_length(content):
    print(f"Content-Length: {len(content)}")
###############################################################################

content = f"""
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
     <link rel="stylesheet" href="res.css">
    <title>Form Submission Response</title>
</head>
<body>
    <div class="container">
        <h1>Received Form Data:</h1>
        <p><strong>Name:</strong> {name}</p>
        <p><strong>Age:</strong> {age}</p>
        <p><strong>Email:</strong> {email}</p>
        <p><strong>Feedback:</strong> {feedback}</p>
        <p><strong>Favorite Colors:</strong> {', '.join(colors)}</p>
        <p><strong>Country:</strong> {country}</p>
    </div>
</body>
</html>
""";

# Nothing could go here, so it's safe to pass in 200 status code Mr. Tourabi :)
print_header("200 OK", "text/html");
print_length(content);
print("\r\n\r\n");
print(content);

