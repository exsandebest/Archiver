#include "archiver.h"
#include <fstream>
#include <QApplication>
#include <QFile>

Archiver::Archiver(QProgressBar *progressBar) {
    this->progressBar = progressBar;
}

const QChar SEPARATOR = '/';
const int BYTE_SIZE = 8;
std::map<unsigned char, int> mainMap;
std::map<unsigned char, QVector<bool>> table;
QVector<bool> code;
Node *root;

bool comp(Node *&a, Node *&b) { return a->k < b->k; }

QString Archiver::getPath(QString str) {
    return str.left(str.lastIndexOf(SEPARATOR));
}

QString Archiver::cutPath(QString str) {
    return str.right(str.size() - str.lastIndexOf(SEPARATOR) - 1);
}

QString Archiver::getExtension(QString str) {
    return str.mid(str.lastIndexOf(".") + 1).toLower();
}

std::pair<QString, QString> Archiver::err(const char *title, const char *text) {
    return std::make_pair(QString(title), QString(text));
}

void Archiver::encode(QString path) {
    if (path.isEmpty()) {
        throw err("Empty path", "Please select a file");
    } else if (!QFile(path).exists()) {
        throw err("No such file", "File does not exist");
    } else if (getExtension(path) == "xxx") {
        throw err("Error", "You can't encode '.xxx' files");
    }
    filePath = path;
    encode();
}

void Archiver::decode(QString path) {
    if (path.isEmpty()) {
        throw err("Empty path", "Please select a file");
    } else if (!QFile(path).exists()) {
        throw err("No such file", "File does not exist");
    } else if (getExtension(path) != "xxx") {
        throw err("Error", "You can decode only '.xxx' files");
    }
    filePath = path;
    decode();
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
        code.push_back(0);
        buildTable(p->l);
    }

    if (p->r != nullptr) {
        code.push_back(1);
        buildTable(p->r);
    }

    if (p->b) table[p->c] = code;
    if (!code.isEmpty()) code.pop_back();
}

void Archiver::encode() {
    reset();
    char c;
    std::ifstream fin(filePath.toStdString(), std::ios::binary);
    while (!fin.eof()) {
        fin.read(&c, sizeof(char));
        ++mainMap[c];
        QApplication::processEvents();
    }
    --mainMap[c];

    buildTree();
    buildTable(root);
    std::map<unsigned char, int>::iterator h;
    QString newName(filePath + ".xxx");
    std::ofstream fout(newName.toStdString(), std::ios::binary);
    fout << (unsigned char)(cutPath(filePath).length());
    fout << cutPath(filePath).toStdString();
    fout << (unsigned char)(mainMap.size() >> BYTE_SIZE);
    fout << (unsigned char)(mainMap.size());
    std::map<unsigned char, int>::iterator it;
    for (it = mainMap.begin(); it != mainMap.end(); ++it) {
        unsigned char charSelf = it->first;
        fout << charSelf;
        fout << (unsigned char)(table[charSelf].size());
        QVector<bool> currentByteCode = table[charSelf];
        if (table[charSelf].size() % BYTE_SIZE) {
            for (int i = 0; i < BYTE_SIZE - (table[charSelf].size() % BYTE_SIZE); ++i) {
                currentByteCode.push_back(0);
            }
        }

        unsigned char currentByte = 0;
        int bufferCount = 0;
        for (int i = 0; i < currentByteCode.size(); ++i) {
            currentByte <<= 1;
            if (currentByteCode[i]) {
                currentByte |= 1;
            }

            ++bufferCount;
            if (bufferCount == BYTE_SIZE) {
                bufferCount = 0;
                fout << currentByte;
                currentByte = 0;
            }
        }
    }

    long long rawDataSize = 0;
    std::map<unsigned char, int>::iterator q;
    for (q = mainMap.begin(); q != mainMap.end(); ++q) {
        rawDataSize += q->second * table[q->first].size();
    }

    if (progressBar) progressBar->setMaximum(rawDataSize / BYTE_SIZE);
    unsigned char currentByte;
    for (int j = 64 - BYTE_SIZE; j >= 0; j -= BYTE_SIZE) {
        currentByte = (unsigned char)(rawDataSize >> j);
        fout << currentByte;
    }

    fin.clear();
    fin.seekg(0, fin.beg);

    c = 0;
    int bufferCount = 0;
    currentByte = 0;
    while (!fin.eof()) {
        if (progressBar) progressBar->setValue(progressBar->value() + 1);
        fin.read(&c, sizeof(char));
        if (fin.eof()) break;
        QVector<bool> v = table[c];
        for (int j = 0; j < v.size(); ++j) {
            currentByte <<= 1;
            if (v[j]) {
                currentByte |= 1;
            }

            ++bufferCount;
            if (bufferCount == BYTE_SIZE) {
                bufferCount = 0;
                fout << currentByte;
                currentByte = 0;
                QCoreApplication::processEvents();
            }
        }
    }

    currentByte <<= (BYTE_SIZE - bufferCount);
    fout << currentByte;
    fout << rawDataSize;
    fout.close();
    fin.close();
}

void Archiver::decode() {
    reset();
    QApplication::processEvents();
    std::ifstream fin(filePath.toStdString(), std::ios::binary);
    char c;
    fin.read(&c, sizeof(char));
    int nameSize = int(c);
    QString originalName = "";
    for (int i = 0; i < nameSize; ++i) {
        fin.read(&c, sizeof(char));
        originalName += c;
    }

    std::ofstream fout((getPath(filePath) + SEPARATOR + originalName).toStdString(),
                  std::ios::binary);

    fin.read(&c, sizeof(char));
    int tableSize = ((int)c) << BYTE_SIZE;
    fin.read(&c, sizeof(char));
    tableSize |= c;
    Node *localRoot = new Node();
    Node *p;
    for (int i = 0; i < tableSize; ++i) {
        char charSelf;
        fin.read(&charSelf, sizeof(char));
        fin.read(&c, sizeof(char));
        int charCodeSize = c;
        int charCodeByteSize = charCodeSize / BYTE_SIZE + (charCodeSize % BYTE_SIZE ? 1 : 0);
        std::vector<bool> charCode;
        int cnt = 0;
        for (int j = 0; j < charCodeByteSize; ++j) {
            fin.read(&c, sizeof(char));
            for (int k = 0; k < BYTE_SIZE; ++k) {
                if (c & 128) {
                    charCode.push_back(1);
                } else {
                    charCode.push_back(0);
                }

                c <<= 1;
                cnt++;
                if (cnt == charCodeSize) break;
            }
        }

        p = localRoot;
        for (int j = 0; j < charCodeSize; ++j) {
            if (charCode[j]) {
                if (p->r == nullptr) {
                    p->r = new Node;
                }

                p = p->r;
                if (j == charCodeSize - 1) {
                    p->b = true;
                    p->c = charSelf;
                }
            } else {
                if (p->l == nullptr) {
                    p->l = new Node;
                }

                p = p->l;
                if (j == charCodeSize - 1) {
                    p->b = true;
                    p->c = charSelf;
                }
            }
        }

        charCode.clear();
    }

    long long rawDataSize = 0;
    for (int j = BYTE_SIZE - 1; j >= 0; --j) {
        fin.read(&c, sizeof(char));
        long long tmpLong = (unsigned char)c;
        rawDataSize |= tmpLong << (j * BYTE_SIZE);;
    }

    long long rawDataByteSize = (rawDataSize / BYTE_SIZE + (rawDataSize % BYTE_SIZE ? 1 : 0));
    if (progressBar) progressBar->setMaximum(rawDataByteSize);
    Node *cur = localRoot;
    long long cnt = 0;
    for (int j = 0; j < rawDataByteSize; ++j) {
        if (progressBar) progressBar->setValue(progressBar->value() + 1);
        fin.read(&c, sizeof(char));
        for (int k = 0; k < BYTE_SIZE; ++k) {
            if (c & 128) {
                cur = cur->r;
            } else {
                cur = cur->l;
            }

            if (cur->b) {
                fout << cur->c;
                cur = localRoot;
                QCoreApplication::processEvents();
            }

            c <<= 1;
            ++cnt;
            if (cnt == rawDataSize) break;
        }
    }

    fout.close();
    fin.close();
}


