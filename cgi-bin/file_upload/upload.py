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
    except Exception as e:
        message = f'An error occurred while uploading the file: {str(e)}'
else:
    message = 'No file was uploaded'

print(f"""\
Content-Type: text/html\n
<html><body>
<p>{message}</p>
<li><a href="http://localhost:8070">Go back to homepage</a></li>
</body></html>
""")
