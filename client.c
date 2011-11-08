/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 300 // max number of bytes we can get at once 

// get sockaddr, IPv4 or IPv6:


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void itoa(int value, char* str, int base);
void insertionSort(int v[], int tam);

int main(int argc, char *argv[])
{
    int sockfd, numbytes;  
    int32_t buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    //Make sure everything is clear inside hints
    memset(&hints, 0, sizeof hints);
    
    //I dont care if its either Ipv4 or Ipv6
    hints.ai_family = AF_UNSPEC;
    
    //TCP protocol, connection oriented, order matters, etc.
    hints.ai_socktype = SOCK_STREAM;

    //Using the IP i've inputed on argv[1] and the hardcoded port populate the servinfo structure to be used on my socket.
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        
        //Once the information was found, I'll use it to implicitly create a socket (connect method does that)
        //and estabilish a connection to the information provided
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    //No information was obtained from the linked list of attribute information generated by getaddrinfo
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    //I mention about this function on the server code, it just make ip readable on 255.255.255.255 format.
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    //This will expect the message from the server Hello World
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

//    buf[numbytes] = '\0';
    
    int i;
    for(i=0;i<numbytes;i++)
        buf[i] = ntohl(buf[i]);
        
    char convertor[100];
    
    int aux;
/*    
    for(i=0;i<(numbytes/sizeof(int32_t));i++)
    {   aux  = (int)buf[i];
        itoa(aux,convertor,10);
        printf("%s\n",convertor);
    }
*/   
    //Since I received the bucket, I'll do my job helping the server sorting this bucket for it.
    //I'll run an insertionsort in place.
    insertionSort(buf,numbytes/sizeof(int32_t));
      
//    printf("client: received '%s'\n",buf);

    for(i=0;i<(numbytes/sizeof(int32_t));i++)
    {   aux  = (int)buf[i];
        itoa(aux,convertor,10);
        printf("%s\n",convertor);
    }



    close(sockfd);

    return 0;
}

/**
	
 * Ansi C "itoa" based on Kernighan & Ritchie's "Ansi C"
	
 * with slight modification to optimize for specific architecture:
	
 */
	
void strreverse(char* begin, char* end) {
	
	char aux;
	
	while(end>begin)
	
		aux=*end, *end--=*begin, *begin++=aux;
	
}
	
void itoa(int value, char* str, int base) {
	
	static char num[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	
	char* wstr=str;
	
	int sign;
	
	div_t res;
	

	
	// Validate base
	
	if (base<2 || base>35){ *wstr='\0'; return; }
	

	
	// Take care of sign
	
	if ((sign=value) < 0) value = -value;
	

	
	// Conversion. Number is reversed.
	
	do {
	
		res = div(value,base);
	
		*wstr++ = num[res.rem];
	
	}while(value=res.quot);
	
	if(sign<0) *wstr++='-';
	
	*wstr='\0';
	

	
	// Reverse string
	
	strreverse(str,wstr-1);
	
}

void insertionSort(int32_t V[], int tam)
{         
        int i, j;
        for(i = 1; i < tam; i++){
                j = i;
                while(V[j] < V[j - 1]) {
 
                        int32_t aux = V[j];
                        V[j] = V[j - 1];
                        V[j - 1] = aux;
                        j--;   
 
                        if(j == 0)break;
                }         
        }
        return;
}