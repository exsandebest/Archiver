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
    void buildTree();
    void buildTable(Node *p);
    void encode();
    void decode();
    QString getPath(QString);
    QString cutPath(QString);
    QString getExtension(QString);
    void reset();
    QString currentFilePath = "";
    QProgressBar *progressBar;

};

#endif // ARCHIVER_H
