#!/usr/bin/python3

import cgi;
from http import cookies;

form = cgi.FieldStorage();

username = form.getvalue("username");
email = form.getvalue("email");
password = form.getvalue("password");

C = cookies.SimpleCookie();
C['username'] = username;
C['email'] = email;
C['password'] = password;

'''
C['username']['path'] = '/';
C['password']['path'] = '/';
C['email']['path'] = '/';

C['username']['expires'] = 3600;  
C['password']['expires'] = 3600;
C['email']['expires'] = 3600;
'''


def print_header(status="200 OK", content_type="text/html"):
    print(f"HTTP/1.1 {status}");
    print(f"Content-Type: {content_type}");

def print_length(content):
    print(f"Content-Length: {len(content)}")

content = f"""
<html>
    <body>
        <a href="http://localhost:8070/cookies/index.html">Go back to homepage</a>
    <body>
</html>""";

print_header("200 OK", "text/html");
print_length(content);
print(C);
print();
print(content);


