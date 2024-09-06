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
C['username']['expires'] = 3600  # Cookie expires in 1 hour (optional)
C['password']['expires'] = 3600  # Cookie expires in 1 hour (optional)

print(C)  # This prints the Set-Cookie headers
print("Content-type: text/html\r\n\r\n")

print("<html><body>")
print(f"<p>Cookies for username: {username} and password: {password} have been set.</p>")
print('<a href="http://localhost:8070/cookies/index.html">Go back to homepage</a>')
print("</body></html>")

