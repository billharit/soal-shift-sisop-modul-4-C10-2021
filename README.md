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
**4a**
**4b**
**4c**
**4d**
**4e**
**Kendala**
