#!/usr/bin/env python3

content = """<!DOCTYPE html>
<html>
<head>
	<title>Form</title>
</head>
<body>
    <h2>Uploading a file to server</h2>
    <form action="./upload.py" method="POST" enctype="multipart/form-data">
        <label for="file">Select a file:</label>
        <input type="file" name="file" id="file">
        <br>
        <input type="submit" name="submit" value="Upload">
    </form>
</body>
</html>"""

print("HTTP/1.1 200")
print("Content-Type: text/html")
print("Content-Length: " + str(len(content)))
print("\r\n")
print(content)
