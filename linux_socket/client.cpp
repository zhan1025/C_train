#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <pthread.h>
#include <time.h>
#define BUF_SIZE 100
using namespace std;

char name[30];//    设置支持的用户名字
int sock;
const char *ip = "10.8.10.125";
short port = 1234;
time_t nowtime;

void init(){
//向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充

    serv_addr.sin_family = AF_INET;  //使用IPv4地址

    serv_addr.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址

    serv_addr.sin_port = htons(port);  //端口
    //创建套接字
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("无法连接到服务器");
        exit(-1);
    }
    else
    {
        cout<< "客户端启动成功"<<endl;
    }
};

void start(){
    pthread_t id;
    void *recv_thread(void*);
    //  创建一个线程用于数据接收，一个用于数据发送
    pthread_create(&id,0,recv_thread,0);
    char bufSend[BUF_SIZE] = {0};

    sprintf(bufSend,"%s进入了群聊",name);
    time(&nowtime);

    cout<<"----------"<<ctime(&nowtime)<<"----------"<<endl;

    write(sock, bufSend, strlen(bufSend));

    while(1){
        char buf[100]={};
        cin>>buf;
        char msg[100]={};

        sprintf(msg,"%s发送信息:\n%s",name,buf);

        write(sock,msg,strlen(msg));
        if(strcmp(buf,"quit") == 0){
            memset(bufSend, 0, BUF_SIZE);// 从新分配buf内存，初始化
            sprintf(bufSend,"%s退出了群聊",name);
            write(sock, bufSend, strlen(bufSend));
            break;
        }
    }
    close(sock);
};


void* recv_thread(void* p){

    while(1){

        char buf[100] = {};

        if (read(sock,buf,sizeof(buf)) <= 0){

            break;

        }

        printf("%s\n",buf);

    }

}

int main(){
    
    init();

    printf("请输入用户名：");

    scanf("%s",name);

    printf("\n\n*****************************\n");

    printf("欢迎%s 进入群聊\n",name);

    printf("  输入quit 退出\n");

    printf("\n*****************************\n\n");

    start();
    

    
//     char bufRecv[BUF_SIZE] = {0};
// while(1){
    
//     connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//     printf("Input you message:  ");

//     cin >> bufSend;

//     write(sock, bufSend, strlen(bufSend));

//     //读取服务器传回的数据
//     read(sock, bufRecv, sizeof(bufRecv)-1);

//     printf("Message form server: %s\n", bufRecv);
    
//     memset(bufRecv, 0, BUF_SIZE);
//     close(sock);
//     }

    //关闭套接字
    return 0;
}
