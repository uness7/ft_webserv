#!/usr/bin/env python3

import cgi, cgitb
cgitb.enable()

form = cgi.FieldStorage()

# Printing for testing purposes, the script will be modified later once the body is sent!
print(form);
