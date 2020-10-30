#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <fstream>
#include <QMessageBox>
using namespace std;

const QString SEPARATOR = "/";
map<unsigned char, int> arr;
map<unsigned char, vector<bool>> table;
map<unsigned char, QString> stringTable;
vector<bool> code;
QString currentFilePath = "";
Node *root;

bool comp(Node *&a, Node *&b) { return a->k < b->k; }

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setFixedSize(410, 180);
    ui->progressBar->hide();
    ui->lbl_process->hide();
}

MainWindow::~MainWindow() { delete ui; }

QString MainWindow::getPath(QString str) {
    return str.left(str.lastIndexOf(SEPARATOR));
}

QString MainWindow::cutPath(QString str) {
    return str.right(str.size() - str.lastIndexOf(SEPARATOR) - 1);
}

QString MainWindow::getExtension(QString fn) {
    return fn.mid(fn.lastIndexOf(".") + 1).toLower();
}

void MainWindow::on_btnOpen_clicked() {
    currentFilePath = QFileDialog::getOpenFileName(this, "Directory Dialog", "");
    ui->linePath->setText(currentFilePath);
}

void MainWindow::buildTree() {
    QVector<Node *> v;
    map<unsigned char, int>::iterator k;
    for (k = arr.begin(); k != arr.end(); ++k) {
        Node *p = new Node();
        p->c = k->first;
        p->k = k->second;
        p->b = true;
        v.push_back(p);
    }

    if (v.size() == 1) {
        Node *q = new Node();
        q->l = new Node();
        q->l->b = true;
        q->l->c = v.front()->c;
        q->l->k = v.front()->k;
        root = q;
    } else {
        while (v.size() != 1) {
            sort(v.begin(), v.end(), comp);
            Node *tl, *tr;
            tl = v.front();
            v.pop_front();
            tr = v.front();
            v.pop_front();
            Node *tmp = new Node();
            tmp->l = tl;
            tmp->r = tr;
            tmp->k = tl->k + tr->k;
            v.push_back(tmp);
        }

        root = v.front();
    }
}

void MainWindow::buildTable(Node *p) {
    if (p->l != nullptr) {
        code.push_back(0);
        buildTable(p->l);
    }

    if (p->r != nullptr) {
        code.push_back(1);
        buildTable(p->r);
    }

    if (p->b) table[p->c] = code;
    code.pop_back();
}

void MainWindow::buildStringTable() {
    map<unsigned char, vector<bool>>::iterator k;
    QString s = "";
    for (k = table.begin(); k != table.end(); ++k) {
        s = "";
        for (int i = 0; i < k->second.size(); ++i) {
            if (k->second[i]) {
                s += "1";
            } else {
                s += "0";
            }
        }

        stringTable[k->first] = s;
    }
}

void MainWindow::encode() {
    ui->lbl_process->setText("Encoding...");
    ui->lbl_process->show();
    ui->progressBar->setMaximum(0);
    ui->progressBar->setValue(0);
    ui->progressBar->show();
    QApplication::processEvents();
    root->clear();
    root = nullptr;
    arr.clear();
    table.clear();
    stringTable.clear();
    code.clear();
    char c;
    unsigned char uc;
    ifstream fiin(currentFilePath.toStdString(), ios::binary);
    while (!fiin.eof()) {
        fiin.read(&c, sizeof(char));
        uc = (unsigned char)c;
        QApplication::processEvents();
        ++arr[uc];
    }

    --arr[uc];
    buildTree();
    buildTable(root);
    buildStringTable();
    map<unsigned char, int>::iterator h;
    QString str = "";
    QString newName(currentFilePath + ".xxx");
    ofstream fout(newName.toStdString(), ios::binary);
    fout << (unsigned char)(cutPath(currentFilePath).length());
    fout << cutPath(currentFilePath).toStdString();
    fout << (unsigned char)(arr.size() >> 8);
    fout << (unsigned char)(arr.size());
    map<unsigned char, int>::iterator it;
    for (it = arr.begin(); it != arr.end(); ++it) {
        unsigned char charSelf = it->first;
        fout << charSelf;
        fout << (unsigned char)(table[it->first].size());
        QString tmpS = "";
        if (table[it->first].size() % 8) {
            tmpS.fill('0', 8 - (table[it->first].size() % 8));
        }

        tmpS = stringTable[it->first] + tmpS;
        unsigned char mychar = 0;
        int bufferCount = 0;
        for (int i = 0; i < tmpS.size(); ++i) {
            mychar <<= 1;
            if (tmpS[i] == "1") {
                mychar |= 1;
            }

            ++bufferCount;
            if (bufferCount == 8) {
                bufferCount = 0;
                fout << mychar;
                mychar = 0;
            }
        }
    }

    long long sum = 0;
    map<unsigned char, int>::iterator q;
    for (q = arr.begin(); q != arr.end(); ++q) {
        sum += q->second * table[q->first].size();
    }

    ui->progressBar->setMaximum(sum / 8);
    unsigned char tmpCharSpecial;
    for (int j = 56; j >= 0; j -= 8) {
        tmpCharSpecial = (unsigned char)(sum >> j);
        fout << tmpCharSpecial;
    }

    ifstream fin(currentFilePath.toStdString(), ios::binary);
    c = 0;
    int k = 0;
    unsigned char z = 0;
    while (!fin.eof()) {
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        fin.read(&c, sizeof(char));
        if (fin.eof()) break;
        vector<bool> v = table[c];
        for (int j = 0; j < v.size(); ++j) {
            z <<= 1;
            if (v[j]) {
                z = z | 1;
            }

            ++k;
            if (k == 8) {
                k = 0;
                fout << z;
                z = 0;
                QCoreApplication::processEvents();
            }
        }
    }

    z <<= (8 - k);
    fout << z;
    fout << sum;
    fout.close();
    fin.close();
    ui->progressBar->hide();
    ui->lbl_process->hide();
}

void MainWindow::decode() {
    ui->lbl_process->setText("Decoding...");
    ui->lbl_process->show();
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);
    ui->progressBar->show();
    QApplication::processEvents();
    ifstream fin(currentFilePath.toStdString(), ios::binary);
    char c;
    fin.read(&c, sizeof(char));
    int len = int(c);
    QString originalName = "";
    for (int i = 0; i < len; ++i) {
        fin.read(&c, sizeof(char));
        originalName += c;
    }

    ofstream fout((getPath(currentFilePath) + SEPARATOR + originalName).toStdString(),
                  ios::binary);
    char c1, c2;
    unsigned char uc1, uc2;
    len = 0;
    fin.read(&c1, sizeof(unsigned char));
    uc1 = (unsigned char)c1;
    len = ((int)uc1) << 8;
    fin.read(&c2, sizeof(char));
    uc2 = (unsigned char)c2;
    len |= uc2;
    Node *localRoot = new Node();
    Node *p = localRoot;
    for (int i = 0; i < len; ++i) {
        char tmpChar;
        fin.read(&tmpChar, sizeof(char));
        fin.read(&c, sizeof(char));
        int ln = c;
        int byteLn = ln / 8 + (ln % 8 ? 1 : 0);
        char t;
        unsigned char ut;
        vector<bool> v;
        int cnt = 0;
        for (int j = 0; j < byteLn; ++j) {
            fin.read(&t, sizeof(char));
            ut = (unsigned char)t;
            for (int k = 0; k < 8; ++k) {
                if (ut & 128) {
                    v.push_back(1);
                } else {
                    v.push_back(0);
                }

                ut <<= 1;
                cnt++;
                if (cnt == ln) {
                    break;
                }
            }
        }

        p = localRoot;
        for (int j = 0; j < ln; ++j) {
            if (v[j]) {
                if (p->r == nullptr) {
                    p->r = new Node;
                }

                p = p->r;
                if (j == ln - 1) {
                    p->b = true;
                    p->c = tmpChar;
                }
            } else {
                if (p->l == nullptr) {
                    p->l = new Node;
                }

                p = p->l;
                if (j == ln - 1) {
                    p->b = true;
                    p->c = tmpChar;
                }
            }
        }

        v.clear();
    }

    long long rawDataLen = 0;
    char m;
    unsigned char um;
    for (int j = 7; j >= 0; --j) {
        fin.read(&m, sizeof(char));
        um = (unsigned char)m;
        long long tmplong = um;
        tmplong <<= (j * 8);
        rawDataLen |= tmplong;
    }

    long long RDLByte = (rawDataLen / 8 + (rawDataLen % 8 ? 1 : 0));
    ui->progressBar->setMaximum(RDLByte);
    Node *cur = localRoot;
    long long sch = 0;
    for (int j = 0; j < RDLByte; ++j) {
        ui->progressBar->setValue(ui->progressBar->value() + 1);
        fin.read(&m, sizeof(char));
        um = (unsigned char)m;
        for (int k = 0; k < 8; ++k) {
            if (um & 128) {
                cur = cur->r;
            } else {
                cur = cur->l;
            }

            if (cur->b) {
                fout << cur->c;
                cur = localRoot;
                QCoreApplication::processEvents();
            }

            um <<= 1;
            ++sch;
            if (sch == rawDataLen) break;
        }
    }

    fout.close();
    fin.close();
    ui->progressBar->hide();
    ui->lbl_process->hide();
}

void MainWindow::on_btnEncode_clicked() {
    if (getExtension(currentFilePath) == "xxx") {
        QMessageBox::warning(this, "Sorry", "You can't encode '.xxx' files");
        return;
    }

    if (currentFilePath != "") {
        encode();
        QMessageBox::information(this, "OK", "Successfully encoded!");
    }
}

void MainWindow::on_btnDecode_clicked() {
    if (getExtension(currentFilePath) != "xxx") {
        QMessageBox::warning(this, "Warning", "You can decode only '.xxx' files");
        return;
    }

    if (currentFilePath != "") {
        decode();
        QMessageBox::information(this, "OK", "Successfully decoded!");
    }
}
