#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;
#define MAXSIZE 1024
// #define Thread_MAX 10
// #define PORT 12345
int port = 12345;
typedef struct Socket_Info{
    struct sockaddr_in Srv_Addr;
    struct sockaddr_in Peer_Addr;
}Socket_Info;



void *pthread_func(void *arg)
{
    int ret;
    struct Socket_Info* info = (struct Socket_Info*)arg;
    struct sockaddr_in Peer_Addr = info->Peer_Addr;
    struct sockaddr_in Srv_Addr;
    memset(&Srv_Addr, 0, sizeof(Srv_Addr));
    Srv_Addr.sin_port = htons(++port);

    socklen_t Peer_Addr_len = sizeof(Peer_Addr);
    socklen_t Srv_Addr_len = sizeof(Srv_Addr);
    //socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd == -1)
    {
        cout << "socket2 error :" << errno << endl;
    }
    else
    {
        cout << "socket2 succ" << endl;
    }
    //bind
    ret = bind(sockfd, (struct sockaddr*)&Srv_Addr, Srv_Addr_len);
    if(ret == -1)
    {
        cout << "bind2 error: " << errno << endl;
    }
    else
    {
        cout << "bind2 succ" << endl;
    }
    char tmp[MAXSIZE] = {"pthread Begin!!!"};
    
    ret = sendto(sockfd, tmp, sizeof(tmp), 0, (struct sockaddr*)&info->Peer_Addr, Peer_Addr_len);
    if(ret > 0)
    {
        cout << "send data :" << tmp << "to client succ!" << endl;
    }
    
    int count = 0;
    while(1)
    {   
        cout << "threadid: " << pthread_self() << endl;
        count ++;
        cout << "count : " << count << endl; 
        memset(tmp, 0, MAXSIZE);
        
        ret = recvfrom(sockfd, tmp, sizeof(tmp), 0, (struct sockaddr*)&Peer_Addr, &Peer_Addr_len);
        if(ret == -1)
        {
            cout << "recvfrom error: " << errno << endl;
            continue;
        }
        else if(ret == 0)
        {
            cout << "connection closed" << endl;
            continue;
        }
        else
        {
            cout << "recieve from client: " << tmp << endl;
        }
        
        ret = sendto(sockfd, tmp, sizeof(tmp), 0, (struct sockaddr*)&Peer_Addr, Peer_Addr_len);
        if(ret > 0)
        {
            cout << "send data :" << tmp << "to client" << endl;
        }
    }
    close(sockfd);
    pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
    int ret;
    struct Socket_Info info;  
    pthread_t thread_id;
    pthread_attr_t attr;
    
    char recvbuf[MAXSIZE] = {0};
    struct sockaddr_in Srv_Addr;
    memset(&Srv_Addr, 0, sizeof(Srv_Addr));
    
    Srv_Addr.sin_family = AF_INET;
    Srv_Addr.sin_port = htons(port);
    
    Srv_Addr.sin_addr.s_addr = htonl(INADDR_ANY);
    

    //socket
    int sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(sockfd == -1)
    {
        cout << "socket error :" << errno << endl;
    }
    else
    {
        cout << "socket succ" << endl;
    }
    

    int opt = SO_REUSEADDR;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    // fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFD,0)|O_NONBLOCK);
    
    //bind
    ret = bind(sockfd, (struct sockaddr*)&Srv_Addr,sizeof(Srv_Addr));
    if(ret == -1)
    {
        cout << "bind error: " << errno << endl;
    }
    else
    {
        cout << "bind succ" << endl;
    }
    
    string Peer_Ip;
    uint16_t Peer_Port;
    string reply = "Echo: server ready to begin!!!";
    cout << reply << endl;
    while(1)
    {
        memset(recvbuf, 0, MAXSIZE);
        //recv data
       struct sockaddr_in Peer_Addr;
        Peer_Addr.sin_family = AF_INET;
        socklen_t Peer_Addr_len = sizeof(Peer_Addr);
        
        ret = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&Peer_Addr, &Peer_Addr_len);
        if(ret > 0)
        {
            // cout << "recvfrom succ" << endl;
            // cout << "Client Ip: " <<   << " port: " <<   << endl;
            cout << "recieve from client: " << recvbuf << endl;
        }
        else
        {
            // cout << "rev ret: " << ret << endl;
            continue;
        }
        
        info.Srv_Addr = Srv_Addr;
        info.Peer_Addr = Peer_Addr;
        

        pthread_attr_init(&attr);
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        
        if(pthread_create(&thread_id, &attr, pthread_func, (void*)&(info)) != 0)
        {
            cout << "create pthread error: " << errno << endl;
        }
        else
        {
            cout << "pthread_create succ" << endl;
        }
    }
    close(sockfd);
    return 0;
}