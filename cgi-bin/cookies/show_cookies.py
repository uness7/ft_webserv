#!/usr/bin/env python3

import os
from http import cookies

# Retrieve cookies from the environment
cookie = cookies.SimpleCookie(os.environ.get("HTTP_COOKIE"))

# Print content-type header
print("Content-Type: text/html\n")

# Start HTML output
print("<html><body>")
print("<h1>Saved Cookies</h1>")

# Check if any cookies are saved
if cookie:
    for key, morsel in cookie.items():
        print(f"<p>{key}: {morsel.value}</p>")
else:
    print("<p>No cookies found</p>")

print('<a href="http://localhost:8070/cookies/index.html">Go back to homepage</a>')
print("</body></html>")

