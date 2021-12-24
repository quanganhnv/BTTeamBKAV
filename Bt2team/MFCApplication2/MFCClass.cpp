// MFCClass.cpp : implementation file
//

#include "pch.h"
#include "MFCApplication2.h"
#include "MFCClass.h"
#include "afxdialogex.h"


// MFCClass dialog

IMPLEMENT_DYNAMIC(MFCClass, CDialog)

MFCClass::MFCClass(CWnd* pParent /*=nullptr*/)
	: CDialog(, pParent)
{

}

MFCClass::~MFCClass()
{
}

void MFCClass::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MFCClass, CDialog)
END_MESSAGE_MAP()


// MFCClass message handlers
