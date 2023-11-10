# share-a-keyb

I might have sort of destroyed my main PC's keyboard with water damage... This program is going to be a really simple way to share a keyboard between two computers. You just need to run the server on the computer with a keyboard and the client on the computer without one. Simple as.

Note; make sure you have Python 3 installed.

To build the full package on your system simply:
```bash
cmake -B ./build
cmake --build ./build
```

Both the server and the client for your operating system will now be located in `./build/` under the very unassuming filenames of "`server`" and "`client`".

The server will read the keystrokes from your machine, when you run it you just need to provide it with a port it should work under:
```bash
# run server on port 2138
./server 2138
```

The client receives the keystrokes from the server and emulates them on the machine it's running on. To run it we need the server's IP address and port:
```bash
# connect to a server on machine 10.1.1.12 on port 2138
./client 10.1.1.12 2138
```

Now you may realise that you cannot close the server via linux signals (ctrl+c or ctrl+z). This is fully intentional behaviour. These keystrokes are often used and needed in programming (which is what I need this keyboard for, after all), so I disabled their default behaviour. The connection is meant to be broken on the client's side; after that the server will quit.


Features (bold if implemented, italic if to-be implemented):
- **Linux Server**
- *Linux Client*
- **Windows Server**
- **Windows Client**