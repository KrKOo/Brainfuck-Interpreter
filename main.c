#include "stdio.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "unistd.h"
#include "string.h"
#include <sys/socket.h>

int main()
{
    struct sockaddr_in server, client;

    //AF_INET = 2
    //SOCK_STREAM = 1 
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9090);    
    
    //This is how the (sockaddr_in)server looks like in the memory
    //char srv[16] = {0x02, 0x00, 0x23, 0x82, 0x00, 0x00, 0x00, 0x00, 0x10, 0x54, 0x55, 0x55, 0x55, 0x55, 0x00, 0x00};

    //if(bind(socket_desc, srv, sizeof(srv)) < 0)  <- this would work too
    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Bind failed");
        return 1;
    } 

    puts("Bind finished");

    listen(socket_desc, 3);

    puts("Waiting for incoming connections...");

    int new_socket;
    socklen_t c = sizeof(struct sockaddr_in);
    //char c[] = {0,0,0,16};    <- this can replace the above row
    char len = 132;
     

    while((new_socket = accept(socket_desc, &client, &c)) >= 0)
    {
        puts("Connection accepted");
        char* message = "HTTP/1.1 200 OK\n"
                        "Connection: close\n"
                        "Accept-Ranges: bytes\n"
                        "Content-Length: 15\n"
                        "Content-Type: text/html\n"
                        "\n"
                        "<h1>KrKO_o</h1>\n";
        
        char length = strlen(message); 
        write(new_socket , message , length);
    }

    if (new_socket<0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}
