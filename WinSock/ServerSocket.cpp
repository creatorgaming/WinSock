// SocketServer.cpp is C++ file which handles all server operations
// This server code is written to make a server that can handle multiple clients
#include <iostream>
#include <winsock.h>
using namespace std;

// Defining a port on which we will do all the communcation
#define PORT 9909

/*
# this sockaddr_in contains :
		short   sin_family;
		u_short sin_port;
		struct  in_addr sin_addr;	# machine address ip address
		char    sin_zero[8];	# just some array we initalize with 0 since while commnication we can store info here
*/
struct sockaddr_in srv;

/*
# content of fd_set - fd_set limit is 64 i.e we can store at max 64 file discriptors in fd_set
	u_int   fd_count;               // how many are SET?,  number of file discripters that i am going to set
	SOCKET  fd_array[FD_SETSIZE];   // an array of SOCKETs, those socket discripters will be stores in this array

# fr - collection of those socket discripters for which i am ready to read, on which we are reading over the network
# fw - collection of those file/socket discriptor on which i am going to write on the network
# fe - those socket discriptors which are showing some errors/exceptions

# All the socket discriptor we store in this fd_set that is a set that will be maintained within a table
# the value stores in that table is a socket discriptor and that socket discriptor will be pointing to an open
# file discriptor within the kernal, basically whenever we open a socket discriptor here that will be an Kernal Object.
# (Kernal objects are created in the kernal memory that is seperate from user memory, that is maintained by the kernal
# and is available to us through some apis). Any alye that is place withing the fd_array is pointing to a particular location
# in kernal containing the information about an open connection over the network.
*/
fd_set fr, fw, fe;


int nMax;

int main() {

	int nRet = 0;
	// Initializing the WSA variables (For windows WSA is an envirement that supports socket progamming in windows envirement)
	// WSA - enables those API that enables socket programming in windows platform
	WSADATA ws;
	if (WSAStartup(MAKEWORD(2, 2), &ws) < 0) {
		cout << "WSA failed to initialize" << endl;
		exit(EXIT_FAILURE);
	}
	else {
		cout << "WSA initialized successfully" << endl;
	}

	// Initializing the socket
	// # we are using connection oriented socket
	int nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  // socket(address, socket stream, protocol) # 0 - system underlying protocol i.e http Protocol
	// # if we are using TCP protocol we should use AF_INET as address in windows

	if (nSocket < 0) {
		cout << "The socket not opened" << endl;
		exit(EXIT_FAILURE);
	}
	else {
		cout << "The Socket opened successfully, with socket ID : " << nSocket << endl;
	}

	// Initialize the environment for sockaddr structure
	srv.sin_family = AF_INET;
	srv.sin_port = htons(PORT);	// # htons helps in conversion from network to host byte orders.
	srv.sin_addr.s_addr = INADDR_ANY; // # i am assigning my IP address to s_addr
	memset(&(srv.sin_zero), 0, 8);	// we are intitializing all the elements of sin_zero to 0

	// Setting Blocking and Non Blocking structure of sockets
	// optval = 0 means blocking and optval != 0 means non blocking
	//u_long optval = 0;
	//nRet = ioctlsocket(nSocket, FIONBIO, &optval);
	if (nRet != 0) {
		cout << "ioctsocket call failed" << endl;
	}
	else {
		cout << "ioctsocket call passed" << endl;
	}


	// Bind to the local Port
	// We need to bind server to local port i.e which process port should server respond to and accept request
	nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));
	if (nRet < 0) {
		cout << "Failed to bind to local Port" << endl;
		exit(EXIT_FAILURE);
	}
	else {
		cout << "Successfully binded to local port" << endl;
	}

	// Listen the request from client (queues the requests)
	nRet = listen(nSocket, 5);	// listen(listening socket id, backlog)
	// # backlog is how many requests our server can pool at a time
	// # that is no. of requests that can be on active pool of our server
	// # if more than 5 clients come the they have to be in waiting queue
	if (nRet < 0) {
		cout << "Failed to listen to local Port" << endl;
		exit(EXIT_FAILURE);
	}
	else {
		cout << "Started listening to local port" << endl;
	}

	nMax = nSocket;
	struct timeval tv;
	tv.tv_sec = 1;	// how many secound to wait
	tv.tv_usec = 0;	// we are not providing any microseconds



	// Keep waiting for new requests and proceed as per the request
	while (1) {
		// Clear all socket/file discriptors
		//  
		FD_ZERO(&fr);
		FD_ZERO(&fw);
		FD_ZERO(&fe);

		// This is nessary since when select system call occurs it clear all the file descriptors 
		// so before calling select again we need to set these file descriptors again
		FD_SET(nSocket, &fr); // current system socket is added to read set of socket discriptors and exception set but not write set.
		FD_SET(nSocket, &fe);

		cout << "Before select call : " << fr.fd_count << endl;

		nRet = select(nMax + 1, &fr, &fw, &fe, &tv);
		// select() tells us how many socket descriptors are ready to read, how many are ready to write etc.
		// before the select returns it clears all these socket descriptors sets, SO WE NEED TO AGAIN SET THEM
		// # tv - it is time value that tells us timeout i.e for how many secound we need to wait to see that 
		//        any of these file discriptors are ready.
		if (nRet > 0) {
			// If nRet has some value that means these many socket discriptors are ready to 
			// read, write or throw the exceptions over the network
			// Case when somewhen connects or communicates with a message over dedicated connection

		}
		else if (nRet == 0) {
			// No Connection or any communication request made or we can say 
			// that none of the socket descriptors are ready
			cout << "Nothing on port : " << PORT << endl;
		}
		else {
			// Select function failed and your application should show some useful message
			cout << "The select call failed" << endl;
			exit(EXIT_FAILURE);
		}
		cout << "After select call : " << fr.fd_count << endl;
	}
	return 0;
}