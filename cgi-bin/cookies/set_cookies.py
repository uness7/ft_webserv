#!/usr/bin/python3

import cgi
from http import cookies

form = cgi.FieldStorage()
username = form.getvalue("username")
password = form.getvalue("password")

C = cookies.SimpleCookie()
C['username'] = username
C['password'] = password

C['username']['path'] = '/'
C['password']['path'] = '/'
C['username']['expires'] = 3600  
C['password']['expires'] = 3600 

print("Content-type: text/html\r\n")  # Content-type header first
print(C);

print("<html><body>")
print(
    '<a href="http://localhost:8070/cookies/index.html">'
    'Go back to homepage'
    '</a>'
)
print("</body></html>")

