#!/usr/bin/python3

import cgi
import cgitb
from os import environ

cgitb.enable()

print("Content-Type: text/html")
print()

# HTML Header
print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Cookies</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
            background-color: #f4f4f4;
        }
        table {
            width: 100%;
            border-collapse: collapse;
            margin: 20px 0;
            background: #fff;
            border: 1px solid #ddd;
        }
        th, td {
            padding: 10px;
            text-align: left;
            border-bottom: 1px solid #ddd;
        }
        th {
            background-color: #f2f2f2;
        }
        h1 {
            color: #333;
        }
    </style>
""")

# HTML Body
print("""
</head>
<body>
    <h1>Cookies</h1>
    <table>
        <thead>
            <tr>
                <th>Key</th>
                <th>Value</th>
            </tr>
        </thead>
        <tbody>
""")

if 'HTTP_COOKIE' in environ:
    cookies = environ['HTTP_COOKIE']
    
    for cookie in cookies.split(';'):
        cookie = cookie.strip()  
        if '=' in cookie:
            key, value = cookie.split('=', 1)
            print(f"""
            <tr>
                <td>{key}</td>
                <td>{value}</td>
            </tr>
            """)

else:
    print("""
        <tr>
            <td colspan="2">No cookies found.</td>
        </tr>
    """)

# HTML Footer
print("""
        </tbody>
    </table>
    <a href="./index.html">Back to Homepage</a>
</body>
</html>
""")

