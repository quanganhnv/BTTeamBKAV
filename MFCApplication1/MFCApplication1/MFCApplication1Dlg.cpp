#include "pch.h"
#include "framework.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#undef UNICODE 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)
#define NAMEPIPES "\\\\.\\pipe\\pipeOfANHNVQ"
#define TXTFILE "C:\\Users\\Admin\\Desktop\\statusService.txt"

// CAboutDlg dialog used for App About
int ProcessingFile(char* nameFile);

CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCApplication1Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCApplication1Dlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMFCApplication1Dlg message handlers

BOOL CMFCApplication1Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCApplication1Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication1Dlg::OnBnClickedButton1()
{
	//Add File
	char AddFile[520] = { 0 };
	char result[512] = { 0 };
	lstrcatA(AddFile, "AddFile:");
	GetWindowTextA((GetDlgItem(IDC_MFCEDITBROWSE1)->m_hWnd), result, 512);	//lay ten file
	lstrcatA(AddFile, result);
	ProcessingFile(AddFile);
}


void CMFCApplication1Dlg::OnBnClickedButton2()
{
	//Delete File
	char DelFile[520] = { 0 };
	char result[512] = { 0 };
	lstrcatA(DelFile, "DelFile:");
	GetWindowTextA((GetDlgItem(IDC_MFCEDITBROWSE1)->m_hWnd), result, 512);	//lay ten file
	lstrcatA(DelFile, result);
	ProcessingFile((char*)DelFile);
}

int ProcessingFile(char* nameFile) {
    DWORD cbToWrite, cbWritten, cbRead ;
    char chBuf[512] = { 0 };
	HANDLE hPipe;
    char Err[512] = { 0 };
    //Ket noi toi PIPES
    //https://docs.microsoft.com/en-us/windows/win32/ipc/named-pipe-client
    DWORD dwMode = PIPE_READMODE_MESSAGE;
    BOOL fCheck = FALSE;
     while (1) 
     { 
        hPipe = CreateFileA( 
            NAMEPIPES,   // pipe name 
            GENERIC_READ | GENERIC_WRITE,  // read and write access 
            0,              // no sharing 
            NULL,           // default security attributes
            OPEN_EXISTING,  // opens existing pipe 
            0,              // default attributes 
            NULL);          // no template file 
 
        // !!! Break if the pipe handle is valid. 
        if (hPipe != INVALID_HANDLE_VALUE) 
            break; 
 
        // Exit if an error other than ERROR_PIPE_BUSY occurs. 
        if (GetLastError() != ERROR_PIPE_BUSY) 
        {
            sprintf(Err, "Could not open pipe. GLE=%d\n", GetLastError());
            MessageBoxA(0, Err, "Tittle", MB_OK);
            return -1;
        }
 
        // All pipe instances are busy, so wait for 20 seconds. 
        if (!WaitNamedPipeA(NAMEPIPES, 20000)) 
        { 
            MessageBoxA(0, "Could not open pipe: 20 second wait timed out.", "Tittle", MB_OK);
            return -1;
        }
     } 
     // The pipe connected; change to message-read mode. 
     //Đặt chế độ đọc và chế độ chặn của đường ống có tên đã chỉ định
     fCheck = SetNamedPipeHandleState(
         hPipe,                     // pipe handle 
         &dwMode,                   // new pipe mode 
         NULL,                      // don't set maximum bytes 
         NULL);                     // don't set maximum time 
     if (!fCheck)
     {
         sprintf(Err, "SetNamedPipeHandleState failed. GLE=%d\n", GetLastError());
         MessageBoxA(0, Err, "Tittle", MB_OK);
         return -1;
     }

     cbToWrite = (lstrlenA(nameFile) + 1) *sizeof(TCHAR);
     while (1) 
     {
         fCheck = WriteFile(
             hPipe,                  // pipe handle 
             nameFile,				// message 
             cbToWrite,              // message length 
             &cbWritten,             // bytes written 
             NULL);                  // not overlapped 

         if (!fCheck)
         {
             sprintf(Err, "WriteFile to pipe failed. GLE=%d\n", GetLastError());
             MessageBoxA(0, Err, "Tittle", MB_OK);
             return -1;
         }
         break;
     }
     printf("\nMessage sent to server, receiving reply as follows:\n");
     do
     {
         // Read from the pipe. 
         fCheck = ReadFile(
             hPipe,                     // pipe handle 
             chBuf,                     // buffer to receive reply 
             512 * sizeof(TCHAR),		// size of buffer 
             &cbRead,                   // number of bytes read 
             NULL);                     // not overlapped 

         if (!fCheck && GetLastError() != ERROR_MORE_DATA)
             break;

		 MessageBoxA(0, chBuf, "Tittle", MB_OK);

     } while (!fCheck);  // repeat loop if ERROR_MORE_DATA
	 CloseHandle(hPipe);    
}