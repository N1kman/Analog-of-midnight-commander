#include "currentdirectory.h"

//конструктор
CurrentDirectory::CurrentDirectory()
{
    files = NULL;
    count = 0;
    strncpy(path, "/", PATH_MAX);
}

//деструктор
CurrentDirectory::~CurrentDirectory(){
    clear(&(this->files));
}

//чтение каталога
void CurrentDirectory::read(){
    clear(&(this->files));
    read_directory(path, &(this->files), &(this->count));
}

//получения структур файлов
struct file* CurrentDirectory::getfiles(){
    return files;
}

//получение пути
char* CurrentDirectory::getpath(){
    return path;
}

//получение количества каталогов и файлов
int CurrentDirectory::getcount(){
    return count;
}

//сортировка каталога
void CurrentDirectory::sorting(){
    mysort(this->files, this->count);
}
