#ifndef ARCHIVER_H
#define ARCHIVER_H

#include <QMainWindow>
#include <QObject>
#include <QProgressBar>
#include <Node.h>

class Archiver : public QObject
{
    Q_OBJECT
public:
    Archiver(QProgressBar *progressBar = nullptr);
    QString getPath(QString);
    QString cutPath(QString);
    QString getExtension(QString);
    void encode(QString);
    void decode(QString);

private:
    QString filePath;
    QProgressBar *progressBar;
    void reset();
    void encode();
    void decode();
    void buildTree();
    void buildTable(Node *);
    std::pair<QString, QString> err(const char *, const char *);

};

#endif // ARCHIVER_H
