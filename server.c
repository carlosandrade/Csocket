/*
** server.c -- a stream socket server demo

This application will implement a distributed system bucket sort. 

The main idea is that the program reads an input of numbers from file 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>



#define PORT "3490"  // the port users will be connecting to
#define MAXDATASIZE 300 // max number of bytes we can get at once 

//#define BACKLOG 2     // how many pending connections queue will hold


void init_time(struct timeval *start_time) {
//time_t start_time;    
gettimeofday(start_time, NULL);
}

long long int get_time(struct timeval start_time) {
struct timeval t;
gettimeofday(&t, NULL);
return (long long int) (t.tv_sec - start_time.tv_sec) * 1000000
+ (t.tv_usec - 
start_time.tv_usec);
}

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int decideBucketPosition(int elementoAtual,int numProcessos);
void splitArrayElementsOnBuckets(int *vetor,int bucket[300][300], int numProcessos, int posicao[16],int numFuncoes);
void itoa(int value, char* str, int base);
void strreverse(char* begin, char* end);

int main(int argc, char *argv[])
{ int status; //for waiting children processes
    //For File management
    FILE *fr; 
    
    //For loops
    int i;
    
    //For time measurement
    struct timeval start_time;
      init_time(&start_time);

      //time
      int long long start1,end1; //This pair measures how long it takes to do get the data ready, but it halts measuring
      //before it starts waiting for the connection
    
      
      start1 = get_time(start_time);   
      
    
    if(argc != 2)
    {
        printf("invalid amount of parameters, input: ./a.out 2\n");
        exit(0);
    }
        
    int BACKLOG = atoi(argv[1]);
    if(BACKLOG <=1)
    {
        printf("invalid number of hosts. min of 2\n");
        exit(0);
    }
      int long long startEnd[BACKLOG][2]; //For every backlog, we shall take a start end pair that starts measuring time
      //as soon as it accepts the connection and ends as soon as it ends the connection to the host
      //this way time measurement will ignory how long human reactions takes to obtain the time
    
    //I need to start reading the functions from file, here I go..!


    if(!(fr=fopen("randomInput.txt", "rt")))  /* open the file for reading */
    {
        printf("Error! Couldn't open file!\n");
      	exit(1);	
    }
    int numFuncoes,numProcessos=BACKLOG;
    char numElemen[100];

    fgets(numElemen, sizeof(numElemen), fr);
    numFuncoes = atoi(numElemen);
    char line[numFuncoes][80];
    int unsort[numFuncoes];
    int bucket[300][300];
    int j=0;
    while( (fgets(line[j], sizeof(numElemen), fr) != NULL) && j < numFuncoes ) j++;

    fclose(fr);  /* close the file prior to exiting the routine */


    //		for(i=0;i<numFuncoes;i++)
    //			printf("Posicoes no vetor: %s\n",line[i]);
    for(i=0;i<numFuncoes;i++)
        unsort[i] = atoi(line[i]);
    //  		for(i=0;i<numFuncoes;i++)
    //			printf("%d\n",unsort[i]);

    		//para saber o num de elementos de cada bucket
    int numElemenBucket[numProcessos];
    splitArrayElementsOnBuckets(unsort,bucket,numProcessos,numElemenBucket,numFuncoes);
    
    
    //Up to this point I should have all my buckets ready to be sent to the host computers
    //This will print the bucket
    
    /*
    for(i=0;i<numProcessos;i++)
    {
        for(j=0;j<numElemenBucket[i];j++)
            printf("bucket[%d][%d] = %d\n",i,j,bucket[i][j]);
    }
    */
    
    
   // exit(0);
    
    
    //Below this point is totally related to estabilish a connection between server and client
    
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    //I'll use memset to make sure the struct is empyt
    memset(&hints, 0, sizeof hints);
    
    //I won't specify its IPv4 or IPv6 -- increases portability
    hints.ai_family = AF_UNSPEC;
    
    //TCP stream sockets -- connection oriented, no loss of data, guaranteed order, etc is necessary here
    hints.ai_socktype = SOCK_STREAM;
    
    //Function that will be called later will fill my IP at the appropriated structure, I won't hardcode it!
    hints.ai_flags = AI_PASSIVE;

    //This method fills in the serverinfo structure using the hints, serverinfo includes a lot of info!
    
    /**
    struct addrinfo {
        int              ai_flags;     // AI_PASSIVE, AI_CANONNAME, etc.
        int              ai_family;    // AF_INET, AF_INET6, AF_UNSPEC
        int              ai_socktype;  // SOCK_STREAM, SOCK_DGRAM
        int              ai_protocol;  // use 0 for "any"
        size_t           ai_addrlen;   // size of ai_addr in bytes
        struct sockaddr *ai_addr;      // struct sockaddr_in or _in6
        char            *ai_canonname; // full canonical hostname

        struct addrinfo *ai_next;      // linked list, next node
    };
    
    */
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    //The  getaddrinfo returns a linked list with all the results of the data. We run this list looking for the proper server info.
    //Once we find it, we create a socket in this case tha is AF_UNSPEC, SOCK_STREAM (uses TCP) and the appropriated protocol.
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        //Just in case I closed the connection and it complains the connection is already in use, I'll just let my application
        //knows that I may reuse that port for other data. This ocorred at the python code when I'd suddently close the tic-tac-toe
        //code. It won't occur here thanks to this, however :-)
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        
        //Since my generic socket is already set or being reused, and I know the necessary information about it
        //now I'll give it a port that will be used so that the clients know which process will be listening and waiting for them!
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    //This is the case where while looking for the information obtained by getaddrinfo nothing was found on the servinfo linked list!
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }

    //I've already created the socket and bind it, so the serverinfo information is no longer necessary, get ride of it!
    freeaddrinfo(servinfo); // all done with this structure

    //Now as the server I'll listen (wait for other hosts). Processes who try to estabilish a connection with me will be added
    //to a queue of received connection requests. The number of hosts waiting in this queue is defined as an int on the BACKLOG
    //define that I've set on the start of this code!
    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    //I'll fork processes that will deal with different hosts. That being said, I'll obviously be a murderer and kill a lot of them!
    //Well, I don't want zombies messing up my stuff or my sysadmin complaining, so I'll get ride of them now!
    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    //All set, let the guy who is running me that I'm ready to wait for other hosts!
    //printf("server: waiting for connections...\n"); 
    int bucketShard = 0;
    //I accept all connections in this loop and deal with them1
    
    end1 = get_time(start_time); 
    
    printf("\tpid\ttime_in_ms\n");
    
    while(bucketShard<BACKLOG) {  // main accept() loop 
        sin_size = sizeof their_addr;
        
        //Everytime I accept a connection, a new file description (remember! everything on C is a file descriptor)
        //no matter if it is for network connection, to open I/O Files or w/e is created and will be use to send and 
        //receive messages from the host on that connection. The original socket created before will still be listening
        //to other hosts based on the quantity of the backlog. 
        //Since theyre all file descriptors use open and close to them! 
        //(well there are better way to get things working around with sockets, check source of this code)
        
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        
        //printf("The current shard being sent is the bucketshard[%d]\n",bucketShard);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        //inet_ntop, inet n(network)to p(presentation), converts the ip strucutures to the 255.255.255.255 readable format!
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        //printf("server: got connection from %s\n", s);

        //As the server, the main process will keep listening, and the children process will deal with each host. 
        if (!fork()) { // this is the child process
            startEnd[bucketShard][0] = get_time(start_time);   
            close(sockfd); // child doesn't need the listener
            
            //Start of test of sending integers
//            write(new_fd,&numElemenBucket[0],sizeof(numElemenBucket[0])); //Send amount of elements of this bucket

            int32_t bucketAux[300][300];
            for(i=0; i<numElemenBucket[bucketShard]; i++)
               bucketAux[bucketShard][i] = htonl((int32_t)(bucket[bucketShard][i]));
               
           // for(i=0; i<numElemenBucket[bucketShard]; i++)
             //   printf("valor em int antes da conversao: %d\n",bucket[bucketShard][i]);
            
            //End of test of sending integers
            
            
            if (send(new_fd,bucketAux[bucketShard],sizeof(int32_t)*numElemenBucket[bucketShard], 0) == -1)
                perror("send");
                
                
            int numbytes;   
            //At this point all hosts should have requested the bucket data and started sorting it. 
            //Now each process will wait for the data to be sent back to them:
            if ((numbytes = recv(new_fd, bucketAux[bucketShard], MAXDATASIZE-1, 0)) == -1) {
                perror("recv");
                
                exit(1);
            }
            
            
            //Convert back to this computer architecture for portability
            for(i=0;i<(numbytes/sizeof(int32_t));i++)
                  bucketAux[bucketShard][i] = ntohl(bucketAux[bucketShard][i]);
            
            
              //printf("Im the server child process in charge of receiving the bucket shard %d.\n",bucketShard);
              
              char convertor[100];

              int aux;
/*
              for(i=0;i<(numbytes/sizeof(int32_t));i++)
              {   aux  = (int)bucketAux[bucketShard][i];
                  itoa(aux,convertor,10);
                  printf("%s\n",convertor);
              }
 */           
              startEnd[bucketShard][1] = get_time(start_time);

            //number of children process, time taken to collect data
            printf("\t%d\t%f\n",bucketShard,(float)(startEnd[bucketShard][1]-startEnd[bucketShard][0])/1000);    
            close(new_fd);
            exit(0);
        }
        close(new_fd);  // parent doesn't need this
        
        bucketShard++; //Everytime the master forks, it increases the bucket that will be sent
        //Do notice that the max number of buckets is always the same amount of hosts that the server will wait 
        //to receive a message. That way, each host always gets a bucket shard, nothing less and nothing more.
    }
    //Parent process should not finish before children process received all the bucketsort data.
    int waitOnForkChildren = BACKLOG;
    while(waitOnForkChildren > 0)
    {
        wait(&status);
        waitOnForkChildren--;
    }
    
    //printf("fin\n");

  
    
    //Time nhosts, sizefile,time in miliseconds
    printf("\n"); //just to separate from each children process measurement
    printf("\tn_hosts\tn_elem\ttime_in_ms\n");
    printf("\t%d\t%d\t%f\n",BACKLOG,numFuncoes,(float)(end1-start1)/1000);
    
    return 0;
}


void splitArrayElementsOnBuckets(int *vetor,int bucket[300][300], int numProcessos, int posicao[16],int numFuncoes)
{int i,j;
	int posicaoBucket;
	
	for(i=0;i<numProcessos;i++)
		posicao[i] = 0;
	
    for(i = 0; i<numFuncoes;i++)
	{
		posicaoBucket = decideBucketPosition(vetor[i],numProcessos);
		bucket[posicaoBucket][posicao[posicaoBucket]] = vetor[i];
		posicao[posicaoBucket]++;
	}
}

int decideBucketPosition(int elementoAtual,int numProcessos)
{
	int i=0;
	    if(elementoAtual == 0)
	        return numProcessos/2;

	    int elementoAtualEmModulo = elementoAtual;

	    if(elementoAtualEmModulo<0)
	        elementoAtualEmModulo = elementoAtualEmModulo*-1;

	    for(;i<(numProcessos/2);i++)
	    {
	        if(elementoAtualEmModulo < ((5000/((float)numProcessos/2))*(i+1)))
	        {
	            if(elementoAtual < 0)
	                return((numProcessos/2) - (i+1));
	            else
	                return((numProcessos/2) + i);
	        }
	    }
	    if(elementoAtual < 0)
	        return (numProcessos/2 - i); 
	    else
	        return((numProcessos/2) + i-1); //So ocorre quando elemento igual a 5000	
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
void strreverse(char* begin, char* end) {
	
	char aux;
	
	while(end>begin)
	
		aux=*end, *end--=*begin, *begin++=aux;
	
}
