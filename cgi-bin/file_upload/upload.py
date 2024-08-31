#!/usr/bin/env python3

import cgi
import os
import cgitb
cgitb.enable()

form = cgi.FieldStorage()

# A nested FieldStorage instance holds the file
fileitem = form['file']

# Test if the file was uploaded
if fileitem.filename:

    # Strip leading path from file name to avoid directory traversal attacks
    fn = os.path.basename(fileitem.filename)
    upload_dir = 'uploads'
    file_path = os.path.join(upload_dir, fn)

    # Ensure the uploads directory exists
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)

    try:
        # Open the file in binary mode and write the uploaded content
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
</body></html>
""")
