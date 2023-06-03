#include "mainwindow.h"
#include <QApplication>
#include <QFile>

//старт программы
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //настройка стиля
    QFile stylesheetFile(":/new/prefix1/Perstfic.qss");
    stylesheetFile.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(stylesheetFile.readAll());
    a.setStyleSheet(styleSheet);

    //настройка главного окна
    MainWindow w;
    w.setWindowTitle("Remote Explorer");
    w.setMinimumHeight(600);
    w.setMaximumHeight(600);
    w.setMinimumWidth(950);
    w.setMaximumWidth(950);
    w.show();
    return a.exec();
}
