#include <stdio.h>  
#include <string.h>  
#include <stdlib.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/un.h>  
#include <arpa/inet.h>  
#include <netinet/in.h>  
  
const int port = 8000;  //监听的端口
const char* ip = "192.168.252.128";   //本地地址
  
int main()  
{  
      //创建套接字,即创建socket   
      int ser_sock = socket(AF_INET, SOCK_STREAM, 0);   
      if(ser_sock < 0)  
      {  
          //创建失败  
          perror("socket");  
          return 1;  
      }  
  
      //绑定信息，即命名socket   
      struct sockaddr_in addr; //sockaddr_in是internet环境下套接字的地址形式，用来建立所需的信息

      addr.sin_family = AF_INET;   // 使用IPv4 网络协议进行通信
      addr.sin_port = htons(port);  // 将端口号整型变量从主机字节顺序转变成网络字节顺序
      addr.sin_addr.s_addr = inet_addr(ip); //inet_addr函数将用点分十进制字符串表示的IPv4地址转化为用网络字节序整数表示的IPv4地址     
	  // bind(int sockfd, const struct sockaddr *addr,socklen_t *addrlen);
	  //调用bind()函数之后，为socket()函数创建的套接字关联一个相应地址，发送到这个地址的数据可以通过该套接字读取与使用
      if(bind(ser_sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)   
      {                //将sockaddr_in强制转换为sockaddr
           //命名失败   
           perror("bind");   
           return 2;         
      }   
  
      //创建监听套接口  监听队列长度为5  
      int listen_sock = listen(ser_sock, 5);  
      if(listen_sock < 0)  
      {  
          //监听失败  
          perror("listen");  
          return 3;  
      }  
  
      //接受连接  
      //系统调用从listen监听队列中接受一个连接  
      //int accept(int sockfd, struct sockaddr* addr, socklen_t* addrlen)  
      //sockfd参数是执行过listen系统调用的监听socket；addr参数用来获取被  
      //接受连接的远端socket地址，该socket地址的长度由addrlen参数指出  
      //accept成功时返回一个新的连接socket，该socket唯一的标识了被接受  
      //的这个连接，服务器可通过读写该socket来与被接受连接对应的客户端通信  
      struct sockaddr_in peer;  
      socklen_t peer_len;  
      char buf[1024];  
      int accept_fd = accept(ser_sock, (struct sockaddr*)&peer, &peer_len);  
  
      if(accept_fd < 0)  //连接失败
      {  
          perror("accept");  
          return 4;  
      }  
      else  
      {  
          printf("connected with ip: %s  and port: %d\n", inet_ntop(AF_INET,&peer.sin_addr, buf, 1024), ntohs(peer.sin_port));  
          
      }  
  
      while(1)  
      {  
          memset(buf, '\0', sizeof(buf));  
		  //read函数从终端设备读，通常以行为单位，读到换行符就返回了。
		  //如果从终端输入的数据没有换行符，调用read读终端设备就会阻塞。
		  //成功返回读取的字节数，出错返回-1并设置errno，如果在调read之前已到达文件末尾，则这次read返回0
		  //第三个参数是请求读取的字节数，读上来的数据保存在缓冲区buf中，同时文件的当前读写位置向后移
          ssize_t size = read(accept_fd, buf, sizeof(buf) - 1);  
          if(size > 0)  
          {  
              printf("The information from client is: %s\n", buf);  
          }  
          else if(size == 0)  
          {  
              printf("Finish...\n");  
              break;  
          }  
          else   
          {  
              perror("read");  
              break;  
          }  
          printf("server, please enter your word: ");  
          fflush(stdout);  //fflush函数的作用：把文件流中未写出的数据全部写出去。
          size = read(0, buf, sizeof(buf) - 1);  //标准输入的文件描述符是 0
          if(size > 0)  
          {  
              buf[size - 1] = '\0';  
          }  
          else if(size == 0)  
          {  
              printf("Finish...\n");  
              break;  
          }  
          else  
          {  
              perror("read");  
              break;  
          }     
          write(accept_fd, buf, strlen(buf));  //把自己写的东西放到套接字里
      }  
        close(ser_sock);  //结束，断开连接
      return 0;  
}  
