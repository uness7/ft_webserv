import requests
import threading
import datetime


def send_request(url):
    response = "No response"
    match url[0]:
        case "GET":
            response = requests.get(url[1])
        case "POST":
            response = requests.post(url[1], json={ "Attack": "DDOS"})
        case "DELETE":
            response = requests.delete(url[1], json={ "DELETE": "DDOS"})
    ct = datetime.datetime
    print(f"[{ct.now().microsecond}] Response from {url}: {response} ")

if __name__ == "__main__":
    urls = [
        ["GET", "http://localhost:8080/public/images/restaurants/stil-u2Lp8tXIcjw-unsplash.jpg"],
        ["GET", "http://localhost:8090/index.html"],
        ["DELETE", "http://localhost:8080/index.html"],
        ["GET", "http://localhost:8090/la_note_enchantee.html"],
    ]

    u_len = len(urls);
    threads = []

    for n in range(u_len * 1000):
        thread = threading.Thread(target=send_request, args=(urls[n % u_len],))
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

