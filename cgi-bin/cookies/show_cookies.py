#!/usr/bin/python3

import cgi
import cgitb
from os import environ

# Enable debugging
cgitb.enable()

# Output the header for HTML
print("Content-Type: text/html")
print()  # End of headers

# Check if there are cookies in the environment
if 'HTTP_COOKIE' in environ:
    cookies = environ['HTTP_COOKIE']
    
    # Split cookies by ';' and process each one
    for cookie in cookies.split(';'):
        cookie = cookie.strip()  # Remove any leading/trailing whitespace
        if '=' in cookie:
            key, value = cookie.split('=', 1)  # Split at the first '='
            print(f"key: {key}<br>")
            print(f"value: {value}<br>")
else:
    print("No cookies found.")

