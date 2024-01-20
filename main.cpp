#include "widget.h"

#include <QApplication>
#include <windows.h>
#include "Xfsapi.h"
#include "zf_log.h"
#include "Xfsptr.h"
#define USER_INTERACTION_TIMEOUT 70000

static HAPP hApp;
HSERVICE hService;
HWND hwnd;  //window handler, you need set qwidget::winId to hwnd


int initWosaStartUp();
int wfsSyncOpen(const char *logicalName, int timeout);
int wfsSyncRegister();
HRESULT wfsSyncExecute(int CMD, int timeout, LPVOID readData, WFSRESULT& wfsResult);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();


     hwnd = (HWND)w.winId(); // your main window from qt
    initWosaStartUp();

    const char *logicalName= "ReceiptPrinter";//logical name of device
    int timeout =USER_INTERACTION_TIMEOUT;
    wfsSyncOpen(logicalName,timeout);
     HRESULT r ;
     WFSRESULT wfsResult;
     QString printableStr;
     QByteArray strByteArray;
     strByteArray.clear();
     strByteArray.append("iranianutopia.ir :)\n");
     strByteArray.append("irulearning.ir :)\n");
     strByteArray.append("iruit.ir :)\n");

     WFSPTRRAWDATA rawData;
     unsigned char rawDataString[strByteArray.size()];
     memset(rawDataString,'0',strByteArray.size());
     for (int i = 0; i < strByteArray.size(); ++i) {
         rawDataString[i] = strByteArray.at(i);
     }
     rawDataString[strByteArray.size()]='\0';
     rawData.lpbData = rawDataString;
     rawData.ulSize = strByteArray.size();
     rawData.wInputData = WFS_PTR_NOINPUTDATA;
     strByteArray.clear();

     r=wfsSyncExecute(WFS_CMD_PTR_RAW_DATA,timeout,&rawData,wfsResult);


     DWORD dwMediaControl = 0;
     dwMediaControl |= WFS_PTR_CTRLEJECT;

     r= wfsSyncExecute(WFS_CMD_PTR_CONTROL_MEDIA,timeout,&dwMediaControl,wfsResult);

    return a.exec();
}



int initWosaStartUp()
{
    int returnCode=0;
    HRESULT r;
    char MinMinor = 0
    , MinMajor = 1
    , MaxMinor = 2
    , MaxMajor = 2;
    DWORD Ver = (DWORD)MaxMajor | (DWORD)MaxMinor << 8 | (DWORD)MinMajor << 16 | (DWORD)MinMinor << 24; //66050
    WFSVERSION wfsVersion;// = { 3, 1, 3, "", "" };

    if ((r = WFSStartUp(Ver, &wfsVersion)) == WFS_SUCCESS) {
        ZF_LOGI("startup done.lets create app handle, result: %ld", r);
    } else {
        ZF_LOGE("startup failed. result: %ld", r);
        return -1;
    }
    if (WFSCreateAppHandle(&hApp) != WFS_SUCCESS) {
        ZF_LOGI("StartUp FAILED, result: %ld", r);
        return -2;
    } else {
        ZF_LOGI("WFSCreateAppHandle is WFS_SUCCESS, result: %ld", r);
        ZF_LOGI("Description---->%s", wfsVersion.szDescription);
    }
    return returnCode;
}


int wfsSyncOpen(const char *logicalName, int timeout)
{
    ZF_LOGI("wfsSyncOpen()");
    HRESULT r;
    LPSTR lpszAppID = (LPSTR)"Mousavi";
    DWORD dwTraceLevel = 0;
    char MinMinor = 0
    , MinMajor = 1
    , MaxMinor = 0
    , MaxMajor = 3;

    DWORD dwSrvcVersionsRequired = (DWORD)MaxMajor | (DWORD)MaxMinor << 8 |
                                   (DWORD)MinMajor << 16 | (DWORD)MinMinor << 24; // 01000103;
    WFSVERSION lpSrvcVersion;
    WFSVERSION lpSPIVersion;
    if ((r = WFSOpen((LPSTR)logicalName,hApp, lpszAppID, dwTraceLevel, timeout,
                     dwSrvcVersionsRequired, &lpSrvcVersion, &lpSPIVersion, &hService)) != WFS_SUCCESS) {
        return r;
    }
    return r;
}



HRESULT wfsSyncExecute(int CMD, int timeout, LPVOID readData, WFSRESULT& wfsResult)
{

    HRESULT r;
    LPWFSRESULT lpwfsResult=NULL;
    Sleep(50);
    r = WFSExecute(hService, CMD, readData, timeout, &lpwfsResult);

    if (r == WFS_SUCCESS) {
        ZF_LOGI("wfsSyncExecute execute successfully, result: %ld", r);
        wfsResult = *lpwfsResult;
    }else {
        ZF_LOGE("wfsSyncExecute could not execute, result: %ld", r);
    }
    WFSFreeResult(lpwfsResult);
    return r;
}
