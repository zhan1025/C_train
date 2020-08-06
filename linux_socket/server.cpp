#include <stdio.h>
#include <iostream>
#include <string.h>

#include <stdlib.h>

#include <unistd.h>
#include <arpa/inet.h>

#include <sys/socket.h>

#include <netinet/in.h>
//  in_addr_t

#include <pthread.h>
#include <time.h>
#define BUF_SIZE 100

using namespace std;

int serv_sock;
int client[100];        // 客户端的socketf   为请求队列的最大长度
int size = 50;          //用来控制进入聊天室的人数为50以内
const char *ip = "10.8.10.125"; //   主机ip
short port = 1234;      //    端口号
time_t nowtime;

void init()
{
    //创建套接字
    serv_sock = socket(AF_INET, SOCK_STREAM, 0); //    linux下返回文件描述符
    if (serv_sock == -1)
    {
        perror("服务端启动失败");
        exit(-1);
    }
    //将套接字和ip、端口绑定
    struct sockaddr_in serv_addr;

    memset(&serv_addr, 0, sizeof(serv_addr)); //每个字节都用0填充

    serv_addr.sin_family = AF_INET; //使用ipv4地址

    serv_addr.sin_addr.s_addr = inet_addr(ip); //具体的ip地址

    serv_addr.sin_port = htons(port); //端口

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
    {
        perror("套接字绑定失败");
        exit(-1);
    }
    if (listen(serv_sock, 100) == -1)
    { //监听最大连接数

        perror("设置监听失败");

        exit(-1);
    }
}

//  分发信息
void sendAll(char *msg)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if (client[i] != 0)
        {

            printf("发送给%d\n", client[i]);

            printf("发送的信息是: %s\n", msg);

            //  写文件
            char buf[1024];
            FILE *logs = fopen("log.txt", "a+");
            if (logs == NULL)
            {
                printf("open file erroe: \n");
            }
            else
            {
                sprintf(buf, "进入时间：%s\tip地址：%s\n", ctime(&nowtime), ip);

                fputs(buf, logs);

                sprintf(buf, "所发信息：%s\n", msg);

                fputs(buf, logs);

                fclose(logs);
            }
            write(client[i], msg, strlen(msg));
        }
    }
}

void *server_thread(void *p)
{
    int fd = *(int *)p;

    printf("pthread = %d\n", fd);
    while (1)
    {
        char buf[100] = {};

        if (read(fd, buf, sizeof(buf)) <= 0)
        {

            int i;

            for (i = 0; i < size; i++)
            {

                if (fd == client[i])
                {

                    client[i] = 0;

                    break;
                }
            }

            printf("退出：fd = %d 退出了。\n", fd);

            char buf[1024];

            FILE *logs = fopen("log.txt", "a");
            if (logs == NULL)
            {
                printf("open file error: \n");
            }
            else
            {
                sprintf(buf, "退出时间：%s\tip地址：%s\n", ctime(&nowtime),ip);
                fputs(buf, logs);
                fclose(logs);
            }

            pthread_exit(0);
        }

        //把服务器接受到的信息发给所有的客户端

        sendAll(buf);
    }
}

void server(){

    printf("服务器启动\n");

    while(1){

        struct sockaddr_in fromaddr;

        socklen_t len = sizeof(fromaddr);

        int fd = accept(serv_sock, (struct sockaddr*)&fromaddr, &len);

//调用accept进入堵塞状态，等待客户端的连接

        if (fd == -1){

            printf("客户端连接出错...\n");

            continue;

        }

        int i = 0;

        for (i = 0;i < size;i++){

            if (client[i] == 0){

                //记录客户端的socket

                client[i] = fd;

                printf("线程号= %d\n",fd);//

                //有客户端连接之后，启动线程给此客户服务

                pthread_t tid;

                pthread_create(&tid,0,server_thread,&fd);

                break;

            }

        if (size == i){

            //发送给客户端说聊天室满了

            const char* str = "对不起，聊天室已经满了!";

            write(fd,str,strlen(str));

            close(fd);

        }

        }

    }

}
int main()
{
    init();
    server();
    //     //进入监听状态，等待用户发起请求
    //     listen(serv_sock, 20);

    //     //接收客户端请求
    //     struct sockaddr_in clnt_addr;

    //     socklen_t clnt_addr_size = sizeof(clnt_addr);

    //     char str[BUF_SIZE] = {0};
    //     cout<<"socket服务已启动"<<endl;

    // while(1){
    //     int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    //     //向客户端发送数据

    //     read(clnt_sock, str, BUF_SIZE);  //接收客户端发来的数据
    //     write(clnt_sock, str, sizeof(str));

    //     memset(str, 0, BUF_SIZE);  //重置缓冲区
    //     close(clnt_sock);  //关闭套接字
    //     }

    //     //关闭套接字
    //     // close(clnt_sock);
    //     close(serv_sock);
    return 0;
}