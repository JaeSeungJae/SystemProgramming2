	///////////////////////////////////////////////////////////////////////////////////////////
	// File name  : 2018202018_spls_advanced.c                                     //
	// Date      : 2023/04/05                                                 //
	// Os         : Ubuntu 16.04 LTS 64bits                                        //
	// Authors    : Yu Seung Jae                                                //
	// Student ID : 2018202018                                                  //
	//---------------------------------------------------------------------------------------//
	// Title : System Programming Assignment #1-2                                            //
	// Description : 저번주에 구현한 ls 명령어에 옵션과 예외 처리를 추가하고 파일의 절대경로를 추가하여       //
	// 출력하는 프로그램                                                        //
	///////////////////////////////////////////////////////////////////////////////////////////
	#include <stdio.h>
	#include <dirent.h> // using DIR, struct dirent
	#include <stdlib.h>
	#include <string.h> // strcasecmp ( sorting ascending )
	#include <unistd.h> // linux function
	#include <pwd.h> // struct passwd
	#include <grp.h> // struct group
	#include <sys/stat.h> // struct stat
	#include <sys/types.h> // struct stat, passwd, ..
	#include <time.h> // struct tm
	#include <fnmatch.h>
	static FILE* fio;
	///////////////////////////////////////////////////////////////////////////////////////////
	// file_check                                                          					 //
	// ======================================================================================//
	// Input : struct stat -> to use st_mode                                     			 //
	// Output : void                                                       					 //
	// Purpose : checking file type                                              			 //
	///////////////////////////////////////////////////////////////////////////////////////////
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
	fprintf(fio,"<td>%c", f);
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
		fprintf(fio, "%c", perm[i]);
	}
	fprintf(fio, "</td>");
	fprintf(fio, "&nbsp&nbsp\n");
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
	fprintf(fio, "<td>%lu&nbsp&nbsp</td>", link);
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
	fprintf(fio, "<td>%s&nbsp&nbsp</td>", owner_name); 
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
	fprintf(fio, "<td>%s&nbsp&nbsp</td>", group_name);
	return;
	}
	///////////////////////////////////////////////////////////////////////////////////////////
	// file_size                                                          //
	// ======================================================================================//
	// Input : struct stat -> to use st_size                                     //
	// Output : void                                                       //
	// Purpose : checking file size                                              //
	///////////////////////////////////////////////////////////////////////////////////////////
	void file_size_(struct stat buf, int hflag){
	off_t size = 0;
	size = buf.st_size; //file size
	if (hflag == 1){
		if (size >= 1000*1000*1000){fprintf(fio, "<td>%ldG&nbsp&nbsp</td>", size/(1000*1000*1000));}
		else if (size >= (1000*1000)){fprintf(fio, "<td>%ldM&nbsp&nbsp</td>", size/(1000*1000));}
		else if (size >= 10000){fprintf(fio, "<td>%ldK&nbsp&nbsp</td>", size/1000);}
		else if (size >= 1000){fprintf(fio, "<td>%.1fK&nbsp&nbsp</td>", (double)size/1000);}
		else{
		fprintf(fio, "<td>%ld&nbsp&nbsp</td>", size);}}
	else {
		fprintf(fio, "<td>%ld&nbsp&nbsp</td>", size);
	}
	return;
	}

	void file_size(struct stat buf){
	off_t size = 0;
	size = buf.st_size; //file size
	fprintf(fio, "<td>%ld&nbsp&nbsp&nbsp</td>", size);
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
	fprintf(fio, "<td>%s&nbsp&nbsp%d&nbsp%d:%d&nbsp&nbsp</td>", Mon[Mon_num], day, hour, min); 
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
	// print_ls                                                               				 //
	// ======================================================================================//
	// Input : struct stat -> checking file information                                		 //
	// Output : void                                                       					 //
	// Purpose : print file informations when use hflag                                      //
	///////////////////////////////////////////////////////////////////////////////////////////
	void print_ls_(struct stat buf,int hflag){ // select file stat and print file information using file stat
		file_check(buf);
			file_permission(buf);
			file_link(buf);
			file_owner_name(buf);
			file_group_name(buf);
			file_size_(buf, hflag);
			file_time(buf);
	return;
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	// ls                                                                 //
	// ======================================================================================//
	// Input : DIR* -> which file selected                                         //
	// Output : void                                                       //
	// Purpose : 디렉토리 내의 파일들 이름 출력 / 파일 이름 출력                             //
	///////////////////////////////////////////////////////////////////////////////////////////
	void ls(DIR *a, char* argv, int Sflag, int rflag) {
	int count = 0; // use to compare string
	char *temp; // use to sort
	char *name_list[1024]; // use to sort
	struct dirent *dir;
	int wild[1000];
	struct stat buf[1000];
	unsigned long lssize[1000];
	//int wild[1000];
	while ((dir = readdir(a)) != NULL){ // until all variable
	if (dir->d_name[0] == '.'){
		continue;
	}
	
		name_list[count] = dir->d_name;
		count++;  // counting only non-hidden files
	}
	char argv_orig[1000];
	for (int i = 0; i < count; i++){
		strcpy(argv_orig, argv); // make absolute path
		strcat(argv_orig, "/");
		strcat(argv_orig, name_list[i]);
		lstat(argv_orig, &buf[i]);
	}
	
	if (Sflag == 0 && rflag == 0){
		for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
			for (int j = 0; j < count-1-i; j++){
				if (strcasecmp(name_list[j], name_list[j+1]) > 0){
                    struct stat tbuf = buf[j]; // sort buf
					buf[j] = buf[j + 1];
					buf[j + 1] = tbuf;
					temp = name_list[j]; // sort name_list
					name_list[j] = name_list[j+1];
					name_list[j+1] = temp;
				}
			}
		}
	}
	else if (Sflag == 1 && rflag == 0){
		for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
				for (int j = 0; j < count-1-i; j++){
					if (strcasecmp(name_list[j], name_list[j+1]) > 0){
						temp = name_list[j];
						name_list[j] = name_list[j+1]; // sort name_list
						name_list[j+1] = temp;
						struct stat tbuf = buf[j]; // sort buf
						buf[j] = buf[j + 1];
						buf[j + 1] = tbuf;
					}
				}
			}
		for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
			for (int j = 0; j < count-1-i; j++){
				if (buf[j].st_size < buf[j + 1].st_size){
					struct stat tbuf = buf[j]; // sort buf
					buf[j] = buf[j + 1];
					buf[j + 1] = tbuf;
					temp = name_list[j]; // sort name_list
					name_list[j] = name_list[j+1];
					name_list[j+1] = temp;
				}
			}
		}
	}
	else if (Sflag == 0 && rflag == 1){
		for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
			for (int j = 0; j < count-1-i; j++){
				if (strcasecmp(name_list[j], name_list[j+1]) < 0){
					temp = name_list[j]; // sort name_list
					name_list[j] = name_list[j+1];
					name_list[j+1] = temp;
                    struct stat tbuf = buf[j]; // sort buf
					buf[j] = buf[j + 1];
					buf[j + 1] = tbuf;
				}
			}
		}
	}
	else if (Sflag == 1 && rflag == 1){
		for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
				for (int j = 0; j < count-1-i; j++){
					if (strcasecmp(name_list[j], name_list[j+1]) < 0){
						temp = name_list[j];
						name_list[j] = name_list[j+1]; // sort name_list
						name_list[j+1] = temp;
						struct stat tbuf = buf[j]; // sort buf
						buf[j] = buf[j + 1];
						buf[j + 1] = tbuf;
					}
				}
			}
		for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
			for (int j = 0; j < count-1-i; j++){
				if (buf[j].st_size > buf[j + 1].st_size){
					struct stat tbuf = buf[j]; // sort buf
					buf[j] = buf[j + 1];
					buf[j + 1] = tbuf;
					temp = name_list[j]; // sort name_list
					name_list[j] = name_list[j+1];
					name_list[j+1] = temp;
				}
			}
		}
	}
	fprintf(fio, "<table border = \"1\"><tr><th>File name</th></tr>");
	for (int i = 0; i < count; i++){
		if (file_check_(buf[i]) == 'd'){ // directory = blue
			fprintf(fio, "<tr style=\"color:blue\"><td>%s</td></tr>", name_list[i]);
		}
		else if (file_check_(buf[i]) == 'l'){ // link = green
			fprintf(fio, "<tr style=\"color:green\"><td>%s</td></tr>", name_list[i]);
		}
		else { // other = red
			fprintf(fio, "<tr style=\"color:red\"><td>%s</td></tr>", name_list[i]);
		}
	}
	fprintf(fio, "</table>");
	}
	///////////////////////////////////////////////////////////////////////////////////////////
	// ls_a                                                                 //
	// ======================================================================================//
	// Input : DIR* -> which file selected                                         //
	// Output : void                                                       //
	// Purpose : 디렉토리 내의 파일들 이름 출력 / 파일 이름 출력(숨김파일 포함)                      //
	///////////////////////////////////////////////////////////////////////////////////////////
	void ls_a(DIR *a, char* argv, int Sflag, int rflag) {
			int count = 0; // use to compare string
			char *temp1; // use to sort
		char *temp2;
			char *name_list[1024]; // use to sort
			struct dirent *dir;
			struct stat buf[1000];

			while ((dir = readdir(a)) != NULL){ // until all variable
					name_list[count] = dir->d_name;
					count++;  // counting only non-hidden files
			}
			char argv_orig[1000];
			for (int i = 0; i < count; i++){
				strcpy(argv_orig, argv);
				strcat(argv_orig, "/");
				strcat(argv_orig, name_list[i]);
				lstat(argv_orig, &buf[i]);
			}
			if (Sflag == 0 && rflag == 0){
				for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
					for (int j = 0; j < count-1-i; j++){ // ignore hidden icon '.'
						if (name_list[j][0] == '.'){ // if hidden file, compare second characater
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
						if (strcasecmp(temp1, temp2) > 0){ // sort name_list all
							char* temp = name_list[j+1];
							name_list[j+1] = name_list[j];
							name_list[j] = temp;
                            struct stat tbuf = buf[j+1]; // sort buf
						    buf[j+1] = buf[j];
						    buf[j] = tbuf;
						}
					}
				}
			}
			else if (Sflag == 1 && rflag == 0){
				for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
					for (int j = 0; j < count-1-i; j++){ // ignore hidden icon '.'
						if (name_list[j][0] == '.'){ // if hidden file, compare second characater
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
						if (strcasecmp(temp1, temp2) > 0){ // sort alphabet
							struct stat tbuf = buf[j+1]; // sort buf
							buf[j+1] = buf[j];
							buf[j] = tbuf;
							char* temp = name_list[j+1]; // sort name_list all
							name_list[j+1] = name_list[j];
							name_list[j] = temp;
						}
					}
				}
				for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
					for (int j = 0; j < count-1-i; j++){ // ignore hidden icon '.'
						if (name_list[j][0] == '.'){ // if hidden file, compare second characater
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
						if (buf[j].st_size < buf[j + 1].st_size){ // sort size
							struct stat tbuf = buf[j+1];
							buf[j+1] = buf[j];
							buf[j] = tbuf;
							char* temp = name_list[j+1]; // sort name_list
							name_list[j+1] = name_list[j];
							name_list[j] = temp;
						}
					}
				}
			}
			else if (Sflag == 0 && rflag == 1){
				for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
					for (int j = 0; j < count-1-i; j++){ // ignore hidden icon '.'
						if (name_list[j][0] == '.'){ // if hidden file, compare second characater
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
						if (strcasecmp(temp1, temp2) < 0){ // sort alphabet
							char* temp = name_list[j+1];
							name_list[j+1] = name_list[j];
							name_list[j] = temp;
                            struct stat tbuf = buf[j+1]; // sort buf
						    buf[j+1] = buf[j];
						    buf[j] = tbuf;
						}
					}
				}
			}
			else if (Sflag == 1 && rflag == 1){
				for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
					for (int j = 0; j < count-1-i; j++){ // ignore hidden icon '.'
						if (name_list[j][0] == '.'){ // if hidden file, compare second characater
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
						if (strcasecmp(temp1, temp2) < 0){ // sort alphabet
							char* temp = name_list[j+1];
							name_list[j+1] = name_list[j];
							name_list[j] = temp;
							struct stat tbuf = buf[j+1];
							buf[j+1] = buf[j];
							buf[j] = tbuf;
						}
					}
				}
				for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
					for (int j = 0; j < count-1-i; j++){ // ignore hidden icon '.'
						if (name_list[j][0] == '.'){ // if hidden file, compare second characater
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
						if (buf[j].st_size > buf[j + 1].st_size){ // sort size
							struct stat tbuf = buf[j+1];
							buf[j+1] = buf[j];
							buf[j] = tbuf;
							char* temp = name_list[j+1];
							name_list[j+1] = name_list[j];
							name_list[j] = temp;
						}
					}
				}
			}
			fprintf(fio, "<table border = \"1\"><tr><th>File name</th></tr>");
			for (int i = 0; i < count; i++){
				if (file_check_(buf[i]) == 'd'){ // directory = blue
					fprintf(fio, "<tr style=\"color:blue\"><td>%s</td></tr>", name_list[i]);
				}
				else if (file_check_(buf[i]) == 'l'){ // link = green
					fprintf(fio, "<tr style=\"color:green\"><td>%s</td></tr>", name_list[i]);
				}
				else { // other = red
					fprintf(fio, "<tr style=\"color:red\"><td>%s</td></tr>", name_list[i]);
				}
			}
			fprintf(fio, "</table>");
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// ls_al                                                               //
	// ======================================================================================//
	// Input : DIR* -> which file selected                                         //
	// Output : void                                                       //
	// Purpose :디렉토리 내의 파일들의 정보 출력 / 경로 출력                               //
	//          파일의 정보 출력 (숨김파일 포함)                                        //
	///////////////////////////////////////////////////////////////////////////////////////////
	void ls_al(DIR *a, char *argv, int hflag, int Sflag, int rflag) {
		int count = 0; // use to compare string
			char *temp; // use to sort
			char *name_list[1024]; // use to sort
			struct dirent *dir;
		char *name_list_plus[1024];
		char *temp1, *temp2;
		char buf_link[1024][100];
		char ch;
	//struct stat buf[1024];
	struct stat buf[100];
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	int blocksize = 0;
			while ((dir = readdir(a)) != NULL){ // until all variable
					name_list[count] = dir->d_name;
					count++; // contain hidden file
		}
	char argv_orig[1000];
	strcpy(argv_orig, argv);
	if (argv[0] == '/'){
		fprintf(fio, "<h3>Directory Path : %s</h3><br>", argv);
	}
	else if (argv[0] == '.' && argv[1] == '/' && argv[2] == ' '){
		fprintf(fio, "<h3>Directory Path : %s</h3><br>", cwd);
	}
	else if (argv[0] == '.' && argv[1] == ' '){
		fprintf(fio, "<h3>Directory path : %s</h3><br>", cwd);
	}
	else {
		fprintf(fio, "<h3>Directory path : %s/%s</h3><br>", cwd, argv);
	}
	strcat(argv_orig, "/"); // directory + /
	for (int i = 0; i < count; i++){
		strcpy(argv_orig, argv);
		strcat(argv_orig, "/");
		lstat(strcat(argv_orig, name_list[i]), &buf[i]); // lstat((directory + / + file name[i]), &buf[i])
		blocksize += buf[i].st_blocks; // blocksize add
		ch = file_check_(buf[i]); // checking file type
		if (ch == 'l'){
			int trash = readlink(argv_orig, buf_link[i], 100); // trash is not used, save origin file name of link in buf_link
		}
		memset(argv_orig, 0, sizeof(argv)); // initialize continuously
	}
		if (Sflag == 0 && rflag == 0){ // no more flag
			for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
					for (int j = 0; j < count-1-i; j++){
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
						struct stat buf_temp = buf[j];
						buf[j] = buf[j+1];
						buf[j+1] = buf_temp;
				}
				}
			}
		}
		else if (Sflag == 1 && rflag == 0){ // consider flag S, r
			for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
					for (int j = 0; j < count-1-i; j++){
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
						struct stat buf_temp = buf[j];
						buf[j] = buf[j+1];
						buf[j+1] = buf_temp;
				}
				}
			}
			for (int i = 0; i < count-1; i++){
				for (int j = 0; j < count-1-i; j++){
				if (buf[j].st_size < buf[j+1].st_size){ // compare sizes of components
					temp = name_list[j]; // swap d_name and struct stat
					name_list[j] = name_list[j+1];
					name_list[j+1] = temp;
					struct stat buf_temp = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = buf_temp;
					// block size bubble sort
				}
				}
			}
		}
		else if (Sflag == 0 && rflag == 1){ // consider flag r, S
			for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
				for (int j = 0; j < count-1-i; j++){
						if (strcasecmp(name_list[j], name_list[j+1]) < 0){
							temp = name_list[j];
						name_list[j] = name_list[j+1];
						name_list[j+1] = temp;
					struct stat buf_temp = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = buf_temp;
					}
				}
			}
		}
		else if (Sflag == 1 && rflag == 1){ // consider flag S, r
			for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
					for (int j = 0; j < count-1-i; j++){
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
				if (strcasecmp(temp1, temp2) < 0){ // bubble sort
						char* temp = name_list[j+1];
						name_list[j+1] = name_list[j];
						name_list[j] = temp;
						struct stat buf_temp = buf[j];
						buf[j] = buf[j+1];
						buf[j+1] = buf_temp;
				}
				}
			}
			for (int i = 0; i < count-1; i++){
				for (int j = 0; j < count-1-i; j++){
				if (buf[j].st_size > buf[j+1].st_size){  // compare size
					temp = name_list[j]; // swap d_name and struct stat
					name_list[j] = name_list[j+1];
					name_list[j+1] = temp;
					struct stat buf_temp = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = buf_temp;
					// block size bubble sort
				}
				}
			}
		}
	fprintf(fio, "<table border=\"1\">");
	fprintf(fio, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th><tr>");
	fprintf(fio, "<h3>total : %d</h3><br>", blocksize/2); // total blocksize print
	for (int i = 0; i < count; i++){
		if (file_check_(buf[i]) == 'd'){
			fprintf(fio, "<tr style=\"color:blue\">");
		}
		else if (file_check_(buf[i]) == 'l'){
			fprintf(fio, "<tr style=\"color:green\">");
		}
		else {
			fprintf(fio, "<tr style=\"color:red\">");
		}
		// fprintf(fio, "<table border=\"1\"><tr>");
		fprintf(fio, "<td><a href=\"%s/./%s\">%s</a></td>", argv, name_list[i], name_list[i]); // print file name
		print_ls_(buf[i], hflag); // print file information
		
		// if (file_check_(buf[i]) == 'l'){ // if file is symbolic link
		// 	strcpy(argv_orig, argv);
		// 	strcat(argv_orig, "/");
		// 	strcat(argv_orig, name_list[i]);
		// 	int trash = readlink(argv_orig, buf_link[i], 100); // trash is not used, save origin file name of link in buf_link
		// 	printf(" -> %s", buf_link[i]); // show origin file name of symbolic link
		// 	memset(argv_orig, 0, sizeof(argv_orig));
		// }
		fprintf(fio, "</tr>");
	}
	fprintf(fio, "</table>");
	}
	//////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////
	// ls_l                                                                  //
	// ======================================================================================//
	// Input : DIR* -> which file selected                                         //
	// Output : void                                                       //
	// Purpose :디렉토리 내의 파일들의 정보 출력 / 경로 출력                               //
	//          파일의 정보 출력                                                 //
	///////////////////////////////////////////////////////////////////////////////////////////
	void ls_l(DIR *a, char* argv, int hflag, int Sflag, int rflag) {
			int count = 0; // use to compare string, number of files in argv(directory)
			char *temp; // use to sort
			char *name_list[1024]; // use to sort
			struct dirent *dir; // dirent
		char buf_link[1024][100]; // checking if this file is symbolic link
		char ch; // file checking
		int save_size[1024];
		int temp_size;
	//struct stat buf[1024];
	struct stat buf[100]; // file information array
	char cwd[1024]; // current working directory
	int blocksize = 0; // blocksize initialize
	getcwd(cwd , sizeof(cwd));
			while ((dir = readdir(a)) != NULL){ // until all variable
			if (dir->d_name[0] == '.'){ // ignore hidden file
					continue;
			}   
		else {
					name_list[count] = dir->d_name;
					count++;  // counting only non-hidden files
			}
		}
		
	char argv_orig[1000]; // not to modify argv array;
	strcpy(argv_orig, argv); // copy argv to argv_orig
	if (argv[0] == '/'){
		fprintf(fio, "<h3>Directory Path : %s</h3><br>", argv);
	}
	else {
		fprintf(fio, "<h3>Directory Path : %s/%s</h3><br>", cwd, argv);
	}


	strcat(argv_orig, "/"); // directory + /
	for (int i = 0; i < count; i++){
		strcpy(argv_orig, argv); // initialize continuously
		strcat(argv_orig, "/"); // directory + /
		lstat(strcat(argv_orig, name_list[i]), &buf[i]); // lstat((directory + / + file name[i]), &buf[i])
		blocksize += buf[i].st_blocks; // blocksize add
		ch = file_check_(buf[i]); // file type checking
		memset(argv_orig, 0, sizeof(argv)); // initialize strcat
	}


		if (Sflag == 0 && rflag == 0){ // consider flag S, r
			for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
				for (int j = 0; j < count-1-i; j++){
						if (strcasecmp(name_list[j], name_list[j+1]) > 0){ // name sort
							temp = name_list[j];
						name_list[j] = name_list[j+1];
						name_list[j+1] = temp;
					struct stat buf_temp = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = buf_temp;
					}
				}
			}
		}
		else if (Sflag == 1 && rflag == 0){ // consider flag r, S
			for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
				for (int j = 0; j < count-1-i; j++){
					if (strcasecmp(name_list[j], name_list[j+1]) > 0){
						temp = name_list[j];
						name_list[j] = name_list[j+1];
						name_list[j+1] = temp;
						struct stat tbuf = buf[j];
						buf[j] = buf[j + 1];
						buf[j + 1] = tbuf;
					}
				}
			}
			for (int i = 0; i < count-1; i++){
				for (int j = 0; j < count-1-i; j++){
				if (buf[j].st_size < buf[j+1].st_size){ // compare size
					// temp = name_list[j];
					// name_list[j] = name_list[j+1];
					// name_list[j+1] = temp;
					temp = name_list[j]; // sort d_name and struct stat
						name_list[j] = name_list[j+1];
						name_list[j+1] = temp;
					struct stat buf_temp = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = buf_temp;
					// block size bubble sort
				}
				}
			}
		}
		else if (Sflag == 0 && rflag == 1){ // consider flag r, S
			for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
				for (int j = 0; j < count-1-i; j++){
						if (strcasecmp(name_list[j], name_list[j+1]) < 0){ // compare name reverse
							temp = name_list[j];
						name_list[j] = name_list[j+1]; // swap name and stat
						name_list[j+1] = temp;
					struct stat buf_temp = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = buf_temp;
					}
				}
			}
		}
		else if (Sflag == 1 && rflag == 1){
			for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
				for (int j = 0; j < count-1-i; j++){
					if (strcasecmp(name_list[j], name_list[j+1]) < 0){
						temp = name_list[j];
						name_list[j] = name_list[j+1];
						name_list[j+1] = temp;
						struct stat tbuf = buf[j];
						buf[j] = buf[j + 1];
						buf[j + 1] = tbuf;
					}
				}
			}
			for (int i = 0; i < count-1; i++){
				for (int j = 0; j < count-1-i; j++){
				if (buf[j].st_size > buf[j+1].st_size){ // compare size
					temp = name_list[j]; // swap name and stat
					name_list[j] = name_list[j+1];
					name_list[j+1] = temp;
					struct stat buf_temp = buf[j];
					buf[j] = buf[j+1];
					buf[j+1] = buf_temp;
					// block size bubble sort
				}
				}
			}
		}
	fprintf(fio, "<table border=\"1\">");
	fprintf(fio, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th><tr>");
	fprintf(fio, "<h3>total : %d</h3><br>", blocksize/2); // total blocksize print
	
	for (int i = 0; i < count; i++){
		if (file_check_(buf[i]) == 'd'){
			fprintf(fio, "<tr style=\"color:blue\">");
		}
		else if (file_check_(buf[i]) == 'l'){
			fprintf(fio, "<tr style=\"color:green\">");
		}
		else {
			fprintf(fio, "<tr style=\"color:red\">");
		}
		// fprintf(fio, "<table border=\"1\"><tr>");
		fprintf(fio, "<td><a href=\"%s/%s\">%s</a></td>", argv, name_list[i], name_list[i]); // print file name
		print_ls_(buf[i], hflag); // print file information
		fprintf(fio, "</tr>");
	}
	fprintf(fio, "</table>");
	}


	///////////////////////////////////////////////////////////////////////////////////////////
	// wild_check                                                                 			 //
	// ======================================================================================//
	// Input : char* -> argument                                                             //
	// Output : int		0 -> argument is wildcard											 //
	//					1 -> argument is non-wildcard                                        //
	// Purpose : checking if argument is wildcard or not                                  	 //
	///////////////////////////////////////////////////////////////////////////////////////////
	int wild_check(char* argv){
		int wcheck = 0;
		for (int i = strlen(argv) - 1; i >= 0; i--){
			if (argv[i] == '?' || argv[i] == '*' || argv[i] == '[' || argv[i] == ']'){ // check this argument has wildcard pointer
				wcheck++;
			}
		}
		if (wcheck > 0) // has wild card pattern
			return 0;
		else
			return 1; // non-wildcard
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	// ls_wild				                                                                 //
	// ======================================================================================//
	// Input : int -> argument count(equal main)											 //
	//         char** -> argument vector(equal main)										 //
	// Output : void                                                       					 //
	// Purpose : if argv[1] is wildcard, print argv[1]                              		 //
	///////////////////////////////////////////////////////////////////////////////////////////
	int ls_wild(int argc, char** argv){
    
    DIR* dirp; // file
    DIR* dirp2; // down file
    struct dirent *dir; // file pointer
    struct dirent *dir2; // down file
    int count = 0; // file count
    char* name_list[1024]; // file name list
    char temp[1024]; // temporarily variablee
    int asdf = 0; // slash pointer
    int count2 = 0; // down file count
    struct stat buf; // temporarily stat
    char* name_list2[1024]; // down file name list
    struct stat buf2[1000]; // down file stat
	char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    if (argc == 2){
    for (int i = strlen(argv[1]); i > -1; i--){
        if (argv[1][i] == '/'){ // absolute path
            asdf = i; // check where slash is
            break; // find last slash
        }
    }
    if (asdf == 0){ // asdf : slash pointer
            dirp = opendir("."); // cwd
            while ((dir = readdir(dirp)) != NULL){
                if (dir->d_name[0] != '.'){
                    name_list[count] = dir->d_name;
                    count++;
                }
            }
            for (int i = 0; i < count; i++){
                for (int j = 0; j < count - 1 - i; j++){
                    if (strcasecmp(name_list[j], name_list[j+1]) > 0){ // sort name
                        char *name_temp = name_list[j];
                        name_list[j] = name_list[j+1];
                        name_list[j+1] = name_temp;
                    }
                }
            }
			fprintf(fio, "<table border=\'1\'><tr><th>File Name</th></tr>");
            for (int i = 0; i < count; i++){
                if (fnmatch(argv[1], name_list[i], 0) == 0){ // do ls only matching
                    //printf("%s\n", name_list[i]);
                    dirp2 = opendir(name_list[i]);
                    if (dirp2 == NULL){
                        lstat(name_list[i], &buf);
                        if (file_check_(buf) == 'l'){
                            fprintf(fio, "<tr style=\"color:green\"><td>%s</td></tr>", name_list[i]); // link = green
                        }
                        else {
                            fprintf(fio, "<tr style=\"color:red\"><td>%s</td></tr>", name_list[i]); // other = red
                        }                        
                    }
                }
                else {
                    continue;
                }
                count2 = 0;
            }
			fprintf(fio, "</table>");
			for (int i = 0; i < count; i++){
				if (fnmatch(argv[1], name_list[i], 0) == 0){
					dirp2 = opendir(name_list[i]);
					if (dirp2 != NULL) {
                        fprintf(fio, "<h4>%s&nbsp:<h4><br>", name_list[i]);
                        while((dir2 = readdir(dirp2)) != NULL){
                            if (dir2->d_name[0] != '.'){
                                name_list2[count2] = dir2->d_name;
                                count2++;
                            }
                        }
                        for (int i = 0; i < count2; i++){
                            for (int j = 0; j < count2 - 1 - i; j++){
                                if (strcasecmp(name_list2[j], name_list2[j+1]) > 0){
                                    char *name_temp = name_list2[j];
                                    name_list2[j] = name_list2[j+1];
                                    name_list2[j+1] = name_temp;
                                }
                            }
                        }
						fprintf(fio, "<table border=\"1\"><tr><th>File Name</th></tr>"); // print head File Name
                        for (int k = 0; k < count2; k++){
                            char tempo[1024];
                            struct stat buf2;
                            strcpy(tempo, cwd);
                            strcat(tempo, "/");
                            strcat(tempo, name_list[i]);
                            strcat(tempo, "/");
                            strcat(tempo, name_list2[k]); // make absolute path
                            lstat(tempo, &buf2);
                            if (file_check_(buf2) == 'd'){ // directory = blue
                                fprintf(fio, "<tr style=\"color:blue\"><td>%s</td></tr>\n", name_list2[k]);    
                            }
                            else if (file_check_(buf2) == 'l'){ // link = green
                                fprintf(fio, "<tr style=\"color:green\"><td>%s</td></tr>\n", name_list2[k]);    
                            }
                            else { // other = red
                                fprintf(fio, "<tr style=\"color:red\"><td>%s</td></tr>\n", name_list2[k]);    
                            }
                        }
						fprintf(fio, "</table>");
                    }
				}
			}
      //  }
    }
    else { /////////////////// argc == 2, asdf != 0 (/ exist)
    int i = 0;
    for (; i < asdf+1; i++){
        temp[i] = argv[1][i];
    }
    temp[i] = '\0';
    dirp = opendir(temp);
    if (dirp == NULL){
        fprintf(fio, "no such file or directory<br>\n");
        return 0;
    }
    // char* cwd;
    // getcwd(cwd, sizeof(cwd));
    // printf("%s\n", cwd);
    // for (int i = 0; i < argc; i++){
    //     printf("%s\n", argv[i]);
    // }
    // printf("%d\n", argc);
    while ((dir = readdir(dirp)) != NULL){
        name_list[count] = dir->d_name;
        count++;
    }
    for (int i = 0; i < count; i++){
        for (int j = 0; j < count - 1 - i; j++){
            if (strcasecmp(name_list[j], name_list[j+1]) > 0){
                char *name_temp = name_list[j];
                name_list[j] = name_list[j+1];
                name_list[j+1] = name_temp;
            }
        }
    }

	fprintf(fio, "<table border=\"1\"><tr><th>File Name</th></tr>"); // print head File Name and make table
    for (int i = 0; i < count; i++){
        char argv_temp[1024];
        strcpy(argv_temp, temp);
        //strcat(argv_temp, "/");
        strcat(argv_temp, name_list[i]);
        //printf("argv_temp : %s\n", argv_temp);
        if (fnmatch(argv[1], argv_temp, 0) == 0){
            //printf("%s\n", name_list[i]);
            //printf("Directory : %s\n", name_list[i]);
            dirp2 = opendir(argv_temp);
            if (dirp2 == NULL){
                        lstat(argv_temp, &buf);
                        if (file_check_(buf) != 'd'){ // non-directory
                            if (name_list[i][0] == '.'){
                                continue;
                            }
                            else {
								if (file_check_(buf) == 'l'){
                                	fprintf(fio, "<tr style=\"color:green\"><td>%s</td></tr>\n", name_list[i]); // link = green
								}
								else{
									fprintf(fio, "<tr style=\"color:red\"><td>%s</td></tr>\n", name_list[i]); // other = red
								}
                            }
                        }
                        else {
                            fprintf(fio, "error!<br>\n");
                        }           
            }
        }
        else{
            continue;
        }
        memset(argv_temp, 0, sizeof(argv));
        count2 = 0;
    }
	fprintf(fio, "</table>");
	for (int i = 0; i < count; i++){
        char argv_temp[1024];
        strcpy(argv_temp, temp);
        //strcat(argv_temp, "/");
        strcat(argv_temp, name_list[i]);
        //printf("argv_temp : %s\n", argv_temp);
        if (fnmatch(argv[1], argv_temp, 0) == 0){
            //printf("%s\n", name_list[i]);
            //printf("Directory : %s\n", name_list[i]);
            dirp2 = opendir(argv_temp);
            if (dirp2 != NULL) {
                while((dir2 = readdir(dirp2)) != NULL){
                    if (name_list[i][0] == '.'){
                        continue;
                    }
                    else{
                        if (dir2->d_name[0] != '.'){
                            name_list2[count2] = dir2->d_name;
                            count2++;
                        }
                        else {
                            continue;
                        }
                    }
                }
                for (int i = 0; i < count2; i++){
                    for (int j = 0; j < count2 - 1 - i; j++){
                        if (strcasecmp(name_list2[j], name_list2[j+1]) > 0){ // sort name ASCII
                            char *name_temp = name_list2[j];
                            name_list2[j] = name_list2[j+1];
                            name_list2[j+1] = name_temp;
                        }
                    }
                } 
                if (name_list[i][0] != '.'){
					fprintf(fio, "<h3>%s :</h3><br>\n", argv_temp); // separate argv
                    fprintf(fio, "<table border=\"1\"><tr><th>File Name</th></tr>\n"); // print head File Name
                    for(int i = 0; i < count2; i++){
						struct stat buf2;
						lstat(name_list2[i], &buf2);
						if (file_check_(buf2) == 'd'){
                        	fprintf(fio, "<tr style=\"color:blue\"><td>%s</td></tr>\n", name_list2[i]); // directory = blue
						}
						else if (file_check_(buf2) == 'l'){
							fprintf(fio, "<tr style=\"color:green\"><td>%s</td></tr>\n", name_list2[i]); // link = green
						}
						else {
							fprintf(fio, "<tr style=\"color:red\"><td>%s</td></tr>\n", name_list2[i]); // other = red
						}
                    }
					fprintf(fio, "</table>");
                }
                else {
                    continue;
                }
            }
        }
        else{
            continue;
        }
        memset(argv_temp, 0, sizeof(argv));
        count2 = 0;
    }
    }
    }
    return 0;
}
    ///////////////////////////////////////////////////////////////////////////////////////////
	// ls_orig  			                                                                 //
	// ======================================================================================//
	// Input : DIR* -> directory pointer        											 //
	//         char* -> Directory path what pointed DIR*               					     //
	// Output : void                                                       					 //
	// Purpose : execute ls with no options                              		             //
	///////////////////////////////////////////////////////////////////////////////////////////
	void ls_orig(DIR *a, char* path) {
	int count = 0; // use to compare string
	char *temp; // use to sort
	char *name_list[1024]; // use to sort
	struct dirent *dir;
	int wild[1000];
	//int wild[1000];
	while ((dir = readdir(a)) != NULL){ // until all variable
	if (dir->d_name[0] == '.'){
		continue;
	}
	
		name_list[count] = dir->d_name;
		count++;  // counting only non-hidden files
	}
		for (int i = 0; i < count-1; i++){ // sort name_list using bubble sorting
			for (int j = 0; j < count-1-i; j++){
				if (strcasecmp(name_list[j], name_list[j + 1]) > 0){ // sort to compare name ASCII
				temp = name_list[j];
				name_list[j] = name_list[j+1]; 
				name_list[j+1] = temp;
			}
		}
	}
	char path_orig[1024];
	fprintf(fio, "<table border=\"1\"><tr><th>File Name</th></tr>"); // make table
	for (int i = 0; i < count; i++){
		struct stat buf2;
		strcpy(path_orig, path);
		strcat(path_orig, "/");
		strcat(path_orig, name_list[i]); // make absolute path
		lstat(path_orig, &buf2);
		if (file_check_(buf2) == 'd'){ 
			fprintf(fio, "<tr style=\"color:blue\"><td>%s</td></tr>\n", name_list[i]); // directory = blue
		}
		else if (file_check_(buf2) == 'l'){
			fprintf(fio, "<tr style=\"color:green\"><td>%s</td></tr>\n", name_list[i]); // link = green
		}
		else {
			fprintf(fio, "<tr style=\"color:red\"><td>%s</td></tr>\n", name_list[i]); // other = red
		}
		memset(path_orig, 0, sizeof(path_orig));
	}
	fprintf(fio, "</table>"); // close table
	}
	///////////////////////////////////////////////////////////////////////////////////////////
	// main                                                                                  //
	// ======================================================================================//
	// Input : int -> argument count, char** -> argument vector                              //
	// Output : void                                                                         //
	// Purpose : 프로그램의 인자를 확인하여 flag를 정의하고 인자의 갯수에 따라 각기 다른 실행              //
	///////////////////////////////////////////////////////////////////////////////////////////
	int main (int argc, char **argv){
	int aflag = 0;
			int lflag = 0;
		int hflag = 0;
		int rflag = 0;
		int Sflag = 0; // option flags
			int c = 0; // option getopt
		char cwd[1024]; // current working directory
		fio = fopen("html_ls.html", "w"); // write on html_ls.html
		getcwd(cwd, sizeof(cwd)); // get current working directory using no argument of directory
		fprintf(fio, "<html><body><h1>");
		for (int i = 0; i < argc; i++){
			fprintf(fio, "%s ", argv[i]);
		}
		fprintf(fio, "</h1><br>");
		//printf("%d %s\n", argc, argv[1]);
			while ((c = getopt(argc, argv, "alhrS")) != -1) // flag setting
			{
					switch(c)
					{
					case 'a': // -a
						aflag = 1;
						break;
					case 'l': // -l
						lflag = 1;
						break;
					case 'h': // -h
						hflag = 1;
						break;
					case 'r': // -r
						rflag = 1;
						break;
					case 'S': // -S
						Sflag = 1;
						break;
					case '?': // else
						fprintf(fio, "Error : command is not exist\n");
						exit(0);
					}
			}
		//printf("%d %d\n", aflag, lflag);
			DIR *dirp;
		struct stat st[999]; // temporarily stat
		char checking; // file_check_
	if (argc == 1){ // if no argument or argument is "."
		if ((dirp = opendir(".")) == NULL){ // only "ls cwd";
			lstat(argv[1], &st[0]); 
			file_check(st[0]); // file type check
			fprintf(fio, "cannot access %s: No such file or directory\n", argv[1]);
			exit(0);
		} // assign current directory
		ls(dirp, cwd, Sflag, rflag);
	}
	else if (argc == 2){ // only an option or a file
		// if (1){
		//    printf("????\n");
		//    dirp = opendir(".");
		//    ls_wild(dirp, argv[1]);
		//    return 0;
		// }
		int wc = 0;
		if ((argv[1][0]) == '-'){ // "ls argv[1]" or "ls -(options)"
			wc = 1; // optind role
			dirp = opendir(".");
		} 
		//dirp = opendir(argv[1]);
		else if (wild_check(argv[1]) == 0){ // if argument has wildcard pattern
			ls_wild(argc, argv);
			return 0;
		}
		else
		{
			dirp = opendir(argv[1]);
			lstat(argv[1], &st[0]);
			checking = file_check_(st[0]);
			if (dirp == NULL){
				if (checking == 'n'){ // non exist file
				fprintf(fio, "cannot access %s: No such file or directory\n", argv[1]); // error print
				exit(0);
				}
				else {
					fprintf(fio, "<table border=\"1\"><tr><th>File Name</th></tr>"); // make table
					if (argv[1][0] == '/'){
						fprintf(fio, "<tr><td><a href=\"%s\">%s</td></tr>\n", argv[1], argv[1]); // ls (non-directory file)
					}
					else {
						fprintf(fio, "<tr><td><a href=\"%s/%s\">%s</a></td></tr>\n", cwd, argv[1], argv[1]); // ls (non-directory file)
					}
					fprintf(fio, "</table>"); // close table
					return 0;
				}
			}
		}
		
		if (aflag == 0 && lflag == 0){ // ls
			if (wc == 1){
				dirp = opendir(".");
			}
			else {
			dirp = opendir(argv[1]); // file name
			}
			ls(dirp, cwd, Sflag, rflag);
		}
		else if (aflag == 1 && lflag == 0){ // ls -a
			dirp = opendir("."); // cwd
					ls_a(dirp, cwd, Sflag, rflag);
			}
			else if (aflag == 0 && lflag == 1){ // ls -l
			dirp = opendir("."); // cwd
					ls_l(dirp, cwd, hflag, Sflag, rflag);
			}
			else { // ls -la , -al, -a -l, ..
			dirp = opendir("."); // cwd
					ls_al(dirp, cwd, hflag, Sflag, rflag);
		}
	}
	
		else if (argc > 2){
		int oc = 0; // how many options
		int arr[1000]; // marking file type 
					// 1 : non-directory 
					// 2 : non-exist file
		char* temtem; // sort argument 
		
		for (int i = 1; i < argc; i++){ // option count == optind
			if (argv[i][0] == '-'){
				oc++;
			}
		}
		for (int i = 1+oc; i < argc; i++){ // sort first
			for (int j = 1+oc; j < argc-1-i; j++){
				if (strcasecmp(argv[j], argv[j + 1]) > 0){
				temtem = argv[j];
				argv[j] = argv[j+1];
				argv[j+1] = temtem;
				}
			}
		}

		if (oc == 0) {
			char *fill[1024];
			int count_fill = 0; // how many argument
			int cut = 0; // where slash
			struct dirent *dir;
			for (int i = 1; i < argc; i++){
				if (wild_check(argv[i]) == 0){ //wildcard
					if (argv[i][0] == '/'){ // absolute directory
						char tt[1024]; // temporarily variable
						DIR* dirp2;

						for (int j = strlen(argv[i]); j > -1; j--){
							if (argv[i][j] == '/'){ // find slash point
								cut = j;
								break;
							} 
						}

						int p = 0;
						for (; p < cut+1; p++){
							tt[p] = argv[i][p]; // parent directory of wildcard argument
						}
						tt[p] = '\0';

						dirp2 = opendir(tt);
						if (dirp2 == NULL){
							printf("error dirp2\n");
						}

						else {
							while((dir = readdir(dirp2)) != NULL){
								if ((dir->d_name[0]) != '.'){
									char alpha[1024][1024];
									strcpy(alpha[count_fill], tt);
									// printf("dir->d_name : %s\n", dir->d_name);
									strcat(alpha[count_fill], dir->d_name);
									// printf("argv[i] : %s\n", argv[i]);
									if (fnmatch(argv[i], alpha[count_fill], 0) == 0){
										fill[count_fill] = alpha[count_fill];
										count_fill++;
									}	
								}
								else{
									continue;
								}
							}
						}

					}
					else { // relative directory
						DIR* dirp2;
						dirp2 = opendir(".");
						if (dirp2 == NULL){
							printf("error dirp2\n");
						}
						else {
							while((dir = readdir(dirp2)) != NULL){
								if (dir->d_name[0] != '.'){
									if (fnmatch(argv[i], dir->d_name, 0) == 0){
										char alpha[1024][1024];
										strcpy(alpha[count_fill], cwd);
										strcat(alpha[count_fill], "/");
										strcat(alpha[count_fill], dir->d_name);
										fill[count_fill] = alpha[count_fill];
										count_fill++;
									}
								}
							}
						}
					}
				}
				else if (wild_check(argv[i]) != 0){ // non-wildcard
					DIR *test;
					test = opendir(argv[i]);
					if (test == NULL){
						struct stat bf;
						lstat(argv[i], &bf);
						checking = file_check_(bf);
						if (checking == 'n'){
							continue;
						}
						else {
							fill[count_fill] = argv[i];
							count_fill++;
						}
					}
					else{
						fill[count_fill] = argv[i];
						count_fill++;
					}
				}
				cut = 0;
			}
			
			for (int i = 0; i < count_fill; i++){
				for (int j = 0; j < count_fill - 1 - i; j++){
					if (strcasecmp(fill[j], fill[j + 1]) > 0){ // sort file name ASCII
						char *temp = fill[j];
						fill[j] = fill[j + 1];
						fill[j + 1] = temp;
					}
				}
			}
			fprintf(fio, "<table border=\"1\"><tr><th>File Name</th></tr>"); // print head File Name
			for (int i = 0; i < count_fill; i++){
				DIR* dirp3;
				dirp3 = opendir(fill[i]);
				if (dirp3 == NULL){
					struct stat buf2;
					lstat(fill[i], &buf2);
					
					if (file_check_(buf2) == 'd'){
						fprintf(fio, "<tr style=\"color:blue\"><td>%s</td></tr><br>\n", fill[i]); // directory = blue
					}
					else if (file_check_(buf2) == 'l'){
						fprintf(fio, "<tr style=\"color:green\"><td>%s</td></tr><br>\n", fill[i]); // link = green
					}
					else {
						fprintf(fio, "<tr style=\"color:red\"><td>%s</td></tr><br>\n", fill[i]); // other = red
					}
					
				}
			}
			fprintf(fio, "</table>");
			for (int i = 0; i < count_fill; i++){
				DIR* dirp3;
				dirp3 = opendir(fill[i]);
				if (dirp3 != NULL){
					fprintf(fio, "%s : \n", fill[i]);
					ls_orig(dirp3, fill[i]);
					fprintf(fio, "<br>");
				}
			}


			return 0;
		}






		for (int i = 1+oc; i < argc; i++){ // first, checking non exist files
			lstat(argv[i], &st[i]);
			checking = file_check_(st[i]);
			if (checking == 'n'){ // no exist
				fprintf(fio, "cannot access '%s' : No such file or directory\n", argv[i]);
				arr[i] = 2; // 2 = no exist file
			}
		}
		for (int i = 1+oc; i < argc; i++){
			lstat(argv[i], &st[i]);
			checking = file_check_(st[i]);
			if (checking == 'n'){ // unknown file
				continue;
			}
			else if (checking != 'd'){ // second, print non directory files
				//printf("cannot access '%s': No such file or directory\n", argv[i]);
				arr[i] = 1;
				if (aflag == 0 && lflag == 0){
					fprintf(fio, "<table border=\"1\"><tr><th>File Name</th></tr>");
					if (argv[i][0] == '/'){ // absolute path
						fprintf(fio, "<tr><td><a href=\"%s\">%s</a></td></th>\n", argv[i], argv[i]); // make linking
					}
					else { // relative path
						fprintf(fio, "<tr><td><a href=\"%s/%s\">%s</a></td></th>\n", cwd, argv[i], argv[i]); // make linking
					}
					fprintf(fio, "</table>");
				}
				else if (aflag == 1 && lflag == 0){
					fprintf(fio, "<table border=\"1\"><tr><th>File Name</th></tr>");
					if (argv[i][0] == '/'){ // absolute path
						fprintf(fio, "<tr><td><a href=\"%s\">%s</a></td></th>\n", argv[i], argv[i]); // make linking
					}
					else { // relative path
						fprintf(fio, "<tr><td><a href=\"%s/%s\">%s</a></td></th>\n", cwd, argv[i], argv[i]); // make linking
					}
					fprintf(fio, "</table>");
				}
				else if (aflag == 0 && lflag == 1){
					lstat(argv[i], &st[i]);
					fprintf(fio, "<table border=\"1\">"); // make table
					fprintf(fio, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th><tr>"); // print contents
					fprintf(fio, "<tr>"); // make row
					fprintf(fio, "<td><a href=\"%s\">%s</a></td>\n", argv[i], argv[i]); // make linking
					print_ls(st[i]);
					fprintf(fio, "</tr>");//close row
				}
				else {
					lstat(argv[i], &st[i]);
					fprintf(fio, "<table border=\"1\">"); // make table
					fprintf(fio, "<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th><tr>"); // print contents
					fprintf(fio, "<tr>"); // make row
					fprintf(fio, "<td><a href=\"%s\">%s</a></td>\n", argv[i], argv[i]); // make linking
					print_ls(st[i]);
					fprintf(fio, "</tr>"); // close row
				}
			}
		}
			// else, directory path print
			if (aflag == 0 && lflag == 0){ // ls
			for (int i = 1+oc; i < argc; i++){
				if (arr[i] == 1 || arr[i] == 2){
				continue;
				}
				else if (oc == 0){ // no option
					dirp = opendir(".");
					ls(dirp, cwd, Sflag, rflag); // ls no option
				}
				else {
				dirp = opendir(argv[i]);
				if (argv[i] == ".\0"){
					fprintf(fio, "<h4>%s : </h4><br>\n", cwd); // print current working directory
				}
				else{
					fprintf(fio, "<h4>%s : </h4><br>\n", argv[i]); // header
				}
				ls(dirp, argv[i], Sflag, rflag); // ls no option
				}
			}
			}
			else if (aflag == 1 && lflag == 0){ // ls -a
			for (int i = 1+oc; i < argc; i++){
				if (arr[i] == 1 || arr[i] == 2){ // non-directory
				continue;
				}
				else{
				if (argc > 3){
					fprintf(fio, "<h4>%s : </h4><br>", argv[i]); // header
				}
				dirp = opendir(argv[i]);
					ls_a(dirp, argv[i], Sflag, rflag); // ls -a
				}
			}
			}
			else if (aflag == 0 && lflag == 1){ // ls -l
			for (int i = 1+oc; i < argc; i++){
				if (arr[i] ==  1 || arr[i] == 2){ // non-directory
				continue;
				}
				else {
				dirp = opendir(argv[i]); 
				ls_l(dirp, argv[i], hflag, Sflag, rflag); // ls -l
				}
			}         
		}
			else if (aflag == 1 && lflag == 1){ // ls -al
			// if (arr[i] == 1){
			//    continue;
			// }
			//else {
				if ((oc + 1) == argc){ // only one argument
				dirp = opendir(cwd);
				ls_al(dirp, cwd, hflag, Sflag, rflag);
				}
				if (oc == 1){
				for (int i = 2; i < argc; i++){
					if (arr[i] == 1 || arr[i] == 2){ // non-directory
						continue;
					}
					else {
						dirp = opendir(argv[i]);
						ls_al(dirp, argv[i], hflag, Sflag, rflag); // ls -al
					}
				}
				}
				else if (oc >= 2){
				for (int i = (1+oc); i < argc; i++){
					if (arr[i] == 1 || arr[i] == 2){ // non-directory
						continue;
					}
					else{
						dirp = opendir(argv[i]);
							ls_al(dirp, argv[i], hflag, Sflag, rflag); // ls -al
					}
				}
				}
				else {
				printf("Wrong options\n");
				}
			//}
		//}
		}
	}

			closedir(dirp);
			return 0;
	}
