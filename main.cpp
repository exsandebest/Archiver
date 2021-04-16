#include "mainwindow.h"
#include <QTest>
#include "test.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "RUS");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QTest::qExec(new Test, argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
