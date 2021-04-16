#include "archiver.h"
#include <QPushButton>
#include <fstream>
#include <QApplication>

Archiver::Archiver(QProgressBar *progressBar) {
    this->progressBar = progressBar;
}

const QChar SEPARATOR = '/';
std::map<unsigned char, int> mainMap;
std::map<unsigned char, QString> table;
QString code;
QPushButton* buttons[3];
Node *root;

bool comp(Node *&a, Node *&b) { return a->k < b->k; }

QString Archiver::getPath(QString str) {
    return str.left(str.lastIndexOf(SEPARATOR));
}

QString Archiver::cutPath(QString str) {
    return str.right(str.size() - str.lastIndexOf(SEPARATOR) - 1);
}

QString Archiver::getExtension(QString fn) {
    return fn.mid(fn.lastIndexOf(".") + 1).toLower();
}

void Archiver::reset() {
    root->clear();
    root = nullptr;
    mainMap.clear();
    table.clear();
    code.clear();
}

void Archiver::buildTree() {
    QVector<Node *> v;
    std::map<unsigned char, int>::iterator k;
    for (k = mainMap.begin(); k != mainMap.end(); ++k) {
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
        Node *tl, *tr;
        while (v.size() != 1) {
            std::sort(v.begin(), v.end(), comp);
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

void Archiver::buildTable(Node *p) {
    if (p->l != nullptr) {
        code += '0';
        buildTable(p->l);
    }

    if (p->r != nullptr) {
        code += '1';
        buildTable(p->r);
    }

    if (p->b) table[p->c] = code;
    code.chop(1);
}

void Archiver::encode() {
    reset();
    char c;
    std::ifstream fin(currentFilePath.toStdString(), std::ios::binary);
    while (!fin.eof()) {
        fin.read(&c, sizeof(char));
        QApplication::processEvents();
        ++mainMap[c];
    }
    --mainMap[c];

    buildTree();
    buildTable(root);
    std::map<unsigned char, int>::iterator h;
    QString newName(currentFilePath + ".xxx");
    std::ofstream fout(newName.toStdString(), std::ios::binary);
    fout << (unsigned char)(cutPath(currentFilePath).length());
    fout << cutPath(currentFilePath).toStdString();
    fout << (unsigned char)(mainMap.size() >> 8);
    fout << (unsigned char)(mainMap.size());
    std::map<unsigned char, int>::iterator it;
    for (it = mainMap.begin(); it != mainMap.end(); ++it) {
        unsigned char charSelf = it->first;
        fout << charSelf;
        fout << (unsigned char)(table[charSelf].size());
        QString tmpS = "";
        if (table[charSelf].size() % 8) {
            tmpS.fill('0', 8 - (table[charSelf].size() % 8));
        }

        tmpS = table[charSelf] + tmpS;
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
    std::map<unsigned char, int>::iterator q;
    for (q = mainMap.begin(); q != mainMap.end(); ++q) {
        sum += q->second * table[q->first].size();
    }

    if (progressBar) progressBar->setMaximum(sum / 8);
    unsigned char tmpCharSpecial;
    for (int j = 56; j >= 0; j -= 8) {
        tmpCharSpecial = (unsigned char)(sum >> j);
        fout << tmpCharSpecial;
    }

    fin.clear();
    fin.seekg(0, fin.beg);

    c = 0;
    int k = 0;
    unsigned char z = 0;
    while (!fin.eof()) {
        if (progressBar) progressBar->setValue(progressBar->value() + 1);
        fin.read(&c, sizeof(char));
        if (fin.eof()) break;
        QString v = table[c];
        for (int j = 0; j < v.size(); ++j) {
            z <<= 1;
            if (v[j] == '1') {
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
}

void Archiver::decode() {
    reset();
    QApplication::processEvents();
    std::ifstream fin(currentFilePath.toStdString(), std::ios::binary);
    char c;
    fin.read(&c, sizeof(char));
    int len = int(c);
    QString originalName = "";
    for (int i = 0; i < len; ++i) {
        fin.read(&c, sizeof(char));
        originalName += c;
    }

    std::ofstream fout((getPath(currentFilePath) + SEPARATOR + originalName).toStdString(),
                  std::ios::binary);

    fin.read(&c, sizeof(unsigned char));
    len = ((int)c) << 8;
    fin.read(&c, sizeof(char));
    len |= c;
    Node *localRoot = new Node();
    Node *p;
    for (int i = 0; i < len; ++i) {
        char tmpChar;
        fin.read(&tmpChar, sizeof(char));
        fin.read(&c, sizeof(char));
        int ln = c;
        int byteLn = ln / 8 + (ln % 8 ? 1 : 0);
        char t;
        std::vector<bool> v;
        int cnt = 0;
        for (int j = 0; j < byteLn; ++j) {
            fin.read(&t, sizeof(char));
            for (int k = 0; k < 8; ++k) {
                if (t & 128) {
                    v.push_back(1);
                } else {
                    v.push_back(0);
                }

                t <<= 1;
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
    if (progressBar) progressBar->setMaximum(RDLByte);
    Node *cur = localRoot;
    long long sch = 0;
    for (int j = 0; j < RDLByte; ++j) {
        if (progressBar) progressBar->setValue(progressBar->value() + 1);
        fin.read(&m, sizeof(char));
        for (int k = 0; k < 8; ++k) {
            if (m & 128) {
                cur = cur->r;
            } else {
                cur = cur->l;
            }

            if (cur->b) {
                fout << cur->c;
                cur = localRoot;
                QCoreApplication::processEvents();
            }

            m <<= 1;
            ++sch;
            if (sch == rawDataLen) break;
        }
    }

    fout.close();
    fin.close();
}


