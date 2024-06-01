// #include"../headers/utils.h"


// #include"../headers/utils.h"
// #include "../headers/client.h"


#ifndef UTILS_H
#define UTILS_H
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<errno.h>
#include<string.h>
#include<signal.h>
#include<time.h>
#include<sys/select.h>
#include<strings.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/sem.h>
#include<stdbool.h>


#define MAX_CLIENTS 100
#define PORT 5000
#define BUF BUFSIZ
#define UBUF 100
#define memz(a) memset(a,0,sizeof(a))  // memz is a macro to zero out memory
#define pr perror
#define getl(a) scanf("%[^\n]",a)
#define pt(a) printf("%s",a)
#define check(x,msg) if(x<0){pr(msg);exit(1);}
#define ADMIN 1
#define USER 2
#define UNAUTHORISED 401
#define DUPLICATE 402
#define BAD_REQUEST 400
#define OK 200
#define NOT_ALLOWED 403
struct User
{
    char username[100];
    char password[100];
    bool isAdmin;
};

struct Book
{
    int id;
    int copies;
    char title[100];
    char author[100];
    bool valid;
};

struct user_book
{
    int id;
    char username[100];
};

#endif


#ifndef SERVER_H
#define SERVER_H

// #include"utils.h"

int authentication(int nsd);
void *connection(void *args);

int admin_portal(int nsd);
void add_new_user(int nsd);
void add_book(int nsd);
void get_all_books(int fd);
void delete_book(int nsd);
void add_more_copies(int nsd);

int client_portal(int nsd);
void issue_book(int nsd);
void get_all_issue_entry();
void get_all_issue_entry_nsd(int nsd);
void return_book(int nsd);

#define MAX_BOOKS 10000 
#define max(a,b) a>b?a:b
#endif // SERVER_H


#ifndef CLIENT_H
#define CLIENT_H

int admin_portal_client(int sd);
int user_portal_client(int sd, struct User *user);


#endif // CLIENT_H



int main() {
    struct User admin;
    memz(admin.username);
    memz(admin.password);
    pt("Enter the username of the admin:\n=>");
    scanf("%s", admin.username);
    pt("Enter the password of the admin:\n=>");
    scanf("%s", admin.password);
    struct flock lock;
    admin.isAdmin = 1;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = getpid();
    int fd = open("/home/hemang/Desktop/IMT2022098_OSProject/users.txt", O_RDWR | O_CREAT, 0666);
    check(fd, "open");
    int sz = sizeof(struct User);
    check(fcntl(fd, F_SETLKW, &lock), "fcntl");
    struct User temp;
    while (read(fd, &temp, sz)) {
        if (strcmp(temp.username, admin.username) == 0) {
            pt("ADMIN EXISTED Try Entering Again\n");

            // Logging the failed attempt
            FILE *logFile = fopen("admin_creation.log", "a");
            if (logFile != NULL) {
                time_t now = time(NULL);
                char *timestamp = ctime(&now);
                timestamp[strlen(timestamp) - 1] = '\0'; // Remove the newline character
                fprintf(logFile, "[%s] Admin creation attempt for username '%s': Failed - Admin already exists\n", timestamp, admin.username);
                fclose(logFile);
            }

            exit(0);
        }
    }
    lock.l_type = F_UNLCK;
    check(fcntl(fd, F_SETLKW, &lock), "fcntl");
    lseek(fd, 0, SEEK_END);
    lock.l_type = F_WRLCK;
    lock.l_start = lseek(fd, 0, SEEK_END);
    lock.l_len = sz;
    check(fcntl(fd, F_SETLKW, &lock), "fcntl");
    write(fd, &admin, sz);
    lock.l_type = F_UNLCK;
    check(fcntl(fd, F_SETLKW, &lock), "fcntl");
    close(fd);

    // Logging the successful attempt
    FILE *logFile = fopen("admin_creation.log", "a");
    if (logFile != NULL) {
        time_t now = time(NULL);
        char *timestamp = ctime(&now);
        timestamp[strlen(timestamp) - 1] = '\0'; // Remove the newline character
        fprintf(logFile, "[%s] Admin creation attempt for username '%s': SUCCESS\n", timestamp, admin.username);
        fclose(logFile);
    }
}