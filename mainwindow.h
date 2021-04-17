#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include "archiver.h"



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
    void on_btnOpen_clicked();
    void on_btnEncode_clicked();
    void on_btnDecode_clicked();

private:
    Ui::MainWindow *ui;
    QPushButton* buttons[3];
    QLineEdit *linePath;
    QProgressBar *progressBar;
    Archiver *a;
    QString path;

};
#endif  // MAINWINDOW_H
