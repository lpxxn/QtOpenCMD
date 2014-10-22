#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QProcess>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void onreadyReadStandardOutput();
    void onReadyReadStdErr();
    void onhidWin();

private:

    void hidSubWindow(Q_PID pid);

private:
    Ui::Dialog *ui;
    QProcess* m_process;    
};

#endif // DIALOG_H
