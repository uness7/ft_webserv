import cgi
import os

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
# print(path)
if username and password: 
    if auth(username, password):
        template = read_template(path)
        template = template.format(name=username, status='', age='', email='', 
                                   feedback='', colors='', country='')
        print()
        print(template)
    else:
        print()
        print(f"""
        <html>
            <body>
                <h1>Invalid Credentials</h1>
            </body>
        </html>"""
        )
else:
    print()
    print(f"""
        <html>
            <body>
                <h1>Missing Credentials</h1>
            </body>
        </html>"""
    )
