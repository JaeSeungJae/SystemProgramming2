#define _GNU_SOURCE
///////////////////////////////////////////////////////////////////////////////////////////
// File name  : 2018202018_adv_server.c                                                  //
// Date      : 2023/05/10                                                                //
// Os         : Ubuntu 16.04 LTS 64bits                                                  //
// Authors    : Yu Seung Jae                                                             //
// Student ID : 2018202018                                                               //
//---------------------------------------------------------------------------------------//
// Title : System Programming Assignment #2-3                                            //
// Description :                                                                         //
// 저번주의 web_server에 추가적으로 원하지 않는 ip의 접속을 막고,접속 기록을 주기적으로 출력해주는 프로그램 //
///////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h> // socket, bind, listen...
#include <netinet/in.h> // htonl, htons, ntohl, ntohs, inet_addr
#include <arpa/inet.h> // inet_addr
#include <unistd.h>
#include <stdio.h>
#include <dirent.h> // using DIR, struct dirent
#include <stdlib.h>
#include <pwd.h> // struct passwd
#include <grp.h> // struct group
#include <sys/stat.h> // struct stat
#include <time.h> // struct tm
#include <fnmatch.h> // fnmatch
#include <fcntl.h> // GNU SOURCE
#include <signal.h>
#include <sys/wait.h>

#define URL_LEN 256
#define BUFSIZE 1000000
#define PORTNO 40000

char response_message[BUFSIZE] = {0, }; // global
int client_fd, socket_fd, file_fd; // global (file descriptor)
char cli_address[1024];
int Port_NUM, pid; // connection list data
struct tm *time_; // connection list data
int No = 0; // connection list data
struct Point { // connection list의 정보를 담기 위한 struct
   int s_No, s_Port_NUM, s_pid;
   char s_cli_address[1024];
   int s_wday, s_mon, s_mday, s_hour, s_min, s_sec, s_year;
   time_t s_time;
};
struct Point request_client[1024];

void file_check(struct stat buf){
   char f;
   if (S_ISDIR(buf.st_mode)){ // directory
      f = 'd';
   }
   else if (S_ISLNK(buf.st_mode)){ // link
      f = 'l';
   }
   else if (S_ISCHR(buf.st_mode)){ // character special
      f = 'c';
   }
   else if (S_ISBLK(buf.st_mode)){ // block special
      f = 'b';
   }
   else if (S_ISSOCK(buf.st_mode)){ // socket
      f = 's';
   }
   else if (S_ISFIFO(buf.st_mode)){ // FIFO
      f = 'P';
   }
   else if (S_ISREG(buf.st_mode)){ // regular
      f = '-';
   }
   sprintf(response_message,"%s<td>%c", response_message, f);
   return;
}
   ///////////////////////////////////////////////////////////////////////////////////////////
   // file_check_                                                          //
   // ======================================================================================//
   // Input : struct stat -> to use st_mode                                     //
   // Output : char -> 'd' = directory, 'l' = symbolic link, ... 'n' = no exist file          //
   // Purpose : checking file type and return                                     //
   ///////////////////////////////////////////////////////////////////////////////////////////
char file_check_(struct stat buf){
   char f;
   if (S_ISDIR(buf.st_mode)){ // directory
      f = 'd';
   }
   else if (S_ISLNK(buf.st_mode)){ // link
      f = 'l';
   }
   else if (S_ISCHR(buf.st_mode)){ // character special
      f = 'c';
   }
   else if (S_ISBLK(buf.st_mode)){ // block special
      f = 'b';
   }
   else if (S_ISSOCK(buf.st_mode)){ // socket
      f = 's';
   }
   else if (S_ISFIFO(buf.st_mode)){  // FIFO
      f = 'P';
   }
   else if (S_ISREG(buf.st_mode)){ // regular
      f = '-';
   }
   else { // no exist file
      f = 'n';
   }
   return f;
}
   ///////////////////////////////////////////////////////////////////////////////////////////
   // file_permission                                                       //
   // ======================================================================================//
   // Input : struct stat -> to use st_mode                                     //
   // Output : void                                                       //
   // Purpose : checking file permission                                        //
   ///////////////////////////////////////////////////////////////////////////////////////////
void file_permission(struct stat buf){
   char perm[12];
   if (buf.st_mode & S_IRUSR){ // whether read permission to user is or not
      perm[0] = 'r';
   }
   else {
      perm[0] = '-';
   }
   if (buf.st_mode & S_IWUSR) { // whether write permission to user is or not
      perm[1] = 'w';
   }
   else {
      perm[1] = '-';
   }
   if (buf.st_mode & S_IXUSR) { // whether execute permission to user is or not
      perm[2] = 'x';   
   }
   else {
      perm[2] = '-';
   }
   if (buf.st_mode & S_IRGRP) { // whether read permission to group is or not
      perm[3] = 'r';
   }
   else {
      perm[3] = '-';
   }
   if (buf.st_mode & S_IWGRP) { // whether write permission to group is or not
      perm[4] = 'w';
   }
   else {
      perm[4] = '-';
   }
   if (buf.st_mode & S_IXGRP) { // whether execute permission to group is or not
      perm[5] = 'x';
   }
   else {
      perm[5] = '-';
   }
   if (buf.st_mode & S_IROTH) { // whether read permission to others is or not
      perm[6] = 'r';
   }
   else {
      perm[6] = '-';
   }
   if (buf.st_mode & S_IWOTH) { // whether write permission to others is or not
      perm[7] = 'w';
   }
   else {
      perm[7] = '-';
   }
   if (buf.st_mode & S_IXOTH) { // whether execute permission to others is or not
      perm[8] = 'x';
   }
   else {
      perm[8] = '-';
   }
   for (int i = 0; i < 9; i++){ // print permission 
      sprintf(response_message, "%s%c", response_message, perm[i]);
   }
   sprintf(response_message, "%s</td>", response_message);
   sprintf(response_message, "%s&nbsp&nbsp\n", response_message);
   return;
}
   ///////////////////////////////////////////////////////////////////////////////////////////
   // file_check                                                          //
   // ======================================================================================//
   // Input : struct stat -> to use st_nlink                                     //
   // Output : void                                                       //
   // Purpose : checking how many are links of file                               //
   ///////////////////////////////////////////////////////////////////////////////////////////
void file_link(struct stat buf){
   unsigned long link = 0;
   link = buf.st_nlink; // the number of link
   sprintf(response_message, "%s<td>%lu&nbsp&nbsp</td>", response_message, link);
   return;
}
   ///////////////////////////////////////////////////////////////////////////////////////////
   // file_owner_name                                                       //
   // ======================================================================================//
   // Input : struct stat -> to use st_uid                                         //
   // Output : void                                                       //
   // Purpose : checking file user id                                           //
   ///////////////////////////////////////////////////////////////////////////////////////////
void file_owner_name(struct stat buf){
   char* owner_name;
   struct passwd *o; // struct passwd to get pw_name
   o = getpwuid(buf.st_uid);
   owner_name = o->pw_name;
   sprintf(response_message, "%s<td>%s&nbsp&nbsp</td>", response_message, owner_name); 
   return;
}
   ///////////////////////////////////////////////////////////////////////////////////////////
   // file_group_name                                                       //
   // ======================================================================================//
   // Input : struct stat -> to use st_gid                                         //
   // Output : void                                                       //
   // Purpose : checking file group id                                           //
   ///////////////////////////////////////////////////////////////////////////////////////////
void file_group_name(struct stat buf){
   char* group_name;
   struct group *o; // struct group to get gr_name
   o = getgrgid(buf.st_gid);
   group_name = o->gr_name;
   sprintf(response_message, "%s<td>%s&nbsp&nbsp</td>",response_message, group_name);
   return;
}
   ///////////////////////////////////////////////////////////////////////////////////////////
   // file_size                                                          //
   // ======================================================================================//
   // Input : struct stat -> to use st_size                                     //
   // Output : void                                                       //
   // Purpose : checking file size                                              //
   ///////////////////////////////////////////////////////////////////////////////////////////

void file_size(struct stat buf){
   off_t size = 0;
   size = buf.st_size; //file size
   sprintf(response_message, "%s<td>%ld&nbsp&nbsp&nbsp</td>", response_message, size);
   return;
}
   ///////////////////////////////////////////////////////////////////////////////////////////
   // file_time                                                          //
   // ======================================================================================//
   // Input : struct stat -> to use st_mtime                                     //
   // Output : void                                                       //
   // Purpose : checking file's modified time                                     //
   ///////////////////////////////////////////////////////////////////////////////////////////
void file_time(struct stat buf){
   char* Mon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; // 0 ~ 11
   struct tm *o; // struct tm to get st_mtime
   const time_t *mtime = &buf.st_mtime;
   o = localtime(mtime);
   int Mon_num = o->tm_mon;
   int day = o->tm_mday;
   int hour = o->tm_hour;
   int min = o->tm_min;
   sprintf(response_message, "%s<td>%s&nbsp&nbsp%d&nbsp%d:%d&nbsp&nbsp</td>", response_message, Mon[Mon_num], day, hour, min); 
   return;
}
   ///////////////////////////////////////////////////////////////////////////////////////////
   // print_ls                                                               //
   // ======================================================================================//
   // Input : struct stat -> checking file information                                //
   // Output : void                                                       //
   // Purpose : print file informations                                        //
   ///////////////////////////////////////////////////////////////////////////////////////////
void print_ls(struct stat buf){ // select file stat and print file information using file stat
   file_check(buf);
   file_permission(buf);
   file_link(buf);
   file_owner_name(buf);
   file_group_name(buf);
   file_size(buf);
   file_time(buf);
   return;
}
///////////////////////////////////////////////////////////////////////////////////////////
// SignalHandler                                                                         //
// ======================================================================================//
// Input : int Signal                                                                    //
// Output : void                                                                         //
// Purpose : SIGALRM이 발생했을 때 취해야 할 실행을 처리                                        //
///////////////////////////////////////////////////////////////////////////////////////////
void SignalHandler(int Sig) {
   char* Mon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; // Month
   char* Wday[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"}; // the day of week
   printf("=============== Connection History ===============\n");
   printf("Number of request(s) : %d\n", No);
   printf("No.\tIP\t\tPID\tPORT\tTIME\n"); // format
   for (int i = 0; i < No - 1; i++) {
      for (int j = 0; j < No - 1 - i; j++) {
         struct Point temp;
         if (request_client[j].s_time <= request_client[j + 1].s_time) {
            temp = request_client[j];
            request_client[j] = request_client[j + 1];
            request_client[j + 1] = temp;
         }
      }
   }
   if (No < 10){
      for (int i = 0; i < No; i++) {
         request_client[i].s_No = i + 1;
         printf("%d\t%s\t%d\t%d\t%s %s %d %d:%d:%d %d\n", request_client[i].s_No, request_client[i].s_cli_address, 
         request_client[i].s_pid, request_client[i].s_Port_NUM, Wday[request_client[i].s_wday], Mon[request_client[i].s_mon], request_client[i].s_mday, 
         request_client[i].s_hour, request_client[i].s_min, request_client[i].s_sec, request_client[i].s_year);
      }
   }
   else {
      for (int i = 0; i < 10; i++) {
         request_client[i].s_No = i + 1;
         printf("%d\t%s\t%d\t%d\t%s %s %d %d:%d:%d %d\n", request_client[i].s_No, request_client[i].s_cli_address, 
         request_client[i].s_pid, request_client[i].s_Port_NUM, Wday[request_client[i].s_wday], Mon[request_client[i].s_mon], request_client[i].s_mday, 
         request_client[i].s_hour, request_client[i].s_min, request_client[i].s_sec, request_client[i].s_year);
      }
   }
   alarm(10);
}
///////////////////////////////////////////////////////////////////////////////////////////
// Main                                                                                  //
// ======================================================================================//
// Input : void                                                                          //
// Output : return 0                                                                     //
// Purpose : 소켓을 생성하여 server와 client을 연결하고, 정보를 출력함                            //
///////////////////////////////////////////////////////////////////////////////////////////
int main() {
   struct sockaddr_in server_addr, client_addr;
   int len, len_out;
   int opt = 1;
   char cwd[1024];
   char access_addr[1024][1024];
   FILE *fp = fopen("accessible.usr", "r"); // get data of file what name is accessible.usr
   int add_count = 0;
   char string[1024];
   while ((fgets(string, 1024, fp)) != NULL) { // 
     strcpy(access_addr[add_count], string);
     if (access_addr[add_count][strlen(access_addr[add_count]) - 1] == '\n') {
        access_addr[add_count][strlen(access_addr[add_count]) - 1] = '\0';
     }
     add_count++;
   }
   getcwd(cwd, sizeof(cwd)); // current working directory
   if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) { // create socket in server
       printf("Server : can't open stream socket\n");
       return 0;
   }
   setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
   memset(&server_addr, 0, sizeof(server_addr));
   server_addr.sin_family = AF_INET; // IPv4
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // any address
   server_addr.sin_port = htons(PORTNO); // host to network short

   if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){ // address and socket binding
        printf("Server : can't bind local address\n");
        return 0;
    }

   listen(socket_fd, 5); // wait client (web client)
   
   while (1) {
      struct in_addr inet_client_address;
      char buf[BUFSIZE] = {0, };
      char tmp[BUFSIZE] = {0, };
      char response_header[BUFSIZE] = {0, };
      char url[URL_LEN] = {0, };
      char method[20] = {0, };
      char * tok = NULL;
      int access_perm = 0;
      len = sizeof(client_addr);
      client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len); // client accept socket
      if (client_fd < 0){
          printf("Server : accept failed\n");
          return 0;
      }
      if (read(client_fd, buf, BUFSIZE) == 0){ // 클라이언트 파일 디스크립트를 통해 수신한 데이터를 buf에 저장
         continue;
      }
      inet_client_address.s_addr = client_addr.sin_addr.s_addr;
////////////////////////
      pid = fork();
////////////////////////
      if (pid == 0){ // child process
         for (int i = 0; i < add_count; i++){
            if (fnmatch(access_addr[i], inet_ntoa(inet_client_address), 0) == 0) {
               access_perm++;
            }
         }
         if (access_perm > 0){
            strcpy(tmp, buf); // tmp에 buf 그대로 옮겨놓음
            tok = strtok(tmp, " "); // space tokenize
            strcpy(method, tok); // copy tok in method
            if (strcmp(method, "GET") == 0){ // if method == GET
               tok = strtok(NULL, " "); // tokenize next
               strcpy(url, tok); // url == tok
            }
            if (url[strlen(url) - 1] == '/' && strlen(url) > 1)
               url[strlen(url) - 1] = '\0';
            if (strcmp(url, "/favicon.ico") == 0) {
               exit(0);
            }
            puts("=============== New Client ===============");
            printf("IP : %s\nPort : %d\n", inet_ntoa(inet_client_address), client_addr.sin_port);
            puts("==========================================");
            struct stat buf_url;
            char abs_url[2002];
            char header_tag[2000];
            strcpy(abs_url, cwd);
            strcat(abs_url, url); // abs_url = absolute path of url
            if (abs_url[strlen(abs_url) - 1] == '/'){ // delete slash ("/")
               abs_url[strlen(abs_url) - 1] = '\0';
            }
            lstat(abs_url, &buf_url);
            if (file_check_(buf_url) == 'd'){ // directory
               strcpy(header_tag ,"text/html");
            }
            else if (
               fnmatch("*.jpg", url, FNM_CASEFOLD) == 0 ||
               fnmatch("*.png", url, FNM_CASEFOLD) == 0 ||
               fnmatch("*.jpeg", url, FNM_CASEFOLD) == 0 ) // image file
            {
               strcpy(header_tag, "image/*");          
            }
            else {
               strcpy(header_tag, "text/plain");
            }
            // setting header tag using in response header
            DIR* dirp;
            struct dirent *dir;
            char *name_list[1024];
            int count = 0;
            struct stat buf_name[1024];
            struct stat wild;
            int blocksize = 0;
            int readint = 0;
            ////////////////////// home ///////////////////////////
            if (strcmp(url, "/") == 0) { // url == "/"
               dirp = opendir(cwd);
               while ((dir = readdir(dirp)) != NULL) { // only non-hidden file
                  if (dir->d_name[0] != '.'){
                     name_list[count] = dir->d_name;
                     count++;
                  }
               }
               for (int i = 0; i < count; i++){
                  char path[1024];
                  strcpy(path, cwd);
                  strcat(path, "/");
                  strcat(path, name_list[i]); // path define
                  
                  lstat(path, &buf_name[i]);
                  blocksize += buf_name[i].st_blocks;
               }
               for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
                  for (int j = 0; j < count-1-i; j++){
                     if (strcasecmp(name_list[j], name_list[j+1]) > 0){ // bubble sort
                           char* temp = name_list[j+1];
                           name_list[j+1] = name_list[j];
                           name_list[j] = temp;
                           struct stat buf_temp = buf_name[j];
                           buf_name[j] = buf_name[j+1];
                           buf_name[j+1] = buf_temp;
                     }
                  }
               }
               sprintf(response_message, "<h1>Welcome to System Programming Http</h1>"
                  "<h3>Directory Path : %s</h3><br>"
                  "<table border=\"1\">"
                  "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th><tr>"
                  "<h3>total : %d</h3><br>", cwd, blocksize/2); // table의 기본 틀 출력
               for (int i = 0; i < count; i++){ // matching color
                  if (file_check_(buf_name[i]) == 'd'){
                     sprintf(response_message, "%s<tr style=\"color:blue\">", response_message);
                  }
                  else if (file_check_(buf_name[i]) == 'l'){
                     sprintf(response_message, "%s<tr style=\"color:green\">", response_message);
                  }
                  else {
                     sprintf(response_message, "%s<tr style=\"color:red\">", response_message);
                  }
                  sprintf(response_message, "%s<td><a href=\"/%s\">%s</a></td>", response_message, name_list[i], name_list[i]); // print file name
                  print_ls(buf_name[i]);
                  sprintf(response_message, "%s</tr>", response_message);
               }
               sprintf(response_message, "%s</table>", response_message); // Save all response_message
               closedir(dirp);
            }
            //////////////////////////////////////////////////////////////////////
            ////////////////////// No home ////////////////////////////////////
            else {
               if ((dirp = opendir(abs_url)) == NULL){
                  if (stat(abs_url, &wild) == -1) {
                     if (url[strlen(url) - 1] == '/')
                           url[strlen(url) - 1] = '\0';
                     sprintf(response_message, "Not Found\n");
                     sprintf(response_message, "%sThe request URL %s was not found on this server\n", response_message, url);
                     sprintf(response_message, "%sHTTP 404 - Not Page Found\n", response_message);
                  }
                  else if (stat(abs_url, &wild) != -1){
                     if (url[strlen(url) - 1] == '/')
                           url[strlen(url) - 1] = '\0';
                     char path[1024];
                     strcpy(path, cwd);
                     strcat(path, url);
                     file_fd = open(path, O_RDONLY);
                     while (readint = read(file_fd, response_message, BUFSIZE)){
                        sprintf(response_header,
                              "HTTP/1.0 200 OK\r\n"
                              "Server:2019 simple web server\r\n"
                              "Content-length:%d\r\n"
                              "Content-type:%s\r\n\r\n", readint, header_tag);
                        write(client_fd, response_header, strlen(response_header)); // 서버에서 클라이언트에 response를 write 해줌
                        write(client_fd, response_message, readint); // 서버에서 클라이언트에 response를 write 해줌
                        memset(response_message, 0, sizeof(response_message)); // response_message reset
                     }
                     close(client_fd);
                  }
               }
               else if ((dirp = opendir(abs_url)) != NULL) {
                  if (url[strlen(url) - 1] == '/')
                     url[strlen(url) - 1] = '\0';
                  while ((dir = readdir(dirp)) != NULL) {
                     name_list[count] = dir->d_name;
                     count++;
                  }
                  for (int i = 0; i < count; i++){
                     char path[1024];
                     strcpy(path, abs_url);
                     strcat(path, "/");
                     strcat(path, name_list[i]); // path define
                     
                     lstat(path, &buf_name[i]);
                     blocksize += buf_name[i].st_blocks;
                  }
                  for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
                     for (int j = 0; j < count-1-i; j++){
                        char *temp1, *temp2;
                        if (name_list[j][0] == '.'){ // hidden file, consider second file character
                           temp1 = name_list[j] + 1;
                        }
                        else {
                           temp1 = name_list[j];
                        }
                        if (name_list[j+1][0] == '.'){
                           temp2 = name_list[j+1] + 1;
                        }
                        else {
                           temp2 = name_list[j+1];
                        }
                        if (strcasecmp(temp1, temp2) > 0){ // bubble sort
                           char* temp = name_list[j+1];
                           name_list[j+1] = name_list[j];
                           name_list[j] = temp;
                           struct stat buf_temp = buf_name[j];
                           buf_name[j] = buf_name[j+1];
                           buf_name[j+1] = buf_temp;
                        }
                     }
                  }
                  sprintf(response_message, "<h1>System Programming Http</h1>"
                     "<h3>Directory Path : %s</h3><br>"
                     "<table border=\"1\">"
                     "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th><tr>"
                     "<h3>total : %d</h3><br>", abs_url, blocksize/2); // table의 기본 틀 출력
                  for (int i = 0; i < count; i++){ // matching color
                     if (file_check_(buf_name[i]) == 'd'){
                           sprintf(response_message, "%s<tr style=\"color:blue\">", response_message);
                     }
                     else if (file_check_(buf_name[i]) == 'l'){
                           sprintf(response_message, "%s<tr style=\"color:green\">", response_message);
                     }
                     else {
                           sprintf(response_message, "%s<tr style=\"color:red\">", response_message);
                     }
                     sprintf(response_message, "%s<td><a href=\"%s/%s\">%s</a></td>", response_message, url, name_list[i], name_list[i]); // print file name
                     print_ls(buf_name[i]);
                     sprintf(response_message, "%s</tr>", response_message);
                  }
                  sprintf(response_message, "%s</table>", response_message); // Save all response_message
               }
            }
            ///////////////////////////////////////////////////////////////////
            if (readint == 0 || strcmp(header_tag, "text/html") == 0){
               sprintf(response_header,
                     "HTTP/1.0 200 OK\r\n"
                     "Server:2019 simple web server\r\n"
                     "Content-length:%lu\r\n"
                     "Content-type:%s\r\n\r\n", strlen(response_message), header_tag); 
               write(client_fd, response_header, strlen(response_header)); // 서버에서 클라이언트에 response를 write 해줌
               write(client_fd, response_message, strlen(response_message)); // 서버에서 클라이언트에 response를 write 해줌
            }
            puts("=============== Disconnected Client ===============");
            printf("IP : %s\nPort : %d\n", inet_ntoa(inet_client_address), client_addr.sin_port);
            puts("===================================================");
            close(client_fd);
            memset(response_message, 0, sizeof(response_message));
            exit(0);
         }
         else {
            sprintf(response_message, "<body><h1>Access denied!<br></h1>");
            sprintf(response_message, "%sYour IP : %s<br>", response_message, inet_ntoa(inet_client_address));
            sprintf(response_message, "%sYou have no permission to access this web server.<br>", response_message);
            sprintf(response_message, "%sHTTP 403.6 - Forbidden:IP address reject<br></body>", response_message);
            sprintf(response_header,
                        "HTTP/1.0 200 OK\r\n"
                        "Server:2019 simple web server\r\n"
                        "Content-length:%lu\r\n"
                        "Content-type:text/html\r\n\r\n", strlen(response_message)); 
            write(client_fd, response_header, strlen(response_header)); // 서버에서 클라이언트에 response를 write 해줌      
            write(client_fd, response_message, strlen(response_message));
            exit(0);
         }
      }
      else { // parent process
         char abs_url[2002];
         struct stat wild;
         DIR* dirp;
         int choice = 0; // if 1, there is error or unexpected signal
         for (int i = 0; i < add_count; i++){
            if (fnmatch(access_addr[i], inet_ntoa(inet_client_address), 0) == 0) {
               access_perm++;
            }
         }
         if (access_perm == 0) {
            choice = 1;
         }
         strcpy(tmp, buf); // tmp에 buf 그대로 옮겨놓음
         tok = strtok(tmp, " "); // space tokenize
         strcpy(method, tok); // copy tok in method
         if (strcmp(method, "GET") == 0){ // if method == GET
            tok = strtok(NULL, " "); // tokenize next
            strcpy(url, tok); // url == tok
         }
         if (strcmp(url, "/favicon.ico") == 0) { // unexpected signal (favicon)
            choice = 1;
         }
         if (url[strlen(url) - 1] == '/' && strlen(url) > 1)
            url[strlen(url) - 1] = '\0';
         strcpy(abs_url, cwd);
         strcat(abs_url, url); // abs_url = absolute path of url
         if (abs_url[strlen(abs_url) - 1] == '/'){ // delete slash ("/")
            abs_url[strlen(abs_url) - 1] = '\0';
         }
         if ((dirp = opendir(abs_url)) == NULL){ // 404 error
            if (stat(abs_url, &wild) == -1) {
               if (url[strlen(url) - 1] == '/')
                  url[strlen(url) - 1] = '\0';
               choice = 1;
            }
         }
         //setting header tag using in response header
         
         if (choice == 0) { // 문제가 없을 시
            int status;
            time_t cur_time;
            cur_time = time(NULL);
            time_ = localtime(&cur_time);
            request_client[No].s_No = No + 1;
            strcpy(request_client[No].s_cli_address, inet_ntoa(inet_client_address));
            request_client[No].s_Port_NUM = client_addr.sin_port;
            request_client[No].s_pid = pid;
            request_client[No].s_wday = time_->tm_wday;
            request_client[No].s_mday = time_->tm_mday;
            request_client[No].s_mon = time_->tm_mon;
            request_client[No].s_hour = time_->tm_hour;
            request_client[No].s_min = time_->tm_min;
            request_client[No].s_sec = time_->tm_sec;
            request_client[No].s_year = time_->tm_year + 1900;
            request_client[No].s_time = cur_time;
            signal(SIGALRM, SignalHandler);
            alarm(10);
            No++;
            close(client_fd);
            wait(&status); // 
         }
      }
   }
   close(socket_fd);
   return 0;
}