Build steps: Two option for build.  
=================================  
form topdir:  
1) build for topdir(optional).  
cd socketintro;  
make  
above step will compile server and cleint.  

From each modules(server and cleint).  
==================================    
How to build server  
2) cd socketintro/server  
    make  

makefile output for reference:  
	mkdir -p bin/  
	mkdir -p obj/  
	gcc -Wall  -Wno-unused-variable  -DLOG_USE_COLOR -g -I./include/ -c src/log.c src/server.c src/server_impl.c  
	mv *.o obj/  
	gcc  obj/log.o obj/server.o obj/server_impl.o -o bin/server -lpthread  
    	

How to build cleint.   
cd socketintro/client   
   make  

makefile outout:   
   mkdir -p bin/  
   mkdir -p obj/  
   gcc -Wall -std=c99 -g -I../include/ -c src/client.c -o obj/client.o  


How to run:   
===============================     
for server:  
cd socketintro/server (run as root, because log file will be genrated as /var/log/server.log)  
./bin/server  
===============================    
for client:  
cd socketintro/client.  
./bin/client  
================================  
DONE:  
1) ls command support.  
2) bye command support.  
3) server will give proper message to client for unsupported cmds.  
================================  
TODO:  
1) use proper locking for logging as logging is happening in multi threaded environment.  
2) want to use prtobuf library, as this will eliminate the machine type dependency.(need more time to understand, .protoc files :( )  
3) change dir functionality need to implemted.  
