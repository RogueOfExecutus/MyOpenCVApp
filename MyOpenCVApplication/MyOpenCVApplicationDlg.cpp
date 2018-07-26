
// MyOpenCVApplicationDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyOpenCVApplication.h"
#include "MyOpenCVApplicationDlg.h"
#include "afxdialogex.h"
#include <opencv2/opencv.hpp> 
#include "MyReduceImage.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cv;
using namespace std;

void on_mouse(int mouse_event, int x, int y, int flags, void *ustc);
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

CMyOpenCVApplicationDlg *thisDlg;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_CREATE()
//ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CMyOpenCVApplicationDlg �Ի���



CMyOpenCVApplicationDlg::CMyOpenCVApplicationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MYOPENCVAPPLICATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_last_spin_num = 0;
}

CMyOpenCVApplicationDlg::~CMyOpenCVApplicationDlg()
{
	/*destroyWindow(readWindowName);
	destroyWindow(showWindowName);*/
	destroyAllWindows();
	showWindowName.clear();
}

void CMyOpenCVApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_METHOD, selectMethod);
	DDX_Control(pDX, IDC_SPIN_ONE, m_spin_one);
	DDX_Control(pDX, IDC_EDIT_NUM, m_num_edit);
	DDX_Control(pDX, IDC_STATIC_ONE, m_static_one);
	DDX_Control(pDX, IDC_METHOD_ONE_SELECTER, method_one_selecter);
	DDX_Control(pDX, IDC_EDIT_ALPHA, m_alpha_edit);
	DDX_Control(pDX, IDC_STATIC_THREE, m_static_three);
	DDX_Control(pDX, IDC_STATIC_FOUR_ALPHA, m_static_four_alpha);
	DDX_Control(pDX, IDC_STATIC_FOUR_BETA, m_static_four_beta);
	DDX_Control(pDX, IDC_STATIC_SIX, m_static_six);
	DDX_Control(pDX, IDC_STATIC_SEVEN_EIGHT, m_static_seven_eight);
	DDX_Control(pDX, IDC_STATIC_TEN, m_static_ten);
	DDX_Control(pDX, IDC_STATIC_NINE, m_static_nine);
	DDX_Control(pDX, IDC_MULTIPLE_BLEND, m_multiple_blend);
	DDX_Control(pDX, IDC_CHECK_DRAW_CONTOURS, m_check_is_draw_contours);
	DDX_Control(pDX, IDC_EDIT_MIN_LIN_LEN, m_double_one);
	DDX_Control(pDX, IDC_EDIT_MAX_LINE_GAP, m_double_two);
	DDX_Control(pDX, IDC_BUTTON_CONFIG, m_config_bt);
}

BEGIN_MESSAGE_MAP(CMyOpenCVApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMyOpenCVApplicationDlg::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_METHOD, &CMyOpenCVApplicationDlg::OnCbnSelchangeComboMethod)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyOpenCVApplicationDlg::OnBnClickedButton1)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDCANCEL, &CMyOpenCVApplicationDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, &CMyOpenCVApplicationDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyOpenCVApplicationDlg::OnBnClickedButton3)
	ON_COMMAND(ID_CUT_MAT, &CMyOpenCVApplicationDlg::OnCutMat)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_COPY_COORDINATE, &CMyOpenCVApplicationDlg::OnCopyCoordinate)
	ON_COMMAND(ID_HANDLE_PART, &CMyOpenCVApplicationDlg::OnHandlePart)
	ON_BN_CLICKED(IDC_MULTIPLE_BLEND, &CMyOpenCVApplicationDlg::OnBnClickedMultipleBlend)
	ON_CBN_SELCHANGE(IDC_METHOD_ONE_SELECTER, &CMyOpenCVApplicationDlg::OnCbnSelchangeMethodOneSelecter)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG, &CMyOpenCVApplicationDlg::OnBnClickedButtonConfig)
END_MESSAGE_MAP()


// CMyOpenCVApplicationDlg ��Ϣ�������

BOOL CMyOpenCVApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	showWindowName = "showImage";
	readWindowName = "readImage";
	selectMethod.InsertString(0, _T("LUT����"));
	selectMethod.InsertString(1, _T("filter2D"));
	selectMethod.InsertString(2, _T("Blending"));
	selectMethod.InsertString(3, _T("�ı�ԱȶȺ�����"));
	selectMethod.InsertString(4, _T("��ɢ����Ҷ�任"));
	selectMethod.InsertString(5, _T("ͼ��ƽ������"));
	selectMethod.InsertString(6, _T("��ʴ"));
	selectMethod.InsertString(7, _T("����"));
	selectMethod.InsertString(8, _T("ͼ�������"));
	selectMethod.InsertString(9, _T("��ֵ��"));
	selectMethod.InsertString(10, _T("��ӱ߽�"));
	selectMethod.InsertString(11, _T("��Ե���"));
	selectMethod.InsertString(12, _T("�����߱任"));
	selectMethod.InsertString(13, _T("����Բ�任"));
	selectMethod.InsertString(14, _T("��ӳ��"));
	selectMethod.InsertString(15, _T("����任"));
	selectMethod.InsertString(16, _T("��תͼ�񣨷��䣩"));
	selectMethod.InsertString(17, _T("ֱ��ͼ���⻯"));
	selectMethod.InsertString(18, _T("ֱ��ͼ����"));
	selectMethod.InsertString(19, _T("ֱ��ͼ�Ա�"));
	selectMethod.InsertString(20, _T("����ͶӰ"));
	selectMethod.InsertString(21, _T("ģ��ƥ��"));
	selectMethod.InsertString(22, _T("Ѱ������"));
	selectMethod.InsertString(23, _T("Ѱ��͹��"));
	selectMethod.InsertString(24, _T("��������"));
	selectMethod.InsertString(25, _T("����ͼ��"));
	selectMethod.InsertString(26, _T("ɨ��"));
	selectMethod.InsertString(27, _T("ɸѡ��ά������"));
	selectMethod.InsertString(28, _T("����㷨"));
	selectMethod.InsertString(29, _T("�ǵ���"));
	selectMethod.InsertString(30, _T("������"));
	selectMethod.SetCurSel(0);
	method_one_selecter.InsertString(0, _T("��ɫ������"));
	method_one_selecter.InsertString(1, _T("The iterator (safe) method"));
	method_one_selecter.SetCurSel(0);
	m_spin_one.SetRange32(0, 255);//��ʾ��ֵֻ����0��255�ڱ仯
	m_spin_one.SetBase(10);//���ý�����,ֻ����10���ƺ�16����
	m_num_edit.SetWindowText(_T("8"));

	thisDlg = this;
	//����ԴͼƬ����
	namedWindow(readWindowName, WINDOW_NORMAL);
	//mcb = test_mouse;
	setMouseCallback(readWindowName, on_mouse, 0);//���ûص����� 
	//��������ͼƬ����
	namedWindow(showWindowName, WINDOW_NORMAL);
	CRect rect_r, rect_s;
	GetDlgItem(IDC_READIMAGE)->GetClientRect(rect_r);
	GetDlgItem(IDC_SHOWIMAGE)->GetClientRect(rect_s);

	resizeWindow(readWindowName, rect_r.Width(), rect_r.Height());
	resizeWindow(showWindowName, rect_s.Width(), rect_s.Height());

	HWND hWnd_r = (HWND)cvGetWindowHandle(readWindowName.c_str());
	HWND hParent_r = ::GetParent(hWnd_r);
	::SetParent(hWnd_r, GetDlgItem(IDC_READIMAGE)->m_hWnd);
	::ShowWindow(hParent_r, SW_HIDE);

	HWND hWnd_s = (HWND)cvGetWindowHandle(showWindowName.c_str());
	HWND hParent_s = ::GetParent(hWnd_s);
	::SetParent(hWnd_s, GetDlgItem(IDC_SHOWIMAGE)->m_hWnd);
	::ShowWindow(hParent_s, SW_HIDE);

	initLineConfig();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMyOpenCVApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMyOpenCVApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMyOpenCVApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMyOpenCVApplicationDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//CDialogEx::OnOK();
	if (image_r.depth() != CV_8U)
	{
		MessageBox(_T("ͼ���ʽ���ԣ�"));
		return;
	}
	Mat J;
	switch (m_last_spin_num)
	{
	case 0:
	{
		J = Mat::zeros(image_r.size(), image_r.type());
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		//GetDlgItem(IDC_EDIT_NUM)->GetWindowText(str);
		int parameter = _ttoi(str0);

		switch (method_one_selecter.GetCurSel())
		{
		case 0:
			reduceImage.SetDivideWith(parameter);
			reduceImage.UseLUT(image_r, J);
			break;
		default:
			break;
		}
	}
		break;
	case 1:
		J = Mat::zeros(image_r.size(), image_r.type());
		reduceImage.UseFilter2D(image_r,J);
		break;
	case 2:
	{
		J = Mat::zeros(image_r.size(), image_r.type());
		//�ڶ�ͼ��·��
		String imgFile = OpenImageFile();
		Mat K = imread(imgFile, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
		if (K.size() != image_r.size()) {
			MessageBox(_T("ͼƬ������ȣ����ɵ��ӣ�"));
			return;
		}
		CString str0;
		m_alpha_edit.GetWindowTextW(str0);
		double alpha = _ttof(str0);
		if (alpha <= 0.0 || alpha >= 1.0)
			alpha = 0.5;
		reduceImage.UseAddWeighted(image_r, K, alpha, J);
	}
		break;
	case 3:
	{
		J = Mat::zeros(image_r.size(), image_r.type());
		CString str0;
		m_alpha_edit.GetWindowTextW(str0);
		double alpha = _ttof(str0);
		CString str1;
		m_num_edit.GetWindowTextW(str1);
		int beta = _ttoi(str1);
		reduceImage.ChangeAlphaBeta(image_r, J, alpha, beta);
	}
		break;
	case 4:
	{
		if (image_r.channels() != 1)
		{
			INT_PTR res = MessageBox(_T("��ͼ���ǻҶ�ͼ���Ƿ���ת���ɻҶ�ͼ����ִ��"),_T("��ʾ��"),MB_ACTIVE);
			if (res != IDOK) {
				return;
			}
			Mat temp;
			cvtColor(image_r,temp, CV_RGB2GRAY);
			reduceImage.UseDTF(temp, J);
			break;
		}
		reduceImage.UseDTF(image_r, J);
	}
		break;
	case 5:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int size = _ttoi(str0);
		if (size > 31 || size < 1)
		{
			MessageBox(_T("Element Size ����С��1�����31��"));
			break;
		}
		if (size % 2 == 0) 
		{
			MessageBox(_T("Element Size ֻ����������"));
			break;
		}
		int filter = method_one_selecter.GetCurSel();
		reduceImage.UseBlur(image_r, J, filter, size);
	}
		break;
	case 6:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int size = _ttoi(str0);
		if (size > 21 || size < 1)
		{
			MessageBox(_T("Kernel Size ����С��1�����21��"));
			break;
		}
		int erosion_type, erosion_elem = method_one_selecter.GetCurSel();
		if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
		else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
		else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
		reduceImage.UseErosion(image_r, J, erosion_type, size);
	}
		break;
	case 7:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int size = _ttoi(str0);
		if (size > 21 || size < 1)
		{
			MessageBox(_T("Kernel Size ����С��1�����21��"));
			break;
		}
		int dilation_type, dilation_elem = method_one_selecter.GetCurSel();;
		if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
		else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
		else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }
		reduceImage.UseDilation(image_r, J, dilation_type, size);
	}
		break;
	case 8:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int cycle = _ttoi(str0);
		if (cycle == 0)
		{
			MessageBox(_T("ѭ����������Ϊ0��"));
			break;
		}
		reduceImage.UsePyr(image_r, J, method_one_selecter.GetCurSel(), cycle);
	}
		break;
	case 9:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int value = _ttoi(str0);
		reduceImage.UseThreshold(image_r, J, value, method_one_selecter.GetCurSel());
	}
		break;
	case 10:
	{
		RNG rng(12345);
		Scalar value = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		reduceImage.UseCopyMakeBorder(image_r, J, value, method_one_selecter.GetCurSel());
	}
		break;
	case 11: 
		reduceImage.UseEdgeDetection(image_r, J, method_one_selecter.GetCurSel());
		break;
	case 12:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int threshold = _ttoi(str0); 
		double minLinLength, maxLineGap;
		if (method_one_selecter.GetCurSel() == 1) 
		{
			CString str1;
			m_double_one.GetWindowTextW(str1);
			minLinLength = _ttof(str1);
			CString str2;
			m_double_one.GetWindowTextW(str2);
			maxLineGap = _ttof(str2);
		}
		else
		{
			minLinLength = 0.0;
			maxLineGap = 0.0;
		}
		reduceImage.UseHoughLines(image_r, J, 1, CV_PI/900, threshold, method_one_selecter.GetCurSel(), minLinLength, maxLineGap);
	}
		break;
	case 13:
		reduceImage.UseHoughCircles(image_r, J);
		break;
	case 14:
		J = Mat::zeros(image_r.size(), image_r.type());
		reduceImage.UseRemap(image_r, J, method_one_selecter.GetCurSel());
		break;
	case 15:
		J = Mat::zeros(image_r.size(), image_r.type());
		reduceImage.UseWarpAffine(image_r, J);
		break;
	case 16:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		double angle = _ttof(str0);
		reduceImage.RotateImage(image_r, J, angle);
	}
		break;
	case 17:
		reduceImage.UseEqualizeHist(image_r, J);
		break;
	case 18:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int bins = _ttoi(str0);
		reduceImage.UseCalcHistAndDraw(image_r, J, bins);
	}
		break;
	case 19:
	{
		//�ڶ�ͼ��·��
		String imgFile = OpenImageFile();
		J = imread(imgFile, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
		if (!J.data)
		{
			MessageBox(_T("��ͼƬʧ��"));
			return;
		}
		double result = reduceImage.UseCompareHist(image_r, J, method_one_selecter.GetCurSel());
		CString msg;
		msg.Format(_T("�ԱȽ����%lf"), result);
		MessageBox(msg);
	}
		break;
	case 20:
	{
		//�ڶ�ͼ��·��
		String imgFile = OpenImageFile();
		Mat _J = imread(imgFile, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
		if (!_J.data)
		{
			MessageBox(_T("��ͼƬʧ��"));
			return;
		}
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int bins = _ttoi(str0);
		reduceImage.UseCalcBackProject(image_r, _J, J, 180, bins);
	}
		break;
	case 21:
	{
		//ģ��ͼ��·��
		String imgFile = OpenImageFile();
		Mat templ = imread(imgFile, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
		if (!templ.data)
		{
			MessageBox(_T("��ģ��ʧ��"));
			return;
		}
		reduceImage.UseMatchTemplate(image_r, templ, J, method_one_selecter.GetCurSel());
	}
		break;
	case 22:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int thresh = _ttoi(str0);
		J = Mat::zeros(image_r.size(), CV_8UC3);
		reduceImage.FindAndDrawContours(image_r, J, thresh);
	}
		break;
	case 23:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int thresh = _ttoi(str0);
		J = Mat::zeros(image_r.size(), CV_8UC3);
		reduceImage.FindAndDrawConvexHull(image_r, J, thresh, m_check_is_draw_contours.GetCheck());
	}
		break;
	case 24:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int thresh = _ttoi(str0);
		J = Mat::zeros(image_r.size(), CV_8UC3);
		reduceImage.UseApproxPolyDP(image_r, J, thresh, m_check_is_draw_contours.GetCheck());
	}
		break;
	case 25:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int thresh = _ttoi(str0);
		J = Mat::zeros(image_r.size(), CV_8UC3);
		reduceImage.DrawRectOrCircle(image_r, J, thresh, method_one_selecter.GetCurSel());
	}
		break;
	case 26:
	{
		string type;
		string data;
		reduceImage.ScanBarCode(image_r, type, data);
		MessageBoxA(this->GetSafeHwnd(), data.c_str(), type.c_str(),0);
		J = image_r.clone();
	}
		break;
	case 27:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int thresh = _ttoi(str0);
		J = image_r.clone();
		RotatedRect rotatedRect;
		reduceImage.FindCodeCoutours(image_r, J, codeConfig, rotatedRect);
	}
		break;
	case 28:
	{
		switch (method_one_selecter.GetCurSel())
		{
		case 0:
			CodeJudgement(J);
			break;
		default:
			break;
		}
		
	}
		break;
	case 29:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int thresh = _ttoi(str0);
		switch (method_one_selecter.GetCurSel())
		{
		case 0:
			reduceImage.DrawCornerHarris(image_r, J, 2, 3, 0.04, thresh);
			break;
		case 1:
			reduceImage.DrawCorners(image_r, J, thresh, 0.01, 10, 3, false, 0.04);
			break;
		default:
			break;
		}
	}
		break;
	default:
		MessageBox(_T("û��ѡ��ͼ��������"));
		break;
	}

	if (!J.data)
	{
		MessageBox(_T("����ͼƬʧ��"));
		return;
	}
	else
	{
		image_s = J;
		//��ͼ����ʾ���´����Ĵ�����
		imshow(showWindowName, J);
	}
}


void CMyOpenCVApplicationDlg::OnCbnSelchangeComboMethod()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	switch (m_last_spin_num)
	{
	case 0:
		HideMethodOne();
		break;
	case 1:
		break;
	case 2:
		HideMethodThree();
		break;
	case 3:
		HideMethodFour();
		break;
	case 4:
		break;
	case 5:
		HideMethodSix();
		break;
	case 6:
		HideMethodSevenOrEight();
		break;
	case 7:
		HideMethodSevenOrEight();
		break;
	case 8:
		HideMethodNine();
		break;
	case 9:
		HideMethodTen();
		break;
	case 10:
		HideMethodEleven();
		break;
	case 11:
		HideMethodTwelve();
		break;
	case 12:
		HideMethodThirteen();
		break;
	case 13:
		HideMethodFourteen();
		break;
	case 14:
		HideMethodFifteen();
		break;
	case 15:
		break;
	case 16:
		HideMethodSeventeen();
		break;
	case 17:
		break;
	case 18:
		HideMethodNineteen();
		break;
	case 19:
		HideMethodTwenty();
		break;
	case 20:
		HideMethodTwentyOne();
		break;
	case 21:
		HideMethodTwentyTwo();
		break;
	case 22:
		HideMethodTwentyThree();
		break;
	case 23:
		HideMethodTwentyFour();
		break;
	case 24:
		HideMethodTwentyFive();
		break;
	case 25:
		HideMethodTwentySix();
		break;
	case 26:
		break;
	case 27:
		HideMethodTwentyEight();
		break;
	case 28:
		HideMethodTwentyNine();
		break;
	case 29:
		HideMethodThirty();
		break;
	default:
		break;
	}
	switch (selectMethod.GetCurSel()) {
	case 0 :
		ShowMethodOne(); 
		m_last_spin_num = 0;
		break;
	case 1:
		m_last_spin_num = 1;
		break;
	case 2:
		ShowMethodThree();
		m_last_spin_num = 2;
		break;
	case 3:
		ShowMethodFour();
		m_last_spin_num = 3;
		break;
	case 4:
		m_last_spin_num = 4;
		break;
	case 5:
		ShowMethodSix();
		m_last_spin_num = 5;
		break;
	case 6:
		ShowMethodSevenOrEight();
		m_last_spin_num = 6;
		break;
	case 7:
		ShowMethodSevenOrEight();
		m_last_spin_num = 7;
		break;
	case 8:
		ShowMethodNine();
		m_last_spin_num = 8;
		break;
	case 9:
		ShowMethodTen();
		m_last_spin_num = 9;
		break;
	case 10:
		ShowMethodEleven();
		m_last_spin_num = 10;
		break;
	case 11:
		ShowMethodTwelve();
		m_last_spin_num = 11;
		break;
	case 12:
		ShowMethodThirteen();
		m_last_spin_num = 12;
		break;
	case 13:
		ShowMethodFourteen();
		m_last_spin_num = 13;
		break;
	case 14:
		ShowMethodFifteen();
		m_last_spin_num = 14;
		break;
	case 15:
		m_last_spin_num = 15;
		break;
	case 16:
		ShowMethodSeventeen();
		m_last_spin_num = 16;
		break;
	case 17:
		m_last_spin_num = 17;
		break;
	case 18:
		ShowMethodNineteen();
		m_last_spin_num = 18;
		break;
	case 19:
		ShowMethodTwenty();
		m_last_spin_num = 19;
		break;
	case 20:
		ShowMethodTwentyOne();
		m_last_spin_num = 20;
		break;
	case 21:
		ShowMethodTwentyTwo();
		m_last_spin_num = 21;
		break;
	case 22:
		ShowMethodTwentyThree();
		m_last_spin_num = 22;
		break;
	case 23:
		ShowMethodTwentyFour();
		m_last_spin_num = 23;
		break;
	case 24:
		ShowMethodTwentyFive();
		m_last_spin_num = 24;
		break;
	case 25:
		ShowMethodTwentySix();
		m_last_spin_num = 25;
		break;
	case 26:
		m_last_spin_num = 26;
		break;
	case 27:
		ShowMethodTwentyEight();
		m_last_spin_num = 27;
		break;
	case 28:
		ShowMethodTwentyNine();
		m_last_spin_num = 28;
		break;
	case 29:
		ShowMethodThirty();
		m_last_spin_num = 29;
		break;
	default:
		break;
	}
}

// չ����ɫ��������ѡ��
void CMyOpenCVApplicationDlg::ShowMethodOne()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	m_static_one.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("��ɫ������"));
	method_one_selecter.InsertString(1, _T("The iterator (safe) method"));
	method_one_selecter.SetCurSel(0);
}


// ������ɫ������ѡ��
void CMyOpenCVApplicationDlg::HideMethodOne()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	m_static_one.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


void CMyOpenCVApplicationDlg::OnBnClickedButton1()
{
	//ͼ��·��
	String imgFile = OpenImageFile();
	if (imgFile == "no")
	{
		MessageBox(_T("��ͼƬʧ��"));
		return;
	}
	//����ͼ��
	//image_r = imread(imgFile, CV_LOAD_IMAGE_GRAYSCALE);
	image_r = imread(imgFile, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	
	if (!image_r.data)
	{
		MessageBox(_T("��ͼƬʧ��"));
		return;
	}
	else
	{
		imshow(readWindowName, image_r);
	}
}


int CMyOpenCVApplicationDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	return 0;
}


void CMyOpenCVApplicationDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


void CMyOpenCVApplicationDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CDialogEx::OnClose();
}


// չʾͼ����ͷ�ѡ��
void CMyOpenCVApplicationDlg::ShowMethodThree()
{
	m_multiple_blend.ShowWindow(SW_SHOW);
	m_alpha_edit.ShowWindow(SW_SHOW);
	m_static_three.ShowWindow(SW_SHOW);
}


// ����ͼ����ͷ�ѡ��
void CMyOpenCVApplicationDlg::HideMethodThree()
{
	m_multiple_blend.ShowWindow(SW_HIDE);
	m_alpha_edit.ShowWindow(SW_HIDE);
	m_static_three.ShowWindow(SW_HIDE);
}


// ��ͼƬ
String CMyOpenCVApplicationDlg::OpenImageFile()
{
	CFileDialog findFileDlg(
		TRUE,  // TRUE�Ǵ������ļ��Ի���FALSE�򴴽����Ǳ����ļ��Ի���
		_T(".jpg"),  // Ĭ�ϵĴ��ļ�������
		NULL,  // Ĭ�ϴ򿪵��ļ���
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // ��ֻ���ļ�
		_T("�ı��ļ�(*.jpg)|*.jpg|����ļ�(*.PNG)|*.PNG|�����ļ� (*.*)|*.*||")  // ���п��Դ򿪵��ļ�����
	);
	//ͼ������
	String imgFile;
	if (IDOK == findFileDlg.DoModal())
	{
		imgFile = (LPCSTR)(CStringA)(findFileDlg.GetPathName());
	}
	else
	{
		imgFile = "no";
	}
	return imgFile;
}


// չʾ�ı����ȺͶԱȶ�ѡ��
void CMyOpenCVApplicationDlg::ShowMethodFour()
{
	m_static_four_alpha.ShowWindow(SW_SHOW);
	m_static_four_beta.ShowWindow(SW_SHOW);
	m_alpha_edit.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
}


// ���ظı����ȺͶԱȶ�ѡ��
void CMyOpenCVApplicationDlg::HideMethodFour()
{
	m_static_four_alpha.ShowWindow(SW_HIDE);
	m_static_four_beta.ShowWindow(SW_HIDE);
	m_alpha_edit.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
}


// ����ƽ������ѡ��
void CMyOpenCVApplicationDlg::HideMethodSix()
{
	m_static_six.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// չʾƽ������ѡ��
void CMyOpenCVApplicationDlg::ShowMethodSix()
{
	m_static_six.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("��ֵƽ��"));
	method_one_selecter.InsertString(1, _T("��˹ƽ��"));
	method_one_selecter.InsertString(2, _T("��ֵƽ��"));
	method_one_selecter.InsertString(3, _T("˫��ƽ��")); 
	method_one_selecter.InsertString(4, _T("�Զ��������˲���"));
	method_one_selecter.SetCurSel(0);
}


// չʾ��ʴѡ��
void CMyOpenCVApplicationDlg::ShowMethodSevenOrEight()
{
	m_static_seven_eight.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("MORPH RECT"));
	method_one_selecter.InsertString(1, _T("MORPH CROSS"));
	method_one_selecter.InsertString(2, _T("MORPH ELLIPSE"));
	method_one_selecter.SetCurSel(0);
}


// ���ظ�ʴѡ��
void CMyOpenCVApplicationDlg::HideMethodSevenOrEight()
{
	m_static_seven_eight.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// չʾͼ�������ѡ��
void CMyOpenCVApplicationDlg::ShowMethodNine()
{
	m_static_nine.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("���ϲ���"));
	method_one_selecter.InsertString(1, _T("���²���"));
	method_one_selecter.SetCurSel(0);
}


// ����ͼ�������ѡ��
void CMyOpenCVApplicationDlg::HideMethodNine()
{
	m_static_nine.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}

//չʾ��ֵ��ѡ��
void CMyOpenCVApplicationDlg::ShowMethodTen()
{
	m_static_ten.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("��������ֵ"));
	method_one_selecter.InsertString(1, _T("����������ֵ"));
	method_one_selecter.InsertString(2, _T("�ض���ֵ"));
	method_one_selecter.InsertString(3, _T("0��ֵ"));
	method_one_selecter.InsertString(4, _T("��0��ֵ"));
	method_one_selecter.SetCurSel(0);
}


// ������ֵ��ѡ��
void CMyOpenCVApplicationDlg::HideMethodTen()
{
	m_static_ten.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


void CMyOpenCVApplicationDlg::ShowMethodEleven()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("�������"));
	method_one_selecter.InsertString(1, _T("�ٽ�ֵ���"));
	method_one_selecter.SetCurSel(0);
}


void CMyOpenCVApplicationDlg::HideMethodEleven()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


void CMyOpenCVApplicationDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!image_r.data) 
	{
		MessageBox(_T("û������ͼ��"));
		return;
	}
	else if(image_r.channels() == 1)
	{
		MessageBox(_T("��ͼ���ǻҶ�ͼ��"));
		return;
	}
	cvtColor(image_r,image_r, CV_RGB2GRAY);
	imshow(readWindowName, image_r);
}


void CMyOpenCVApplicationDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	image_r = image_s;
	imshow(readWindowName, image_r);
}


// ��ʾ��Ե���ѡ��
void CMyOpenCVApplicationDlg::ShowMethodTwelve()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("Sobel����"));
	method_one_selecter.InsertString(1, _T("Scharr����"));
	method_one_selecter.InsertString(2, _T("Laplace����"));
	method_one_selecter.InsertString(3, _T("Canny����"));
	method_one_selecter.SetCurSel(0);
}


// ���ر�Ե���ѡ��
void CMyOpenCVApplicationDlg::HideMethodTwelve()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// ��ʾ�����߱任ѡ��
void CMyOpenCVApplicationDlg::ShowMethodThirteen()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("��׼�����߱任"));
	method_one_selecter.InsertString(1, _T("ͳ�Ƹ��ʻ����߱任"));
	method_one_selecter.SetCurSel(0);
}


// ���ػ����߱任ѡ��
void CMyOpenCVApplicationDlg::HideMethodThirteen()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE); 
	m_double_one.ShowWindow(SW_HIDE);
	m_double_two.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// ��ʾ����Բ�任ѡ��
void CMyOpenCVApplicationDlg::ShowMethodFourteen()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("�����ݶȷ�"));
	method_one_selecter.SetCurSel(0);
}


// ���ػ���Բ�任ѡ��
void CMyOpenCVApplicationDlg::HideMethodFourteen()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// �����Ӧ������
void on_mouse(int event, int x, int y, int flags, void *ustc)
{
	//CMyOpenCVApplicationDlg *pWnd = (CMyOpenCVApplicationDlg *)CWnd::FromHandle(hWnd);
	if (event == CV_EVENT_LBUTTONDOWN)//������£���ȡ��ʼ����
	{
		//pWnd->SetSPt(x, y);
		thisDlg->SetSPt(x, y);
	}
	if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))
	{
		if (x != thisDlg->GetSPt().x && y != thisDlg->GetSPt().y) 
		{
			thisDlg->SetEPt(x, y);
			thisDlg->DrawRectangle();
		}
	}
	if (event == CV_EVENT_LBUTTONUP)
	{
		if (x != thisDlg->GetSPt().x && y != thisDlg->GetSPt().y)
		{
			thisDlg->SetEPt(x, y);
			thisDlg->DrawRectangle();
		}
	}
}

Point CMyOpenCVApplicationDlg::GetSPt()
{
	return s_pt;
}


void CMyOpenCVApplicationDlg::SetSPt(int x,int y)
{
	s_pt = Point(x, y);
}


Point CMyOpenCVApplicationDlg::GetEPt()
{
	return e_pt;
}


void CMyOpenCVApplicationDlg::SetEPt(int x, int y)
{
	e_pt = Point(x, y);
}


void CMyOpenCVApplicationDlg::DrawRectangle()
{
	Mat image_temp;
	image_r.copyTo(image_temp);
	reduceImage.UseRectangle(image_temp, s_pt, e_pt);
	imshow(readWindowName, image_temp);
}


void CMyOpenCVApplicationDlg::OnCutMat()
{
	// TODO: �ڴ���������������
	Mat temp(image_r, Rect(min(s_pt.x, e_pt.x), min(s_pt.y, e_pt.y), abs(e_pt.x - s_pt.x), abs(e_pt.y - s_pt.y)));
	temp.copyTo(image_s);
	imshow(showWindowName, image_s);
}


void CMyOpenCVApplicationDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: �ڴ˴������Ϣ����������
	CRect rect;//�����������
	GetDlgItem(IDC_READIMAGE)->GetWindowRect(&rect);//��ȡ�ؼ�����ľ���
	if (rect.PtInRect(point))
	{
		CMenu   menu;
		VERIFY(menu.LoadMenu(IDR_MENU1));

		CMenu*   pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this); //����굱ǰλ����ʾָ���˵�
	}
}


void CMyOpenCVApplicationDlg::OnCopyCoordinate()
{
	// TODO: �ڴ���������������
	if (OpenClipboard())
	{
		EmptyClipboard();
		CString str;
		str.Format(_T("(%d,%d),(%d,%d)"), min(s_pt.x, e_pt.x), min(s_pt.y, e_pt.y), max(s_pt.x, e_pt.x), max(s_pt.y, e_pt.y));
		HANDLE hClip = GlobalAlloc(GMEM_MOVEABLE, str.GetLength() + 1);
		USES_CONVERSION;
		strcpy_s((char *)GlobalLock(hClip),str.GetLength()+1, T2A(str));
		GlobalUnlock(hClip);//����  
		SetClipboardData(CF_TEXT, hClip);//���ø�ʽ  
		CloseClipboard();
	}
}


void CMyOpenCVApplicationDlg::OnHandlePart()
{
	// TODO: �ڴ���������������
	Mat temp;
	Mat p0(image_r, Rect(min(s_pt.x, e_pt.x), min(s_pt.y, e_pt.y), abs(e_pt.x - s_pt.x), abs(e_pt.y - s_pt.y)));
	image_r.copyTo(temp);
	p0.copyTo(image_r);
	OnBnClickedOk();
	temp.copyTo(image_r);
}


// ��ʾ��ӳ��ѡ��
void CMyOpenCVApplicationDlg::ShowMethodFifteen()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("��Сһ�������ʾ"));
	method_one_selecter.InsertString(1, _T("���µߵ�"));
	method_one_selecter.InsertString(2, _T("���ҵߵ�"));
	method_one_selecter.InsertString(3, _T("�������ҵߵ�"));
	method_one_selecter.SetCurSel(0);
}


// ������ӳ��ѡ��
void CMyOpenCVApplicationDlg::HideMethodFifteen()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// ��ʾ��תͼƬѡ��
void CMyOpenCVApplicationDlg::ShowMethodSeventeen()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_spin_one.SetRange32(-360, 360);
}


// ������תͼƬѡ��
void CMyOpenCVApplicationDlg::HideMethodSeventeen()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_spin_one.SetRange32(0, 255);//��ʾ��ֵֻ����1��256�ڱ仯
}


// ��ʾֱ��ͼ���㷽��
void CMyOpenCVApplicationDlg::ShowMethodNineteen()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_spin_one.SetRange32(1, 256);
}

// ����ֱ��ͼ���㷽��
void CMyOpenCVApplicationDlg::HideMethodNineteen()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_spin_one.SetRange32(0, 255);//��ʾ��ֵֻ����1��256�ڱ仯
}


// չʾֱ��ͼ�Աȷ���
void CMyOpenCVApplicationDlg::ShowMethodTwenty()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("Correlation"));
	method_one_selecter.InsertString(1, _T("Chi-square"));
	method_one_selecter.InsertString(2, _T("Intersection"));
	method_one_selecter.InsertString(3, _T("Bhattacharyya"));
	method_one_selecter.SetCurSel(0);
}


// ����ֱ��ͼ�Աȷ���
void CMyOpenCVApplicationDlg::HideMethodTwenty()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// չʾ����ͶӰ����
void CMyOpenCVApplicationDlg::ShowMethodTwentyOne()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_spin_one.SetRange32(1, 256);
}


// ���ط���ͶӰ����
void CMyOpenCVApplicationDlg::HideMethodTwentyOne()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_spin_one.SetRange32(0, 255);//��ʾ��ֵֻ����1��256�ڱ仯
}


// չʾģ��ƥ�䷽��
void CMyOpenCVApplicationDlg::ShowMethodTwentyTwo()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("SQDIFF"));
	method_one_selecter.InsertString(1, _T("SQDIFF NORMED"));
	method_one_selecter.InsertString(2, _T("CCORR"));
	method_one_selecter.InsertString(3, _T("CCORR NORMED"));
	method_one_selecter.InsertString(4, _T("CCOEFF "));
	method_one_selecter.InsertString(5, _T("CCOEFF NORMED"));
	method_one_selecter.SetCurSel(0);
}


// ����ģ��ƥ�䷽��
void CMyOpenCVApplicationDlg::HideMethodTwentyTwo()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


void CMyOpenCVApplicationDlg::OnBnClickedMultipleBlend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	vector<CString> vecFiles;
	CString imgFile = PickImageFile();
	if (imgFile.IsEmpty()) {
		return;
	}
	TraverseDir(imgFile, vecFiles);
	int len = vecFiles.size();
	double skip_num = 0.0;
	Mat I = imread((LPCSTR)(CStringA)vecFiles.at(0), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	Mat J;
	for (int i = 1; i < len; i++)
	{
		double alpha = 1.0 / (i + 1.0 - skip_num);
		Mat next = imread((LPCSTR)(CStringA)vecFiles.at(i), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
		if (I.size() != next.size()) 
		{
			skip_num += 1.0;
			continue;
		}
		reduceImage.UseAddWeighted(next, I, alpha, J);
		I = J.clone();
	}
	imshow(showWindowName, I);
	CString str;
	str.Format(_T("����%d��ͼƬ����%d�����һ�Ŵ�С��һ"), len, (int)skip_num);
	MessageBox(str);
}


// ��ͼƬ�ļ���
CString CMyOpenCVApplicationDlg::PickImageFile()
{
	CFolderPickerDialog findFileDlg(NULL, 0, this, 0);
	CString imgFile;
	if (IDOK == findFileDlg.DoModal())
	{
		imgFile = findFileDlg.GetPathName();
	}
	return imgFile;
}


// �ݹ�ɨ���ļ������ļ�
void CMyOpenCVApplicationDlg::TraverseDir(CString& strDir, vector<CString>& vecFiles)
{
	CFileFind ff;
	//��·���������\*.*��׺
	if (strDir.Right(1) != "\\")
		strDir += "\\";
	strDir += "*.jpg";
	BOOL ret = ff.FindFile(strDir);
	while (ret)
	{
		ret = ff.FindNextFile();
		if (ff.IsDirectory() && !ff.IsDots())
		{
			CString path = ff.GetFilePath();
			TraverseDir(path, vecFiles);
		}
		else if (!ff.IsDirectory() && !ff.IsDots())
		{
			//CString name = ff.GetFileName();//��ȡ����׺���ļ���
			CString name = ff.GetFilePath();//��ȡ�ļ�·��
											//CString name = ff.GetFileTitle();//��ȡ������׺���ļ���
			vecFiles.push_back(name);
		}

	}

}

// չʾѰ����������
void CMyOpenCVApplicationDlg::ShowMethodTwentyThree()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
}

// ����Ѱ����������
void CMyOpenCVApplicationDlg::HideMethodTwentyThree()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
}

// չʾѰ��͹������
void CMyOpenCVApplicationDlg::ShowMethodTwentyFour()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_check_is_draw_contours.ShowWindow(SW_SHOW);
}

// ����Ѱ��͹������
void CMyOpenCVApplicationDlg::HideMethodTwentyFour()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_check_is_draw_contours.ShowWindow(SW_HIDE);
}


// չʾ�������Ϸ���
void CMyOpenCVApplicationDlg::ShowMethodTwentyFive()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_check_is_draw_contours.ShowWindow(SW_SHOW);
}


// ���ض������Ϸ���
void CMyOpenCVApplicationDlg::HideMethodTwentyFive()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_check_is_draw_contours.ShowWindow(SW_HIDE);
}


// չʾ����ͼ�η���
void CMyOpenCVApplicationDlg::ShowMethodTwentySix()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("��С����������"));
	method_one_selecter.InsertString(1, _T("��С����б����"));
	method_one_selecter.InsertString(2, _T("��С����Բ��"));
	method_one_selecter.InsertString(3, _T("��С������Բ"));
	method_one_selecter.InsertString(4, _T("������"));
	method_one_selecter.SetCurSel(0);
}


// ���ذ���ͼ�η���
void CMyOpenCVApplicationDlg::HideMethodTwentySix()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


void CMyOpenCVApplicationDlg::OnCbnSelchangeMethodOneSelecter()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_last_spin_num != 12)
		return;
	if (method_one_selecter.GetCurSel() == 1) 
	{
		m_double_one.ShowWindow(SW_SHOW);
		m_double_two.ShowWindow(SW_SHOW);
	}
	else
	{
		m_double_one.ShowWindow(SW_HIDE);
		m_double_two.ShowWindow(SW_HIDE);
	}
}


// չʾѰ�Ҷ�ά�뷽��
void CMyOpenCVApplicationDlg::ShowMethodTwentyEight()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
}


// ����Ѱ�Ҷ�ά�뷽��
void CMyOpenCVApplicationDlg::HideMethodTwentyEight()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
}


// չʾ��Ϸ���
void CMyOpenCVApplicationDlg::ShowMethodTwentyNine()
{
	m_config_bt.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("��ά���ж�"));
	method_one_selecter.SetCurSel(0);
}


// ������Ϸ���
void CMyOpenCVApplicationDlg::HideMethodTwentyNine()
{
	m_config_bt.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// �㵽�ߵ���̾���
Point CMyOpenCVApplicationDlg::PointToLineDist(Point p, Point p1, Point p2)
{
	Point result;
	//�����ڻ�������1Ϊ��1���㣬����2Ϊ��1����2
	double cross = (p2.x - p1.x) * (p.x - p1.x) + (p2.y - p1.y) * (p.y -p1.y);
	//����2�ĳ��ȵ�ƽ��
	double d2 = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
	//rΪ����1������2��ͶӰ����������2�ı�ֵ�������෴��Ϊ����
	double r = cross / d2;
	result.x = p1.x + (p2.x - p1.x) * r;
	result.y = p1.y + (p2.y - p1.y) * r;
	return result;
		//sqrt((p.x - result.x) * (p.x - result.x) + (p.y - result.y) * (p.y - result.y));
}


void CMyOpenCVApplicationDlg::OnBnClickedButtonConfig()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MessageBox(_T("�������"));
}


// ���߲�����ʼ��
void CMyOpenCVApplicationDlg::initLineConfig()
{
	RLineConfig.canny_size = 3;
	RLineConfig.line_direction = true;
	RLineConfig.maxLineGap = 20;
	RLineConfig.minLinLength = 200;
	RLineConfig.threshold = 40;
	RLineConfig.threshold1 = 80;
	RLineConfig.threshold2 = 200;

	DLineConfig.canny_size = 3;
	DLineConfig.line_direction = false;
	DLineConfig.maxLineGap = 20;
	DLineConfig.minLinLength = 50;
	DLineConfig.threshold = 35;
	DLineConfig.threshold1 = 80;
	DLineConfig.threshold2 = 200;

	ScanConfig.ErosionMethod = 0;
	ScanConfig.ErosionSize = 1;
	ScanConfig.ErosionTimes = 0;
	ScanConfig.threshold = 100;

	codeConfig.BlurSize = 3;
	codeConfig.ErosionMethod = 0;
	codeConfig.ErosionSize = 1;
	codeConfig.ErosionTimes = 1;
	codeConfig.threshold = 100;
}


Point CMyOpenCVApplicationDlg::FindMidPoint(Point p1, Point p2, int x_offset, int y_offset)
{
	return Point((p1.x + p2.x) / 2 + x_offset, (p1.y + p2.y) / 2 + y_offset);
}


double CMyOpenCVApplicationDlg::GetLineLenght(Point p1, Point p2, double x_proportion, double y_proportion)
{
	double x1 = p1.x / x_proportion;
	double x2 = p2.x / x_proportion;
	double y1 = p1.y / y_proportion;
	double y2 = p2.y / y_proportion;
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}


// ��ά���ж�
void CMyOpenCVApplicationDlg::CodeJudgement(Mat& J)
{
	J = image_r.clone();
		if (J.channels() == 1)
			cvtColor(J, J, CV_GRAY2BGR);
		RotatedRect rotatedRect;
		Mat code(image_r, Rect(1371, 403, 418, 379));
		Mat temp = code.clone();

		string type = NULL;
		string data;
		
		if (reduceImage.FindCodeCoutours(code, temp, codeConfig, rotatedRect)) 
		{
			Rect tempRect = rotatedRect.boundingRect();
			Rect scanRect = Rect(tempRect.x + 1371, tempRect.y + 403, tempRect.width, tempRect.height);
			Mat ScanRect = Mat(image_r, scanRect).clone();
			reduceImage.PretreatmentForScanCode(ScanRect, ScanRect, ScanConfig);

			reduceImage.ScanBarCode(ScanRect, type, data);
		}
		
		if (type == NULL)
		{
			MessageBox(_T("error code"));
			return;
		}
		Point2f fourPoint2f[4];

		/** returns 4 vertices of the rectangle
		@param pts The points array for storing rectangle vertices. The order is bottomLeft, topLeft, topRight, bottomRight.
		*/
		//��rectPoint�����д洢������ֵ�ŵ� fourPoint��������  
		rotatedRect.points(fourPoint2f);

		Point Rmid1 = FindMidPoint(fourPoint2f[2], fourPoint2f[3], 1371, 403);
		Mat Rline(image_r, Rect(2147, 467, 300, 328));
		Vec4i Rl;
		double Rlen;
		double CRlen = GetLineLenght(fourPoint2f[2], fourPoint2f[3], 37.0, 37.0);
		if (reduceImage.PretreatmentForFindLine(Rline, RLineConfig, Rl))
		{
			Point R1 = Point(Rl[0] + 2147, Rl[1] + 467);
			Point R2 = Point(Rl[2] + 2147, Rl[3] + 467);
			Point Rmid2 = PointToLineDist(Rmid1, R1, R2);
			line(J, R1, R2, Scalar(0, 0, 255), 5);
			line(J, Rmid1, Rmid2, Scalar(0, 0, 255), 5);
			Rlen = GetLineLenght(Rmid1, Rmid2, 37.0, 37.0);
		}
		

		Point Dmid1 = FindMidPoint(fourPoint2f[0], fourPoint2f[3], 1371, 403);
		Mat Dline(image_r, Rect(976, 1641, 303, 246));
		Vec4i Dl;
		double Dlen;
		double CDlen = GetLineLenght(fourPoint2f[0], fourPoint2f[3], 37.0, 37.0);
		if (reduceImage.PretreatmentForFindLine(Dline, DLineConfig, Dl))
		{
			Point D1 = Point(Dl[0] + 976, Dl[1] + 1641);
			Point D2 = Point(Dl[2] + 976, Dl[3] + 1641);
			Point Dmid2 = PointToLineDist(Dmid1, D1, D2);
			line(J, D1, D2, Scalar(0, 0, 255), 5);
			line(J, Dmid1, Dmid2, Scalar(0, 0, 255), 5);
			Dlen = GetLineLenght(Dmid1, Dmid2, 37.0, 37.0);
		}
		
		float angle = rotatedRect.angle;
		CString info;
		info.Format(_T("�������ͣ�%s\n���룺%s\n��ߣ�%.3lf\n���%.3lf\n�Ҿ��룺%.3lf\n����룺%.3lf\n�Ƕȣ�%f"),
			CStringW(type.c_str()), CStringW(data.c_str()), CRlen, CDlen, Rlen, Dlen, angle);
		imshow(showWindowName, J);
		MessageBox(info);
}


// չʾ�ǵ��ⷽ��
void CMyOpenCVApplicationDlg::ShowMethodThirty()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("Harris�ǵ���"));
	method_one_selecter.InsertString(1, _T("Shi-Tomasi�ǵ���"));
	method_one_selecter.SetCurSel(0);
}


// ���ؽǵ��ⷽ��
void CMyOpenCVApplicationDlg::HideMethodThirty()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}
