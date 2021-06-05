#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
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



static  const  char *dirpath = "/home/dicksen/Downloads"; 
char *log_path = "/home/vyra/SinSeiFS.log"; //Path file log

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

    f = fopen("/home/vyra/SinSeiFS.log", "a+");
    fprintf(f, "%s\n", res);
    fclose(f);
}

//enkripsi atbash
void atBash_encrypt(char *path_file)
{
    int i, j;
    int path_length = strlen(path_file);
    char temp;
    int upper_lower = 0; // 0 artinya huruf kapital, 1 artinya huruf biasa
    if(strcmp(path_file,".") == 0 || strcmp(path_file, "..") == 0) return;
    for(i = 0; i < path_length; i++){
        if(!(path_file[i] == '/' || isalpha(path_file[i]) == 0)){
            temp = path_file[i];
            if(islower(temp)){
                temp -= 'a';
                upper_lower = 1;
            }
            else{
                temp -= 'A';
                upper_lower = 0;
            }
            //Atbash cipher
            temp = 25 - temp;
            if(upper_lower == 0){
                temp += 'A';
            }
            else temp += 'a';
            path_file[i] = temp;
        }
    }
}

//dekripsi atbash
void atBash_decrypt(char *path_file)
{
    int i, j;
    int path_length = strlen(path_file);
    char temp;
    int upper_lower = 0; // 0 artinya huruf kapital, 1 artinya huruf biasa
    if(strcmp(path_file,".") == 0 || strcmp(path_file, "..") == 0) return;
    for(i = 0; i < path_length; i++){
        if(!(path_file[i] == '/' || isalpha(path_file[i]) == 0)){
            temp = path_file[i];
            if(islower(temp)){
                temp -= 'a';
                upper_lower = 1;
            }
            else{
                temp -= 'A';
                upper_lower = 0;
            }
            //Atbash cipher
            temp = 25 - temp;
            if(upper_lower == 0){
                temp += 'A';
            }
            else temp += 'a';
            path_file[i] = temp;
        }
    }
}

static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    sprintf(fpath,"%s%s",dirpath,path);

    res = lstat(fpath, stbuf);

    if (res == -1) return -errno;

    return 0;
}



static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(fpath);

    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));

        if(res!=0) break;
    }

    closedir(dp);

    return 0;
}



static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;

        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;
    int fd = 0 ;

    (void) fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1) return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) res = -errno;

    close(fd);

    return res;
}

static int xmp_atbash_encode() 

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read,
};



int  main(int  argc, char *argv[])
{
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}
