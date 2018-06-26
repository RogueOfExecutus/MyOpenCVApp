
// MyOpenCVApplicationDlg.cpp : 实现文件
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
	selectMethod.InsertString(21, _T("不处理"));
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
		//第二图像路径
		String imgFile = OpenImageFile();
		Mat K = imread(imgFile, CV_LOAD_IMAGE_COLOR);
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
			INT_PTR res = MessageBox(_T("该图像不是灰度图像，是否先转换成灰度图像再执行"),_T("提示！"),MB_ACTIVE);
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
		reduceImage.UseHoughLines(image_r, J, method_one_selecter.GetCurSel());
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
		//第二图像路径
		String imgFile = OpenImageFile();
		J = imread(imgFile, CV_LOAD_IMAGE_COLOR);
		double result = reduceImage.UseCompareHist(image_r, J, method_one_selecter.GetCurSel());
		CString msg;
		msg.Format(_T("对比结果：%lf"), result);
		MessageBox(msg);
	}
		break;
	case 20:
	{
		//第二图像路径
		String imgFile = OpenImageFile();
		Mat _J = imread(imgFile, CV_LOAD_IMAGE_COLOR);
		CString str0;
		m_num_edit.GetWindowTextW(str0);
		int bins = _ttoi(str0);
		reduceImage.UseCalcBackProject(image_r, _J, J, 180, bins);
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
	String imgFile = OpenImageFile();

	//读入图像
	//image_r = imread(imgFile, CV_LOAD_IMAGE_GRAYSCALE);
	image_r = imread(imgFile, CV_LOAD_IMAGE_COLOR);
	
	if (!image_r.data)
	{
		MessageBox(_T("打开图片失败"));
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
	m_alpha_edit.ShowWindow(SW_SHOW);
	m_static_three.ShowWindow(SW_SHOW);
}


// 隐藏图像求和法选项
void CMyOpenCVApplicationDlg::HideMethodThree()
{
	m_alpha_edit.ShowWindow(SW_HIDE);
	m_static_three.ShowWindow(SW_HIDE);
}


// 打开图片
String CMyOpenCVApplicationDlg::OpenImageFile()
{
	CFileDialog findFileDlg(
		TRUE,  // TRUE是创建打开文件对话框，FALSE则创建的是保存文件对话框
		_T(".jpg"),  // 默认的打开文件的类型
		NULL,  // 默认打开的文件名
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,  // 打开只读文件
		_T("文本文件(*.jpg)|*.jpg|表格文件(*.PNG)|*.PNG|所有文件 (*.*)|*.*||")  // 所有可以打开的文件类型
	);
	//图像名称
	String imgFile;
	if (IDOK == findFileDlg.DoModal())
	{
		imgFile = (LPCSTR)(CStringA)(findFileDlg.GetPathName());
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
	else if(image_r.channels() == 1)
	{
		MessageBox(_T("该图像是灰度图像！"));
		return;
	}
	cvtColor(image_r,image_r, CV_RGB2GRAY);
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
	method_one_selecter.ShowWindow(SW_SHOW);
	method_one_selecter.InsertString(0, _T("标准霍夫线变换"));
	method_one_selecter.InsertString(1, _T("统计概率霍夫线变换"));
	method_one_selecter.SetCurSel(0);
}


// 隐藏霍夫线变换选项
void CMyOpenCVApplicationDlg::HideMethodThirteen()
{
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
	if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标
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
		strcpy_s((char *)GlobalLock(hClip),str.GetLength()+1, T2A(str));
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
