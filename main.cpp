#include "mainwindow.h"
#include <QCoreApplication>
#include <QTest>
#include <QApplication>
#include "test.h"

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "RUS");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    if (QCoreApplication::arguments().contains("--test")) {
        return QTest::qExec(new Test);
    }
    MainWindow w;
    w.show();
    return a.exec();
}
