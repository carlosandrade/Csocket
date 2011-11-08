This code implements a distributed bucketsort.

1. Server starts requesting for number of H hosts to be connected to server using server.c.
	1.1 Server uses a function to create H buckets, each of them will be sent to one of the hosts.
	1.2 Server waits for each bucket connection.
		Obs: Last time measurement output on server host includes only 1.1, it doesnt counts 1.2
2. Client hosts start connection using client.c 
	2.1 Client request bucket to server
	2.2 Client waits for bucket 
	2.3 Client uses InsertionSort on bucket to order elements
	2.4 Client send data back
	2.5 Client ends execution.
		Obs: Client time measurement includes 2.1,2.2, 2.3, 2.4 and 2.5
3. Server fork children processes to deal with each connection, that is, we have H children processes.
	3.1. For each children process:
		3.1.1. Children receives data
		3.1.2. Children kills itself
	3.2 Parent process is blocked until all children kill itself after receiving the data
	3.3 Server ends execution
		Obs: Server outputs including pid and time measurement includes 3.1.1 and 3.1.2
		


Credits:
	Basic client/server socket communication and background theory: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#htonsman
	More introductory material used for basic socket understanding: http://net.pku.edu.cn/~course/cs501/2011/code/BSD_Socket.t/sockets.pdf 
	Insertion Sort: http://pt.wikipedia.org/wiki/Insertion_sort
	
	
Experimental Section--

Altough code lacks a lot of parts that requires improvents: e.g., Many casts are used, unnecessary loops, call for functions to maintain portability (local host to network and vice-versa), etc I've adapted the code to print the time outputs in case code gets improved for real measurement.

Just for illustrative purposes, here are data obtained from few experimental tests:

(Pay tention to input order from each terminal window)

undine:Csocket carlosviansi$ ./server 2
	pid	time_in_ms
	0	13.790000
	1	2.818000

	n_hosts	n_elem	time_in_ms
	2	300	0.950000
undine:Csocket carlosviansi$ ./server 2
	pid	time_in_ms
	0	3.800000
	1	0.407000

	n_hosts	n_elem	time_in_ms
	2	300	1.494000
undine:Csocket carlosviansi$ ./server 2
	pid	time_in_ms
	0	4.236000
	1	3.878000

	n_hosts	n_elem	time_in_ms
	2	300	0.929000
undine:Csocket carlosviansi$ ./server 2
	pid	time_in_ms
	0	0.390000
	1	4.233000

	n_hosts	n_elem	time_in_ms
	2	300	0.929000
undine:Csocket carlosviansi$ ./server 2
	pid	time_in_ms
	0	5.171000
	1	3.723000

	n_hosts	n_elem	time_in_ms
	2	300	1.431000
undine:Csocket carlosviansi$ ./server 2
	pid	time_in_ms
	0	5.880000
	1	3.832000

	n_hosts	n_elem	time_in_ms
	2	300	1.135000


Host 1 (Another terminal tab -- I'm sore I don't own a lanhouse)

undine:Csocket carlosviansi$ ./client localhost
	n_elem	time_in_ms
	74	2.833000
undine:Csocket carlosviansi$ ./client localhost
	n_elem	time_in_ms
	74	3.067000
undine:Csocket carlosviansi$ ./client localhost
	n_elem	time_in_ms
	74	2.957000
undine:Csocket carlosviansi$ ./client localhost
	n_elem	time_in_ms
	74	1.861000
undine:Csocket carlosviansi$ ./client localhost
	n_elem	time_in_ms
	74	2.976000
undine:Csocket carlosviansi$ ./client localhost
	n_elem	time_in_ms
	74	3.307000

Host 2 - An HP computer 1.8 Ghz, special edition with a dragon (since the code lacks performance improvements I wont drop hardware information here)

carlos@midgard:~/Desktop/carlosandrade-Csocket-8a4833b$ ./client 192.168.1.3
    n_elem    time_in_ms
    23    120.125000
carlos@midgard:~/Desktop/carlosandrade-Csocket-8a4833b$ ./client 192.168.1.3
    n_elem    time_in_ms
    23    10.103000
carlos@midgard:~/Desktop/carlosandrade-Csocket-8a4833b$ ./client 192.168.1.3
    n_elem    time_in_ms
    23    7.145000
carlos@midgard:~/Desktop/carlosandrade-Csocket-8a4833b$ ./client 192.168.1.3
    n_elem    time_in_ms
    23    680.051000
carlos@midgard:~/Desktop/carlosandrade-Csocket-8a4833b$ ./client 192.168.1.3
    n_elem    time_in_ms
    23    193.426000


-- Finally, you may be wondering how long it would take if I was running a bucketsort on my own computer just with threads. The code provided on other of my assignments in Java at [1] gave the following output graph:

http://dl.dropbox.com/u/6222648/bucketsort_threads_graph.png
	
[1] https://github.com/carlosandrade/bucketsort_threads/blob/master/BucketSort.java

Again, you may be wondering why the performance on the C code is so much worse in this sample even the thread code being coded in Java. Not only both codes needs improvement (as they were not intended to be the best available solution by means of performance, but just an university assignment to learn threads and socket usage), but the afore mentioned itens for the distributed bucketsort might affect this as well. 