Socket program to send raw packets (eth->ipv4->tcp)

Linux cmd to create shared libraries and compile program after changes.
	sudo gcc -shared -o ipv4.so -fPIC ipv4.c
	sudo gcc -shared -o tcp.so -fPIC tcp.c
	sudo gcc sendFrames.c -o sendFrames -ldl

To run app from command line. "lo" is sample interface.
	sudo ./sendFrames lo

Parametrs:
	a - acknowledgment number + 
	b - interface +
	d - destination ip address +
	e - sequence number + 
	f - don't fragment +
	g - fin flag +
	h - help +
	i - identification +
	j - fragment offset 
	n - destination port +
	o - type of service +
	p - push flag +
	q - ack flag +
	r - rst flag +
	s - source port +
	t - time to live  +
	u - urg flag +
	w - window size +
	x - urgent pointer +
	y - syn flag +
	z - number of frames to send +