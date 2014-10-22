#include "dialog.h"
#include "ui_dialog.h"

#include <qt_windows.h>

#include <QThread>
#include <QTimer>
#include <QDebug>

#include <Windows.h>
#include <shellapi.h>
#include <TlHelp32.h>

typedef struct
{
    HWND hWnd;
    DWORD dwPid;
}WNDINFO;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    WNDINFO* pInfo = (WNDINFO*)lParam;
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &dwProcessId);

    if(dwProcessId == pInfo->dwPid)
    {
        pInfo->hWnd = hWnd;
        return FALSE;
    }
    return TRUE;
}

HWND GetHwndByProcessId(DWORD dwProcessId)
{
    WNDINFO info = {0};
    info.hWnd = NULL;
    info.dwPid = dwProcessId;
    EnumWindows(EnumWindowsProc, (LPARAM)&info);
    return info.hWnd;
}

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    m_process  = new QProcess(this);        
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{

    QString strCmdPath(tr("cmd"));
    //QString strCmdPath(tr("notepad"));
    m_process->setProgram(strCmdPath);
    m_process->setWorkingDirectory("C:/Windows/System32");
    connect(m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(onreadyReadStandardOutput()));
    connect(m_process, SIGNAL(readyReadStandardError()), this, SLOT(onReadyReadStdErr()));
    m_process->start();

}

void Dialog::on_pushButton_2_clicked()
{

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
    ui->cmdOutput->append(QString::fromLocal8Bit(out));
    // hid sub Window

    QTimer::singleShot(800, this, SLOT(onhidWin()));
}

void Dialog::onReadyReadStdErr()
{
    QByteArray error = m_process->readAllStandardError();
    ui->cmdOutput->append(QString::fromLocal8Bit(error));
}

void Dialog::onhidWin()
{
    hidSubWindow(m_process->pid());
}


void Dialog::hidSubWindow(Q_PID pid)
{
    _PROCESS_INFORMATION* pi = pid;
    HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(PROCESSENTRY32);

    if(Process32First(h, &pe)) {
        do {
            if (pe.th32ParentProcessID == pi->dwProcessId) {
                HWND currentHwnd = GetHwndByProcessId(pe.th32ProcessID);
                qDebug() << "currentHand1:  " << currentHwnd;
                if (currentHwnd)
                {
                    ShowWindow(currentHwnd, SW_HIDE);
                }
            }
        } while( Process32Next(h, &pe));
    }
    CloseHandle(h);
}
