#include "dialog.h"
#include "ui_dialog.h"

#include <qt_windows.h>

#include <QProcess>
#include <QThread>
#include <QDebug>

#include <Windows.h>
#include <shellapi.h>
#include <TlHelp32.h>

//typedef struct EnumFunArg
//{
//    HWND      hWnd;
//    DWORD    dwProcessId;
//}EnumFunArg,*LPEnumFunArg;

//BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
//{

//    EnumFunArg  *pArg = (LPEnumFunArg)lParam;
//    DWORD  processId;
//    GetWindowThreadProcessId(hwnd, &processId);
//    if( processId == pArg->dwProcessId)
//    {
//        pArg->hWnd = hwnd;
//        return TRUE;
//    }
//    return FALSE;
//}

//HWND ReturnWnd(DWORD processID)
//{
//   HWND retWnd=NULL;
//   EnumFunArg wi;
//    wi.dwProcessId   =processID;
//    wi.hWnd   =  NULL;
//    EnumWindows(lpEnumFunc,(LPARAM)&wi);
//   if(wi.hWnd){     retWnd=wi.hWnd;
//   }   return retWnd;
//}



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

    // fun 3
    //    HWND hwnd = FindWindow(L"ConsoleWindowClass", NULL);
    //    qDebug() << "find Handl" <<hwnd;
    //    if (hwnd) {
    //        ShowWindow(hwnd, SW_HIDE);
    //    }


}

void Dialog::onreadyReadStandardOutput()
{
    QByteArray out = m_process->readAllStandardOutput();
    //qDebug() << QString::fromLocal8Bit(out);
    ui->cmdOutput->append(QString::fromLocal8Bit(out));
    qDebug() << "____________________________";
//    for (const QObject* obj : m_process->children()) {
//        qDebug() << "Object =  " << obj;
//    }

    // fun 4

      int pid = m_process->processId();
      HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
      PROCESSENTRY32 pe = { 0 };
      pe.dwSize = sizeof(PROCESSENTRY32);

      if( Process32First(h, &pe)) {
          do {
              if (pe.th32ParentProcessID == pid) {
                  qDebug() << "PID:  " << pid << "  ; ProcessID :  " << pe.th32ProcessID <<" ; PPID: " <<  pe.th32ParentProcessID;
                  QThread::sleep(1);
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

void Dialog::onReadyReadStdErr()
{
    QByteArray error = m_process->readAllStandardError();
    ui->cmdOutput->append(QString::fromLocal8Bit(error));
}
