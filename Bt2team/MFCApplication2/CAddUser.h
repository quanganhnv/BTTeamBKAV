#pragma once


// CAddUser dialog

class CAddUser : public CDialog
{
	DECLARE_DYNAMIC(CAddUser)

public:
	CAddUser(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAddUser();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboBoxCtrl;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonUpdate();
	CDateTimeCtrl m_Time;
};
