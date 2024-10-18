import cgi
import os

def print_header(status="200 OK", content_type="text/html"):
    print(f"HTTP/1.1 {status}");
    print(f"Content-Type: {content_type}");

def print_length(content):
    print(f"Content-Length: {len(content)}")

USER_CREDENTIALS = {
        "admin": "adminpassword",
        "user": "userpassword"
}

def auth(username, password):
    return USER_CREDENTIALS.get(username) == password

def read_template(filename):
    with open(filename, 'r') as file:
        return file.read()

form = cgi.FieldStorage()
username = form.getvalue("username")
password = form.getvalue("password")

path = os.getcwd() + "/cgi-bin/auth/response.html";
if username and password: 
    if auth(username, password):
        template = read_template(path)
        print_header("200 ok", "text/html");
        print_length(template);
        print("\r\n\r\n");
        print(template);
    else:
        print_header("401 Unauthorized", "text/html");
        content = "<html><body><h1>Invalid Credentials</h1></body></html>";
        print_length(content);
        print("\r\n\r\n");
        print(content);
else:
    print_header("400 Bad Request", "text/html");
    content = "<html><body><h1>Missing Credentials</h1></body></html>";
    print_length(content);
    print("\r\n");
    print(content);
