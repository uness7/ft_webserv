#!/usr/bin/env python3

import cgi
import os

def read_template(filename):
    try:
        with open(filename, 'r') as file:
            return file.read()
    except IOError as e:
        print(f"<p>Error reading template file: {e}</p>")
        return ""

script_dir = os.path.dirname(os.path.abspath(__file__))
template_path = os.path.join(script_dir, 'res.html')

# Read and check template
template = read_template(template_path)

form = cgi.FieldStorage()

# Process form data
content = ""

if "name" in form:
    name = form.getvalue("name")
    content += f"<h2>Hello, {name}!</h2>"
    content += "<p>Thank you for using our script.</p>"
else:
    content += "<p>No name recorded</p>"

if "happy" in form:
    content += "<p>Yayy! We're happy too! 😊</p>"
else:
    content += "<p>No value recorded</p>"

if "sad" in form:
    content += "<p>Oh no! Why are you sad? 😢</p>"
else:
    content += "<p>No value recorded</p>"

# Replace placeholder in template with actual content
html_response = template.replace("{{content}}", content)

print(html_response)

