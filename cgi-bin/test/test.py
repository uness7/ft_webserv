import os

content = "<html><h1> Elements: </h1><ul>"
for name, value in os.environ.items():
    content += "<li>%s -> %s</li>" % (name, value)

content += "</ul></html>"

test = 0;
while True:
    test+=1
    

print("{} 200 OK".format(os.environ["SERVER_PROTOCOL"]))
print("Content-Type: text/html");
print(f"Content-Length: {len(content)}\r\n\r\n")
print(content)
