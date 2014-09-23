#include "dialog.h"
#include "ui_dialog.h"

#include <qt_windows.h>
#include <Windows.h>
#include <shellapi.h>

#include <QProcess>
#include <QDebug>


SHELLEXECUTEINFO ShExecInfo;

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // fun 1
    ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS ;
    ShExecInfo.hwnd = NULL;
    ShExecInfo.lpVerb = NULL;
    ShExecInfo.lpParameters = L"";
    ShExecInfo.lpDirectory = NULL;
    ShExecInfo.nShow = SW_SHOW;
    ShExecInfo.hInstApp = NULL;

    // fun 2
    m_process  = new QProcess(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    // fun1
//    ShExecInfo.lpFile = L"notepad";
//    ShExecInfo.lpFile = L"cmd";
//    ShellExecuteEx(&ShExecInfo);


    // fun2
    //QString strCmdPath(tr("C:/Windows/System32/cmd.exe"));
    // also work
    QString strCmdPath(tr("cmd"));
    //QString strCmdPath(tr("notepad"));
    m_process->setProgram(strCmdPath);
    m_process->setWorkingDirectory("C:/Windows/System32");
    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(onreadyReadStandardOutput()));
    connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(onReadyReadStdErr()));
    m_process->start();
    qDebug() << "------------start-----------";
}

void Dialog::on_pushButton_2_clicked()
{

    // fun1
//    if( ShExecInfo.hProcess != NULL)
//    {
//        TerminateProcess(ShExecInfo.hProcess,0);
//        ShExecInfo.hProcess = NULL;
//    }

    // fun2
    qDebug() << "pid " << m_process->processId();
    QString str = ui->line_Command->text().trimmed();
    if (!str.isEmpty()) {
        QByteArray data;
        data.append(str);
        data.append('\n');
        m_process->write(data);
    }
}

void Dialog::onreadyReadStandardOutput()
{
     QByteArray out = m_process->readAllStandardOutput();
     //qDebug() << QString::fromLocal8Bit(out);
     ui->cmdOutput->append(QString::fromLocal8Bit(out));
}

void Dialog::onReadyReadStdErr()
{
    QByteArray error = m_process->readAllStandardError();
    ui->cmdOutput->append(QString::fromLocal8Bit(error));
}
