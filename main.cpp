#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QTest>
#include "test.h"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RUS");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    if (QCoreApplication::arguments().contains("--test")) {
        return QTest::qExec(new Test, argc, argv);
    }
    MainWindow w;
    w.show();
    return app.exec();
}
