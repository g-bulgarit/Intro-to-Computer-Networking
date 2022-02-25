# Programming Assignment 1: Noisy Channel

## Hard-coded parameters
We set the ports on the noisy channel to be **6000** for the Sender<->Channel interface, and **6001** for the Channel<->Reciever interface.
Running the code with other ports in mind will require compiling the code for ```channel``` with new definitions.


## How to run:
Make sure to run **channel.exe** and **reciever.exe** before running **sender.exe**

**Example parameters**:
>```reciever.exe 127.0.0.1 6000```
>
>```channel.exe -r 250 1337```
>
>```sender.exe 127.0.0.1 6001```

## Network diagram:
* **Sender** (6000) -> (6000) **Noisy Channel** (6001) -> (6001) **Reciever**
