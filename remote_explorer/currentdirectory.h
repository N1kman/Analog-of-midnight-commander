#ifndef CURRENTDIRECTORY_H
#define CURRENTDIRECTORY_H

#include "functions.h"

//класс текущего каталога
class CurrentDirectory
{
public:
    //конструктор
    CurrentDirectory();
    //деструктор
    ~CurrentDirectory();
    //чтение каталога
    void read();
    //получения структур файлов
    struct file* getfiles();
    //получение количества каталогов и файлов
    int getcount();
    //получение пути
    char* getpath();
    //сортировка каталога
    void sorting();

private:
    //указатель на структуры файлов
    struct file* files;
    //количество файлов и каталогов
    int count;
    //полный путь
    char path[PATH_MAX];
};

#endif // CURRENTDIRECTORY_H
