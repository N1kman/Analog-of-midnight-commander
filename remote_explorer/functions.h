#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <iostream>

using namespace std;

#define timelength 13
#define namelength 300

//структура файла
struct file {
    char name[namelength];        //имя
    long size;                    //размер
    bool is_directory;            //является ли каталогом
    char edittime[timelength];    //время модификации
    char roots[4];                //права на файл
};

//добавление имени файла к пути
void add_name(char* oldpath, char* newpath);
//чтение каталога
int read_directory(const char* path, struct file** files, int* count);
//открытие файла
int open(const char* str);
//очистка памяти
void clear(struct file** files);
//заход в каталог
void intodir(char* name, char* into);
//сортировка позырьком
void mysort(struct file* files, int count);
//смена местами строк
void swapstrings(char* string1, char* string2, int size);
//смена местами переменных типа лонг
void swaplong(long* long1, long* long2);
//смена местами переменных типа инт
void swapint(int* int1, int* int2);
//смена местами переменных типа бул
void swapbool(bool* bool1, bool* bool2);
//сохранение времени
void taketime(char* buff, char* time);
//создание директории
int makedirectory(const char *path);
//удаление файлов
int deletefile(const char* path);
//переименование
int rename_c(char* oldpath, char* newpath);
//перемещение каталога либо файла
int move_c(char *src_path, char *dst_path);
//копирование каталога либо файла
int copy_c(char *src_path, char *dst_path);

#endif // FUNCTIONS_H
