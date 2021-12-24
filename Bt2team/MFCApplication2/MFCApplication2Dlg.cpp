
// MFCApplication2Dlg.cpp : implementation file
//


#include "pch.h"
#include "framework.h"
#include "MFCApplication2.h"
#include "MFCApplication2Dlg.h"
#include "afxdialogex.h"
#include "ConnectToMySQL.h"
#include "CAddUser.h"
#include <mysql.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

// CMFCApplication2Dlg dialog

HWND g_hLV;
BOOL g_bCheckAdd;
CHAR g_charID[5] = { 0 };

void updateDatabase();
void showResult(int cot, int hang, LPWSTR strShow);

CMFCApplication2Dlg::CMFCApplication2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ListView);
}

BEGIN_MESSAGE_MAP(CMFCApplication2Dlg, CDialogEx)
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ADDBUTTON, &CMFCApplication2Dlg::OnBnClickedAddbutton)
	ON_BN_CLICKED(IDC_FIXBUTTON, &CMFCApplication2Dlg::OnBnClickedFixbutton)
	ON_BN_CLICKED(IDC_DELETEBUTTON, &CMFCApplication2Dlg::OnBnClickedDeletebutton)
END_MESSAGE_MAP()


// CMFCApplication2Dlg message handlers

BOOL CMFCApplication2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// 
	// ListView Init
	g_hLV = m_ListView.m_hWnd;
	m_ListView.SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT);
	m_ListView.InsertColumn(0, _T("ID"), 0, 50, 0);
	m_ListView.InsertColumn(1, _T("Account"), 0, 100, 1);
	m_ListView.InsertColumn(2, _T("Hoten"), 0, 130, 2);
	m_ListView.InsertColumn(3, _T("QueQuan"), 0, 100, 3);
	m_ListView.InsertColumn(4, _T("NgaySinh"), 0, 99, 4);
	m_ListView.InsertColumn(5, _T("GioiTinh"), 0, 50, 5);
	m_ListView.InsertColumn(6, _T("TruongHoc"), 0, 184, 6);
	
	// Khởi tạo kết nối SQL
	if (ConnectToSQL() == 0) {
		exit(1);
	}
	updateDatabase();
	return TRUE;
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCApplication2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void updateDatabase() {
	MYSQL_ROW row = 0;
	MYSQL_RES* res;
	CHAR charText[512] = { 0 };
	WCHAR wcharText[512] = { 0 };
	int item = 0;
	res = selectAllQuery();							//SELECT * FROM tb_nhanvien
	while ((row = mysql_fetch_row(res)))			// Truy suất từng cột trong bảng
	{
		for (int i = 0; i < res->field_count; i++)	//field_count Trả về SỐ CỘT cho truy vấn gần đây nhất
		{
			ZeroMemory(charText, sizeof(charText));
			ZeroMemory(wcharText, sizeof(wcharText));

			sprintf(charText, "%s", row[i]);		// Thông tin ở vị trí row[i] 
			MultiByteToWideChar(CP_ACP, 0, charText, sizeof(charText), wcharText, sizeof(wcharText) / 2); // chuyển char => wchar

			showResult(i, item, wcharText);			//Xuất kết quả wchar trên mfc. item là hàng, i là cột
		}
		item++;
		//in ra từ trái sang phải, từ trên xuống dưới
	}
	return;
}


void showResult(int cot, int hang, LPWSTR strShow) {
	LVITEM lvi;

	lvi.mask = LVIF_TEXT;
	lvi.iItem = hang;	//hang
	lvi.iSubItem = cot;	//cot
	lvi.pszText = (LPWSTR)strShow;
	lvi.cchTextMax = 512;
	if (cot == 0)
		ListView_InsertItem(g_hLV, &lvi);
	else
		ListView_SetItem(g_hLV, &lvi);
}

void CMFCApplication2Dlg::OnBnClickedAddbutton()
{
	UpdateData();
	g_bCheckAdd = TRUE;
	CHAR cText[512] = { 0 };
	WCHAR wcText[512] = { 0 };
	int j = 0;
	CAddUser dlgAddUser;
	dlgAddUser.DoModal();
	ListView_DeleteAllItems(m_ListView.m_hWnd);	// Xóa listview cũ
	updateDatabase();		//Cập nhật listview mới,database mới
}

void CMFCApplication2Dlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void CMFCApplication2Dlg::OnBnClickedFixbutton()
{
	CString Temp;
	char notificaion[512] = { 0 };
	int selectedCount = m_ListView.GetSelectedCount();
	sprintf(notificaion,"So nhan vien ban chon la: %d",selectedCount);

	if (selectedCount == 0) {
		MessageBoxA(0, "Chua chon nhan vien nao de sua", "Error", 0);
		return;
	}
	else if (selectedCount != 1) {
		MessageBoxA(0, "Chi chon 1 nhan vien de sua !", "Error", 0);
		return;
	}
	g_bCheckAdd = FALSE;	//Bấm Sửa
	POSITION position = m_ListView.GetFirstSelectedItemPosition();	//mục đầu tiên trong bảng list view
	int item = m_ListView.GetNextSelectedItem(position);			//mục tiếp theo bảng listview (mục ID)

	Temp = m_ListView.GetItemText(item, 0);							// Lấy ID nhân viên
	sprintf(g_charID, "%S", Temp);			
	CAddUser AddUserDlg;
	AddUserDlg.DoModal();

	ListView_DeleteAllItems(m_ListView.m_hWnd);
	updateDatabase();
}


void CMFCApplication2Dlg::OnBnClickedDeletebutton()
{
	CString strItemText;
	char strDel[512] = { 0 };
	char notification[512] = { 0 };
	int selectedCount = m_ListView.GetSelectedCount();	//GetSelectedCount(): Lấy số nhân  viên chọn
	sprintf(notification, "Ban co chac chan xoa %d nhan vien khong ?", selectedCount);
	if (MessageBoxA(0, notification, "Notify", MB_OKCANCEL) == 1)
	{
		POSITION position = m_ListView.GetFirstSelectedItemPosition();		//mục đầu tiên trong listview
		while (position) //Lặp khi position != 0
		{
			int item = m_ListView.GetNextSelectedItem(position);	//Mục tiếp theo trong bảng listview(ID)
			strItemText = m_ListView.GetItemText(item, 0);			//Lấy ID nhân viên
			sprintf(strDel, "DELETE FROM tb_nhanvien WHERE ID = %S", strItemText);		// Câu lệnh SQL xóa nhân viên có ID = strItemText

			if (Query(strDel) == 0)
				MessageBox(L"loi delete roi", L"error", 0);

		}
		ListView_DeleteAllItems(m_ListView.m_hWnd);
		updateDatabase();
	}
}


