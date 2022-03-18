# Programming Assignment 1: **Noisy Channel**

## **Overview**
in this assignment, we were tasked with transferring a message across a noisy channel.
The goal was to retrieve the message, and fix single-bit errors using `(26, 31, 3)-hamming` code.

The code is written in `C` with the use of `winsock` for using sockets on windows machines.
A lot of the basic principles were taken from [binarytides WinSockets tutorial](https://www.binarytides.com/winsock-socket-programming-tutorial/), other references are marked in the corresponding places in the source code.

## **How to run:**
1. Run `Channel.exe` first. \
    If needed, specify the noise type with the corresponding flags:
    > ```-d <n-th bit to flip> - Deterministic noise, flips a bit every n bits throughout the entire message.```\
    ```-r <probability> <random seed> - Random noise, for each bit, choose wether to flip or not based on a random number generator and the specified probability out of 2^16```

2. `Channel.exe` will output params like the IP address and the port for the sender and receiver programs.
3. Feed these params to `Sender.exe <IP> <port>` and `Receiver.exe <IP> <port>`.
4. Follow the instructions on screen, as specified in the assignment handout.

## **Example run-through**:
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

_____________
# TODO
* [ ] Make infinite loop
* [ ] Clean up prints, move to `stderr` instead of `stdout`
