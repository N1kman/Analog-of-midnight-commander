    #include "mainwindow.h"
#include "ui_mainwindow.h"

//конструктор
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //установка фокуса
    ui->pushButton_1->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_2->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_3->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_4->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_5->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_6->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_7->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_8->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_9->setFocusPolicy(Qt::NoFocus);
    ui->pushButton_A->setFocusPolicy(Qt::NoFocus);
    ui->listLeft->setFocusPolicy(Qt::TabFocus);
    ui->listRight->setFocusPolicy(Qt::TabFocus);

    //установка шрифта
    QFont font("Liberation Mono");
    ui->listLeft->setFont(font);
    ui->listRight->setFont(font);

    //чтение и сортировка
    this->curentleft.read();
    this->curentright.read();
    this->curentleft.sorting();
    this->curentright.sorting();

    if(chdir(this->curentright.getpath()) == -1){
        QMessageBox::critical(NULL, "Ошибка", "chdir не выполнен");
    }

    //вывод элементов
    for(int i = 0; i < this->curentleft.getcount(); ++i){
        ui->listLeft->addItem(toPrint((this->curentleft.getfiles())[i].name,
                                      (this->curentleft.getfiles())[i].size,
                                      (this->curentleft.getfiles())[i].edittime));

        ui->listRight->addItem(toPrint((this->curentright.getfiles())[i].name,
                                       (this->curentright.getfiles())[i].size,
                                       (this->curentright.getfiles())[i].edittime));
    }
    ui->listLeft->setCurrentRow(0);
}

//подготовка к выводу
QString MainWindow::toPrint(char* name_c, long size_c, char* date_c){
    QString name(name_c);  //имя файла или католога
    QString size;          //размер
    if(name == "/.."){
        size = ">Выше<";
    } else {
        size = size.number(size_c, 10);
    }
    QString date(date_c);  //дата модификации
    QString buff;          //буфер для вывода
    //подготовка буфера вывода
    if(name.size() > 19){
        for(int i = 0; i < 11 ; ++i){
            buff.append(name[i]);
        }
        buff.append('~');
        for(int i = name.size() - 7; i < name.size() ; ++i){
            buff.append(name[i]);
        }
    } else {
        buff = buff + name;
        while(buff.size() < 19)
            buff.append(' ');
    }
    for(int i = 13 - size.size(); i > 0; --i){
        buff.append(' ');
    }
    buff = buff + size;
    buff.append(' ');
    buff = buff + date;
    return buff;
}

//деструктор
MainWindow::~MainWindow(){
    delete ui;
}

//обработчик клавиатуры
void MainWindow::keyPressEvent(QKeyEvent *event){
    int key = event->key();

    //завершение работы
    if(key == Qt::Key_F10){
        QApplication::exit();
    }

    //вывод информации
    if(key == Qt::Key_F1){
        this->info();
    }

    //копирование файла либо каталога
    if(key == Qt::Key_F2){
        this->copy();
    }

    //перемещение файла либо каталога
    if(key == Qt::Key_F3){
        this->move();
    }

    //удаления файла
    if(key == Qt::Key_F4){
        this->delete_file();
    }

    //создание католога
    if(key == Qt::Key_F5){
        this->mymkdir();
    }

    //вывод прав
    if(key == Qt::Key_F6){
        if(ui->listLeft->hasFocus()){
            int index = ui->listLeft->currentRow();
            printroots(this->curentleft.getfiles()[index].roots,
                       this->curentleft.getfiles()[index].name);
        } else {
            int index = ui->listRight->currentRow();
            printroots(this->curentright.getfiles()[index].roots,
                       this->curentright.getfiles()[index].name);
        }
    }

    //переименование
    if(key == Qt::Key_F7){
        this->myrename();
    }

    //нажатие энтера
    if(key == Qt::Key_Return || key == Qt::Key_Enter){
        if(ui->listLeft->hasFocus()){
            if(open_something(ui->listLeft, &(this->curentleft), ui->listLeft->currentItem())){
                QMessageBox::critical(NULL, "Ошибка", "chdir не выполнен");
            }
        } else {
            if(open_something(ui->listRight, &(this->curentright), ui->listRight->currentItem())){
                QMessageBox::critical(NULL, "Ошибка", "chdir не выполнен");
            }
        }
    }

    //подключение к серверу
    if(key == Qt::Key_F8){
        connectleft();
    }

    //подключение к серверу
    if(key == Qt::Key_F9){
        connectright();
    }
}

//подключение к серверу
void MainWindow::connectright(){
    bool ok;
    char list[8192];
    //опрос пользователя
    QString IP = QInputDialog::getText(nullptr, "Подключение к серверу", "Введите ip-адрес:",
                                       QLineEdit::Normal, "127.0.0.1", &ok);
    if(getlistfromserver(list, IP.toStdString().c_str()) == -1){
        QMessageBox::critical(NULL, "Ошибка", "Сервер не доступен");
        return;
    }
    QString str = list;
    str = "Доступно:\n" + str + "Введите имя файла:";
    QString text = QInputDialog::getText(nullptr, "Подключение к серверу", str,
                                         QLineEdit::Normal, 0, &ok);
    if(chdir(this->curentright.getpath()) == -1){
        QMessageBox::critical(NULL, "Ошибка", "сhdir не выполнен");
        return;
    }
    if(!text.isEmpty() && ok){
        if(takefromserver(text.toStdString().c_str(), IP.toStdString().c_str()) == -1){
            QMessageBox::critical(NULL, "Ошибка", "Сервер или файл не доступен");
        }
        update(ui->listRight, &(this->curentright));
    }
}

//подключение к серверу
void MainWindow::connectleft(){
    bool ok;
    char list[8192];
    QString IP = QInputDialog::getText(nullptr, "Подключение к серверу", "Введите ip-адрес:",
                                       QLineEdit::Normal, "127.0.0.1", &ok);
    if(getlistfromserver(list, IP.toStdString().c_str()) == -1){
        QMessageBox::critical(NULL, "Ошибка", "Сервер не доступен");
        return;
    }
    //опрос пользователя
    QString str = list;
    str = "Доступно:\n" + str + "Ввод имени файла:";
    QString text = QInputDialog::getText(nullptr, "Подключение к серверу", str,
                                         QLineEdit::Normal, 0, &ok);
    if(chdir(this->curentleft.getpath()) == -1){
        QMessageBox::critical(NULL, "Ошибка", "chdir не выполнен");
        return;
    }
    if(!text.isEmpty() && ok){
        if(takefromserver(text.toStdString().c_str(), IP.toStdString().c_str()) == -1){
            QMessageBox::critical(NULL, "Ошибка", "Сервер или файл не доступен");
        }
        update(ui->listLeft, &(this->curentleft));
    }
}

//завершение работы
void MainWindow::on_pushButton_A_clicked(){
    QApplication::exit();
}

//удаление файла
void MainWindow::delete_file(){
    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    if(ui->listLeft->hasFocus()){
        //взятие текущего индекса
        int index = ui->listLeft->currentRow();
        QString name = this->curentleft.getfiles()[index].name;
        if(name == "/." || name == "/.."){
            QMessageBox::critical(NULL, "Ошибка", "не может быть удален");
        } else {
            //формирование опроса
            QString query = "Вы хотите удалить ";
            query += name;
            query += "?";
            msgBox.setText(query);
            int reply = msgBox.exec();

            if(reply == QMessageBox::Yes){
                char buff[PATH_MAX + 1];
                strncpy(buff, this->curentleft.getpath(), PATH_MAX);
                strncat(buff, this->curentleft.getfiles()[index].name, namelength);
                if(deletefile(buff) == -1){
                    QMessageBox::critical(NULL, "Ошибка", "не может быть удалено");
                }
            }
        }
    } else {
        //взятие текущего индекса
        int index = ui->listRight->currentRow();
        QString name = this->curentright.getfiles()[index].name;
        if(name == "/." || name == "/.."){
            QMessageBox::critical(NULL, "Ошибка", "не может быть удалено");
        } else {
            //формирование опроса
            QString query = "Вы хотите удалить ";
            query += name;
            query += "?";
            msgBox.setText(query);
            int reply = msgBox.exec();

            if(reply == QMessageBox::Yes){
                char buff[PATH_MAX + 1];
                strncpy(buff, this->curentright.getpath(), PATH_MAX);
                strncat(buff, this->curentright.getfiles()[index].name, namelength);
                if(deletefile(buff) == -1){
                    QMessageBox::critical(NULL, "Ошибка", "не может быть удалено");
                }
            }
        }
    }
    update(ui->listLeft, &(this->curentleft));
    update(ui->listRight, &(this->curentright));
}

//обработка двойного клика
void MainWindow::on_listLeft_itemDoubleClicked(QListWidgetItem *item)
{
    if(open_something(ui->listLeft, &(this->curentleft), item) == -1){
        QMessageBox::critical(NULL, "Ошибка", "chdir не выполнен");
    }
}

//обработка двойного клика
void MainWindow::on_listRight_itemDoubleClicked(QListWidgetItem *item)
{
    if(open_something(ui->listRight, &(this->curentright), item) == -1){
        QMessageBox::critical(NULL, "Ошибка", "chdir не выполнен");
    }
}

//открытие каталога
int MainWindow::open_something(QListWidget* list, CurrentDirectory* dir,
        QListWidgetItem *item){
    int row = list->row(item);
    if((dir->getfiles()[row]).is_directory){
        //проход каталога
        intodir(dir->getpath(), (dir->getfiles()[row]).name);
        if(chdir(dir->getpath()) == -1){
            return -1;
        }
        update(list, dir);
        list->setCurrentRow(0);
    } else {
        if(chdir(dir->getpath()) == -1){
            return -1;
        }
        //подготовка имени
        char fullpath[PATH_MAX];
        strncpy(fullpath, "./\"", 5);
        strncat(fullpath, (dir->getfiles())[row].name, namelength);
        strncat(fullpath, "\"", 3);
        //открытие файла
        if(open(fullpath) == -1){
            QMessageBox::critical(NULL, "Ошибка", "не открыто");
        }
    }
    return 0;
}

//перемещение файла либо каталога
void MainWindow::move(){
    bool ok;
    int index;
    char oldpath[PATH_MAX];
    QString str;
    QString res = "Переместить ";
    QString def;
    //получение элемента для перемещения
    if(ui->listLeft->hasFocus()){
        index = ui->listLeft->currentRow();
        str = this->curentleft.getpath();
        str += "/";
        str.append(this->curentleft.getfiles()[index].name);
        res.append(this->curentleft.getfiles()[index].name);
        def = this->curentright.getpath();
    } else {
        index = ui->listRight->currentRow();
        str = this->curentright.getpath();
        str += "/";
        str.append(this->curentright.getfiles()[index].name);
        res.append(this->curentright.getfiles()[index].name);
        def = this->curentleft.getpath();
    }
    strncpy(oldpath, str.toStdString().c_str(), PATH_MAX - 1);
    //опрос пользователя
    QString text = QInputDialog::getText(nullptr, res, "Введите новый путь:",
                                         QLineEdit::Normal, def, &ok);
    if(!text.isEmpty() && ok){
        char newpath[PATH_MAX];
        strncpy(newpath, text.toStdString().c_str(), PATH_MAX - 1);
        if(move_c(oldpath, newpath) == -1){
            QMessageBox::critical(NULL, "Ошибка", "Не может быть перемещено");
        } else {
            //обновление каталога
            update(ui->listLeft, &(this->curentleft));
            //обновление каталога
            update(ui->listRight, &(this->curentright));
        }
    }
}

//копрование файла либо каталога
void MainWindow::copy(){
    bool ok;
    int index;
    char oldpath[PATH_MAX];
    QString str;
    QString res = "Копировать ";
    QString def;
    //получение элемента для копирования
    if(ui->listLeft->hasFocus()){
        index = ui->listLeft->currentRow();
        str = this->curentleft.getpath();
        str += "/";
        str.append(this->curentleft.getfiles()[index].name);
        res.append(this->curentleft.getfiles()[index].name);
        def = this->curentright.getpath();
    } else {
        index = ui->listRight->currentRow();
        str = this->curentright.getpath();
        str += "/";
        str.append(this->curentright.getfiles()[index].name);
        res.append(this->curentright.getfiles()[index].name);
        def = this->curentleft.getpath();
    }
    strncpy(oldpath, str.toStdString().c_str(), PATH_MAX - 1);
    //опрос пользователя
    QString text = QInputDialog::getText(nullptr, res, "Введите новый путь:",
                                         QLineEdit::Normal, def, &ok);
    if(!text.isEmpty() && ok){
        char newpath[PATH_MAX];
        strncpy(newpath, text.toStdString().c_str(), PATH_MAX - 1);
        if(copy_c(oldpath, newpath) == -1){
            QMessageBox::critical(NULL, "Ошибка", "Не может быть скопировано");
        } else {
            //обновление каталога
            update(ui->listLeft, &(this->curentleft));
            //обновление каталога
            update(ui->listRight, &(this->curentright));
        }
    }
}

//создание каталога
void MainWindow::update(QListWidget* list, CurrentDirectory* dir){
    dir->read();
    dir->sorting();
    list->clear();
    for(int i = 0; i < dir->getcount(); ++i){
        list->addItem(toPrint((dir->getfiles())[i].name,
                              (dir->getfiles())[i].size,
                              (dir->getfiles())[i].edittime));
    }
}

//создание каталога
void MainWindow::mymkdir(){
    bool ok;
    //опрос пользователя
    QString text = QInputDialog::getText(nullptr, "MkDir", "Введите имя:",
                                         QLineEdit::Normal, "", &ok);
    if(!text.isEmpty() && ok){
        char path[PATH_MAX];
        strncpy(path, text.toStdString().c_str(), PATH_MAX - 1);
        if(makedirectory(path) == -1){
            QMessageBox::critical(NULL, "Ошибка", "Нет доступа либо каталог уже существует");
        } else {
            //обновление каталога
            update(ui->listLeft, &(this->curentleft));
            //обновление каталога
            update(ui->listRight, &(this->curentright));
        }
    }
}

//вывод прав доступа
void MainWindow::printroots(char* roots_c, char * name){
    QString result;
    QString roots = roots_c;
    result.append("хозяин группа другие\n");
    for(int i = 0; i < roots.size(); ++i){
        if(i == 0){
            result.append("   [");
        } else {
            result.append("      [");
        }
        if(roots[i] == '7'){
            result.append("rwx");
        }
        if(roots[i] == '6'){
            result.append("rw-");
        }
        if(roots[i] == '5'){
            result.append("r-x");
        }
        if(roots[i] == '4'){
            result.append("r--");
        }
        if(roots[i] == '3'){
            result.append("-wx");
        }
        if(roots[i] == '2'){
            result.append("-w-");
        }
        if(roots[i] == '1'){
            result.append("--x");
        }
        if(roots[i] == '0'){
            result.append("---");
        }
        result.append("]");
    }
    QMessageBox::information(nullptr, name, result);
}

//вывод информации
void MainWindow::info(){
    QString info;
    info.append("Tab - смена панели.\n");
    info.append("F1 - получение информации.\n");
    info.append("F2 - копировать файл.\n");
    info.append("F3 - переместить файл.\n");
    info.append("F4 - удалить файл.\n");
    info.append("F5 - создать каталог.\n");
    info.append("F6 - получить права.\n");
    info.append("F7 - переименовать.\n");
    info.append("F8 - получение ftp подключения к левой панели.\n");
    info.append("F9 - получение ftp подключения к правой панели.\n");
    info.append("F10 - выход.\n");
    QMessageBox::information(nullptr, "Помощь", info);
}

//вывод информации
void MainWindow::on_pushButton_1_clicked()
{
    info();
}

//вывод прав доступа
void MainWindow::on_pushButton_6_clicked()
{
    if(ui->listLeft->hasFocus()){
        int index = ui->listLeft->currentRow();
        printroots(this->curentleft.getfiles()[index].roots,
                   this->curentleft.getfiles()[index].name);
    } else {
        int index = ui->listRight->currentRow();
        printroots(this->curentright.getfiles()[index].roots,
                   this->curentright.getfiles()[index].name);
    }
}

//переименование
void MainWindow::myrename(){
    bool ok;
    int index;
    char oldpath[PATH_MAX];
    QString str;
    QString res;
    //получение элемента для переименования
    if(ui->listLeft->hasFocus()){
        index = ui->listLeft->currentRow();
        str = this->curentleft.getfiles()[index].name;
        if(str[0] == '/'){
            for(int i = 1; i < str.size(); ++i){
                res.append(str[i]);
            }
        } else {
            res = str;
        }
    } else {
        index = ui->listRight->currentRow();
        str = this->curentright.getfiles()[index].name;
        if(str[0] == '/'){
            for(int i = 1; i < str.size(); ++i){
                res.append(str[i]);
            }
        } else {
            res = str;
        }
    }
    str = "Переименовать  ";
    str.append(res);
    strncpy(oldpath, res.toStdString().c_str(), PATH_MAX - 1);
    //опрос пользователя
    QString text = QInputDialog::getText(nullptr, str, "Введите новое имя:",
                                         QLineEdit::Normal, "", &ok);
    if(!text.isEmpty() && ok){
        char newpath[PATH_MAX];
        strncpy(newpath, text.toStdString().c_str(), PATH_MAX - 1);
        if(rename_c(oldpath, newpath) == -1){
            QMessageBox::critical(NULL, "Ошибка", "Не может быть переименовано");
        } else {
            //обновление каталога
            update(ui->listLeft, &(this->curentleft));
            //обновление каталога
            update(ui->listRight, &(this->curentright));
        }
    }
}

//установка фокуса
void MainWindow::on_listLeft_itemClicked(QListWidgetItem *item)
{
    ui->listLeft->setFocus();
    ui->listLeft->setCurrentItem(item);
    if(chdir(this->curentleft.getpath()) == -1){
        QMessageBox::critical(NULL, "Ошибка", "Проблемы с приложением");
    }
}

//установка фокуса
void MainWindow::on_listRight_itemClicked(QListWidgetItem *item)
{
    ui->listRight->setFocus();
    ui->listRight->setCurrentItem(item);
    if(chdir(this->curentright.getpath()) == -1){
        QMessageBox::critical(NULL, "Ошибка", "Проблемы с приложением");
    }
}

//создание каталога
void MainWindow::on_pushButton_5_clicked()
{
    this->mymkdir();
}

//удаление файла
void MainWindow::on_pushButton_4_clicked()
{
    this->delete_file();
}

//переименование
void MainWindow::on_pushButton_7_clicked()
{
    this->myrename();
}

//перемещение
void MainWindow::on_pushButton_3_clicked()
{
    this->move();
}

//копирование
void MainWindow::on_pushButton_2_clicked()
{
    this->copy();
}

//подключение к серверу
void MainWindow::on_pushButton_8_clicked()
{
    this->connectleft();
}

//подключение к серверу
void MainWindow::on_pushButton_9_clicked()
{
    this->connectright();
}

