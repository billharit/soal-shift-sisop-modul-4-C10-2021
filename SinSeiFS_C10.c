#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>


static const char *dirPath = "/home/dicksen/Downloads";
static const char *log_path = "/home/dicksen/SinSeiFS.log"; //Path file log
static const char *AtoZ = "AtoZ_";

// gcc -Wall `pkg-config fuse --cflags` modul4.c -o modul4 `pkg-config fuse --libs`
// ./modul4  shift4/
// fusermount -u shift4/


void writeLog(char *string, char *path, int check)
{
    char res[1024];
    char flag[10];

    FILE *f;

    if(check == 0)
    {
        strcpy(flag, "WARNING");
    }
    else
    {
        strcpy(flag, "INFO");
    }

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    snprintf(res, "%s::%02d%02d%04d-%02d:%02d:%02d::%s::%s", flag, tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec, string, path);

    f = fopen(log_path, "a+");
    fprintf(f, "%s\n", res);
    fclose(f);
}

/*
Mencari indeks '.' , '/'
type = 0    -> mencari indeks melakukan split berdasakan ada tidaknya '.'
type = 1    -> mencari indeks '.'
type = 2    -> mencari indeks '/', dengan tambahan parameter "end"
*/  
int index_special(char *path, int type, int end)
{
    if(type == 0 || type == 1)
    {
    int exist = 0;
    for(int i = strlen(path)-1; i>=0; i--){
        if(path[i]== '.'){
            //Mencari indeks untuk melakukan split berdasarkan
            //Ada tidaknya '.'
            if(type == 0){
                if(exist == 1)return i;
                else exist = 1;
            }
            //Hanya mencari indeks '.' yang pertama ditemukan
            else if(type == 1){
                return i;
            }
        }
    }
    //Jika tidak ditemukan tanda '.'
    return strlen(path);
    }
    else if(type == 2)
    {
        for(int i = 0; i<strlen(path); i++){
            if(path[i] == '/')return i+1;
        }
        return end;
    }
}

//enkripsi dan dekripsi atbash, 0 enkripsi, 1 dekripsi
void atBash_endecrypt(char *path_file, int type)
{
    if(strcmp(path_file, ".") == 0 || strcmp(path_file, "..")== 0)return;
    
    int start_index = 0;
    int end_index = index_special(path_file, 0, 0);
    if(end_index == strlen(path_file)) end_index = index_special(path_file, 1, 0);
    if(type == 0){
        printf("Enkripsi atbash dengan path : %s\n", path_file);
        start_index = index_special(path_file, 2, 0);
    }
    else if(type == 1){
        printf("Dekripsi atbash dengan path : %s\n", path_file);
        start_index = index_special(path_file, 2, end_index);
    }

    for (int i = start_index; i < end_index; i++){
		if (path_file[i] != '/' && isalpha(path_file[i])){
			char tmp = path_file[i];
			if(isupper(path_file[i])) tmp -= 'A';
			else tmp -= 'a';
			tmp = 25 - tmp; //Atbash cipher
			if(isupper(path_file[i])) tmp += 'A';
			else tmp += 'a';
			path_file[i] = tmp;
		}
	}
}

void enkripsi2(char *fpath)
{
    chdir(fpath);
	DIR *dp;
	struct dirent *dir;
	
	dp = opendir(".");
	if(dp == NULL) return;
	struct stat lol;
	char dirPath[2000];
	char filePath[2000];
	
    while ((dir = readdir(dp)) != NULL){
		printf("dirname %s\n", dir->d_name);
		printf("%s/%s\n", fpath, dir->d_name);
		if (stat(dir->d_name, &lol) < 0);
		else if (S_ISDIR(lol.st_mode)){
			if (strcmp(dir->d_name,".") == 0 || strcmp(dir->d_name,"..") == 0) continue;
			sprintf(dirPath,"%s/%s",fpath, dir->d_name);
			enkripsi2(dirPath);
			printf("dirPath %s\n", dirPath);
		}
		else{
			sprintf(filePath,"%s/%s",fpath,dir->d_name);
			FILE *input = fopen(filePath, "r");
			if (input == NULL) return;
			int bytes_read, count = 0;
			void *buffer = malloc(1024);
			while((bytes_read = fread(buffer, 1, 1024, input)) > 0){
				char newFilePath[1000];
				sprintf(newFilePath, "%s.%04d", filePath, count);
				count++;
				FILE *output = fopen(newFilePath, "w+");
				if(output == NULL) return;
				fwrite(buffer, 1, bytes_read, output);
				fclose(output);
				memset(buffer, 0, 1024);
			}
			fclose(input);
			printf("filepath %s\n", filePath);
			remove(filePath);
		}
	}
    closedir(dp);
}

void dekripsi2(char *dir)
{
	chdir(dir);
	DIR *dp;
	struct dirent *de;
	struct stat lol;
	dp = opendir(".");
	if (dp == NULL) return;
	
	char dirPath[1000];
	char filePath[1000];
	
	while ((de = readdir(dp)) != NULL){
		if (stat(de->d_name, &lol) < 0);
		else if (S_ISDIR(lol.st_mode)){
			if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
			sprintf(dirPath, "%s/%s", dir, de->d_name);
			dekripsi2(dirPath);
		}
		else{
			sprintf(filePath, "%s/%s", dir, de->d_name);
			filePath[strlen(filePath) - 5] = '\0';
			FILE *check = fopen(filePath, "r");
			if (check != NULL) return;
			FILE *file = fopen(filePath, "w");
			int count = 0;
			char topath[1000];
			sprintf(topath, "%s.%04d", filePath, count);
			void *buffer = malloc(1024);
			while (1){
				FILE *op = fopen(topath, "rb");
				if (op == NULL) break;
				size_t readSize = fread(buffer, 1, 1024, op);
				fwrite(buffer, 1, readSize, file);
				fclose(op);
				remove(topath);
				count++;
				sprintf(topath, "%s.%04d", filePath, count);
			}
			free(buffer);
			fclose(file);
		}
	}
	closedir(dp);
}


/*
    get name and extension
*/

void getDetail(const char *file, char *name, char *ext) {
    int index = 0;
    int i = 0;

    while(file[i]) {
        if (file[i] == '.') 
            break;
        
        name[index++] = file[i];
        i++;
    }

    name[index] = '\0';

    index = 0;
    while (file[i]) 
    {
        ext[index++] = file[i];
        i++;
    }

    ext[index] = '\0';
}

/*
    if isRX
*/

bool isRX(char *string)
{
    for(int i = 0; i < 3; i++)
    {
        if(string[0] == "R" || string[1] == "X" || string[2] == "_")
        {
            return true;
        }
    }

    return false;
}

/*
    ROT13
*/

void encROT13(char *string) {
    for (int i = 0; string[i]; i++) 
    {
        if ('A' <= string[i] && string[i] <= 'Z') 
            string[i] = ((string[i] - 'A' + 13 ) % 26 ) + 'A';
        else if ('a' <= string[i] && string[i] <= 'z') 
            string[i] = ((string[i] - 'a' + 13 ) % 26 ) + 'a';
    }
}

void decROT13(char *string)
{
    for (int i = 0; string[i]; i++) 
    {
        if ('A' <= string[i] && string[i] <= 'Z') 
            string[i] = ((string[i] - 'A' - 13) % 26) + 'A';
        else if ('a' <= string[i] && string[i] <= 'z') 
            string[i] = ((string[i] - 'a' - 13) % 26) + 'a';
    }
}

/*
    VIGINERE
*/

void encVig(char *string) {
    char key[] = "SISOP";    
    
    // Encode Viginere Cipher
    for (int i = 0; string[i]; i++) 
    {
        if ('A' <= string[i] && string[i] <= 'Z') 
            string[i] = ((string[i]-'A'+(key[i%((sizeof(key)-1))]-'A'))%26)+'A';
        else if ('a' <= string[i] && string[i] <= 'z') 
            string[i] = ((string[i]-'a'+(key[i%((sizeof(key)-1))]-'A'))%26)+'a';
    }
}

void decVig(char *string)
{
    char key[] = "SISOP";
    // Decode Viginere Cipher
    for (int i = 0; string[i]; i++) 
    {
        if ('A' <= string[i] && string[i] <= 'Z') 
            string[i] = ((string[i]-'A'-(key[i%((sizeof(key)-1))]-'A')+26)%26)+'A';
        else if ('a' <= string[i] && string[i] <= 'z') 
            string[i] = ((string[i]-'a'-(key[i%((sizeof(key)-1))]-'A')+26)%26)+'a';
    }
}

/*
    FOLDER RX_
*/

int mkdir_folderRX(const char *path, const char* name, int m)
{
    char enc_name[512];
    strcpy(enc_name, name);

    if(m == 0) //encode
    {
        atBash_endecrypt(path, 0);
        encROT13(enc_name);
    }
    else
    {
        decROT13(enc_name);
        atBash_endecrypt(path, 1);
    }

    char fpath[1024], tpath[1024];
    sprintf(fpath, "%s/%s", path, name);
    sprintf(tpath, "%s/%s", path, enc_name);

    int res = rename(fpath, tpath);

    if(res == -1)
        return -errno;

    return 0;
} 

int rename_folderRX(const char *path, const char* name, int m)
{
    char enc_name[512];
    strcpy(enc_name, name);

    if(m == 0) //encode
    {
        atBash_endecrypt(path, 0);
        encVig(enc_name);
    }
    else
    {
        decVig(enc_name);
        atBash_endecrypt(path, 1);
    }

    char fpath[1024], tpath[1024];
    sprintf(fpath, "%s/%s", path, name);
    sprintf(tpath, "%s/%s", path, enc_name);

    int res = rename(fpath, tpath);

    if(res == -1)
        return -errno;

    return 0;
} 

/*
    FILE RX_
*/

int mkdir_fileRX(char *path, char *name, int m)
{
    char file[1024], ext[64];
    getDetail(name, file, ext);

    if(m == 0)
    {
        //atbash
        encROT13(file);
    }
    else
    {
        decROT13(file);
        //atbash
    }

    char fpath[1024], tpath[1024];
    sprintf(fpath, "%s/%s", path, name);
    sprintf(tpath, "%s/%s", path, file);

    int res = rename(fpath, tpath);

    if(res == -1)
        return -errno;

    return 0; 
}

int rename_fileRX(char *path, char *name, int m)
{
    char file[1024], ext[64];
    getDetail(name, file, ext);

    if(m == 0)
    {
        //atbash
        encVig(file);
    }
    else
    {
        decVig(file);
        //atbash
    }

    char fpath[1024], tpath[1024];
    sprintf(fpath, "%s/%s", path, name);
    sprintf(tpath, "%s/%s", path, file);

    int res = rename(fpath, tpath);

    if(res == -1)
        return -errno;

    return 0; 
}

/*
    RX_ RECURSIVE
*/

int mkdir_RXrec(char *path, int check)
{
    char p[1024];

    struct dirent *dp;
    DIR *dir = opendir(path);

    if(!dir) return 0;

    int count = 0;

    while((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        
        strcpy(p, path);
        strcat(p, "/");
        strcat(p, dp->d_name);

        struct stat path_stat;
        stat(p, &path_stat);

        if(check == 0) //encrypt
        {
            if(!(S_ISREG(path_stat.st_mode)))
            {
                count += mkdir_RXrec(p, 0);
                mkdir_folderRX(path, dp->d_name, 0);
            }
            else
            {
                if(mkdir_fileRX(path, dp->d_name, 0) == 0)
                    count++;
            }
        }
        else //decrypt
        {
            if(!S_ISREG(path_stat.st_mode))
            {
                count += mkdir_RXrec(p, 1);
                mkdir_folderRX(path, dp->d_name, 1);
            }
            else
            {
                if(mkdir_fileRX(path, dp->d_name, 1) == 0)
                    count++;
            }
        }

        closedir(dir);
        return count;
    }
}

int rename_RXrec(char *path, int depth, int check)
{
    char p[1024];

    struct dirent *dp;
    DIR *dir = opendir(path);

    if(!dir) return 0;

    int count = 0;

    while((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        
        strcpy(p, path);
        strcat(p, "/");
        strcat(p, dp->d_name);

        struct stat path_stat;
        stat(p, &path_stat);

        if(check == 0) //encrypt
        {
            if(!S_ISREG(path_stat.st_mode))
            {
                if(depth > 1)
                {
                    count += rename_RXrec(p, depth - 1, 0);
                    rename_folderRX(path, dp->d_name, 0);
                }   
            }
            else
            {
                if(rename_fileRX(path, dp->d_name, 0) == 0)
                    count++;
            }
        }
        else //decrypt
        {
            if(!S_ISREG(path_stat.st_mode))
            {
                if(depth > 1)
                {
                    count += rename_RXrec(p, depth - 1, 1);
                    rename_folderRX(path, dp->d_name, 1);
                }   
            }
            else
            {
                if(rename_fileRX(path, dp->d_name, 1) == 0)
                    count++;
            }
        }

        
        closedir(dir);
        return count;
    }
}

static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
	char fpath[2000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a,1);


	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = lstat(fpath, stbuf);
	if (res == -1) return -errno;
	return 0;
}



static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a,1);
	
	

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	writeLog("READDIR", fpath, 1);

	int res = 0;
	DIR *dp;
	struct dirent *de;

	(void)offset;
	(void)fi;

	dp = opendir(fpath);
	if (dp == NULL) return -errno;

	while ((de = readdir(dp)) != NULL){
		if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (a != NULL) atBash_endecrypt(de->d_name,0);
	
		res = (filler(buf, de->d_name, &st, 0));
		if (res != 0) break;
	}

	closedir(dp);

	return 0;
}



static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a,1);
	
	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	int res = 0;
	int fd = 0;

	(void)fi;
	writeLog("READ", fpath, 1);

	fd = open(fpath, O_RDONLY);
	if (fd == -1) return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);
	return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = mkdir(fpath, mode);
	writeLog("MKDIR", fpath, 1);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	
	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = rmdir(fpath);
	writeLog("RMDIR", fpath, 0);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];

    if(strcmp(from, "/") == 0)
    {
        from = dirPath;
        sprintf(frompath, "%%s", from);
    }
    else
    {
        sprintf(frompath, "%s%s", dirPath, from);
    }

    if(strcmp(to, "/") == 0)
    {
        to = dirPath;
        sprintf(topath, "%s", to);
    }
    else
    {
        sprintf(topath, "%s%s", dirPath, to);
    }
	
	char *a = strstr(to, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);

    //folder
    struct stat path_stat;
    stat(frompath, &path_stat);

    if(!S_ISREG(path_stat.st_mode))
    {
        if(isRX(from) && !isRX(to))
        {
            rename_RXrec(frompath, 1000, 1);
        }
        else if(!isRX(from) && isRX(to))
        {
            rename_RXrec(frompath, 1000, 0);
        }
    }

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = rename(frompath, topath);
	if (res == -1) return -errno;

    ///Fungsi log disini
	char p[1024];
    sprintf(p, "%s::%s", frompath, topath);
    writeLog("RENAME", p, 1);

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);


	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	writeLog("TRUNC", fpath, 1);
	res = truncate(fpath, size);
	
	if (res == -1) return -errno;
	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	


	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	(void)fi;
	writeLog("WRITE", fpath, 1);

	fd = open(fpath, O_WRONLY);
	if (fd == -1) return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1) res = -errno;

	close(fd);
	return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	(void)fi;
	writeLog("CREATE", fpath, 1);
	
	res = creat(fpath, mode);	
	if (res == -1) return -errno;

	close(res);
	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);


	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	
	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = access(fpath, mask);
	if (res == -1) return -errno;
	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	
	

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = open(fpath, fi->flags);

	writeLog("OPEN", fpath, 1);
	if (res == -1) return -errno;

	close(res);
	return 0;
}

static int xmp_unlink(const char *path)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	


	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = unlink(fpath);
	writeLog("UNLINK", fpath, 1);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	


	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = readlink(fpath, buf, size - 1);
	writeLog("READLINK", fpath, 1);
	if (res == -1) return -errno;

	buf[res] = '\0';
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	
	
	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	writeLog("MKNOD", fpath, 1);
	
	/* On Linux this could just be 'mknod(path, mode, rdev)' but this is more portable */
	if (S_ISREG(mode)){
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0) res = close(res);
	}
	else if (S_ISFIFO(mode)) res = mkfifo(fpath, mode);
	else res = mknod(fpath, mode, rdev);
	
	if (res == -1) return -errno;
	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];
	
	char *a = strstr(to, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);

	


	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = symlink(frompath, topath);
	
    ///Tulis log dengan frompath & toPath
    char p[1024];
    sprintf(p, "%s::%s", frompath, topath);
    writeLog("SYMLINK", p, 1);
	
	if (res == -1) return -errno;
	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];
	
	char *a = strstr(to, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	
	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = link(frompath, topath);
	
    ///Tulis log dengan frompath dan topath
    char p[1024];
    sprintf(p, "%s::%s", frompath, topath);
    writeLog("LINK", p, 1);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	
	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = chmod(fpath, mode);
	writeLog("CHMOD", fpath, 1);

	if (res == -1) return -errno;
	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);
	

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = lchown(fpath, uid, gid);
	writeLog("CHOWN", fpath, 1);
	
	if (res == -1) return -errno;
	return 0;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	char fpath[1000];
	
	char *a = strstr(path, AtoZ);
	if (a != NULL) atBash_endecrypt(a, 1);


	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}
	else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = statvfs(fpath, stbuf);
	write("STATFS", fpath, 1);
	
	if (res == -1) return -errno;
	return 0;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
	.mkdir = xmp_mkdir,
	.rmdir = xmp_rmdir,
	.rename = xmp_rename,
	.truncate = xmp_truncate,
    .write = xmp_write,
	.create = xmp_create,
	.utimens = xmp_utimens,
	.access = xmp_access,
	.open = xmp_open,
	.unlink = xmp_unlink,
	.readlink = xmp_readlink,
	.mknod = xmp_mknod,
	.symlink = xmp_symlink,
	.link = xmp_link,
	.chmod = xmp_chmod,
	.chown = xmp_chown,
	.statfs = xmp_statfs,
};



int main(int  argc, char *argv[])
{
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}
