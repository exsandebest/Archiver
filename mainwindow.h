#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <map>

struct Node {
    bool b;
    unsigned char c;
    int k;
    Node *l, *r;

    Node() {
        l = r = nullptr;
        k = 0;
        b = false;
    }

    Node(Node *x, Node *y) {
        l = x;
        r = y;
        k = x->k + y->k;
        b = false;
    }

    void clear() { clear(this); }

    void clear(Node *p) {
        if (p == nullptr) return;
        clear(p->l);
        clear(p->r);
        delete p;
    }
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void buildTree();
    void buildTable(Node *p);
    void encode();
    void decode();
    QString getPath(QString);
    QString cutPath(QString);
    QString getExtension(QString);
    void on_btnOpen_clicked();
    void on_btnEncode_clicked();
    void on_btnDecode_clicked();
    void reset();
    QString currentFilePath = "";

private:
    Ui::MainWindow *ui;
    QProgressBar *progressBar;
    QLineEdit *linePath;

};
#endif  // MAINWINDOW_H
