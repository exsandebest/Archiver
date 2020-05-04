#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


struct Node {
    bool b;
    unsigned char c;
    int k;
    Node * l,*r;
    Node(){
        l = r = nullptr;
        k = 0;
        b = false;
    }
    Node(Node * x, Node *y){
        l = x;
        r = y;
        k = x->k + y->k;
        b = false;
    }
    void clear(){
        clear(this);
    }
    void clear(Node * p){
        if (p == nullptr) return;
        clear(p->l);
        clear(p->r);
        delete p;
    }
};


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString getFileExtension(QString fn);
    void buildTree();
    void buildTable(Node * p);
    void buildTrueTable();
    void encode();
    void decode();
    QString cutExtension(QString);
    QString getPath(QString);
    QString cutPath(QString);
    QString cutName(QString);
    QString getExtension(QString);
private slots:
    void on_btnOpen_clicked();

    void on_btnEncode_clicked();

    void on_btnDecode_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
