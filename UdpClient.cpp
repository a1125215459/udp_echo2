#include <iostream>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;
#define MAXSIZE 1024
// #define CHECK_RET(q) if((q)==false){cout<<"Error: "<<errno<<endl;}

int main(int argc, char *argv[])
{
    int port = 12345;
    if(argc != 2)
    {
        cout << "Input ./UdpClient IP" << endl;
        return -1;
    }

    int ret;
    int flag = 0;
    char rec[MAXSIZE];
    char send[MAXSIZE];
    // uint16_t Srv_Port = stoi(argv[2]);
    struct sockaddr_in Srv_Addr;
    struct sockaddr_in Peer_Addr;
    memset(&Srv_Addr, 0, sizeof(Srv_Addr));
    memset(&Peer_Addr, 0, sizeof(Peer_Addr));
    Srv_Addr.sin_family = AF_INET;
    // Srv_Addr.sin_port = htons(Srv_Port);
    Srv_Addr.sin_port = htons(port);
    Srv_Addr.sin_addr.s_addr = inet_addr(argv[1]);
    socklen_t Srv_Addr_len = sizeof(Srv_Addr);
    socklen_t Peer_Addr_len = sizeof(Peer_Addr);
    
    int sockfd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if(sockfd == -1)
    {
        cout << "socket error :" << errno << endl;
    }
    else
    {
        cout << "socket succ" << endl;
    }


    while(1)
    {
        memset(rec, 0, sizeof(rec));
        memset(send, 0, sizeof(send));
        cout << "send to server: " << endl;
        fgets(send, MAXSIZE, stdin);
        if(send[0] == 'q')
        {
            cout << "connection closed" << endl;
            close(sockfd);
            return 0;
        }
        if(flag == 0)
        {
            ret = sendto(sockfd, send, sizeof(send), 0, (struct sockaddr*)&Srv_Addr, Srv_Addr_len);
            if(ret > 0)
            {
                cout << "send to port " << ntohs(Srv_Addr.sin_port) << " data: " << send;
                // cout << "send data: " <<  send << "to server succ" << endl;
            }
            else
            {
                cout << "send ret: " << ret << endl;
            }
        }
        else
        {
            cout << "this is a new socket" << endl;
            ret = sendto(sockfd, send, sizeof(send), 0, (struct sockaddr*)&Peer_Addr, Peer_Addr_len);
            if(ret > 0)
            {
                cout << "send to port " << ntohs(Peer_Addr.sin_port) << " data: " << send;
                // cout << "send to port " << ntohs(Srv_Addr.sin_port) << " data: " << send;
                // cout << "send data: " <<  send << "to server succ" << endl;
            }
            else
            {
                cout << "send ret: " << ret << endl;
            }
        }

        ret = recvfrom(sockfd, rec, MAXSIZE, 0, (struct sockaddr*)&Peer_Addr, &Peer_Addr_len);
        if(ret > 0)
        {
            if(flag == 0)
            {
                flag = 1;
            }
            // cout << "recvfrom succ" << endl;
            // cout << "Client Ip: " <<   << " port: " <<   << endl;
            cout << "recieve from server: " << rec << endl;
        }
        else
        {
            cout << "rec ret: " << ret << endl;
        }
    }
    
    close(sockfd);
    return 0;
}