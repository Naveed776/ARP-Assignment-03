# ARP Assignment No. 03
	Naveed Manzoor Afridi	| 5149575 |
	Abdul Rauf		| 5226212 |
	Group Name		| NR007   |
## GitHub
https://github.com/Naveed776/ARP-Assignment-03.git

## Description of the programs
The code to design, develop, test and deploy is a modified version of Assignment 2, including client/server features. We refer to this as "application". In the modified application, process B, shared memory and the second ncurses window are unchanged. Process A includes two new features:
1. client connection toward a similar application running on a different machine in the network.
2. server connection for a similar application running on a different machine in the network.

Therefore the application, when launched, asks for one execution modality:
1. normal, as assignment 2
2. server: the application does not use the keyboard for input: it receives input from another application (on a different machine) running in client mode
3. client: the application runs normally as assignment 2 and sends its keyboard input to another application (on a different machine) running in server mode
When selecting modes 2 and 3 the application obviously asks address and port of the companion application.

IP protocol: TCP

data: a byte stream of char, one per key pressed.

(flush data if necessary).

There is also a **master** process already prepared for you, responsible of spawning the entire simulation.

## Folders content

The repository is organized as it follows:
- the `src` folder contains the source code for all the processes
- the `include` folder contains all the data structures and methods used within the ncurses framework to build the two GUIs

After compiling the program other two directories will be created:

- the `bin` folder contains all the executable files
- the `out` folder will contain the saved image as a *bmp* file
- the `log` folder will contain the log files of master, processA and processB

 
## Processes
The program is composed of 3 processes:

- master.c, in addition to the features implemented in the second assignment, will ask the user in which modality to run the program, normal, server or client, and launch the two processes.
- processA.c, depending on how the user launched the program, will work as follows:
        in normal mode the program will work the same as in the second assignment
        in server mode the program will wait until a client is connected and listen for inputs from the client to move the circle in the window
        in client mode the program will connect to a server and command both its window and the server window, by sending via socket the pressed keys
- processB.c will work as in the second assignment, depending on the position of the circle of the processA



## Requirements
The program requires the installation of the **konsole** program, of the **ncurses** library and of the **bitmap** library. To install the konsole program, simply open a terminal and type the following command:
```console
$ sudo apt-get install konsole
```
To install the ncurses library, type the following command:
```console
$ sudo apt-get install libncurses-dev
```

## *libbitmap* installation and usage
To work with the bitmap library, you need to follow these steps:
1. Download the source code from [this GitHub repo](https://github.com/draekko/libbitmap.git) in your file system.
2. Navigate to the root directory of the downloaded repo and run the configuration through command ```./configure```. Configuration might take a while.  While running, it prints some messages telling which features it is checking for.
3. Type ```make``` to compile the package.
4. Run ```sudo make install``` to install the programs and any data files and documentation (sudo permission is required).
5. Upon completing the installation, check that the files have been properly installed by navigating to ```/usr/local/lib```, where you should find the ```libbmp.so``` shared library ready for use.
6. In order to properly compile programs which use the *libbitmap* library, you first need to notify the **linker** about the location of the shared library. To do that, you can simply add the following line at the end of your ```.bashrc``` file:      
```export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
```

## How to compile and run it
I added one file .sh in order to simplify compiling and running all the processes.  
To compile and run it: execute ```compile_run.sh```;  
To correctly run the programs, you also need to install the *libbitmap* library (see the following paragraph).

After running the code, the user will be asked in which modality to launch the program:

    Which modality do you want to launch the program in (insert the number)?
    1. Normal
    2. Server
    3. Client
    4. Exit

Insert the number to choose the modality. In case you choose to run the program in **client** mode, you'll be asked to write the IP address of the server and the port number to use

    Insert the IP address:
    <ip_address>

    Insert the port number (between 1024 and 65535):
    <port_number>

If you choose to run the program in **server** mode, you'll be asked just for the port number.

To work properly, the window related to `processA.c` needs to be 90x30 and the window related to `processB.c` needs to be 80x30 (in case of server mode, resize the window after the client is connected, to avoid errors).

During the execution of the program, if inside of `processA.c` you press **q**, the program will exit and go back to the main menu. It will go back to the main menu also in case of errors.
