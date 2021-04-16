#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "archiver.h"
#include <QFileDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setMaximumSize(450, 150);

    QGridLayout *mainLayout = new QGridLayout(this->centralWidget());

    QPushButton *btnEncode = buttons[0] = new QPushButton("Encode");
    connect(btnEncode, &QPushButton::clicked, this, &MainWindow::on_btnEncode_clicked);
    mainLayout->addWidget(btnEncode, 0, 0);

    QPushButton *btnOpen = buttons[1] = new QPushButton("Open");
    connect(btnOpen, &QPushButton::clicked, this, &MainWindow::on_btnOpen_clicked);
    mainLayout->addWidget(btnOpen, 0, 1);

    QPushButton *btnDecode = buttons[2] = new QPushButton("Decode");
    connect(btnDecode, &QPushButton::clicked, this, &MainWindow::on_btnDecode_clicked);
    mainLayout->addWidget(btnDecode, 0, 2);

    linePath = new QLineEdit();
    linePath->setReadOnly(true);
    mainLayout->addWidget(linePath, 1, 0, 1, 3);

    progressBar = new QProgressBar();
    mainLayout->addWidget(progressBar, 2, 0, 1, 3);

    progressBar->setVisible(false);

    a = new Archiver(progressBar);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_btnOpen_clicked() {
    a->currentFilePath = QFileDialog::getOpenFileName(this, "Directory Dialog", "");
    linePath->setText(a->currentFilePath);
}

void MainWindow::on_btnEncode_clicked() {
    if (a->getExtension(a->currentFilePath) == "xxx") {
        QMessageBox::warning(this, "Sorry", "You can't encode '.xxx' files");
        return;
    } else if (a->currentFilePath.isEmpty()) {
        QMessageBox::warning(this, "Empty path", "Please select a file");
        return;
    }
    for (auto btn: buttons) btn->setDisabled(true);
    progressBar->setMaximum(0);
    progressBar->setValue(0);
    progressBar->setVisible(true);
    QApplication::processEvents();
    a->encode();
    progressBar->setVisible(false);
    for (auto btn: buttons) btn->setDisabled(false);
    QMessageBox::information(this, "OK", "Successfully encoded!");

}

void MainWindow::on_btnDecode_clicked() {
    if (a->getExtension(a->currentFilePath) != "xxx") {
        QMessageBox::warning(this, "Warning", "You can decode only '.xxx' files");
        return;
    } else if (a->currentFilePath.isEmpty()) {
        QMessageBox::warning(this, "Empty path", "Please select a file");
        return;
    }
    for (auto btn: buttons) btn->setDisabled(true);
    progressBar->setMinimum(0);
    progressBar->setValue(0);
    progressBar->setVisible(true);
    a->decode();
    progressBar->setVisible(false);
    for (auto btn: buttons) btn->setDisabled(false);
    QMessageBox::information(this, "OK", "Successfully decoded!");
}
