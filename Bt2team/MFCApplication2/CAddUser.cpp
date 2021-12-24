// CAddUser.cpp : implementation file
//

#include "pch.h"
#include "MFCApplication2.h"
#include "CAddUser.h"
#include <mysql.h>
#include "afxdialogex.h"

#pragma warning(disable : 4996)

// extern: truy cập biến của toàn bộ project
extern CHAR g_charID[5];
extern BOOL g_bCheckAdd;
extern MYSQL_RES* selectOnePersonQuery(char* ID);
extern bool Query(char* query);

IMPLEMENT_DYNAMIC(CAddUser, CDialog)

CAddUser::CAddUser(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_ADD, pParent)
{

}

CAddUser::~CAddUser()
{
}

void CAddUser::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboBoxCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_Time);
	m_comboBoxCtrl.AddString(L"0");	//Thêm dòng "0" cho comboCTrl 
	m_comboBoxCtrl.AddString(L"1");	//Thêm dòng "1" cho comboCTrl 
	CHAR Temp[20] = { 0 };
	CHAR charToInt[10] = { 0 };
	if (g_bCheckAdd == FALSE)		// Chọn chế độ EDIT
	{
		GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(0);
		GetDlgItem(IDC_BUTTON_UPDATE) - ShowWindow(SW_SHOW);
		MYSQL_RES* res;
		MYSQL_ROW row = 0;
	
		SYSTEMTIME dataTime = { NULL };
		res = selectOnePersonQuery(g_charID);
		row = mysql_fetch_row(res);			//Truy suất từng cột

		SetWindowTextA(GetDlgItem(IDC_EDIT1)->m_hWnd, (LPCSTR)row[1]);		//Lấy ra thông tin Account
		SetWindowTextA(GetDlgItem(IDC_EDIT2)->m_hWnd, (LPCSTR)row[2]);		// Lấy ra thông tin Hoten
		SetWindowTextA(GetDlgItem(IDC_EDIT3)->m_hWnd, (LPCSTR)row[3]);		// Lấy ra thông tin QueQuan
		SetWindowTextA(GetDlgItem(IDC_COMBO1)->m_hWnd, (LPCSTR)row[5]);		// Lấy ra thông tin Gioitinh
		SetWindowTextA(GetDlgItem(IDC_EDIT6)->m_hWnd, (LPCSTR)row[6]);		// Lấy ra thông tin Truonghoc
		
		// Set Date
		strcpy(Temp, row[4]);
		lstrcpynA(charToInt, Temp, 3);
		dataTime.wYear = atoi(charToInt);	//Day

		lstrcpynA(charToInt, &Temp[3], 3);
		dataTime.wMonth = atoi(charToInt);	//Month

		lstrcpynA(charToInt, &Temp[6], 5);
		dataTime.wDay = atoi(charToInt);	//Year
		m_Time.SetTime(&dataTime);
	}
	else {
		GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_UPDATE)->EnableWindow(0);
	}
}


BEGIN_MESSAGE_MAP(CAddUser, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CAddUser::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CAddUser::OnBnClickedButtonUpdate)
END_MESSAGE_MAP()


// CAddUser message handlers

void CAddUser::OnBnClickedButtonAdd()
{

	TCHAR* getText[5] = { 0 };
	CString GioiTinh;
	CHAR Time[20] = { 0 };
	CHAR query[512] = { 0 };
	SYSTEMTIME dataDate;
	m_Time.GetTime(&dataDate);
	m_comboBoxCtrl.GetWindowTextW(GioiTinh);
	if (GioiTinh != "1" && GioiTinh != "0")
	{
		MessageBoxA(0, "Vui long nhap gioi tinh 0 hoac 1", "Alert", MB_OK);
		return;
	}
	for (int i = 0; i < 4; i++)
	{
		getText[i] = (TCHAR*)malloc(256 * sizeof(TCHAR));
		memset(getText[i], 0, sizeof(getText[i]));
	}
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(getText[0], 512);
	GetDlgItem(IDC_EDIT2)->GetWindowTextW(getText[1], 512);
	GetDlgItem(IDC_EDIT3)->GetWindowTextW(getText[2], 512);
	GetDlgItem(IDC_EDIT6)->GetWindowTextW(getText[3], 512);

	sprintf(
		Time,
		"%d-%d-%d",
		dataDate.wYear, dataDate.wMonth, dataDate.wDay);
	sprintf(
		query,
		"INSERT INTO tb_nhanvien VALUES (0,'%S','%S','%S','%s','%S','%S')",
		getText[0],getText[1], getText[2], Time, GioiTinh, getText[3]);
	if (Query(query))
		MessageBox(L"Add Succecfuly", L"Notify", 0);
	else
		MessageBox(L"Add Error", L"Error", 0);
	return;
}


void CAddUser::OnBnClickedButtonUpdate()
{
	TCHAR* getText[5] = { 0 };
	CString GioiTinh;
	CHAR Time[20] = { 0 };
	CHAR query[512] = { 0 };
	SYSTEMTIME dataDate;
	m_Time.GetTime(&dataDate);
	m_comboBoxCtrl.GetWindowTextW(GioiTinh);
	if (GioiTinh != "1" && GioiTinh != "0")
	{
		MessageBoxA(0, "Vui long nhap gioi tinh 0 hoac 1", "Alert", MB_OK);
		return;
	}
	for (int i = 0; i < 4; i++)
	{
		getText[i] = (TCHAR*)malloc(256 * sizeof(TCHAR));
		memset(getText[i], 0, sizeof(getText[i]));
	}
	GetDlgItem(IDC_EDIT1)->GetWindowTextW(getText[0], 512);
	GetDlgItem(IDC_EDIT2)->GetWindowTextW(getText[1], 512);
	GetDlgItem(IDC_EDIT3)->GetWindowTextW(getText[2], 512);
	GetDlgItem(IDC_EDIT6)->GetWindowTextW(getText[3], 512);

	sprintf(
		Time,			
		"%d-%d-%d",
		dataDate.wYear, dataDate.wMonth, dataDate.wDay);

	sprintf(
		query,
		"UPDATE tb_nhanvien SET ACCOUNT = '%S',Hoten = '%S',QueQuan = '%S',NgaySinh = '%s',GioiTinh = '%S',TruongHoc = '%S' WHERE ID = '%s'",
		getText[0], getText[1], getText[2], Time, GioiTinh, getText[3], g_charID);
	if (Query(query))
		MessageBox(L"Update Succecfuly", L"Notify", 0);
	else
		MessageBox(L"Update Error", L"Error", 0);
}

