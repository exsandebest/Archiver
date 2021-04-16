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
    void TestEncode1();
    void TestEncode2();
    void TestDecode1();
    void TestDecode2();

};

#endif // TEST_H
