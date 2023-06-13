# DIY_Redis

### Inspired by the book [Build Your Own Redis](https://build-your-own.org/redis/)  

---
The goal of this project was to gain a deeper understanding of network/socket programming. I also wanted to improve my ability to write low-level software, as I usually spend most of my development time a few layers up the stack. In doing this, I learned a lot of very valuable concepts that I think will be useful when developing any kind of application.

Another goal I had was gaining experience writing serious C/C++ applications. My experience with C/C++ prior to this had largely been confined to either very narrow course projects or competitive programming. I wanted to actually get a feel for how you would set up a more "serious" environment, and develop something given the capabilities (and limitations) of C/C++. I also gained a significantly better understanding of a lot of the underlying Linux concepts powering these tools.

---

## Running the code 
In order to actually run the application you can run `make run-server`in the root directory. This spins up a server which is then ready to take commands from the client, which you can run with `make run-client ARGS=<your args>`

The supported commands are GET, SET and DEL. For example:
```bash
$ make run-client ARGS="set 5 'Hej hopp i lingonskogen'"
build/client set 5 'Hej hopp i lingonskogen'
(nil)
$ make run-client ARGS="get 5"
build/client get 5
(str) Hej hopp i lingonskogen
$ make run-client ARGS="del 5"
build/client del 5
(int) 1
$ make run-client ARGS="get 5"
build/client get 5
(nil)
```

There is also a python script which runs some automated (random) commands of the type you see above. This is mainly used for testing purposes, but could also serve as a benchmark for the speed at which we can handle requests. To run this script you can simply run

```sh
py test_commands.py # Assuming you have python installed and it is called py
```
# Code
The server can handle concurrect requests through the use of an event loop. We also take advantage of pipelining in order to increase throughput even further. Requests sent and received are manually read through sockets on both the client and server, though in this case only locally and never through the network. The requests sent between client and server use their own protocol and use serialization to handle different data types (Null, String, Error, Int, Array). 

This project also features some homemade datastructures, namely the AVL tree as well as a hashtable. The hashtable is used for storing the Key-value pairs, but I unfortunately have not had the time to integrate the AVL tree.