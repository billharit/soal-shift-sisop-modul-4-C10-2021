# soal-shift-sisop-modul-4-C10-2021

### KELOMPOK : C10

| **No** | **Nama** | **NRP** | 
| ------------- | ------------- | --------- |
| 1 | Dicksen Alfersius Novian  | 05111940000076 | 
| 2 | Vyra Fania Adelina  | 05111940000109 |
| 3 | Bill Harit Yafi  | 05111940000114 |

### Soal 1
**1a** 
**1b**
**1c**
**1d**
**1e**
**Kendala**
### Soal 2
**2a**
**2b**
**2c**
**2d**
**2e**
**Kendala**
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


