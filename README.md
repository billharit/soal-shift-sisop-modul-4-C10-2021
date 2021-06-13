# soal-shift-sisop-modul-4-C10-2021

### KELOMPOK : C10

| **No** | **Nama** | **NRP** | 
| ------------- | ------------- | --------- |
| 1 | Dicksen Alfersius Novian  | 05111940000076 | 
| 2 | Vyra Fania Adelina  | 05111940000109 |
| 3 | Bill Harit Yafi  | 05111940000114 |

### Soal 1
Pada soal 1, diminta untuk membuat filesystem yang dapat secara otomatis melakukan enkripsi pada direktori yang mengandung "AtoZ_", misalkan "AtoZ_test".
Ketika sebuah direktori di-rename menjadi direktori ter-encode, maka isi dari direktori tersebut akan di enkripsi dengan enkripsi atBash. Jika direktori tersebut diubah namanya menjadi tidak ter-encode, maka isi dari direktori tersebut akan kembali ke semula. Filesystem ini dapat berjalan secara rekursif, dapat mengubah file maupun direktori didalam direktori yang ter-encode. Berikut adalah kode untuk nomor 1 :

Fungsi untuk membantu jalannya fungsi enkripsi
```c
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

```
Fungsi enkripsi dan dekripsi atBash
```c
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
```
Fungsi enkripsi dan dekripsi secara rekursif
```c
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
```
Berikut adalah tankapan layar sebelum enkripsi :
![Soal 1 bofore-1](https://user-images.githubusercontent.com/65166398/121806251-51c66e00-cc79-11eb-970f-b8d06c04d07c.png)

![soal 1 before-2](https://user-images.githubusercontent.com/65166398/121806284-6b67b580-cc79-11eb-87e9-9b93528c420c.png)

Berikut adalah tankapan layar setelah enkripsi :
![Soal 1 after-1](https://user-images.githubusercontent.com/65166398/121806314-84706680-cc79-11eb-9be6-2ec6043e373f.png)

![Soal 1 after-2](https://user-images.githubusercontent.com/65166398/121806315-863a2a00-cc79-11eb-9d67-3615f81ad3b6.png)

**Kendala**
1. Melakukan rekursi dalam fungsi agar dapat mengakses seluruh file dan direktori.
2. Mengintegrasikan fungsi enkripsi dan dekripsi pada fungsi-fungsi fuse operations.
### Soal 2
Pada soal nomor 2 diminta untuk membuat metode enkripsi tambahan saat melakukan pembuatan direktori yang diawali dengan "RX_" maka dilakukan metode tambahan ROT13 dan saat melakukan rename pada sebuah direktori akan dilakukan metode tambahan Viginere

Fungsi untuk encrypt dan decrypt rot13

```c
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
```

encrypt dan decrypt viginere

```c
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
```

Fungsi untuk melakukan enkripsi dan dekripsi secara rekursif

```c
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
```

pada fungsi xmp_rename dan xmp_mkdir memanggil fungsi writeLog untuk mencatat log

**Kendala**
* fungsi dapat mendeteksi "RX_" tetapi belum bisa mengganti nama folder
* ubuntu tidak bisa nyala, tulisannya "BUG : soft lockup - CPU#0 stuck ..."

### Soal 3
**Kendala**
### Soal 4
Fungsi WriteLog untuk menulis log
```
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

    sprintf(res, "%s::%.2d%.2d%.4d-%.2d:%.2d:%.2d::%s::%s", flag, tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec, string, path);

    f = fopen(log_path, "a+");
    fprintf(f, "%s\n", res);
    fclose(f);
}
```
**4a** Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
![image](https://user-images.githubusercontent.com/77628684/121803475-08235680-cc6c-11eb-9d85-6451ec6bfcaf.png)

**4b** Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
![image](https://user-images.githubusercontent.com/77628684/121803508-3bfe7c00-cc6c-11eb-90ad-41c3204049e1.png)
* Dalam fungsi write log terdapat kondiisi check==0 yang berguna untuk membedakan warning dengan info.

![image](https://user-images.githubusercontent.com/77628684/121803670-e70f3580-cc6c-11eb-9755-2e08ebacec6e.png)

**4c** Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.
![image](https://user-images.githubusercontent.com/77628684/121803654-dced3700-cc6c-11eb-882d-8b88e2b7b89e.png)

**4d** Sisanya, akan dicatat pada level INFO.
![image](https://user-images.githubusercontent.com/77628684/121803695-fe4e2300-cc6c-11eb-9a32-34723aedd1f2.png)

**4e** Format untuk logging yaitu :
* [Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]
* Contoh : INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt
![image](https://user-images.githubusercontent.com/77628684/121803727-13c34d00-cc6d-11eb-83f5-101d29408d20.png)


**Kendala**
* Dalam demo terjadi kegagalan pada penulisan format. difungsi writelog 1 line dirubah menjadi berikut 
* dari `snprintf(res, "%s::%02d%02d%04d-%02d:%02d:%02d::%s::%s", flag, tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec, string, path);`
* menjadi `sprintf(res, "%s::%.2d%.2d%.4d-%.2d:%.2d:%.2d::%s::%s", flag, tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec, string, path);`


