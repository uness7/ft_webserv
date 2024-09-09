#!/usr/bin/python3

import cgi
import cgitb
from os import environ

cgitb.enable()

print("Content-Type: text/html")
print()  

if 'HTTP_COOKIE' in environ:
    cookies = environ['HTTP_COOKIE']
    
    for cookie in cookies.split(';'):
        cookie = cookie.strip()  
        if '=' in cookie:
            key, value = cookie.split('=', 1)  
            print(f"key: {key}<br>")
            print(f"value: {value}<br>")
else:
    print("<p>No cookies found.</p>")

