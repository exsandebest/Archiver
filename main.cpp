#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QStringList>
#include <QTest>
#include "test.h"

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "RUS");
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QStringList args = QCoreApplication::arguments();
    const bool runTests = args.removeOne("--test");
    if (runTests) {
        return QTest::qExec(new Test, args);
    }
    MainWindow w;
    w.show();
    return app.exec();
}
