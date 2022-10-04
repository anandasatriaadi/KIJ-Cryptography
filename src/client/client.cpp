#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <filesystem>
#include <string>
#include <iostream>

using namespace std;

#define SIZE_BUF 100
#define FILE_SEND_BUF 1024

int handleRegLog(int server, char cmd[]);
void recieveInput(const char *title, char str[]);
void printWarn(const char *msg);
void downloadBook(int server);
void deleteBook(int server);
void sendFile(int server);
void getBookList(int server);
void findBook(int server);
void sendEncrypt(int server);

int main()
{
    struct sockaddr_in saddr;
    int fd, ret_val;
    struct hostent *local_host; /* need netdb.h for this */
    char message[SIZE_BUF], cmd[SIZE_BUF];

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1)
    {
        fprintf(stderr, "socket failed [%s]\n", hstrerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Let us initialize the server address structure */
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8000);
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);
    /* Step2: connect to the TCP server socket */
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1)
    {
        fprintf(stderr, "Connect failed [%s]\n", hstrerror(errno));
        close(fd);
        return -1;
    }

    // TERIMA MSG SERVE / WAIT
    ret_val = recv(fd, message, SIZE_BUF, 0);
    // puts(message);
    while (strcmp(message, "wait") == 0)
    {
        printf("\e[31mServer is full!\e[0m\n");
        ret_val = recv(fd, message, SIZE_BUF, 0);
    }
    printf("\e[33mServer is now responding to you.\e[0m\n");
    int commandTrue = 0;
    while (1)
    {
        // sign up user
        int b = 0;
        while (!commandTrue)
        {
            recieveInput("\nInsert Command\n(add/delete/see/find/send_encrypt)", cmd);
            for (b = 0; b < strlen(cmd); b++)
            {
                cmd[b] = tolower(cmd[b]);
            }
            ret_val = send(fd, cmd, sizeof(cmd), 0);
            if (!strcmp(cmd, "send_encrypt"))
            {
                sendEncrypt(fd);
            }
        }

        sleep(2);
        if (commandTrue)
            break;
    }
    printf("\e[31mDisconnected from server.\e[0m\n\n");

    /* Last step: close the socket */
    close(fd);

    // sendEncrypt(1);
    return 0;
}

void recieveInput(const char *title, char str[])
{
    printf("\e[0m%s\n> \e[36m", title);
    scanf("%s", str);
    str[strlen(str)] = '\0';
    printf("\e[0m");
}

void printWarn(const char *msg)
{
    printf("\e[31m%s\e[0m\n", msg);
}

#define MAX 1000
void sendEncrypt(int server)
{
    // Pointer to the file to be read from
    FILE *fptr1;

    // for reading bytes from the file
    int fgetcReturn;
    unsigned char c;

    unsigned char buffer[FILE_SEND_BUF];
    unsigned char flag[FILE_SEND_BUF] = "done";
    char fileName[FILE_SEND_BUF], filePath[FILE_SEND_BUF * 2];
    char fileExt[4];

    // Stores the bytes to read
    char str[MAX];
    int i = 0, j = 0, from = 0, to = 10000;

    // If the file exists and has read permission
    recieveInput("Insert file name", fileName);
    sprintf(filePath, "./files/%s", fileName);
    fptr1 = fopen(filePath, "r");
    if (fptr1 == NULL)
    {
        printf("cannot open this file\n");
    }

    // send file name
    if (send(server, fileName, sizeof(fileName), 0) != -1)
    {
        bzero(fileName, FILE_SEND_BUF);
    }

    // send file size
    unsigned long fileSize = filesystem::file_size(filePath);
    printf("file size: %lu\n", fileSize);
    send(server, to_string(fileSize).c_str(), FILE_SEND_BUF, 0);

    int counter = 0;
    while (1)
    {
        // Get the characters in bytes
        fgetcReturn = fgetc(fptr1);
        if (fgetcReturn == EOF)
        {
            // for debugging pupose
            for (int k = 0; k < 16; k++)
            {
                counter++;
                // printf("%2X ", buffer[k]);
            }
            // printf("\n");

            // send remaining bytes
            if (send(server, buffer, sizeof(buffer), 0) != -1)
            {
                bzero(buffer, FILE_SEND_BUF);
            }
            break;
        }

        c = fgetcReturn;
        buffer[j] = c;
        j++;
        // send per 16 bytes
        if (j % 16 == 0)
        {
            // for debugging pupose
            for (int k = 0; k < 16; k++)
            {
                counter++;
                // printf("%2X ", buffer[k]);
            }
            // printf("\n");

            // send the bytes
            j = 0;
            if (send(server, buffer, sizeof(buffer), 0) != -1)
            {
                bzero(buffer, FILE_SEND_BUF);
            }
        }

        i++;
    };

    // print size of the bytes
    // printf("counter = %d\n", counter);

    // send finish flag
    send(server, flag, FILE_SEND_BUF, 0);

    // Close the file
    fclose(fptr1);
}