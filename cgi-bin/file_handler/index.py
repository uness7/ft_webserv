#!/usr/bin/env python3

import cgi
import os
import cgitb
from urllib import parse

class Handler:
    def __init__(self) -> None:
        self.http_version = os.getenv("SERVER_PROTOCOL")
        self.request_method = os.getenv("REQUEST_METHOD") 
        self.query_string = os.getenv("QUERY_STRING") 
        self.upload_dir = "uploads" # Maybe get the upload dir from the config file through env variable if exist  ? 

    def proceed(self):
        if self.request_method == "GET":
            return self.display_files()
        elif self.request_method == "POST":
            return self.upload_file() 
        elif self.request_method == "DELETE":
            return self.delete_file()
        return self.unexpected_method()

    def print_response(self, response_code = "404 Not Found", content_type = "text/plain", content = ""):
        print(f"{self.http_version} {response_code}")
        print(f"Content-Type: {content_type}")
        print(f"Content-Length: {len(content)}\r\n")
        print(content)

    def unexpected_method(self):
        self.print_response("404 Not Found", "text/plain", "Not Found")

    def display_files(self):
        content = """<!DOCTYPE html>
        <html>
        <head>
            <title>Form</title>
        </head>
        <body>
            <h2>Uploading a file to server</h2>
            <form action="./index.py" method="POST" enctype="multipart/form-data">
                <label for="file">Select a file:</label>
                <input type="file" name="file" id="file">
                <br>
                <input type="submit" name="submit" value="Upload">
            </form>
        </body>
        </html>"""
        self.print_response("200 OK", "text/html", content)

    def delete_file(self):
            qs = parse.parse_qs(self.query_string)
            filename = qs.get("filename", [None][0])
            if filename is None:
                return self.print_response("400 Bad Request", "text/plain", "Bad Request: No filename provided")
            filename = filename[0]
            file_path = os.path.join(self.upload_dir, filename)
            if not os.path.isfile(file_path):
                return self.print_response("404 Not Found", "text/plain", f"Not Found: '{filename}' not exist")
            os.remove(file_path) 
            return self.print_response("200 OK", "text/plain", f"{filename} removed")

    def upload_file(self):
        cgitb.enable()
        self.form = cgi.FieldStorage()
        file = self.form["file"]
        if not file.filename:
            return self.print_response("400 Bad Request", "text/plain", "Bad Request")
        filename = os.path.basename(file.filename)
        file_path = os.path.join(self.upload_dir, filename)
        if not os.path.exists(self.upload_dir):
            os.makedirs(self.upload_dir)
        try:
            with open(file_path, 'wb') as f:
                f.write(file.file.read())
            content = f"""<html><body><p>The file "{filename}" was uploaded successfully</p><li><a href="http://localhost:8070">Go back to homepage</a></li></body></html>""";
            return self.print_response("200 OK", "text/html", content)
        except Exception as e:
            message = f'An error occurred while uploading the file: {str(e)}'
            return self.print_response("500 Internal Server Error", "text/plain", message)

handler = Handler()
handler.proceed()
