// Login.cpp : 实现文件
//

#include "stdafx.h"
#include "MyOpenCVApplication.h"
#include "Login.h"
#include "afxdialogex.h"


// CLogin 对话框

IMPLEMENT_DYNAMIC(CLogin, CDialogEx)

CLogin::CLogin(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CLogin::~CLogin()
{
}

void CLogin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogin, CDialogEx)
	ON_BN_CLICKED(IDOK, &CLogin::OnBnClickedOk)
END_MESSAGE_MAP()


// CLogin 消息处理程序


void CLogin::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CString acc,psw;
	GetDlgItem(IDC_ACCOUNT)->GetWindowText(acc);
	GetDlgItem(IDC_PASSWORD)->GetWindowText(psw);
	if (acc == "debug" && psw == "9999") {
		CDialogEx::OnOK();
	}
	else {
		MessageBox(_T("账号或密码错误！！"),_T("提示"));
	}
}
