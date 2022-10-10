#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../encryption/aes.cpp"

// #include <filesystem>
#include <experimental/filesystem>

// for brevity
namespace filesystem = std::experimental::filesystem;
#include <iostream>
#include <string>

using namespace std;

#define SIZE_BUF 100
#define FILE_SEND_BUF 1024
#define PORT 8888

void recieveInput(const char *title, char str[]);
void printWarn(const char *msg);
void sendEncrypt(int server);

int main()
{
    struct sockaddr_in saddr;
    int fd, ret_val;
    struct hostent *local_host; /* need netdb.h for this */
    char message[SIZE_BUF], cmd[SIZE_BUF];

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", hstrerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Let us initialize the server address structure */
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(PORT);
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);
    /* Step2: connect to the TCP server socket */
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1) {
        fprintf(stderr, "Connect failed [%s]\n", hstrerror(errno));
        close(fd);
        return -1;
    }

    // TERIMA MSG SERVE / WAIT
    ret_val = recv(fd, message, SIZE_BUF, 0);
    // puts(message);
    while (strcmp(message, "wait") == 0) {
        printf("\e[31mServer is full!\e[0m\n");
        ret_val = recv(fd, message, SIZE_BUF, 0);
    }
    printf("\e[33mServer is now responding to you.\e[0m\n");
    int commandTrue = 0;
    while (1) {
        // sign up user
        int b = 0;
        while (!commandTrue) {
            recieveInput("\nInsert Command\n(add/delete/see/find/send_encrypt)", cmd);
            for (b = 0; b < strlen(cmd); b++) {
                cmd[b] = tolower(cmd[b]);
            }
            ret_val = send(fd, cmd, sizeof(cmd), 0);
            if (!strcmp(cmd, "send_encrypt")) {
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
    int i = 0, j = 0, from = 0, to = 10000;

    // If the file exists and has read permission
    system("echo 'List of files '; ls -la files");
    recieveInput("Insert file name", fileName);
    sprintf(filePath, "./files/%s", fileName);
    fptr1 = fopen(filePath, "r");
    if (fptr1 == NULL) {
        printWarn("Cannot open this file!");
    }

    // send file name
    if (send(server, fileName, sizeof(fileName), 0) != -1) {
        bzero(fileName, FILE_SEND_BUF);
    }

    // send file size
    unsigned long fileSize = filesystem::file_size(filePath);
    printf("file size: %lu\n", fileSize);
    send(server, to_string(fileSize).c_str(), FILE_SEND_BUF, 0);

    int counter = 0;

    AES aes = AES();
    unsigned char data_buffer[FILE_SEND_BUF];
    while (fread(data_buffer, sizeof(unsigned char), FILE_SEND_BUF, fptr1) > 0) {
        // Encrypt the data
        unsigned char *encrypted_data = (unsigned char *) malloc(FILE_SEND_BUF);
        encrypted_data = aes.encryptCBC_Continous(data_buffer, FILE_SEND_BUF);
        // Send the encrypted data
        send(server, encrypted_data, FILE_SEND_BUF, 0);
        free(encrypted_data);
    }

    fclose(fptr1);
}