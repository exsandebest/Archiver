#include "test.h"
#include "mainwindow.h"
#include <QFile>
#include "archiver.h"

Test::Test(QObject *parent) : QObject(parent) {}

void Test::TestEncode1() {
    QString testInputPath = testPath + "encode/a.txt";
    QString testExpectedOutputPath = testPath + "encode/a.txt.xxx.expected";
    QString testActualOutputPath = testPath + "encode/a.txt.xxx";

    Archiver a;
    a.currentFilePath = testInputPath;
    a.encode();

    QCOMPARE(fileChecksum(testActualOutputPath, QCryptographicHash::Sha256),
             fileChecksum(testExpectedOutputPath, QCryptographicHash::Sha256));

    QFile(testActualOutputPath).remove();
}

void Test::TestEncode2() {
    QString testInputPath = testPath + "encode/b.jpg";
    QString testExpectedOutputPath = testPath + "encode/b.jpg.xxx.expected";
    QString testActualOutputPath = testPath + "encode/b.jpg.xxx";

    Archiver a;
    a.currentFilePath = testInputPath;
    a.encode();

    QCOMPARE(fileChecksum(testActualOutputPath, QCryptographicHash::Sha256),
             fileChecksum(testExpectedOutputPath, QCryptographicHash::Sha256));

    QFile(testActualOutputPath).remove();
}

void Test::TestDecode1() {
    QString testInputPath = testPath + "decode/a.txt.xxx";
    QString testExpectedOutputPath = testPath + "decode/a.txt.expected";
    QString testActualOutputPath = testPath + "decode/a.txt";

    Archiver a;
    a.currentFilePath = testInputPath;
    a.decode();

    QCOMPARE(fileChecksum(testActualOutputPath, QCryptographicHash::Sha256),
             fileChecksum(testExpectedOutputPath, QCryptographicHash::Sha256));

    QFile(testActualOutputPath).remove();
}

void Test::TestDecode2() {
    QString testInputPath = testPath + "decode/b.jpg.xxx";
    QString testExpectedOutputPath = testPath + "decode/b.jpg.expected";
    QString testActualOutputPath = testPath + "decode/b.jpg";

    Archiver a;
    a.currentFilePath = testInputPath;
    a.decode();

    QCOMPARE(fileChecksum(testActualOutputPath, QCryptographicHash::Sha256),
             fileChecksum(testExpectedOutputPath, QCryptographicHash::Sha256));

    QFile(testActualOutputPath).remove();
}

QByteArray Test::fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm) {
    QFile f(fileName);
       if (f.open(QFile::ReadOnly)) {
           QCryptographicHash hash(hashAlgorithm);
           if (hash.addData(&f)) {
               return hash.result();
           }
       }
    return QByteArray();
}
