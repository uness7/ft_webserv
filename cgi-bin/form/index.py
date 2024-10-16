#!/usr/bin/env python3

def print_header(status="200 OK", content_type="text/html"):
    print(f"HTTP/1.1 {status}");
    print(f"Content-Type: {content_type}");

def print_length(content):
    print(f"Content-Length: {len(content)}")

content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="index.css" >
    <title>Form Submission</title>
</head>

<body style='text-align:center;'>
    <h1 style='color: green;'>CGI Form Example: </h1>

    <form action="./script.py" method="POST" enctype="application/x-www-form-urlencoded">
        <label for="name">Name:</label>
        <input type="text" id="name" name="name" required><br>
        <label for="age">Age:</label>
        <input type="number" id="age" name="age" required><br>
        <label for="email">Email:</label>
        <input type="email" id="email" name="email" required><br>
        <label for="feedback">Feedback:</label><br>
        <textarea id="feedback" name="feedback" rows="4" cols="50" placeholder="Your feedback here..."></textarea><br>
        <label>Favorite Colors:</label><br>
        <input type="checkbox" id="red" name="colors" value="red">
        <label for="red">Red</label>
        <input type="checkbox" id="green" name="colors" value="green">
        <label for="green">Green</label>
        <input type="checkbox" id="blue" name="colors" value="blue">
        <label for="blue">Blue</label><br>
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
""";

# Printing Content
print_header("200 OK", "text/html");
print_length(content);
print("\r\n\r\n");
print(content);
