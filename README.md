Socket program to send raw packets (eth->ipv4->tcp)

Linux cmd to create shared libraries and compile program after changes.
	sudo gcc -shared -o ipv4.so -fPIC ipv4.c
	sudo gcc -shared -o tcp.so -fPIC tcp.c
	sudo gcc sendFrames.c -o sendFrames -ldl

To run app from command line. "lo" is sample interface.
	sudo ./sendFrames lo

