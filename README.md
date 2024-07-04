# 42 ft_webserv 

## Table of Contents
- [Requirements](#requirements)
- [TCP/IP Model](#tcp/ipmodel)

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
### What is a Web Server?
### What are sockets? And how are they used?


