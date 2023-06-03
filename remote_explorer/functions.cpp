#include "functions.h"

//открытие файла
int open(const char* str){
    char buff[PATH_MAX];
    strncpy(buff, "xdg-open ", PATH_MAX);
    strncat(buff, str, PATH_MAX - 1);
    if(system(buff) == -1){
        return -1;
    } else {
        return 0;
    }
}

//сортировка позырьком
void mysort(struct file* files, int count){
    for (int i = 0; i < count - 1; ++i) {
        for (int j = 1; j < count - i - 1; ++j) {
            //алфавитное сравнение строк
            if ((strcoll(files[j].name, files[j + 1].name) > 0
                    && ((!files[j].is_directory && !files[j + 1].is_directory)
                    || (files[j].is_directory && files[j + 1].is_directory)))
                    || (!files[j].is_directory && files[j + 1].is_directory)) {
                swapstrings(files[j].name, files[j + 1].name, namelength);
                swapstrings(files[j].edittime, files[j + 1].edittime, timelength);
                swapbool(&(files[j].is_directory), &(files[j + 1].is_directory));
                swaplong(&(files[j].size), &(files[j + 1].size));
                swapstrings(files[j].roots, files[j + 1].roots, 4);
            }
        }
    }
}

//смена местами переменных типа бул
void swapbool(bool* bool1, bool* bool2){
    bool buff = (*bool1);
    (*bool1) = (*bool2);
    (*bool2) = buff;
}

//смена местами переменных типа инт
void swapint(int* int1, int* int2){
    int buff = (*int1);
    (*int1) = (*int2);
    (*int2) = buff;
}

//смена местами переменных типа лонг
void swaplong(long* long1, long* long2){
    long buff = (*long1);
    (*long1) = (*long2);
    (*long2) = buff;
}

//смена местами строк
void swapstrings(char* string1, char* string2, int size){
    char buff[size];
    strncpy(buff, string1, size);
    strncpy(string1, string2, size);
    strncpy(string2, buff, size);
}

//чтение каталога
int read_directory(const char* path, struct file** files, int* count) {
    struct dirent* entry;  //указатель на структуру файла
    (*count) = 1;          //количество файлов и каталогов
    (*files) = NULL;       //структуры файлов и каталогов
    DIR* dir = NULL;       //указатель на структуру каталога

    //добавление каталога ..
    (*files) = (struct file*)realloc((*files),
        (*count) * sizeof(struct file));
    struct file* file = &(*files)[(*count) - 1];
    //занесение имени и прав доступа
    strncpy(file->name, "/..", 4);
    file->is_directory = true;
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", path, "..");
    struct stat st;
    stat(full_path, &st);
    file->size = st.st_size;
    taketime(file->edittime, ctime(&(st.st_ctim.tv_sec)));
    snprintf(file->roots, 4, "%o", st.st_mode & 0777);

    if ((dir = opendir(path)) == NULL) {
        return -1;
    }

    //чтение каталога
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0
                && strcmp(entry->d_name, "..") != 0) {
            //добавление элемента
            (*count)++;
            (*files) = (struct file*)realloc((*files),
                (*count) * sizeof(struct file));
            struct file* file = &(*files)[(*count) - 1];

            //занесение имени и прав доступа
            char full_path[PATH_MAX];
            snprintf(full_path, PATH_MAX, "%s/%s", path, entry->d_name);
            struct stat st;
            stat(full_path, &st);
            taketime(file->edittime, ctime(&(st.st_ctim.tv_sec)));
            file->is_directory = (S_ISDIR(st.st_mode));
            snprintf(file->roots, 4, "%o", st.st_mode & 0777);

            //занесение размера
            if (!file->is_directory) {
                strncpy(file->name, entry->d_name, 256);
                file->size = st.st_size;
            } else {
                strncpy(file->name, "/", 2);
                strncat(file->name, entry->d_name, 256);
                file->size = st.st_size;
            }
        }
    }

    closedir(dir);
    return 0;
}

//создание директории
int makedirectory(const char* path){
    return mkdir(path, S_IRWXG | S_IRWXU | S_IROTH | S_IXOTH);
}

//сохранение времени
void taketime(char* buff, char* time){

    for(int i = 4; i <= 15; ++i){
        buff[i - 4] = time[i];
    }
    buff[12] = 0;
}

//очистка памяти
void clear(struct file** files){
    free(*files);
}

//заход в каталог
void intodir(char* name, char* into){
    if(strcmp(into, "/..") == 0 && strcmp(name, "/") != 0){
        for(int i = strlen(name); i >= 0; i--){
            if(name[i] == '/'){
                name[i] = 0;
                break;
            }
        }
    } else if(strcmp(into, "/..") != 0) {
        strncat(name, into, PATH_MAX);
    }
}

//переименование
int rename_c(char* oldpath, char* newpath){
    return rename(oldpath, newpath);
}

//удаление файлов
int deletefile(const char* path){
    struct stat st;
    //чтение структуры стат
    if (stat(path, &st) != 0) {
        return -1;
    }
    //определение типа файла
    if (S_ISDIR(st.st_mode)) {
        DIR* dir = opendir(path);

        if (dir == NULL) {
            return -1;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
                char full_path[PATH_MAX];
                snprintf(full_path, PATH_MAX, "%s/%s", path, entry->d_name);
                deletefile(full_path);
            }
        }
        //закрытие и удаление
        closedir(dir);
        rmdir(path);
    } else {
        //удаление
        if(unlink(path) == -1){
            return -1;
        }
    }

    return 0;
}

//перемещение каталога либо файла
int move_c(char *src_path, char *dst_path) {
    DIR *dir;
    struct dirent *entry;
    char src_file[PATH_MAX + 1];
    char dst_file[PATH_MAX + 1];
    struct stat statbuf;

    //получение нового пути
    strncpy(dst_file, dst_path, PATH_MAX);
    add_name(src_path, dst_file);

    // получение информации о файле
    if(stat(src_path, &statbuf) == -1){
        return -1;
    }

    // если это каталог, то рекурсивно перемещаем его содержимое
    if (S_ISDIR(statbuf.st_mode)) {
        // создание нового каталога
        if(mkdir(dst_file, 0777) == -1){
            return -1;
        }

        // открытие исходного каталога
        if ((dir = opendir(src_path)) == NULL) {
            return -1;
        }

        // переместить все файлы и подкаталоги из исходного каталога в новый каталог
        while((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                sprintf(src_file, "%s/%s", src_path, entry->d_name);
                if(move_c(src_file, dst_file) == -1){
                    return -1;
                }
            }
        }

        closedir(dir);
        // удаление исходного каталога
        rmdir(src_path);
    } else {
        // если это файл, то перемещаем его
        if(rename(src_path, dst_file) == -1){
            return -1;
        }
    }
    return 0;
}

//добавление имени файла к пути
void add_name(char* oldpath, char* newpath){
    string stroldpath = oldpath;
    string name = "";
    int i = stroldpath.length() - 1;
    //выделение имени
    while(stroldpath[i + 1] != '/'){
        name += stroldpath[i--];
    }
    string reversename = "";
    i = name.length() - 1;
    //реверс имени
    while(i >= 0){
        reversename += name[i--];
    }
    strncat(newpath, reversename.c_str(), 255);
}

//копирование каталога либо файла
int copy_c(char *src_path, char *dst_path){
    DIR *dir;
    struct dirent *entry;
    char src_file[PATH_MAX + 1];
    char dst_file[PATH_MAX + 1];
    struct stat statbuf;

    //получение нового пути
    strncpy(dst_file, dst_path, PATH_MAX);
    add_name(src_path, dst_file);

    if(strcmp(src_path, dst_file) == 0){
        return -1;
    }

    // получение информации о файле
    if(stat(src_path, &statbuf) == -1){
        return -1;
    }

    // если это каталог, то рекурсивно перемещаем его содержимое
    if (S_ISDIR(statbuf.st_mode)) {
        // создание нового каталога
        if(mkdir(dst_file, 0777) == -1){
            return -1;
        }

        // открытие исходного каталога
        if ((dir = opendir(src_path)) == NULL) {
            return -1;
        }

        // переместить все файлы и подкаталоги из исходного каталога в новый каталог
        while((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                sprintf(src_file, "%s/%s", src_path, entry->d_name);
                if(copy_c(src_file, dst_file) == -1){
                    return -1;
                }
            }
        }

        closedir(dir);
    } else {
        // если это файл, то копируем его
        if(link(src_path, dst_file) == -1){
            return -1;
        }
    }
    return 0;
}
