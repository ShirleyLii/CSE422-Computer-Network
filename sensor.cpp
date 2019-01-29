
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cmath>
#include <ctime>
#include <chrono>  // for clock-based random number generation
#include <random>  // for normal distribution
#include "message.h"

#define MEAN 70.0
#define STRDEV 3.0

using namespace std;

float readsensor();

int main(int argc, char *argv[])
{
	int udp_socket, tcp_socket ;
        int client_tcp_socket;
        int mesglen;

	socklen_t slen = sizeof(sockaddr_in);
	sockaddr_in client_tcp_addr;

        
        UDP_Cntr_MSG ctrl_msg;
        Sensor_MSG sensor_msg;

	// create the TCP socket
	tcp_socket = socket(AF_INET, SOCK_STREAM,0);
	if(tcp_socket< 0 )
	{
		cerr << "sensor: failed to create TCP socket. Exiting." << endl;
		return -1;
	}
	   
        client_tcp_addr.sin_family = AF_INET;
        client_tcp_addr.sin_addr.s_addr = INADDR_ANY ;
        
	// bind a name to the TCP socket, letting the OS choose the port number
        client_tcp_addr.sin_port=0;
        bind(tcp_socket,(struct sockaddr *)&client_tcp_addr,
                sizeof(client_tcp_addr));
        

	// get the port number of the socket from the OS and print it out;
	// the port number will be a command-line parameter to the base program
        getsockname(tcp_socket,(struct sockaddr *)&client_tcp_addr,
                &slen);
	cout <<"sensor: TCP connection port number is: "<< client_tcp_addr.sin_port
                <<"\n";
	// configure the TCP socket (using listen) and accept a connection request from the base
        listen(tcp_socket,5);
        client_tcp_socket=accept(tcp_socket,(sockaddr *)&client_tcp_addr,&slen);
        close (tcp_socket);
	   
	// read the control message from the TCP socket and echo its contents
        if (client_tcp_socket == -1){
            cout << "sensor: accept failed. Exiting." << endl;
	}
        
        cout<<"sensor: Successfully connected with base"<<endl;
        mesglen = read(client_tcp_socket,&ctrl_msg,sizeof(ctrl_msg));
        //print out parameters
        cout<<"sensor received control parameters."<<
                    " numsamples: "<<ctrl_msg.numsamples<<", port: "<<
                ctrl_msg.port<<", interval: "<<ctrl_msg.interval<<endl;

        
	
	// create a UDP socket
        sockaddr_in client_udp_addr;
        udp_socket = socket(AF_INET,SOCK_DGRAM,0);
        client_udp_addr.sin_family = AF_INET;
        if(udp_socket< 0 )
	{
		cerr << "sensor: failed to create udp socket. Exiting." << endl;
		return -1;
	}
        client_udp_addr.sin_port = ctrl_msg.port;

	// configure a sockaddr_in data structure for sending to the base; note that the IP address field (sin_addr)
	// can be obtained from information returned from the sockaddr data structure returned by the earlier accept() call
        client_udp_addr.sin_addr = client_tcp_addr.sin_addr;
        
        //get IP address
        cout<<"extracted base IP address: "<<inet_ntoa(client_udp_addr.sin_addr)
                <<endl;

	// loop, each time reading from the sensor, sending the sample to the base, and sleeping from the prescribed interval

        for (int i=1; i<(ctrl_msg.numsamples+1);i++){
            float read_sensor = readsensor();
            sensor_msg.sequence=i;
            sensor_msg.value = read_sensor;
            cout<< i << " "<<read_sensor<<" were sent"<<endl;       
          sendto(udp_socket,&sensor_msg,sizeof(sensor_msg),0,
                  (struct sockaddr *)&client_udp_addr,sizeof(client_udp_addr));
            sleep(ctrl_msg.interval);
        }

    // close both sockets
        close(udp_socket);
        close(client_tcp_socket);
}



float readsensor()
{

  // construct a simple random generator engine from a time-based seed:
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine generator (seed);

  std::normal_distribution<double> distribution (MEAN,STRDEV);

  return(distribution(generator));
}