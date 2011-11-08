/**

This is a sample code removed from http://net.pku.edu.cn/~course/cs501/2011/code/BSD_Socket.t/sockets.pdf

Will build my ideas based on this little guy.

**/
#include <sys/types.h>
#include <sys/socket.h>

struct sockaddr_in
{
    sa_family_t     sin_family; // = AF_INET
    in_port_t       sin_port;   //is a port number
    struct in_addr  sin_addr;   //an IP address
}

int void main(int argc, char *argv[])
{
    sockaddr_in serverAddr;
    sockaddr &serverAddrCast = (sockaddr &)serverAddr;

    //get a tcp/ip socket
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    //host IP # in dotted decimal format!
    inet_pton(AF_INET, serverName, serverAddr.sin_addr);
    serverAddr.sin_port = htons(13);;

    connect(sockFd, serverAddrCast, sizeof(serverAddr));
        // .. read and write operations on sockFd ..
    shutdown(sockFd, 2);
    close(sockFd);
}

