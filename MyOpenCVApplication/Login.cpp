// Login.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyOpenCVApplication.h"
#include "Login.h"
#include "afxdialogex.h"


// CLogin �Ի���

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


// CLogin ��Ϣ�������


void CLogin::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString acc,psw;
	GetDlgItem(IDC_ACCOUNT)->GetWindowText(acc);
	GetDlgItem(IDC_PASSWORD)->GetWindowText(psw);
	if (acc == "debug" && psw == "9999") {
		CDialogEx::OnOK();
	}
	else {
		MessageBox(_T("�˺Ż�������󣡣�"),_T("��ʾ"));
	}
}
