# Programming Assignment 2: DNS Client
Submitted by:
* A
* I

[Link to repository on Github](https://github.com/g-bulgarit/Intro-to-Computer-Networking/tree/main/PA2_DNS_Client)

# **Documentation**

## **Overview**
The goal is to create a command-line-interface that will copy the functionality of the `nsclient` utility, bundled with windows.
In order to send successful DNS requests, we must conform with the protocol defined in [RFC-1035](https://datatracker.ietf.org/doc/html/rfc1035).

-----------------

## **Implementation**
To implement a DNS client of our own, we used a couple of sources for guidance:
1. Obviously, the RFC that defines the DNS protocol.
2. A (very comprehensive!) writeup over at [BinaryTides](https://www.binarytides.com/dns-query-code-in-c-with-winsock/), from which we took inspiration for the general structure of the code, and most notable **the struct for the DNS header** - as noted in `dnsInfra.h`, we had a problem with the struct resizing at compile time, and the code at BinaryTides had a solution to our problem.

-----------------

## **Challanges**
### **Constructing, Parsing and Deconstructing a DNS packet**
The main challange of this project was interfacing with a real DNS server. This means that there is no room for corner-cutting, because sending data in the wrong format simply means that we get no reponse, and we can't change anything about the remote server to compensate for our mistakes.

We referenced a few sources online (as specified above in the **Implementation** section) and got to work on the main part of the project right away.
At first we created the `dnsHeader` struct according to the specification in the RFC, and filled in all the constants accordingly. Sending the message and getting *a* reply was easy enough, but parsing the reply was a pointer hell.

Eventually we pulled through and the program decodes the DNS response packet to fetch the address of the requested domain.

-----------------

### **User input validation**
>These sections are implemented in `validationUtils.c`.

#### **Domain (URL) Validation (and the choice of not using Regex)**
In order to comply with the "runtime example" given in the instructions for `PA2`, we had to come up with a mechanism to determine if a given domain name is valid so we could return a `BADNAME` error in case the name is... bad.
To check the URL for validity, we came up with the following tests:
* Check that there is atleast one `.` in the domain
* Check that there are no sequences of `.` in the domain (for exmaple, `www..test.com` is invalid)
* Check that no part of the domain is biggar than `255` characters.

At first we wanted to solve the problem with Regex, but we noticed that the `<regex.h>` library is not included with the standard windows `c` libs, altough it is included in `POSIX` and by extension also in windows `gnu` ports. Since we didn't know if this project was to be compiled on a unix machine or a windows machine, we decided to play safe by writing the tests from scratch and sacrificing some functionality so that the code could be compiled on any machine. 

#### **IP Validation for the DNS server**
We also decided to test the other user input in the project, the DNS server ip.
In order to do so, we checked the following conditions, assuming that the given IP address will be **IPv4** only:
* Check that there are 3 `.` symbols in the IP and no less
* Check that between these dots, the values are between `0<val<255`.

These conditions apply to any valid IP and thus inputting a wrong IP get caught and the program terminates with an error message.


### **Timeout on `recvfrom`**
This was the first time we had to implement a timeout mechanism on `recvfrom` in order to prevent infinite stalling over a request that will never return.

We saw numerous approaches online, some using `select()`, but we decided to go with the less invasive option of setting the timeout property of the socket itself. We did so using the `setsockopt` function like so, with `timeoutMilliseconds = 2000`:
```
setsockopt(sendSocket, 
           SOL_SOCKET, 
           SO_RCVTIMEO, 
           (const char *)&timeoutMilliseconds, 
           sizeof(timeoutMilliseconds))
```

This worked well and was far easier than the `select()` option, and since we are only working with one socket, we could easily avoid using `select()` for the rest of the project.

-----------------

## **How to run the code**
Run the code with the address of the DNS server as a command line argument, like so:
```
.\nsclient.exe <ip_of_dns_server>
```
Then a familiar shell will appear, along with on-screen-instructions. The usage is identical to `nsclient`.

```
Enter a domain name below, or type 'quit' to exit.
nsclient> www.tau.ac.il
132.66.11.168
```
