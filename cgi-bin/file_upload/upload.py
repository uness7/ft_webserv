#!/usr/bin/env python3

import cgi
import os
import cgitb
cgitb.enable()

form = cgi.FieldStorage()
fileitem = form['file']
if fileitem.filename:
    fn = os.path.basename(fileitem.filename)
    upload_dir = 'uploads'
    file_path = os.path.join(upload_dir, fn)
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)
    try:
        with open(file_path, 'wb') as f:
            f.write(fileitem.file.read())
        message = f'The file "{fn}" was uploaded successfully'
        is_ok = True;
    except Exception as e:
        message = f'An error occurred while uploading the file: {str(e)}'
        is_ok = False;
else:
    message = 'No file was uploaded'
    is_ok = False;

#################################################################################
def print_header(status="200 OK", content_type="text/html"):
    print(f"HTTP/1.1 {status}");
    print(f"Content-Type: {content_type}");

def print_length(content):
    print(f"Content-Length: {len(content)}")
#################################################################################

content = f"""<html><body><p>{message}</p><li><a href="http://localhost:8070">Go back to homepage</a></li></body></html>""";
if is_ok:
    print_header("200 OK", "text/html");
else:
    print_header("400 Bad Request", "text/html");
print_length(content);
print("\r\n\r\n");
print(content);
