#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "func_sockets.h"
#include "currentdirectory.h"
#include <QMainWindow>
#include <QStackedLayout>
#include <QKeyEvent>
#include <Qt>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QInputDialog>
#include <string>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//класс основного окна
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    //конструктор
    MainWindow(QWidget *parent = nullptr);
    //деструктор
    ~MainWindow();
    //подготовка к выводу
    QString toPrint(char* name_c, long size_c, char* date_c);

protected:
    //обработчик клавиатуры
    virtual void keyPressEvent(QKeyEvent *event);
    //открытие файла или каталога
    int open_something(QListWidget* list, CurrentDirectory* dir,
                        QListWidgetItem *item);
    //создание каталога
    void mymkdir();
    //вывод информации
    void info();
    //вывод прав доступа
    void printroots(char* roots_c, char * name);
    //создание каталога
    void update(QListWidget* list, CurrentDirectory* dir);
    //удаление файла
    void delete_file();
    //переименование
    void myrename();
    //перемещение файла либо каталога
    void move();
    //копрование файла либо каталога
    void copy();
    //подключение к серверу
    void connectright();
    //подключение к серверу
    void connectleft();

private slots:
    //обработчик клика кнопки F10
    void on_pushButton_A_clicked();
    //обработка двойного клика
    void on_listLeft_itemDoubleClicked(QListWidgetItem *item);
    //обработка двойного клика
    void on_listRight_itemDoubleClicked(QListWidgetItem *item);
    //вывод информации
    void on_pushButton_1_clicked();
    //вывод прав
    void on_pushButton_6_clicked();
    //установка фокуса
    void on_listLeft_itemClicked(QListWidgetItem *item);
    //установка фокуса
    void on_listRight_itemClicked(QListWidgetItem *item);
    //создание каталога
    void on_pushButton_5_clicked();
    //удаление файла
    void on_pushButton_4_clicked();
    //переименование
    void on_pushButton_7_clicked();
    //перемещение
    void on_pushButton_3_clicked();
    //копирование
    void on_pushButton_2_clicked();
    //подключение к серверу
    void on_pushButton_8_clicked();
    //подключение к серверу
    void on_pushButton_9_clicked();

private:
    //основной интерфейс
    Ui::MainWindow *ui;
    //текущий каталог левой панели
    CurrentDirectory curentleft;
    //текущий каталог правой панели
    CurrentDirectory curentright;

};
#endif // MAINWINDOW_H
