# 42 ft_webserv 

## Table of Contents
- [Requirements](#requirements)
- [TCP/IP Model](#tcp/ipmodel)
- [Project Overview](#project-overview)

## Requirements

1. GET, POST, Delete requests
2. Serve static files
3. It has to have default error pages
4. Clients must be able to upload files
5. It needs to be able to listen to multiple ports
6. It should never die
7. It should work with one CGI
8. It should never hang forever
9. HTTP res status codes must be accurate

## Project Overview



If you want deep dive into this project, feel free to read until the end of this readme file!

-----------------------------------------------------------------------------------------

## TCP/IP Model 

### The Process of Communication : Network Communication

- Computer communicate following a set or rules, and these rules are called PROTOCOLS. NIC is the a hardware piece that make this operation possible (through a LAN cable for instance).
- We can send messages, files or videos. All types of data supported. This message is divied into what we call DATA UNITS aka SEGMENTS, TCP is the protocol that is responsible for making these segments and hands them off to IP which is responsible for delivering them to its final destination to form the original message.
- Protocols are a set of rules that allow machines and application programs to exchange information.
- TCP/IP is a suite of protocols known as the Internet Protocol Suite used for Internet communications. 

### TCP's alternative

UDP is an alternative protocol to TCP, each one of them has advantages and disadvantages and it only depends on the use case. In general, UDP is faster, but doesn't handle errors nor cares weather all of the data segments arrived at their final destination. In the other hand, TCP is slow but ensures that the message reach its final destination as it was sent. 

### What are the layers that compose the TCP/IP MODEL?

| Layer            | Protocols Used            |
|------------------|---------------------------|
| Application      | HTTP, FTP, DNS, POP3      |
| Transport        | TCP, UDP                  |
| Network          | IP, ICMP, ARP             |
| Data Link        | Ethernet                  |
| Physical         | Ethernet                  |

To better understand data transmission, and the minute details of each layer of TCP/IP Model refer to this video :
[TCP/IP MODEL EXPLAINED](https://www.youtube.com/watch?v=2QGgEk20RXM&t=75s)

### What is localhost? What is a PORT? What is HTTP?

- LOCALHOST: It's a term that is used to refer to the local machine that you are currently using.
- PORT: It's a virtual point where network connections start and end. It is used to distinguish different kinds of traffic coming to the same IP address. There are 65,536 possible ports for different uses.
- HTTP: As we've already seen, it's a protocol used in the application layer. It is considered the language used by machines to communicate. TCP, in this case, is considered as the phone line that carries the voice. 

### HTTP Basics :

HyperText Transfer Protocol : resides in the application layer in TCP/IP Model. 
It connects two machines, a client machine and a server. The client machine makes a request. If the server has the data requested by the client machine, it returns the data and it returns 404 status code otherwise.

#### What is a Web Server?

A Web Server is a piece of software that listens on a PORT via a transport protocol and sends a response that holds the requested resources.

#### HTTP STATUS CODES: 

- 1xx => informational
- 2xx => Success
- 3xx => Redirection
- 4xx => Client Error
- 5xx => Server Error

#### HTTP REQUEST:

Start Line ----> GET /index.html HTTP/1.1

Headers    ----> User-Agent: Mozilla/5.0

BODY 	   ----> [For a GET req, there is no body]


#### HTTP RESPONSE:

Start Line ----> HTTP/1.1 404 NOT FOUND

Headers    ----> Content-Type: Image/Png

BODY 	   ----> [The file/resource requested]


### What are sockets? And how are they used?

- It is, in general, a software abstraction used in networking.

- It's an endpoint for communication. So, a pair of proccesses communicating over a network, each of them needs a socket. It's logical! 

- A socket is id'ed by a combination of a source IP address, a source port number, a destination IP address, a destination port number, and a Transport Protocol.

	For example:		
		- Source IP Address: 192.168.1.2
		- Source Port Number: 54321
		- Destination IP Address: 192.168.1.1
		- Destination Port Number: 80
		- Transport Protocol: TCP
