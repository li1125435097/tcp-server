#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

// 单客户端连接tcp服务器
int main() {
    /*
        创建tcp ipv4 socket
        协议簇   AF_INET:ipv4    AF_INET6:ipv6
        类型     SOCK_STREAM:tcp SOCK_DGRAM:udp
        协议     0:默认值
    */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        return -1;
    }

    // 构造监听地址机构体
    sockaddr_in server_addr;
    // 清空结构体
    memset(&server_addr, 0, sizeof(server_addr));
    // 设置地址ipv4
    server_addr.sin_family = AF_INET;
    // 监听所有接口 0.0.0.0
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // 16位端口号转网络字节序
    server_addr.sin_port = htons(6667);

    // socket绑定监听地址
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind failed");
        close(sockfd);
        return -1;
    }

    // 监听连接请求，请求队列长度为10
    if (listen(sockfd, 10) == -1) {
        perror("Listen failed");
        close(sockfd);
        return -1;
    }

    // 输出日志
    std::cout << "Server listening on port 6666..." << std::endl;

    // 创建客户端地址结构体
    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // 请求次数统计
    int num = 0;

    // 循环阻塞式等待客户端连接，每个连接生成新socket
    while(1){
        int new_sock = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
        if (new_sock == -1) {
            perror("Accept failed");
            close(sockfd);
            return -1;
        }

        // 输出客户端地址
        std::cout << "Connection accepted from " << inet_ntoa(client_addr.sin_addr) << std::endl;
        std::cout << "request num:" << ++num << std::endl;

        // 创建缓冲区，等待请求数据
        char buf[1024];
        memset(buf,0,sizeof(buf));
        // read(new_sock,buf,sizeof(buf));
        // std::cout << buf << std::endl;
        
        // 长等待请求数据
        bool flag = true;
        while(flag){
            ssize_t chunk = read(new_sock,buf,sizeof(buf));
            std::cout << buf << std::endl;
            if(chunk == 2) flag = false;
        }

        // 响应客服端字符串
        const char *message = "Hello, Client!";
        ssize_t  bytes_sent = send(new_sock, message, strlen(message), 0);
        if(bytes_sent == -1){
            perror("Send failed");
        }

        // 关闭客户端socket
        close(new_sock);
    }

    // 关闭服务器socket
    close(sockfd);
    return 0;
}
