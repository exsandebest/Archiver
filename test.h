#ifndef TEST_H
#define TEST_H

#include <QObject>
#include <QCryptographicHash>
#include <QTest>

class Test : public QObject
{
    Q_OBJECT
public:
    explicit Test(QObject *parent = 0);

private:
    QByteArray fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm);
    QString testPath = "../Archiver/tests/";

private slots:
    void TestEncodeTxt();
    void TestEncodeJpg();
    void TestEncodeXxx();
    void TestEncodeEmpty();
    void TestEncodeFileNotExists();
    void TestDecodeTxt();
    void TestDecodeJpg();
    void TestDecodeXxx();
    void TestDecodeEmpty();
    void TestDecodeFileNotExists();

};

#endif // TEST_H
