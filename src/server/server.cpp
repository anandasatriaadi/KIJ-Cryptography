#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_CONNECTIONS 10
#define SIZE_BUF 100
#define FILE_SEND_BUF 1024
#define PORT 8000

// Pre-defined Functions
int create_tcp_server_socket();
void checkFile();
void sendEncryptCommand(int client);

int main()
{
    fd_set read_fd_set;
    struct sockaddr_in new_addr;
    int server_fd, new_fd, serving = 1;
    int ret_val, ret_val1, ret_val2, ret_val3, status_val;
    char message[SIZE_BUF], id[SIZE_BUF], password[SIZE_BUF], cmd[SIZE_BUF];
    socklen_t addrlen;
    int all_connections[MAX_CONNECTIONS];

    // Make needed files if not found.
    checkFile();

    /* Get the socket server fd */
    server_fd = create_tcp_server_socket();
    if (server_fd == -1) {
        fprintf(stderr, "[!] Failed to create a server\n");
        return -1;
    }

    /* Initialize all_connections and set the first entry to server fd */
    int i;
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        all_connections[i] = -1;
    }
    all_connections[0] = server_fd;

    printf("\e[32m[!] Server is running....\n\n");
    int userLoggedIn = 0;
    while (1) {
        FD_ZERO(&read_fd_set);
        /* Set the fd_set before passing it to the select call */
        for (i = 0; i < MAX_CONNECTIONS; i++) {
            if (all_connections[i] >= 0) {
                FD_SET(all_connections[i], &read_fd_set);
            }
        }

        /* Invoke select() and then wait! */
        ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

        /* select() woke up. Identify the fd that has events */
        if (ret_val >= 0) {
            /* Check if the fd with event is the server fd */
            if (FD_ISSET(server_fd, &read_fd_set)) {
                /* accept the new connection */
                new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
                if (new_fd >= 0) {
                    printf("\e[32m[!] New connection incoming:\e[0m\n");
                    for (i = 0; i < MAX_CONNECTIONS; i++) {
                        if (all_connections[i] < 0) {
                            all_connections[i] = new_fd;
                            printf("\e[32m    On Index: %d\e[0m\n\n", i);

                            // If the server is still serving a client, make other connections wait.
                            if (i != serving) {
                                ret_val1 = send(all_connections[i], "wait", SIZE_BUF, 0);
                            } else {
                                ret_val1 = send(all_connections[i], "serve", SIZE_BUF, 0);
                            }
                            break;
                        }
                    }
                } else {
                    fprintf(stderr, "\e[31m[!] Accept failed [%s]\e[0m\n", strerror(errno));
                }
                ret_val--;
                if (!ret_val)
                    continue;
            }

            /* Check if the fd with event is a non-server fd */
            for (i = 1; i < MAX_CONNECTIONS; i++) {
                if ((all_connections[i] > 0) &&
                    (FD_ISSET(all_connections[i], &read_fd_set))) {
                    // Receive/read command from client.
                    ret_val1 = recv(all_connections[i], cmd, sizeof(cmd), 0);
                    printf("\e[1;37mServing client FD %d [index %d]\e[0m\n", all_connections[i], i);
                    printf("Command: \e[1;37m%s\e[0m\n", cmd);

                    // Check if client terminate
                    if (ret_val1 == 0) {
                        printf("\e[31mClosing connection for FD: %d\e[0m\n", all_connections[i]);
                        close(all_connections[i]);
                        all_connections[i] = -1;

                        // After a client terminates, find the next open connection.
                        // and serve for that client.
                        while (1) {
                            // If it reaches the end of max connection.
                            if (serving == MAX_CONNECTIONS - 1) {
                                serving = 1;
                                printf("Serving for index [%d]\n", serving);
                                break;
                            }

                            // If the next connection is open
                            if (all_connections[serving + 1] != -1) {
                                serving++;
                                printf("Serving for index [%d]\n", serving);
                                break;
                            }
                            serving++;
                        }
                        if (all_connections[serving] != -1)
                            status_val = send(all_connections[serving], "serve", 100, 0);
                    }
                    if (ret_val1 > 0) {
                        if (!strcmp(cmd, "send_encrypt")) {
                            sendEncryptCommand(all_connections[serving]);
                        }
                    }
                    if (ret_val1 == -1 || ret_val2 == -1 || ret_val3 == -1) {
                        printf("recv() failed for fd: %d [%s]\n",
                               all_connections[i],
                               strerror(errno));
                        break;
                    }
                }
                ret_val1--;
                if (!ret_val1)
                    continue;
            }
        }
    }

    /* Last step: Close all the sockets */
    for (i = 0; i < MAX_CONNECTIONS; i++) {
        if (all_connections[i] > 0) {
            close(all_connections[i]);
        }
    }

    // ;
    return 0;
}

int create_tcp_server_socket()
{
    struct sockaddr_in saddr;
    int fd, ret_val;

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        fprintf(stderr, "Socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("\e[32mCreated a socket with fd: %d\e[0m\n", fd);

    /* Initialize the socket address structure */
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;

    /* Step2: Bind the socket to port 7000 on the local host */
    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val != 0) {
        fprintf(stderr, "Bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    /* Step3: Listen for incoming connections */
    ret_val = listen(fd, 5);
    if (ret_val != 0) {
        fprintf(stderr, "Listen failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}

/*
Check if files folder is exists, and make it if it doesn't.
*/
void checkFile()
{
    struct stat stats;
    stat("./files", &stats);

    if (!S_ISDIR(stats.st_mode))
        mkdir("./files", 0777);
}

/*
Receiving encrypted file
*/
void sendEncryptCommand(int client)
{
    int ret_rec;
    unsigned char data[FILE_SEND_BUF];
    unsigned char flag[FILE_SEND_BUF] = "done";
    char fileName[FILE_SEND_BUF], fileSizeStr[FILE_SEND_BUF], filePath[FILE_SEND_BUF * 2];

    // receive file name
    ret_rec = recv(client, fileName, sizeof(fileName), 0);
    printf("Name of the file: %s\n", fileName);
    sprintf(filePath, "./files/%s", fileName);

    // receive file size
    ret_rec = recv(client, fileSizeStr, FILE_SEND_BUF, 0);
    unsigned long fileSize = (unsigned long)atoi(fileSizeStr);
    printf("Size of the file: %s\n", fileSizeStr);

    // make file
    FILE *newFile = fopen(filePath, "w+");
    int counter = 0;
    while (1) {
        ret_rec = recv(client, data, FILE_SEND_BUF, 0);
        fflush(stdout);
        if (ret_rec != -1) {
            // if client is done sending data
            if (!memcmp(data, flag, FILE_SEND_BUF)) {
                printf("counter = %d\n", counter);
                break;
            }
            // for debugging purpose
            for (int k = 0; k < 16; k++) {
                fileSize--;
                if (fileSize != 0) {
                    fputc(data[k], newFile);
                    printf("%c", data[k]);
                    counter++;
                }
            }
        }
        bzero(data, FILE_SEND_BUF);
    }
    fclose(newFile);
}