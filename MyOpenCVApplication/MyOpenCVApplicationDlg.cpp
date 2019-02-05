// MyOpenCVApplicationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MyOpenCVApplication.h"
#include "MyOpenCVApplicationDlg.h"
#include "afxdialogex.h"
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui_c.h"
#include "MyReduceImage.h"
#include <sstream>
#include "pylon/PylonIncludes.h"
#include <condition_variable>
#include <thread>
#include <mutex>
#include <cstdlib>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cv;
using namespace cv::ml;
using namespace std;
using namespace Pylon;
using namespace GenApi;

void on_mouse(int mouse_event, int x, int y, int flags, void *ustc);
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

CMyOpenCVApplicationDlg *thisDlg;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMyOpenCVApplicationDlg 对话框



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
	DDX_Control(pDX, IDC_COMBO_CODE_TYPE, m_scan_code_type);
	DDX_Control(pDX, IDC_BUTTON_OPEN_CAMERA, m_camera_bt);
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
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CAMERA, &CMyOpenCVApplicationDlg::OnBnClickedButtonOpenCamera)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CMyOpenCVApplicationDlg::OnBnClickedButtonSave)
END_MESSAGE_MAP()


// CMyOpenCVApplicationDlg 消息处理程序

BOOL CMyOpenCVApplicationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	showWindowName = "showImage";
	readWindowName = "readImage";
	selectMethod.InsertString(0, _T("LUT方法"));
	selectMethod.InsertString(1, _T("filter2D"));
	selectMethod.InsertString(2, _T("Blending"));
	selectMethod.InsertString(3, _T("改变对比度和亮度"));
	selectMethod.InsertString(4, _T("离散傅里叶变换"));
	selectMethod.InsertString(5, _T("图像平滑处理"));
	selectMethod.InsertString(6, _T("腐蚀"));
	selectMethod.InsertString(7, _T("膨胀"));
	selectMethod.InsertString(8, _T("图像金字塔"));
	selectMethod.InsertString(9, _T("阈值化"));
	selectMethod.InsertString(10, _T("添加边界"));
	selectMethod.InsertString(11, _T("边缘检测"));
	selectMethod.InsertString(12, _T("霍夫线变换"));
	selectMethod.InsertString(13, _T("霍夫圆变换"));
	selectMethod.InsertString(14, _T("重映射"));
	selectMethod.InsertString(15, _T("仿射变换"));
	selectMethod.InsertString(16, _T("旋转图像（仿射）"));
	selectMethod.InsertString(17, _T("直方图均衡化"));
	selectMethod.InsertString(18, _T("直方图计算"));
	selectMethod.InsertString(19, _T("直方图对比"));
	selectMethod.InsertString(20, _T("反射投影"));
	selectMethod.InsertString(21, _T("模板匹配"));
	selectMethod.InsertString(22, _T("寻找轮廓"));
	selectMethod.InsertString(23, _T("寻找凸包"));
	selectMethod.InsertString(24, _T("多边形拟合"));
	selectMethod.InsertString(25, _T("包覆图形"));
	selectMethod.InsertString(26, _T("扫码"));
	selectMethod.InsertString(27, _T("筛选二维码轮廓"));
	selectMethod.InsertString(28, _T("组合算法"));
	selectMethod.InsertString(29, _T("角点检测"));
	selectMethod.InsertString(30, _T("形态学变换"));
	selectMethod.InsertString(31, _T("basler相机"));
	selectMethod.InsertString(32, _T("knn近邻算法"));
	selectMethod.InsertString(33, _T("SVM支持向量机"));
	selectMethod.InsertString(34, _T("拉普拉斯变换"));
	selectMethod.InsertString(35, _T("特征点检测"));
	selectMethod.InsertString(36, _T("透视变换"));
	selectMethod.InsertString(37, _T("不处理"));
	selectMethod.SetCurSel(0);
	method_one_selecter.InsertString(0, _T("颜色缩减法"));
	method_one_selecter.InsertString(1, _T("The iterator (safe) method"));
	method_one_selecter.SetCurSel(0);
	m_spin_one.SetRange32(0, 255);//表示数值只能在0到255内变化
	m_spin_one.SetBase(10);//设置进制数,只能是10进制和16进制
	m_num_edit.SetWindowText(_T("8"));
	thisDlg = this;
	//创建源图片窗口
	namedWindow(readWindowName, WINDOW_NORMAL);
	//mcb = test_mouse;
	setMouseCallback(readWindowName, on_mouse, 0);//调用回调函数 
	//创建处理图片窗口
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

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMyOpenCVApplicationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMyOpenCVApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMyOpenCVApplicationDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//CDialogEx::OnOK();
	if (image_r.depth() != CV_8U)
	{
		MessageBox(_T("图像格式不对！"));
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
		reduceImage.UseFilter2D(image_r, J);
		break;
	case 2:
	{
		J = Mat::zeros(image_r.size(), image_r.type());
		//第二图像路径
		string imgFile = OpenImageFile();
		Mat K = imread(imgFile, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
		if (K.size() != image_r.size()) {
			MessageBox(_T("图片长宽不相等！不可叠加！"));
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
			INT_PTR res = MessageBox(_T("该图像不是灰度图像，是否先转换成灰度图像再执行"), _T("提示！"), MB_ACTIVE);
			if (res != IDOK) {
				return;
			}
			Mat temp;
			cvtColor(image_r, temp, COLOR_RGB2GRAY);
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
			MessageBox(_T("Element Size 不可小于1或大于31！"));
			break;
		}
		if (size % 2 == 0)
		{
			MessageBox(_T("Element Size 只能是奇数！"));
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
			MessageBox(_T("Kernel Size 不可小于1或大于21！"));
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
			MessageBox(_T("Kernel Size 不可小于1或大于21！"));
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
			MessageBox(_T("循环次数不能为0！"));
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
		reduceImage.UseHoughLines(image_r, J, 1, CV_PI / 900, threshold, method_one_selecter.GetCurSel(), minLinLength, maxLineGap);
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
		CString tt;
		tt.Format(_T("i.rows:%d,i.cols:%d,j.rows:%d,j.cols:%d"), image_r.rows, image_r.cols, J.rows, J.cols);
		MessageBox(tt);
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
		//第二图像路径
		string imgFile = OpenImageFile();
		J = imread(imgFile, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
		if (!J.data)
		{
			MessageBox(_T("打开图片失败"));
			return;
		}
		double result = reduceImage.UseCompareHist(image_r, J, method_one_selecter.GetCurSel());
		CString msg;
		msg.Format(_T("对比结果：%lf"), result);
		MessageBox(msg);
	}
	break;
	case 20:
	{
		//第二图像路径
		string imgFile = OpenImageFile();
		Mat _J = imread(imgFile, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
		if (!_J.data)
		{
			MessageBox(_T("打开图片失败"));
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
		//模板图像路径
		string imgFile = OpenImageFile();
		Mat templ = imread(imgFile, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
		if (!templ.data)
		{
			MessageBox(_T("打开模板失败"));
			return;
		}
		if (image_r.channels() != 1)
		{
			cvtColor(image_r, image_r, COLOR_BGR2GRAY);
		}
		if (templ.channels() != 1)
		{
			cvtColor(templ, templ, COLOR_BGR2GRAY);
		}
		double value;
		Point p;
		reduceImage.UseMatchTemplate(image_r, templ, J, method_one_selecter.GetCurSel(), value, p);
		CString valueStr;
		valueStr.Format(_T("result point x:%d, y:%d"), p.x, p.y);
		MessageBox(valueStr);
	}
	break;
	case 22:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int thresh = _ttoi(str0);
		J = Mat::zeros(image_r.size(), CV_8UC3);
		reduceImage.FindAndDrawContours(image_r, J, thresh, method_one_selecter.GetCurSel());
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
		J = image_r.clone();
		string type;
		string data;
		switch (method_one_selecter.GetCurSel())
		{
		case 0:
			reduceImage.ScanBarCode(image_r, type, data);
			break;
		case 1:
		{
			vector<Point> codePoint;
			reduceImage.ScanBarCodeForZxing(image_r, m_scan_code_type.GetCurSel(), data, codePoint);
			reduceImage.DrawCodePoint(J, codePoint);
		}
			break;
		default:
			break;
		}
		MessageBoxA(this->GetSafeHwnd(), data.c_str(), type.c_str(), 0);
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
		case 1:
		{
			vector<CString> vecFiles;
			CString imgFile = PickImageFile();
			if (imgFile.IsEmpty()) {
				//MessageBox(_T(""));
				return;
			}
			TraverseDir(imgFile, vecFiles);
			//int len = vecFiles.size();
			Mat data, labels;   //特征矩阵
			vector<Rect> rects;
			for (size_t i = 0; i < vecFiles.size(); i++)
			{
				Mat tempr = imread(string(CW2A(vecFiles[i])), IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
				Mat temprr;
				reduceImage.RotateImage(tempr(Rect(1984, 448, 168, 385)), temprr, 90);

				Mat tempe;
				cvtColor(tempr(Rect(2114, 555, 20, 20)), tempe, COLOR_BGR2GRAY);

				Point p;
				double value;
				Mat templ = imread("D:\\my_opencv_save_image.jpg", IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
				imshow("testrr", temprr);
				reduceImage.UseMatchTemplate(temprr, templ, J, 0, value, p);
				imshow("test", temprr(Rect(p, templ.size())));
				//MessageBox(_T("123"));
				reduceImage.findAllWord(temprr(Rect(p, templ.size())), J, i, data, labels, rects);
				data.push_back(tempe.reshape(0, 1));
				labels.push_back((int)' ');
			}
			//(2114,555),(2125,568)
			data.convertTo(data, CV_32FC1);
			//Mat TData = data(Range(0, vecFiles.size() * 10), Range::all());
			//Mat Labels = labels(Range(0, vecFiles.size() * 10), Range::all());
			int K = 5;
			Ptr<TrainData> tData = TrainData::create(data, ROW_SAMPLE, labels);
			Ptr<KNearest> model = KNearest::create();
			model->setDefaultK(K);
			model->setIsClassifier(true);
			model->train(tData);
			model->save("versionKnn.xml");
		}
			break;
		case 2:
		{
			if (!image_r.data)
			{
				MessageBox(_T("未打开图片"));
				return;
			}
			CString labelstr = _T("R-4103968");
			char* labelchar = "R-4103968";
			CString strDir = PickImageFile();
			if (strDir.IsEmpty()) {
				//MessageBox(_T(""));
				return;
			}
			if (strDir.Right(1) != "\\")
				strDir += "\\";
			int len = labelstr.GetLength();
			vector<vector<CString>> vecFiles(len);
			for (int i = 0; i < len; i++)
			{
				CString str = strDir + labelstr.Mid(i, 1);
				MessageBox(str);
				TraverseDir(str, vecFiles[i]);
				/*for (size_t j = 0; j < vecFiles.size(); j++)
				{
					Mat charMat = imread(string(CW2A(vecFiles[i][j])), CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
				}*/
			}

			Mat data, labels;   //特征矩阵
			for (size_t i = 0; i < vecFiles[0].size(); i++)
			{
				for (int j = 0; j < len; j++)
				{
					Mat charMat = imread(string(CW2A(vecFiles[j][i])), IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
					Mat temp;
					resize(charMat, temp, Size(20, 20));
					data.push_back(temp.reshape(0, 1));
					labels.push_back((int)labelchar[j]);
				}
				Mat tempe;
				if (image_r.channels() == 3)
					cvtColor(image_r(Rect(2114, 555, 20, 20)), tempe, COLOR_BGR2GRAY);
				else
					tempe = image_r(Rect(2114, 555, 20, 20));
				//(2103,498),(2141,782)
				data.push_back(tempe.reshape(0, 1));
				labels.push_back((int)' ');
			}
			data.convertTo(data, CV_32FC1);
			int K = 5;
			Ptr<TrainData> tData = TrainData::create(data, ROW_SAMPLE, labels);
			Ptr<KNearest> model = KNearest::create();
			model->setDefaultK(K);
			model->setIsClassifier(true);
			model->train(tData);
			model->save("versionKnn.xml");
		}
			break;
		case 3:
		{
			vector<CString> vecFiles;
			CString imgFile = PickImageFile();
			if (imgFile.IsEmpty()) {
				//MessageBox(_T(""));
				return;
			}
			TraverseDir(imgFile, vecFiles);
			for (size_t i = 0; i < vecFiles.size(); i++)
			{
				reduceImage.UseBlocksChecker(imread(string(CW2A(vecFiles[i])), IMREAD_ANYDEPTH | IMREAD_ANYCOLOR), J, i);
			}
		}
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
	case 30:
	{
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int size = _ttoi(str0);
		if (size > 21 || size < 1)
		{
			MessageBox(_T("Kernel Size 不可小于1或大于21！"));
			break;
		}
		reduceImage.UseMorphologyEx(image_r, J, method_one_selecter.GetCurSel(), 0, size);
	}
		break;
	case 31:
	{

	}
		break;
	case 32:
	{
		switch (method_one_selecter.GetCurSel())
		{
		case 0:
			reduceImage.UseknnTrain(image_r);
			break;
		case 1:
		{
			string trainFile = OpenknnTrainFile();
			if (trainFile == "no")
				return;
			float r;
			image_r.copyTo(J);
			//Mat results;
			vector<float> results, neighborResponses, dist;
			reduceImage.UseknnFindNearest(J, trainFile, r, results, neighborResponses, dist);
			for (size_t i = 0; i < results.size(); i++)
			{
				CString re;
				re.Format(_T("result %d: %f"), i, results[i]);
				MessageBox(re);
			}
			for (size_t i = 0; i < neighborResponses.size(); i++)
			{
				CString re;
				re.Format(_T("neighborResponses %d: %f"), i, neighborResponses[i]);
				MessageBox(re);
			}
			for (size_t i = 0; i < dist.size(); i++)
			{
				CString re;
				re.Format(_T("dist %d: %f"), i, dist[i]);
				MessageBox(re);
			}
			//imshow("results", results);
			//imshow("neighborResponses", neighborResponses);
			//imshow("dist", dist);
			CString msgRes;
			msgRes.Format(_T("识别的数字为：%f"), r);
			MessageBox(msgRes);
		}
			break;
		case 2:
		{
			vector<CString> vecFiles;
			CString imgFile = PickImageFile();
			if (imgFile.IsEmpty()) {
				//MessageBox(_T(""));
				return;
			}
			TraverseDir(imgFile, vecFiles);
			CString labelstr;
			m_num_edit.GetWindowTextW(labelstr);
			int label = _ttoi(labelstr);
			for (size_t i = 0; i < vecFiles.size(); i++)
			{
				Mat temp = imread(string(CW2A(vecFiles[i])));
				if (temp.channels() == 3)
					cvtColor(temp, temp, COLOR_BGR2GRAY);
				resize(temp, temp, Size(20, 20));
				knnData.push_back(temp.reshape(0, 1));
				knnLabels.push_back(label);
			}
			if (IDOK == MessageBox(_T("是否保存KNN学习数据？"), _T("提示！"), MB_ACTIVE))
			{
				knnData.convertTo(knnData, CV_32FC1);
				int K = 5;
				Ptr<TrainData> tData = TrainData::create(knnData, ROW_SAMPLE, knnLabels);
				Ptr<KNearest> model = KNearest::create();
				model->setDefaultK(K);
				model->setIsClassifier(true);
				model->train(tData);
				model->save("knn.xml");
			}
		}
			break;
		default:
			break;
		}
	}
		break;
	case 33:
	{
		switch (method_one_selecter.GetCurSel())
		{
		case 0:
			reduceImage.UseSVMTrain(image_r);
			break;
		case 1:
		{
			string trainFile = OpenknnTrainFile();
			if (trainFile == "no")
				return;
			float r;
			image_r.copyTo(J);
			reduceImage.UseSVMPredict(J, trainFile, r);
			CString msgRes;
			msgRes.Format(_T("识别的数字为：%f"), r);
			MessageBox(msgRes);
		}
			break;
		case 2:
		{
			string dataFile = OpenDataTrainFile();
			if (dataFile == "no")
				return;
			reduceImage.UseSVMTrain(dataFile);
		}
			break;
		default:
			break;
		}
	}
		break;
	case 34:
	{
		reduceImage.UseLaplacian(image_r, J, 5);
	}
		break;
	case 35:
	{
		string imgFile = OpenImageFile();
		Mat templ = imread(imgFile, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
		if (!templ.data)
		{
			MessageBox(_T("打开图片失败"));
			return;
		}
		//reduceImage.UseFeatureDetector(image_r, templ, J, method_one_selecter.GetCurSel());
		Mat input = image_r(Rect(61, 1060, 1394, 1178)).clone();
		double t = (double)getTickCount();
		reduceImage.UseBlocksChecker2(input, J, templ);
		t = ((double)getTickCount() - t) / getTickFrequency();
		CString tempTime;
		tempTime.Format(_T("耗时%lf秒"), t);
		MessageBox(tempTime);
	}
		break;
	case 36:
	{
		double t;
		reduceImage.UsePerspectiveTransform(image_r, J, t);
		CString timeStr;
		timeStr.Format(_T("耗时%lf"), t);
		MessageBox(timeStr);
	}
		break;
	default:
		MessageBox(_T("没有选择图像处理方法！"));
		break;
	}

	if (!J.data)
	{
		MessageBox(_T("处理图片失败"));
		return;
	}
	else
	{
		image_s = J;
		//将图像显示都新创建的窗口中
		imshow(showWindowName, J);
	}
}


void CMyOpenCVApplicationDlg::OnCbnSelchangeComboMethod()
{
	// TODO: 在此添加控件通知处理程序代码
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
		HideMethodTwentySeven();
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
	case 30:
		HideMethodThirtyOne();
		break;
	case 31:
		HideMethodThirtyTwo();
		break;
	case 32:
		HideMethodThirtyThree();
		break;
	case 33:
		HideMethodThirtyFour();
		break;
	case 34:
		break;
	case 35:
		HideMethodThirtySix();
		break;
	case 36:
		break;
	default:
		break;
	}
	switch (selectMethod.GetCurSel()) {
	case 0:
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
		ShowMethodTwentySeven();
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
	case 30:
		ShowMethodThirtyOne();
		m_last_spin_num = 30;
		break;
	case 31:
		ShowMethodThirtyTwo();
		m_last_spin_num = 31;
		break;
	case 32:
		ShowMethodThirtyThree();
		m_last_spin_num = 32;
		break;
	case 33:
		ShowMethodThirtyFour();
		m_last_spin_num = 33;
		break;
	case 34:
		m_last_spin_num = 34;
		break;
	case 35:
		ShowMethodThirtySix();
		m_last_spin_num = 35;
		break;
	case 36:
		m_last_spin_num = 36;
		break;
	default:
		break;
	}
}

// 展开颜色缩减方法选项
void CMyOpenCVApplicationDlg::ShowMethodOne()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	m_static_one.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("颜色缩减法"));
	method_one_selecter.InsertString(1, _T("The iterator (safe) method"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏颜色缩减法选项
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
	//图像路径
	string imgFile = OpenImageFile();
	if (imgFile == "no")
	{
		MessageBox(_T("打开图片失败1"));
		return;
	}
	//读入图像
	//image_r = imread(imgFile, CV_LOAD_IMAGE_GRAYSCALE);
	image_r = imread(imgFile, IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);

	if (!image_r.data)
	{
		MessageBox(_T("打开图片失败2"));
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

	// TODO:  在此添加您专用的创建代码
	return 0;
}


void CMyOpenCVApplicationDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


void CMyOpenCVApplicationDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDialogEx::OnClose();
}


// 展示图像求和法选项
void CMyOpenCVApplicationDlg::ShowMethodThree()
{
	m_multiple_blend.ShowWindow(SW_SHOW);
	m_alpha_edit.ShowWindow(SW_SHOW);
	m_static_three.ShowWindow(SW_SHOW);
}


// 隐藏图像求和法选项
void CMyOpenCVApplicationDlg::HideMethodThree()
{
	m_multiple_blend.ShowWindow(SW_HIDE);
	m_alpha_edit.ShowWindow(SW_HIDE);
	m_static_three.ShowWindow(SW_HIDE);
}

string CMyOpenCVApplicationDlg::OpenDataTrainFile()
{
	CFileDialog findFileDlg(
		TRUE,  // TRUE是创建打开文件对话框，FALSE则创建的是保存文件对话框
		_T(".data"),  // 默认的打开文件的类型
		NULL,  // 默认打开的文件名
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // 打开只读文件
		_T("knnXML(*.data)|*.data|")  // 所有可以打开的文件类型
	);
	//图像名称
	string imgFile;
	if (IDOK == findFileDlg.DoModal())
	{
		imgFile = string(CW2A(findFileDlg.GetPathName()));
	}
	else
	{
		imgFile = "no";
	}
	return imgFile;
}

string CMyOpenCVApplicationDlg::OpenknnTrainFile()
{
	CFileDialog findFileDlg(
		TRUE,  // TRUE是创建打开文件对话框，FALSE则创建的是保存文件对话框
		_T(".xml"),  // 默认的打开文件的类型
		NULL,  // 默认打开的文件名
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // 打开只读文件
		_T("knnXML(*.xml)|*.xml|所有文件 (*.*)|*.*||")  // 所有可以打开的文件类型
	);
	//图像名称
	string imgFile;
	if (IDOK == findFileDlg.DoModal())
	{
		imgFile = string(CW2A(findFileDlg.GetPathName()));
	}
	else
	{
		imgFile = "no";
	}
	return imgFile;
}

// 打开图片
string CMyOpenCVApplicationDlg::OpenImageFile()
{
	CFileDialog findFileDlg(
		TRUE,  // TRUE是创建打开文件对话框，FALSE则创建的是保存文件对话框
		_T(".jpg"),  // 默认的打开文件的类型
		NULL,  // 默认打开的文件名
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // 打开只读文件
		_T("jpg图片(*.jpg)|*.jpg|png图片(*.PNG)|*.PNG|所有文件 (*.*)|*.*||")  // 所有可以打开的文件类型
	);
	//图像名称
	string imgFile;
	if (IDOK == findFileDlg.DoModal())
	{
		imgFile = string(CW2A(findFileDlg.GetPathName()));
	}
	else
	{
		imgFile = "no";
	}
	return imgFile;
}


// 展示改变亮度和对比度选项
void CMyOpenCVApplicationDlg::ShowMethodFour()
{
	m_static_four_alpha.ShowWindow(SW_SHOW);
	m_static_four_beta.ShowWindow(SW_SHOW);
	m_alpha_edit.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
}


// 隐藏改变亮度和对比度选项
void CMyOpenCVApplicationDlg::HideMethodFour()
{
	m_static_four_alpha.ShowWindow(SW_HIDE);
	m_static_four_beta.ShowWindow(SW_HIDE);
	m_alpha_edit.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
}


// 隐藏平滑处理选项
void CMyOpenCVApplicationDlg::HideMethodSix()
{
	m_static_six.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 展示平滑处理选项
void CMyOpenCVApplicationDlg::ShowMethodSix()
{
	m_static_six.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("均值平滑"));
	method_one_selecter.InsertString(1, _T("高斯平滑"));
	method_one_selecter.InsertString(2, _T("中值平滑"));
	method_one_selecter.InsertString(3, _T("双边平滑"));
	method_one_selecter.InsertString(4, _T("自定义线性滤波器"));
	method_one_selecter.SetCurSel(0);
}


// 展示腐蚀选项
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


// 隐藏腐蚀选项
void CMyOpenCVApplicationDlg::HideMethodSevenOrEight()
{
	m_static_seven_eight.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 展示图像金字塔选项
void CMyOpenCVApplicationDlg::ShowMethodNine()
{
	m_static_nine.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("向上采样"));
	method_one_selecter.InsertString(1, _T("向下采样"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏图像金字塔选项
void CMyOpenCVApplicationDlg::HideMethodNine()
{
	m_static_nine.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}

//展示阈值化选项
void CMyOpenCVApplicationDlg::ShowMethodTen()
{
	m_static_ten.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("二进制阈值"));
	method_one_selecter.InsertString(1, _T("反二进制阈值"));
	method_one_selecter.InsertString(2, _T("截断阈值"));
	method_one_selecter.InsertString(3, _T("0阈值"));
	method_one_selecter.InsertString(4, _T("反0阈值"));
	method_one_selecter.InsertString(5, _T("自适应阈值"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏阈值化选项
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
	method_one_selecter.InsertString(0, _T("常数填充"));
	method_one_selecter.InsertString(1, _T("临近值填充"));
	method_one_selecter.SetCurSel(0);
}


void CMyOpenCVApplicationDlg::HideMethodEleven()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


void CMyOpenCVApplicationDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!image_r.data)
	{
		MessageBox(_T("没有载入图像！"));
		return;
	}
	else if (image_r.channels() == 1)
	{
		MessageBox(_T("该图像是灰度图像！"));
		return;
	}
	cvtColor(image_r, image_r, COLOR_RGB2GRAY);
	imshow(readWindowName, image_r);
}


void CMyOpenCVApplicationDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	image_r = image_s;
	imshow(readWindowName, image_r);
}


// 显示边缘检测选项
void CMyOpenCVApplicationDlg::ShowMethodTwelve()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("Sobel算子"));
	method_one_selecter.InsertString(1, _T("Scharr算子"));
	method_one_selecter.InsertString(2, _T("Laplace算子"));
	method_one_selecter.InsertString(3, _T("Canny算子"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏边缘检测选项
void CMyOpenCVApplicationDlg::HideMethodTwelve()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 显示霍夫线变换选项
void CMyOpenCVApplicationDlg::ShowMethodThirteen()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("标准霍夫线变换"));
	method_one_selecter.InsertString(1, _T("统计概率霍夫线变换"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏霍夫线变换选项
void CMyOpenCVApplicationDlg::HideMethodThirteen()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_double_one.ShowWindow(SW_HIDE);
	m_double_two.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 显示霍夫圆变换选项
void CMyOpenCVApplicationDlg::ShowMethodFourteen()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("霍夫梯度法"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏霍夫圆变换选项
void CMyOpenCVApplicationDlg::HideMethodFourteen()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 鼠标响应处理函数
void on_mouse(int event, int x, int y, int flags, void *ustc)
{
	//CMyOpenCVApplicationDlg *pWnd = (CMyOpenCVApplicationDlg *)CWnd::FromHandle(hWnd);
	if (event == EVENT_LBUTTONDOWN)//左键按下，读取初始坐标
	{
		thisDlg->SetSPt(x, y);
	}
	if (event == EVENT_MOUSEMOVE && (flags & EVENT_FLAG_LBUTTON))
	{
		if (x != thisDlg->GetSPt().x && y != thisDlg->GetSPt().y)
		{
			thisDlg->SetEPt(x, y);
			thisDlg->DrawRectangle();
		}
	}
	if (event == EVENT_LBUTTONUP)
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


void CMyOpenCVApplicationDlg::SetSPt(int x, int y)
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
	// TODO: 在此添加命令处理程序代码
	Mat temp(image_r, Rect(min(s_pt.x, e_pt.x), min(s_pt.y, e_pt.y), abs(e_pt.x - s_pt.x), abs(e_pt.y - s_pt.y)));
	temp.copyTo(image_s);
	imshow(showWindowName, image_s);
}


void CMyOpenCVApplicationDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	CRect rect;//定义矩形区域
	GetDlgItem(IDC_READIMAGE)->GetWindowRect(&rect);//获取控件区域的矩形
	if (rect.PtInRect(point))
	{
		CMenu   menu;
		VERIFY(menu.LoadMenu(IDR_MENU1));

		CMenu*   pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this); //在鼠标当前位置显示指定菜单
	}
}


void CMyOpenCVApplicationDlg::OnCopyCoordinate()
{
	// TODO: 在此添加命令处理程序代码
	if (OpenClipboard())
	{
		EmptyClipboard();
		CString str;
		str.Format(_T("(%d,%d),(%d,%d)"), min(s_pt.x, e_pt.x), min(s_pt.y, e_pt.y), max(s_pt.x, e_pt.x), max(s_pt.y, e_pt.y));
		HANDLE hClip = GlobalAlloc(GMEM_MOVEABLE, str.GetLength() + 1);
		USES_CONVERSION;
		strcpy_s((char *)GlobalLock(hClip), str.GetLength() + 1, T2A(str));
		GlobalUnlock(hClip);//解锁  
		SetClipboardData(CF_TEXT, hClip);//设置格式  
		CloseClipboard();
	}
}


void CMyOpenCVApplicationDlg::OnHandlePart()
{
	// TODO: 在此添加命令处理程序代码
	Mat temp;
	Mat p0(image_r, Rect(min(s_pt.x, e_pt.x), min(s_pt.y, e_pt.y), abs(e_pt.x - s_pt.x), abs(e_pt.y - s_pt.y)));
	image_r.copyTo(temp);
	p0.copyTo(image_r);
	OnBnClickedOk();
	temp.copyTo(image_r);
}


// 显示重映射选项
void CMyOpenCVApplicationDlg::ShowMethodFifteen()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("缩小一半居中显示"));
	method_one_selecter.InsertString(1, _T("上下颠倒"));
	method_one_selecter.InsertString(2, _T("左右颠倒"));
	method_one_selecter.InsertString(3, _T("上下左右颠倒"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏重映射选项
void CMyOpenCVApplicationDlg::HideMethodFifteen()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 显示旋转图片选项
void CMyOpenCVApplicationDlg::ShowMethodSeventeen()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_spin_one.SetRange32(-360, 360);
}


// 隐藏旋转图片选项
void CMyOpenCVApplicationDlg::HideMethodSeventeen()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_spin_one.SetRange32(0, 255);//表示数值只能在1到256内变化
}


// 显示直方图计算方法
void CMyOpenCVApplicationDlg::ShowMethodNineteen()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_spin_one.SetRange32(1, 256);
}

// 隐藏直方图计算方法
void CMyOpenCVApplicationDlg::HideMethodNineteen()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_spin_one.SetRange32(0, 255);//表示数值只能在1到256内变化
}


// 展示直方图对比方法
void CMyOpenCVApplicationDlg::ShowMethodTwenty()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("Correlation"));
	method_one_selecter.InsertString(1, _T("Chi-square"));
	method_one_selecter.InsertString(2, _T("Intersection"));
	method_one_selecter.InsertString(3, _T("Bhattacharyya"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏直方图对比方法
void CMyOpenCVApplicationDlg::HideMethodTwenty()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 展示反射投影方法
void CMyOpenCVApplicationDlg::ShowMethodTwentyOne()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_spin_one.SetRange32(1, 256);
}


// 隐藏反射投影方法
void CMyOpenCVApplicationDlg::HideMethodTwentyOne()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_spin_one.SetRange32(0, 255);//表示数值只能在1到256内变化
}


// 展示模板匹配方法
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


// 隐藏模板匹配方法
void CMyOpenCVApplicationDlg::HideMethodTwentyTwo()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


void CMyOpenCVApplicationDlg::OnBnClickedMultipleBlend()
{
	// TODO: 在此添加控件通知处理程序代码
	vector<CString> vecFiles;
	CString imgFile = PickImageFile();
	if (imgFile.IsEmpty()) {
		return;
	}
	TraverseDir(imgFile, vecFiles);
	int len = vecFiles.size();
	double skip_num = 0.0;
	Mat I = imread(string(CW2A(vecFiles[0])), IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
	Mat J;
	for (int i = 1; i < len; i++)
	{
		double alpha = 1.0 / (i + 1.0 - skip_num);
		Mat next = imread(string(CW2A(vecFiles[i])), IMREAD_ANYDEPTH | IMREAD_ANYCOLOR);
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
	str.Format(_T("共有%d张图片，有%d张与第一张大小不一"), len, (int)skip_num);
	MessageBox(str);
}


// 打开图片文件夹
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


// 递归扫码文件夹下文件
void CMyOpenCVApplicationDlg::TraverseDir(CString& strDir, vector<CString>& vecFiles)
{
	CFileFind ff;
	//在路径后面添加\*.*后缀
	if (strDir.Right(1) != "\\")
		strDir += "\\";
	strDir += "*.*";
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
			//CString name = ff.GetFileName();//获取带后缀的文件名
			CString name = ff.GetFilePath();//获取文件路径
											//CString name = ff.GetFileTitle();//获取不带后缀的文件名
			vecFiles.push_back(name);
		}

	}
	ff.Close();
}

// 展示寻找轮廓方法
void CMyOpenCVApplicationDlg::ShowMethodTwentyThree()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("全部轮廓"));
	method_one_selecter.InsertString(1, _T("最外层轮廓"));
	method_one_selecter.SetCurSel(0);
}

// 隐藏寻找轮廓方法
void CMyOpenCVApplicationDlg::HideMethodTwentyThree()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}

// 展示寻找凸包方法
void CMyOpenCVApplicationDlg::ShowMethodTwentyFour()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_check_is_draw_contours.ShowWindow(SW_SHOW);
}

// 隐藏寻找凸包方法
void CMyOpenCVApplicationDlg::HideMethodTwentyFour()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_check_is_draw_contours.ShowWindow(SW_HIDE);
}


// 展示多边形拟合方法
void CMyOpenCVApplicationDlg::ShowMethodTwentyFive()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	m_check_is_draw_contours.ShowWindow(SW_SHOW);
}


// 隐藏多边形拟合方法
void CMyOpenCVApplicationDlg::HideMethodTwentyFive()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	m_check_is_draw_contours.ShowWindow(SW_HIDE);
}


// 展示包覆图形方法
void CMyOpenCVApplicationDlg::ShowMethodTwentySix()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("最小包覆正矩形"));
	method_one_selecter.InsertString(1, _T("最小包覆斜矩形"));
	method_one_selecter.InsertString(2, _T("最小包覆圆形"));
	method_one_selecter.InsertString(3, _T("最小包覆椭圆"));
	method_one_selecter.InsertString(4, _T("轮廓矩"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏包覆图形方法
void CMyOpenCVApplicationDlg::HideMethodTwentySix()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


void CMyOpenCVApplicationDlg::OnCbnSelchangeMethodOneSelecter()
{
	// TODO: 在此添加控件通知处理程序代码
	switch (m_last_spin_num)
	{
	case 12:
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
		break;
	case 26:
		if (method_one_selecter.GetCurSel() == 1)
		{
			m_scan_code_type.ShowWindow(SW_SHOW);
			m_scan_code_type.InsertString(0, _T("QR Code"));
			m_scan_code_type.InsertString(1, _T("Data Matrix"));
			m_scan_code_type.SetCurSel(0);
		}
		else
		{
			m_scan_code_type.ShowWindow(SW_HIDE);
			m_scan_code_type.ResetContent();
		}
		break;
	default:
		break;
	}
}


// 展示寻找二维码方法
void CMyOpenCVApplicationDlg::ShowMethodTwentyEight()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
}


// 隐藏寻找二维码方法
void CMyOpenCVApplicationDlg::HideMethodTwentyEight()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
}


// 展示组合方法
void CMyOpenCVApplicationDlg::ShowMethodTwentyNine()
{
	m_config_bt.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("二维码判定"));
	method_one_selecter.InsertString(1, _T("字符抓取"));
	method_one_selecter.InsertString(2, _T("字符学习"));
	method_one_selecter.InsertString(3, _T("端子检测"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏组合方法
void CMyOpenCVApplicationDlg::HideMethodTwentyNine()
{
	m_config_bt.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 点到线的最短距离
Point CMyOpenCVApplicationDlg::PointToLineDist(Point p, Point p1, Point p2)
{
	Point result;
	//向量内积，向量1为点1到点p，向量2为点1到点2
	double cross = (p2.x - p1.x) * (p.x - p1.x) + (p2.y - p1.y) * (p.y - p1.y);
	//向量2的长度的平方
	double d2 = (p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y);
	//double d2 = norm((p2.x - p1.x), (p2.y - p1.y));
	
	//r为向量1在向量2上投影向量与向量2的比值，方向相反则为负数
	double r = cross / d2;
	result.x = p1.x + (p2.x - p1.x) * r;
	result.y = p1.y + (p2.y - p1.y) * r;
	return result;
	//sqrt((p.x - result.x) * (p.x - result.x) + (p.y - result.y) * (p.y - result.y));
}


void CMyOpenCVApplicationDlg::OnBnClickedButtonConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	MessageBox(_T("后续添加"));
}


// 参数初始化
void CMyOpenCVApplicationDlg::initLineConfig()
{
	RLLineConfig.canny_size = 3;
	RLLineConfig.line_direction = true;
	RLLineConfig.maxLineGap = 20;
	RLLineConfig.minLinLength = 200;
	RLLineConfig.threshold = 40;
	RLLineConfig.threshold1 = 80;
	RLLineConfig.threshold2 = 200;
	RLLineConfig.rect = Rect(2147, 467, 300, 328);
	RLLineConfig.same = true;

	UDLineConfig.canny_size = 3;
	UDLineConfig.line_direction = false;
	UDLineConfig.maxLineGap = 20;
	UDLineConfig.minLinLength = 50;
	UDLineConfig.threshold = 35;
	UDLineConfig.threshold1 = 80;
	UDLineConfig.threshold2 = 200;
	UDLineConfig.rect = Rect(976, 1641, 303, 246);
	UDLineConfig.same = true;

	ScanConfig.ErosionMethod = 0;
	ScanConfig.ErosionSize = 1;
	ScanConfig.ErosionTimes = 2;
	ScanConfig.threshold = 100;

	codeConfig.BlurSize = 3;
	codeConfig.ErosionMethod = 0;
	codeConfig.ErosionSize = 1;
	codeConfig.ErosionTimes = 1;
	codeConfig.threshold = 100;

	projectConfig.codeType = 1;
	//projectConfig.rect = Rect(0, 0, 200, 200);
	projectConfig.rect = Rect(1371, 403, 418, 379);
	projectConfig.pixel_scale_x = 37.0;
	projectConfig.pixel_scale_y = 37.0;
	projectConfig.corner = 3;
}


Point CMyOpenCVApplicationDlg::FindMidPoint(Point p1, Point p2, int x_offset, int y_offset)
{
	return Point((p1.x + p2.x) / 2 + x_offset, (p1.y + p2.y) / 2 + y_offset);
}


double CMyOpenCVApplicationDlg::GetLineLength(Point p1, Point p2, double pixel_scale_x, double pixel_scale_y)
{
	double x1 = p1.x / pixel_scale_x;
	double x2 = p2.x / pixel_scale_x;
	double y1 = p1.y / pixel_scale_y;
	double y2 = p2.y / pixel_scale_y;
	return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}


// 二维码判定
void CMyOpenCVApplicationDlg::CodeJudgement(Mat& J)
{
	//计时
	double t = (double)getTickCount();

	J = image_r.clone();
	if (J.channels() == 1)
		cvtColor(J, J, COLOR_GRAY2BGR);
	RotatedRect rotatedRect;
	Mat code(image_r, projectConfig.rect);
	Mat temp = code.clone();

	string type;
	string data;

	Point2f fourPoint2f[4];
	float angle;

	switch (projectConfig.codeType)
	{
	case 0:
	{
		if (reduceImage.FindCodeCoutours(code, temp, codeConfig, rotatedRect))
		{
			Rect tempRect = rotatedRect.boundingRect();
			Rect scanRect = Rect(tempRect.x + projectConfig.rect.x, tempRect.y + projectConfig.rect.y, tempRect.width, tempRect.height);
			Mat ScanRect = Mat(image_r, scanRect).clone();
			reduceImage.PretreatmentForScanCode(ScanRect, ScanRect, ScanConfig);

			reduceImage.ScanBarCode(ScanRect, type, data);
		}

		if (type.empty())
		{
			MessageBox(_T("error code"));
			return;
		}

		/** returns 4 vertices of the rectangle
		@param pts The points array for storing rectangle vertices. The order is bottomLeft, topLeft, topRight, bottomRight.
		*/
		//将rectPoint变量中存储的坐标值放到 fourPoint的数组中  
		rotatedRect.points(fourPoint2f);

		angle = rotatedRect.angle;

		if (angle < 0)
		{
			Point2f temp = fourPoint2f[0];
			for (int i = 0; i < 3; i++)
			{
				fourPoint2f[i] = fourPoint2f[i + 1];
			}
			fourPoint2f[3] = temp;
		}
		angle += 90.0f;
	}
		break;
	case 1:
	{
		reduceImage.PretreatmentForScanCode(code, temp, ScanConfig);
		vector<Point> codePoint;
		if (reduceImage.ScanBarCodeForZxing(temp, 1, data, codePoint))
		{
			//4点顺序改成与RotatedRect一致
			Point p = codePoint[0];
			codePoint[0] = codePoint[1];
			codePoint[1] = p;
			//角点方向不同，顺序不同，需要调整
			for (int i = 0; i < 4; i++)
				fourPoint2f[i] = codePoint[(i + projectConfig.corner) % 4];
		}
		else
		{
			MessageBox(_T("error code"));
			return;
		}
		type = "data matrix";
		angle = 90.0f;
		//reduceImage.DrawCodePoint(J, codePoint);
	}
		break;
	default:
		break;
	}

	double Rlen;
	double CRlen;
	if (RLLineConfig.same) 
		handleCodeLine(image_r, J, fourPoint2f[2], fourPoint2f[3], RLLineConfig, Rlen, CRlen);
	else
		handleCodeLine(image_r, J, fourPoint2f[0], fourPoint2f[1], RLLineConfig, Rlen, CRlen);

	double Dlen;
	double CDlen;
	if(UDLineConfig.same)
		handleCodeLine(image_r, J, fourPoint2f[0], fourPoint2f[3], UDLineConfig, Dlen, CDlen);
	else
		handleCodeLine(image_r, J, fourPoint2f[1], fourPoint2f[2], UDLineConfig, Dlen, CDlen);

	t = ((double)getTickCount() - t) / getTickFrequency();
	CString info;
	info.Format(_T("条码类型：%s\n条码：%s\n码高：%.3lf\n码宽：%.3lf\n右距离：%.3lf\n左距离：%.3lf\n角度：%f\n耗时：%lf秒"),
		CStringW(type.c_str()), CStringW(data.c_str()), CRlen, CDlen, Rlen, Dlen, angle, t);
	//imshow(showWindowName, J);
	MessageBox(info);
}


// 展示角点检测方法
void CMyOpenCVApplicationDlg::ShowMethodThirty()
{
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("Harris角点检测"));
	method_one_selecter.InsertString(1, _T("Shi-Tomasi角点检测"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏角点检测方法
void CMyOpenCVApplicationDlg::HideMethodThirty()
{
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 暂时扫码选项
void CMyOpenCVApplicationDlg::ShowMethodTwentySeven()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("ZBar"));
	method_one_selecter.InsertString(1, _T("zxing"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏扫码选项
void CMyOpenCVApplicationDlg::HideMethodTwentySeven()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
	m_scan_code_type.ShowWindow(SW_HIDE);
	m_scan_code_type.ResetContent();
}

// 处理二维码到边缘距离
void CMyOpenCVApplicationDlg::handleCodeLine(const Mat& I, Mat& J, Point cp1, Point cp2, configForLine lineConfig, double& Len, double& CLen)
{
	Point mid1 = FindMidPoint(cp1, cp2, projectConfig.rect.x, projectConfig.rect.y);
	Mat lineRect(image_r, lineConfig.rect);
	Vec4i l;
	CLen = GetLineLength(cp1, cp2, projectConfig.pixel_scale_x, projectConfig.pixel_scale_y);
	Point cp1_os = Point(cp1.x + projectConfig.rect.x, cp1.y + projectConfig.rect.y);
	Point cp2_os = Point(cp2.x + projectConfig.rect.x, cp2.y + projectConfig.rect.y);
	line(J, cp1_os, cp2_os, Scalar(0, 0, 255), 5);
	if (reduceImage.PretreatmentForFindLine(lineRect, lineConfig, l))
	{
		Point R1 = Point(l[0] + lineConfig.rect.x, l[1] + lineConfig.rect.y);
		Point R2 = Point(l[2] + lineConfig.rect.x, l[3] + lineConfig.rect.y);
		Point mid2 = PointToLineDist(mid1, R1, R2);
		line(J, R1, R2, Scalar(0, 0, 255), 5);
		//line(J, mid1, mid2, Scalar(0, 0, 255), 5);
		Len = GetLineLength(mid1, mid2, projectConfig.pixel_scale_x, projectConfig.pixel_scale_y);
	}
}


// 两线角度
void CMyOpenCVApplicationDlg::angleLine2Line(Point2f p1, Point2f p2, Point2f p3, Point2f p4)
{
}


// 展示形态学变换方法
void CMyOpenCVApplicationDlg::ShowMethodThirtyOne()
{
	m_static_seven_eight.ShowWindow(SW_SHOW);
	m_num_edit.ShowWindow(SW_SHOW);
	m_spin_one.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("Opening"));
	method_one_selecter.InsertString(1, _T("Closing"));
	method_one_selecter.InsertString(2, _T("Gradient"));
	method_one_selecter.InsertString(3, _T("Top Hat"));
	method_one_selecter.InsertString(4, _T("Black Hat"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏形态学变换方法
void CMyOpenCVApplicationDlg::HideMethodThirtyOne()
{
	m_static_seven_eight.ShowWindow(SW_HIDE);
	m_num_edit.ShowWindow(SW_HIDE);
	m_spin_one.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 相机初始化
void CMyOpenCVApplicationDlg::initCamera()
{
	try 
	{
		CTlFactory& tlFactory = CTlFactory::GetInstance();

		DeviceInfoList_t devices;

		if (tlFactory.EnumerateDevices(devices) == 0)
		{
			//throw RUNTIME_EXCEPTION("No camera present.");
		}

		// Create an array of instant cameras for the found devices.
		CInstantCameraArray cameras(devices.size());
		
		for (size_t i = 0; i < cameras.GetSize(); ++i)
		{
			cameras[i].Attach(tlFactory.CreateDevice(devices[i]));
			const string key = cameras[i].GetDeviceInfo().GetModelName().c_str();
			camerasIndex[key] = i;
			//allCameras.insert(map<string, CInstantCamera>::value_type(key, cameras[i]));
		}
	}
	catch(GenICam::GenericException &e)
	{
		MessageBox(_T("GenericException"));
	}
}


// basler相机
void CMyOpenCVApplicationDlg::ShowMethodThirtyTwo()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.ResetContent();
	thread thc(&CMyOpenCVApplicationDlg::ThreadCamera,this);
	thc.detach();
	m_camera_bt.ShowWindow(SW_SHOW);
}


// basler相机
void CMyOpenCVApplicationDlg::HideMethodThirtyTwo()
{
	//停止线程thc
	if (cameraRunning)
	{
		cameraRunning = false;
		cameraIsOpen = false;
		openCamera.notify_all();
		grabImage.notify_all();
	}
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
	m_camera_bt.ShowWindow(SW_HIDE);
}


// 相机线程
void CMyOpenCVApplicationDlg::ThreadCamera()
{
	PylonInitialize();
	try
	{
		CTlFactory& tlFactory = CTlFactory::GetInstance();

		DeviceInfoList_t devices;

		if (tlFactory.EnumerateDevices(devices) == 0)
		{
			//throw RUNTIME_EXCEPTION("No camera present.");
			PylonTerminate();
			cameraRunning = false;
			//MessageBox(_T("basler"));
			return;
		}
		cameraRunning = true;
		for (size_t i = 0; i < devices.size(); i++)
			method_one_selecter.InsertString(i, CStringW(devices[i].GetModelName().c_str()));
		method_one_selecter.SetCurSel(0);
		while (true)
		{
			try
			{
				if (!cameraRunning)
					break;
				unique_lock<mutex> lock(mtxCamera);
				openCamera.wait(lock);
				lock.unlock();
				if (!cameraRunning)
					break;
				CInstantCamera camera(tlFactory.CreateDevice(devices[method_one_selecter.GetCurSel()]));
				//获取相机节点映射以获得相机参数
				INodeMap& nodemap = camera.GetNodeMap();
				//打开相机
				camera.Open();
				//获取相机成像宽度和高度
				CIntegerPtr width = nodemap.GetNode("Width");
				CIntegerPtr height = nodemap.GetNode("Height");
				//设置相机最大缓冲区,默认为10
				camera.MaxNumBuffer = 5;
				// 新建pylon ImageFormatConverter对象.
				CImageFormatConverter formatConverter;
				//确定输出像素格式
				formatConverter.OutputPixelFormat = PixelType_BGR8packed;
				// 创建一个Pylonlmage后续将用来创建OpenCV images
				CPylonImage pylonImage;
				while (true)
				{
					try
					{
						unique_lock<mutex> lock2(mtxImage);
						grabImage.wait(lock2);
						lock2.unlock();
						if (!cameraIsOpen)
						{
							camera.Close();
							break;
						}
						// 开始抓取c_countOfImagesToGrab images.
						//相机默认设置连续抓取模式
						camera.StartGrabbing(1, GrabStrategy_LatestImageOnly);

						//抓取结果数据指针
						CGrabResultPtr ptrGrabResult;

						// 当c_countOfImagesToGrab images获取恢复成功时，Camera.StopGrabbing() 
						//被RetrieveResult()方法自动调用停止抓取
						while (camera.IsGrabbing())
						{
							// 等待接收和恢复图像，超时时间设置为5000 ms.
							camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
							if (ptrGrabResult->GrabSucceeded())
							{
								//将抓取的缓冲数据转化成pylon image.
								formatConverter.Convert(pylonImage, ptrGrabResult);

								// 将 pylon image转成OpenCV image.
								//image_r = Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t *)pylonImage.GetBuffer());

							}
						}
					}
					catch (const GenericException &e)
					{
						//抓取图片失败
					}
					
				}
			}
			catch (const GenericException &e)
			{
				//相机异常断开连接
			}
		}
	}
	catch (const GenericException &e)
	{

	}
	// Releases all pylon resources. 
	PylonTerminate();
}


void CMyOpenCVApplicationDlg::OnBnClickedButtonOpenCamera()
{
	// TODO: 在此添加控件通知处理程序代码
	if (method_one_selecter.GetCount() == 0)
	{
		MessageBox(_T("无可用相机"));
		return;
	}
	if (cameraIsOpen)
	{
		cameraIsOpen = false;
		openCamera.notify_all();
		grabImage.notify_all();
		m_camera_bt.SetWindowTextW(_T("打开相机"));
	}
	else
	{
		cameraIsOpen = true;
		openCamera.notify_all();
		m_camera_bt.SetWindowTextW(_T("关闭相机"));
	}
}


void CMyOpenCVApplicationDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	if (cameraRunning)
	{
		cameraRunning = false;
		openCamera.notify_all();
		grabImage.notify_all();
	}
}


// 计算两线角度
double CMyOpenCVApplicationDlg::GetTheta(Point p1, Point p2, Point p3, Point p4)
{
	//移动线段
	Point p = Point(p4.x - p3.x + p1.x, p4.y - p3.y + p1.y);
	//向量内积
	double cross = (p2.x - p1.x) * (p.x - p1.x) + (p2.y - p1.y) * (p.y - p1.y);
	//向量的模的积
	double r1 = sqrt((p2.x - p1.x)*(p2.x - p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
	double r2 = sqrt((p.x - p1.x)*(p.x - p1.x) + (p.y - p1.y)*(p.y - p1.y));
	double theta = acos(cross / (r1*r2));
	return theta;
}

// 展示knn算法选项
void CMyOpenCVApplicationDlg::ShowMethodThirtyThree()
{
	m_num_edit.ShowWindow(SW_SHOW);
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("训练数据并保存"));
	method_one_selecter.InsertString(1, _T("识别图像"));
	method_one_selecter.InsertString(2, _T("训练数据(2)"));
	method_one_selecter.SetCurSel(0);
}

// 隐藏knn算法选项
void CMyOpenCVApplicationDlg::HideMethodThirtyThree()
{
	knnData = Mat();
	knnLabels = Mat();
	m_num_edit.ShowWindow(SW_HIDE);
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


// 展示SVM支持向量机算法
void CMyOpenCVApplicationDlg::ShowMethodThirtyFour()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("训练数据并保存"));
	method_one_selecter.InsertString(1, _T("识别图像"));
	method_one_selecter.InsertString(2, _T("训练字母库"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏SVM支持向量机算法
void CMyOpenCVApplicationDlg::HideMethodThirtyFour()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}


void CMyOpenCVApplicationDlg::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
	if (!image_r.data)
	{
		MessageBox(_T("没有图像"));
		return;
	}
	CFileDialog saveFileDlg(
		FALSE,  // TRUE是创建打开文件对话框，FALSE则创建的是保存文件对话框
		NULL,  // 默认的打开文件的类型
		_T("my_opencv_save_image"),  // 默认打开的文件名
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // 打开只读文件
		_T("jpg图片(*.jpg)|*.jpg|png图片(*.PNG)|*.PNG||")  // 所有可以打开的文件类型
	);
	if (IDOK == saveFileDlg.DoModal())
	{
		CString savePath = saveFileDlg.GetPathName();
		if (saveFileDlg.m_ofn.nFilterIndex == 1)
			savePath += _T(".jpg");
		if (saveFileDlg.m_ofn.nFilterIndex == 2)
			savePath += _T(".png");
		imwrite(string(CW2A(savePath.GetString())), image_r);
	}
}


// 特征框架
void CMyOpenCVApplicationDlg::ShowMethodThirtySix()
{
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("Brute Force"));
	method_one_selecter.InsertString(1, _T("FLANN based"));
	method_one_selecter.InsertString(2, _T("寻找图片"));
	method_one_selecter.SetCurSel(0);
}


// 特征框架
void CMyOpenCVApplicationDlg::HideMethodThirtySix()
{
	method_one_selecter.ShowWindow(SW_HIDE);
	method_one_selecter.ResetContent();
}
