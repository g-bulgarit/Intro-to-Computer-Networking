# Final Review
Grade: 100 :)
_______________
# Programming Assignment 1: **Noisy Channel**
Submitted by:
* Alon Budker - id
* Itay Zilka - id

Our repository on github: [repository link](https://github.com/g-bulgarit/Intro-to-Computer-Networking/tree/main/PA1_NoisyChannel).

________________
# Documentation
## **Overview**
in this assignment, we were tasked with transferring a message across a noisy channel.
The goal was to retrieve the message, and fix single-bit errors using `(26, 31, 3)-hamming` code.

The code is written in `C` with the use of `winsock` for using sockets on windows machines.
A lot of the basic principles were taken from [binarytides WinSockets tutorial](https://www.binarytides.com/winsock-socket-programming-tutorial/), other references are marked in the corresponding places in the source code.

### **In the following sections:**
* Roadmap
* Challanges
* How to run the code
* Example run-through of the code
* Limitations and bonuses

## **Implementation Roadmap**
Generally, we tried to follow this roadmap in order to get as many of the project's components working in as little time as possible:
>1. Get two sockets working
>2. Get three sockets working, and try to pass one message without adding noise
>3. Add noise to the message and see if the output is messed up
>4. Implement hamming encoding and decoding and check that some noise can be fixed
>5. Cleanup

In the end, everything worked out accoring to plan.

During the coding phase of the project, we referenced a lot of guides and tutorials. 
On one occasion (getting the size of an unopened file) we referenced a thread on stack-overflow's code-review site, and found a good way to fetch the file size. This place is clearly marked in the code, along with the link to the thread.

All other code in this assignment is ours.
Some sources that we referenced:
* [dWise1 socket programming overview](http://dwise1.net/pgm/sockets/sockets.html#OVERVIEW)
* [BinaryTides winSock tutorial](https://www.binarytides.com/winsock-socket-programming-tutorial/)
* Microsoft's MSDN documentation regarding C functions, along with linux manpages for C stdlib functions that are not unix-specific.
* The socket-programming recitation and presentation given by the course staff :)

## **Challanges:**
### **Sockets**
This was the first time we worked with sockets, so the introduction was a bit rough, but after a few prototyping stages we reached a point where we became confident with our implementation of sockets and we were easily able to extend it to support new functionality.

### **Hamming (encoding & decoding)**
We were very surprised when we noticed that there isn't an open-source implementation of `hamming(26,31,3)` as a library or just as example code. In retrospect, it makes sense, since it is probably more beneficial to implement hamming encoding in hardware, and there are more advaned codes that are in use today.

The hamming encoding and decoding part of the excercise is the code that we are least proud of. The implementation is ugly and very straight-forward, with direct bit manipulation spanning across many lines.

To actually implement it, we made use of helper functions that gave us **bit**-access to our data, by using some bitwise math.

Additionally, we implemented the hamming code with a -1 offset to the left, as it was more comfortable (from a coding prespective) to start enumerating the bits from `0` instead of `1`, and thus in our implementation, the first bit is actually the `0-th` bit.

An ongoing struggle with the hamming implementation was the bit endianity convention.
The algorith is usually defined when the MSB bit (on the left) is labeled `1`, while in bitwise math it was easier to work with bits being numbered from the LSB on the right.
This caused a lot of frustration and forced us to write debug functions that would print all of our chunks in bits, so that we can compare and see what the issue is. In retrospect, it was probably better to rewrite the indices in the hamming algorithm from right to left, and just go with that.

### **Fetching random ports**
We left this part for last, as it was easier to develop the entire project around hard-coded values for ports and IPs, and this change is very local anyway.

We used the `bind()` function with the `port` parameter as `0`, which forces the OS to choose an unused port (usualy in the `> 60000` range).

This worked, but in order to actually use this value in the sender and receiver, we had to fetch the selected port and display it.

In order to do that, we used the function `getsockname` to load the new socket's data and fetch the port.

### **Debugging**
Due to our decision to implement most of the user-related-things as hardcoded parameters, debugging was mostly easy, as there wasn't a need to push clunky command-line arguments in the solution settings in VS.

We've also implemented a `#define DEBUG` flag, which controls the verbosity of the printed messages. If the `DEBUG` flag is not defined, only the neccesary prints remain.
On the other hand, if the flag is defined - the stdout is filled with messages that helped us debug our programs.

The project is compiled (and shipped) with the flag _not defined_.

## **How to run:**
1. Run `Channel.exe` first. \
    If needed, specify the noise type with the corresponding flags:
    > ```-d <n-th bit to flip> - Deterministic noise, flips a bit every n bits throughout the entire message.```\
    ```-r <probability> <random seed> - Random noise, for each bit, choose wether to flip or not based on a random number generator and the specified probability out of 2^16```

2. `Channel.exe` will output params like the IP address and the port for the sender and receiver programs.
3. Feed these params to `Sender.exe <IP> <port>` and `Receiver.exe <IP> <port>`.
4. Follow the instructions on screen, as specified in the assignment handout.
5. On consequitive runs, please make sure to say **yes** to the prompt in `Channel.exe` before proceeding with the other programs.

## **Example run-through**:
> **Note:** some labels may have changed, but the core functionality is the same.


### In `Channel.exe`:
```
>>> clear; .\Channel.exe -d 50

-------[NOISY CHANNEL]-------
[INFO] Started listening, waiting for sender and receiver
[INFO] This machine's IP is 192.168.1.107
[INFO]  Set sender target to IP:        192.168.1.107,  port:62046
[INFO]  Set reciever target to IP:      192.168.1.107,  port:62047
```

### In `Sender.exe`:
```
>>> clear; .\Sender.exe 192.168.1.107 62046

-------[SENDER]-------
[INFO] Starting sender with IP 192.168.1.107 and port 62046
[INFO] Connected to the Noisy Channel
>: Enter file name (or type quit (...to quit)): cello_ascii.txt
```

### In `Reciever.exe`:
```

>>> clear; .\Receiver.exe 192.168.1.107 62047
-------[RECIEVER]-------
[INFO] Started socket, connecting to the noisy channel

[Success] Recieved 868 bytes
[Success] Recieved:
♦♥☺

[Decode] Decoded 728 bytes
[Decode] Decoded:
                            ,-.
                           ((o))
                      _  ,-,,-'
                     ( `/=//_
                    _  /=/='_)
                   ( `/=//_
                     /=/='_)
                    /_/ |
                   / /`.'   ===========
                  / //       ##--------^`
                 / //
                / //
               / //
              / / |
           _ / / -._
         .' / /     `.
        /  / /       `-.
       /  /_/        ' `
      '             :  '
     '.       ,-  ,-'.:
     / /     /.  /  ,-'
    /  __   7   / /
 .,'  '  ` '   `._
 /    |/\|      ,'.
/ _   _        / ,'
:' ` | /     ,' /
`.PG |/ _ , ' ,'
   ---""__ ,-'
[Decode] Overall, fixed 139 bits
```

## **Limitations & Bonuses**
We decided not to work on any of the bonus tasks.


1. Currently, the implementation only supports files that are a **multiple of 26-bits in size**. Therefore, a file that is 270 bits long is not supported (...for example).\
Supporting this was regarded as bonus and we opted not to do so :(

2. The random noise implementation iterates over all the bits, calling the `rand` function for each bit. There was a bonus to implement this with a single `rand` call, and we thought about doing it by generating a sufficiently large number and flipping bits according to it's bit-representation, but ultimately we decided to skip the bonus and focus our attention on the main parts of the project.

3. If the computers are not under the same NAT, in order to run this project, you would have to forward the traffic directly between the devices using port-forwarding. During the implementation and testing phase we assumed that the project will be tested on machines that are on the same network and are connected to the same switch.
