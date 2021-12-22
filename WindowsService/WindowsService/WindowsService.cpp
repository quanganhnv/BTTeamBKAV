#undef UNICODE 
#include <iostream>
#include <Windows.h>


#pragma warning(disable : 4996)

#define SVCNAME "ANHNVQServiceName"
#define TXTFILE  "C:\\Users\\Admin\\Desktop\\statusService.txt"
#define NAMEPIPES "\\\\.\\pipe\\pipeOfANHNVQ"

SERVICE_STATUS          g_SvcStatus;
SERVICE_STATUS_HANDLE   g_SvcStatusHandle;

HANDLE                  g_hSvcStopEvent;

void ServiceInstall();
void WINAPI ServiceMain();
void WINAPI ServiceCtrlHandler(DWORD dwCtrl);
int WriteToTxtFile(char* str);
int WriteToPipes(HANDLE hPipe, char* Buff);
int SendAndRecvPipes(HANDLE hPipe);

int main()
//Source: https://docs.microsoft.com/en-us/windows/win32/services/svc-cpp
{
    SERVICE_TABLE_ENTRY DispatchTable[2];     //ENTRY TABLE, dinh nghia khoi tao service  
    DispatchTable[0].lpServiceName = (LPSTR)SVCNAME;
    DispatchTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain; // tro toi ham main cua services

    DispatchTable[1].lpServiceName = 0;
    DispatchTable[1].lpServiceProc = 0;
    // Must be 2 table, DispatchTable[2] = NULL;
    ServiceInstall();

    if (!StartServiceCtrlDispatcher(DispatchTable))
    {
        int result = WriteToTxtFile((char*)"StartServiceCtrlDispatcher = 0");
    }
}

void ServiceInstall() {
    SC_HANDLE schSCManager; //Handle trinh quan ly services
    SC_HANDLE schService;   //Handle services
    TCHAR szPath[MAX_PATH];

    if (!GetModuleFileName(NULL, szPath, MAX_PATH))
    {
        printf("Cannot install service (%d)\n", GetLastError());
        return;
    }

    // Get a handle to the SCM database. 
    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database 
        SC_MANAGER_ALL_ACCESS);  // full access rights 

    if (NULL == schSCManager)
    {
        printf("OpenSCManager failed (%d)\n", GetLastError());
        return;
    }

    // Create the service

    schService = CreateService(
        schSCManager,              // SCM database 
        SVCNAME,                   // name of service 
        SVCNAME,                   // service name to display 
        SERVICE_ALL_ACCESS,        // desired access 
        SERVICE_WIN32_OWN_PROCESS, // service type 
        SERVICE_DEMAND_START,      // start type, bat dau chay service
        SERVICE_ERROR_NORMAL,      // error control type 
        szPath,                    // path to service's binary 
        NULL,                      // no load ordering group 
        NULL,                      // no tag identifier 
        NULL,                      // no dependencies 
        NULL,                      // LocalSystem account 
        NULL);                     // no password 

    if (schService == NULL)
    {
        printf("CreateService failed (%d)\n", GetLastError());
        CloseServiceHandle(schSCManager);
        return;
    }
    else printf("Service installed successfully\n");

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
    return;
}

void WINAPI ServiceMain() 
{
    // INIT SERVICE 
    BOOL fConnected = FALSE;
    char Err[512] = { 0 };
    g_hSvcStopEvent = CreateEvent(
        NULL,    // default security attributes
        TRUE,    // manual reset event
        FALSE,   // not signaled
        NULL);   // no name

    // Register the handler function for the service
    g_SvcStatusHandle = RegisterServiceCtrlHandler(
        SVCNAME,
        (LPHANDLER_FUNCTION)ServiceCtrlHandler);

    g_SvcStatus.dwServiceType = SERVICE_WIN32;
    g_SvcStatus.dwCurrentState = SERVICE_START_PENDING;
    g_SvcStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;
    g_SvcStatus.dwWin32ExitCode = 0;
    g_SvcStatus.dwServiceSpecificExitCode = 0;
    g_SvcStatus.dwCheckPoint = 0;
    g_SvcStatus.dwWaitHint = 0;

    g_SvcStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_SvcStatusHandle, &g_SvcStatus);

    // MAIN CODE
    // source: https://docs.microsoft.com/en-us/windows/win32/ipc/multithreaded-pipe-server

    //Truy cap khong can quyen admin
    PSECURITY_DESCRIPTOR SecurityDescriptor = NULL;
    BYTE byteTemp[SECURITY_DESCRIPTOR_MIN_LENGTH];
    SecurityDescriptor = (PSECURITY_DESCRIPTOR)byteTemp;

    InitializeSecurityDescriptor(SecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(SecurityDescriptor, TRUE, (PACL)NULL, FALSE); // Thuoc tinh thu 2 = TRUE => True Cap khong can quyen admin
    
    SECURITY_ATTRIBUTES SecurityAttribute = {
        sizeof(SecurityAttribute),
        SecurityDescriptor,
        FALSE
    };

    // Tao Pipe
    while (g_SvcStatus.dwCurrentState == SERVICE_RUNNING)
    {
        HANDLE hPipe = CreateNamedPipe(
            NAMEPIPES,
            PIPE_ACCESS_DUPLEX,        //quy trinh may chu va may khach duoc ghi vao duong ong
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,  //The pipe mode. MESSAGE Thong bao, MESSAGE dưới dạng một luồng thông báo,  chờ vô thời hạn trong một số tình huống để tiến trình khách thực hiện một hành động.
            PIPE_UNLIMITED_INSTANCES,
            512,                        //output buffer size
            512,                        //input buffer size
            0,                          //nDefaultTimeOut;
            &SecurityAttribute);        //lpSecurityAttributes

        if (hPipe == INVALID_HANDLE_VALUE)
        {
            sprintf(Err, "CreateNamedPipe failed, GLE=%d", GetLastError());
            WriteToTxtFile(Err);
            return;
        }
        // Waiting for Client, if not, get error
        fConnected = ConnectNamedPipe(hPipe, 0) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if (fConnected)
        {
            SendAndRecvPipes(hPipe);
        }
    }

    WriteToTxtFile((char*)"Service has stopped!");
    return;
}

void WINAPI ServiceCtrlHandler(DWORD dwCtrl) {
    char status[515] = { 0 };
    switch (dwCtrl)
    {
    case SERVICE_CONTROL_STOP:
        g_SvcStatus.dwWin32ExitCode = 0;
        g_SvcStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(g_SvcStatusHandle, &g_SvcStatus);
        sprintf(status, ("Service has stopped by %d"), g_SvcStatus.dwCurrentState);
        WriteToTxtFile(status);
        return;

    case SERVICE_CONTROL_SHUTDOWN:
        g_SvcStatus.dwWin32ExitCode = 0;
        g_SvcStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(g_SvcStatusHandle, &g_SvcStatus);
        WriteToTxtFile((char*)"Computer has shutdown!");
        return;
    default:
        break;
    }
}

int WriteToTxtFile(char* str) {
    FILE* TxtFile;
    TxtFile = fopen(TXTFILE, "w+");
    if (TxtFile == NULL) {
        return -1;
    }
    if (fprintf(TxtFile, "%s\n", str) < strlen(str)) {   //in str ra file txt
        fclose(TxtFile);
        return -1;
    }
    fclose(TxtFile);
    return 0;
}



int SendAndRecvPipes(HANDLE hPipe) {
    BOOL bChecked = FALSE;
    HANDLE hHeap = GetProcessHeap();
    char* recvBuffer = (char*)HeapAlloc(hHeap, 0, 512 * sizeof(char));
    HANDLE htemp;
    DWORD numberOfByteToRead = 0;
    char BaoLoi[512] = { 0 };
    char tempBuff[512] = { 0 };
    char BufferToWrite[512] = { 0 };
    WriteToTxtFile((char*)"Client connected!");
    //Kiem tra tin nhan nguoi dung gui den 
    while (1)
    {
        bChecked = ReadFile(
            hPipe,                     //Handle of Pipe
            recvBuffer,                //Buffer to receive data
            512 * sizeof(char),         //The maximum number of bytes to be read.
            &numberOfByteToRead,        //The number of bytes to be read.
            NULL);
        if (!bChecked || numberOfByteToRead == 0)          // Neu doc file khong thanh cong hoac so byte doc duoc = 0
        {
            if (GetLastError() == ERROR_BROKEN_PIPE)
                WriteToTxtFile((char*)"Client Disconnected");
            else
            {   
                ZeroMemory(BaoLoi, sizeof(BaoLoi));
                sprintf(BaoLoi, "ReadFile failed Error: %d", GetLastError());
                WriteToTxtFile(BaoLoi);
            }
            break; // Waiting for client connecting
        }
        ZeroMemory(tempBuff, sizeof("AddFile:"));
        lstrcpyn(tempBuff, recvBuffer, lstrlen("AddFile:") + 1);
        if (lstrcmp(tempBuff, "AddFile:") == 0)
        {
            ZeroMemory(tempBuff, sizeof(tempBuff));
            lstrcpyn(tempBuff, &recvBuffer[8], lstrlen(recvBuffer) + 1);
            if ((htemp = CreateFile(tempBuff, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, 0, 0)) != INVALID_HANDLE_VALUE)
            {
                WriteToPipes(hPipe, (char*)"Add File Success !");
                WriteToTxtFile((char*)"Add File Success");
            }
            else
            {
                sprintf(BufferToWrite, "Add File Error: %d", GetLastError());
                WriteToPipes(hPipe, BufferToWrite);
                WriteToTxtFile((char*)"Add File Error");
            }
            CloseHandle(htemp);
        }
        if (lstrcmp(tempBuff, "DelFile:") == 0)
        {
            ZeroMemory(tempBuff, sizeof(tempBuff));
            lstrcpyn(tempBuff, &recvBuffer[8], lstrlen(recvBuffer) + 1);
            if ((htemp = CreateFile(tempBuff, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, 0, OPEN_EXISTING, 0, 0)) != INVALID_HANDLE_VALUE)
            {
                if (DeleteFile(tempBuff))
                {
                    WriteToPipes(hPipe, (char*)"Delete File Success !");
                    WriteToTxtFile((char*)"Delete File Success");
                }

                else
                {
                    sprintf(BufferToWrite, "Delete File Error: %d", GetLastError());
                    WriteToPipes(hPipe, BufferToWrite);
                    WriteToTxtFile((char*)"Delete File Error");
                }
            }
            else
            {
                sprintf(BufferToWrite, "Delete File Error: %d", GetLastError());
                WriteToPipes(hPipe, BufferToWrite);
                WriteToTxtFile((char*)"Delete File Error");
            }
            CloseHandle(htemp);
        }
    }
    FlushFileBuffers(hPipe);    //Flushes the buffers of a specified file and causes all buffered data to be written to a file.
    DisconnectNamedPipe(hPipe); //Disconnect Pipe
    CloseHandle(hPipe);
    HeapFree(hHeap, 0, recvBuffer);
    return 0;
}

int WriteToPipes(HANDLE hPipe, char* Buff)
{
    DWORD nNumberOfBytesToWrite = 512, NumberOfBytesWritten = 512;
    HANDLE handle;
    BOOL fCheck = FALSE;
    char Err[512] = { 0 };

    fCheck = WriteFile(
        hPipe,                  // pipe handle 
        Buff,                   // message 
        nNumberOfBytesToWrite,  // message length
        &NumberOfBytesWritten,   // bytes written 
        NULL);                  // not overlapped

    if (!fCheck)
    {
        sprintf(Err, "WriteFile to pipe failed. GLE=%d\n", GetLastError());
        WriteToTxtFile(Err);
        return -1;
    }
    return 0;
}

