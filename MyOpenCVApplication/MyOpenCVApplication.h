
// MyOpenCVApplication.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMyOpenCVApplicationApp: 
// �йش����ʵ�֣������ MyOpenCVApplication.cpp
//

class CMyOpenCVApplicationApp : public CWinApp
{

#ifdef _DEBUG
protected:
	CMemoryState m_msOld, m_msNew, m_msDiff;
#endif  // _DEBUG
public:
	CMyOpenCVApplicationApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CMyOpenCVApplicationApp theApp;