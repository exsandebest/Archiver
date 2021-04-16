#ifndef NODE_H
#define NODE_H

class Node {
public:
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

#endif // NODE_H
