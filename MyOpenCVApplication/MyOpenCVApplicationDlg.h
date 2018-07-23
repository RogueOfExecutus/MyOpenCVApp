
// MyOpenCVApplicationDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <string>
#include "MyReduceImage.h"
#include <opencv2/opencv.hpp> 

// CMyOpenCVApplicationDlg �Ի���
class CMyOpenCVApplicationDlg : public CDialogEx
{
// ����
public:
	CMyOpenCVApplicationDlg(CWnd* pParent = NULL);	// ��׼���캯��

	virtual ~CMyOpenCVApplicationDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYOPENCVAPPLICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeComboMethod();
	CComboBox selectMethod;
	CSpinButtonCtrl m_spin_one;
	CEdit m_num_edit;
	CStatic m_static_one;
private:
	// չ����ɫ��������ѡ��
	void ShowMethodOne();
	// ������ɫ������ѡ��
	void HideMethodOne();
	int m_last_spin_num;
public:
	afx_msg void OnBnClickedButton1();
private:
	std::string showWindowName;
	std::string readWindowName;
	cv::Mat image_r;
	cv::Mat image_s;
	MyReduceImage reduceImage;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedCancel();
	CComboBox method_one_selecter;
	afx_msg void OnClose();
private:
	// չʾͼ����ͷ�ѡ��
	void ShowMethodThree();
	// ����ͼ����ͷ�ѡ��
	void HideMethodThree();
public:
	CEdit m_alpha_edit;
	CStatic m_static_three;
private:
	// ��ͼƬ
	String OpenImageFile();
public:
	CStatic m_static_four_alpha;
	CStatic m_static_four_beta;
	CStatic m_static_six;
	CStatic m_static_seven_eight;
	CStatic m_static_ten;
	CStatic m_static_nine;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
private:
	// չʾ�ı����ȺͶԱȶ�ѡ��
	void ShowMethodFour();
	// ���ظı����ȺͶԱȶ�ѡ��
	void HideMethodFour();
	// ����ƽ������ѡ��
	void HideMethodSix();
	// չʾƽ������ѡ��
	void ShowMethodSix();
	// չʾ��ʴѡ��
	void ShowMethodSevenOrEight();
	// ���ظ�ʴѡ��
	void HideMethodSevenOrEight();
	// չʾͼ�������ѡ��
	void ShowMethodNine();
	// ����ͼ�������ѡ��
	void HideMethodNine();
	void ShowMethodTen();
	// ������ֵ��ѡ��
	void HideMethodTen();
	void ShowMethodEleven();
	void HideMethodEleven();
	// ��ʾ��Ե���ѡ��
	void ShowMethodTwelve();
	// ���ر�Ե���ѡ��
	void HideMethodTwelve();
	// ��ʾ�����߱任ѡ��
	void ShowMethodThirteen();
	// ���ػ����߱任ѡ��
	void HideMethodThirteen();
	// ��ʾ����Բ�任ѡ��
	void ShowMethodFourteen();
	// ���ػ���Բ�任ѡ��
	void HideMethodFourteen();
	// �����Ӧ������
	//void on_mouse(int mouse_event, int x, int y, int flags, void* ustc);
	Point s_pt;
	Point e_pt;
	bool is_click;
public:
	Point GetSPt();
	void SetSPt(int x, int y);
	Point GetEPt();
	void SetEPt(int x, int y);
	void DrawRectangle();
	afx_msg void OnCutMat();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnCopyCoordinate();
	afx_msg void OnHandlePart();
	afx_msg void OnBnClickedMultipleBlend();
	afx_msg void OnCbnSelchangeMethodOneSelecter();
	CEdit m_double_one;
	CEdit m_double_two;
private:
	// ��ʾ��ӳ��ѡ��
	void ShowMethodFifteen();
	// ������ӳ��ѡ��
	void HideMethodFifteen();
	// ��ʾ��תͼƬѡ��
	void ShowMethodSeventeen();
	// ������תͼƬѡ��
	void HideMethodSeventeen();
	// ��ʾֱ��ͼ���㷽��
	void ShowMethodNineteen();
	// ����ֱ��ͼ���㷽��
	void HideMethodNineteen();
	// չʾֱ��ͼ�Աȷ���
	void ShowMethodTwenty();
	// ����ֱ��ͼ�Աȷ���
	void HideMethodTwenty();
	// չʾ����ͶӰ����
	void ShowMethodTwentyOne();
	// ���ط���ͶӰ����
	void HideMethodTwentyOne();
	// չʾģ��ƥ�䷽��
	void ShowMethodTwentyTwo();
	// ����ģ��ƥ�䷽��
	void HideMethodTwentyTwo();
	// ��ͼƬ�ļ���
	CString PickImageFile();
	// �ݹ�ɨ���ļ������ļ�
	void TraverseDir(CString& strDir, std::vector<CString>& vecFiles);
	CButton m_multiple_blend;
	// չʾѰ����������
	void ShowMethodTwentyThree();
	// ����Ѱ����������
	void HideMethodTwentyThree();
	// չʾѰ��͹������
	void ShowMethodTwentyFour();
    // ����Ѱ��͹������
	void HideMethodTwentyFour();
	CButton m_check_is_draw_contours;
	// չʾ�������Ϸ���
	void ShowMethodTwentyFive();
	// ���ض������Ϸ���
	void HideMethodTwentyFive();
	// չʾ����ͼ�η���
	void ShowMethodTwentySix();
	// ���ذ���ͼ�η���
	void HideMethodTwentySix();
	// չʾѰ�Ҷ�ά�뷽��
	void ShowMethodTwentyEight();
	// ����Ѱ�Ҷ�ά�뷽��
	void HideMethodTwentyEight();
	// չʾ��Ϸ���
	void ShowMethodTwentyNine();
	// ������Ϸ���
	void HideMethodTwentyNine();
	// �㵽�ߵ���̾���
	static Point PointToLineDist(Point p, Point p1, Point p2);
public:
	afx_msg void OnBnClickedButtonConfig();
private:
	CButton m_config_bt;
	configForLine RLineConfig;
	configForLine DLineConfig;
	// ���߲�����ʼ��
	void initLineConfig();
};
