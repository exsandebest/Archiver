#include "test.h"
#include "mainwindow.h"
#include <QFile>
#include "archiver.h"

Test::Test(QObject *parent) : QObject(parent) {}

void Test::TestEncodeTxt() {
    QString testInputPath = testPath + "encode/a.txt";
    QString testExpectedOutputPath = testPath + "encode/a.txt.xxx.expected";
    QString testActualOutputPath = testPath + "encode/a.txt.xxx";

    Archiver a;
    a.encode(testInputPath);

    QCOMPARE(fileChecksum(testActualOutputPath, QCryptographicHash::Sha256),
             fileChecksum(testExpectedOutputPath, QCryptographicHash::Sha256));

    QFile(testActualOutputPath).remove();
}

void Test::TestEncodeJpg() {
    QString testInputPath = testPath + "encode/b.jpg";
    QString testExpectedOutputPath = testPath + "encode/b.jpg.xxx.expected";
    QString testActualOutputPath = testPath + "encode/b.jpg.xxx";

    Archiver a;
    a.encode(testInputPath);

    QCOMPARE(fileChecksum(testActualOutputPath, QCryptographicHash::Sha256),
             fileChecksum(testExpectedOutputPath, QCryptographicHash::Sha256));

    QFile(testActualOutputPath).remove();
}

void Test::TestEncodeXxx() {
    QString testInputPath = testPath + "encode/c.txt.xxx";
    try {
        Archiver a;
        a.encode(testInputPath);
        QVERIFY(false);
    }  catch (std::pair<QString, QString> errorPair) {
        QCOMPARE(errorPair.first, "Error");
        QCOMPARE(errorPair.second, "You can't encode '.xxx' files");
    }
}

void Test::TestEncodeEmpty() {
    QString testInputPath = "";
    try {
        Archiver a;
        a.encode(testInputPath);
        QVERIFY(false);
    }  catch (std::pair<QString, QString> errorPair) {
        QCOMPARE(errorPair.first, "Empty path");
        QCOMPARE(errorPair.second, "Please select a file");
    }
}

void Test::TestEncodeFileNotExists() {
    QString testInputPath = "x^313..--+412gg";
    try {
        Archiver a;
        a.encode(testInputPath);
        QVERIFY(false);
    }  catch (std::pair<QString, QString> errorPair) {
        QCOMPARE(errorPair.first, "No such file");
        QCOMPARE(errorPair.second, "File does not exist");
    }
}

void Test::TestDecodeTxt() {
    QString testInputPath = testPath + "decode/a.txt.xxx";
    QString testExpectedOutputPath = testPath + "decode/a.txt.expected";
    QString testActualOutputPath = testPath + "decode/a.txt";

    Archiver a;
    a.decode(testInputPath);

    QCOMPARE(fileChecksum(testActualOutputPath, QCryptographicHash::Sha256),
             fileChecksum(testExpectedOutputPath, QCryptographicHash::Sha256));

    QFile(testActualOutputPath).remove();
}

void Test::TestDecodeJpg() {
    QString testInputPath = testPath + "decode/b.jpg.xxx";
    QString testExpectedOutputPath = testPath + "decode/b.jpg.expected";
    QString testActualOutputPath = testPath + "decode/b.jpg";

    Archiver a;
    a.decode(testInputPath);

    QCOMPARE(fileChecksum(testActualOutputPath, QCryptographicHash::Sha256),
             fileChecksum(testExpectedOutputPath, QCryptographicHash::Sha256));

    QFile(testActualOutputPath).remove();
}

void Test::TestDecodeXxx() {
    QString testInputPath = testPath + "decode/c.txt";
    try {
        Archiver a;
        a.decode(testInputPath);
        QVERIFY(false);
    }  catch (std::pair<QString, QString> errorPair) {
        QCOMPARE(errorPair.first, "Error");
        QCOMPARE(errorPair.second, "You can decode only '.xxx' files");
    }
}

void Test::TestDecodeEmpty() {
    QString testInputPath = "";
    try {
        Archiver a;
        a.decode(testInputPath);
        QVERIFY(false);
    }  catch (std::pair<QString, QString> errorPair) {
        QCOMPARE(errorPair.first, "Empty path");
        QCOMPARE(errorPair.second, "Please select a file");
    }
}

void Test::TestDecodeFileNotExists() {
    QString testInputPath = "x^313..--+412gg";
    try {
        Archiver a;
        a.decode(testInputPath);
        QVERIFY(false);
    }  catch (std::pair<QString, QString> errorPair) {
        QCOMPARE(errorPair.first, "No such file");
        QCOMPARE(errorPair.second, "File does not exist");
    }
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
