
// MyOpenCVApplicationDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include <string>
#include "MyReduceImage.h"
#include <opencv2/opencv.hpp> 
#include <map>
#include "pylon/PylonIncludes.h"
#include <condition_variable>

// CMyOpenCVApplicationDlg 对话框
class CMyOpenCVApplicationDlg : public CDialogEx
{
// 构造
public:
	CMyOpenCVApplicationDlg(CWnd* pParent = NULL);	// 标准构造函数

	virtual ~CMyOpenCVApplicationDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYOPENCVAPPLICATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
	// 展开颜色缩减方法选项
	void ShowMethodOne();
	// 隐藏颜色缩减法选项
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
	// 展示图像求和法选项
	void ShowMethodThree();
	// 隐藏图像求和法选项
	void HideMethodThree();
	std::string OpenDataTrainFile();
	// 打开knn训练集
	std::string OpenknnTrainFile();
public:
	CEdit m_alpha_edit;
	CStatic m_static_three;
private:
	// 打开图片
	std::string OpenImageFile();
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
	// 展示改变亮度和对比度选项
	void ShowMethodFour();
	// 隐藏改变亮度和对比度选项
	void HideMethodFour();
	// 隐藏平滑处理选项
	void HideMethodSix();
	// 展示平滑处理选项
	void ShowMethodSix();
	// 展示腐蚀选项
	void ShowMethodSevenOrEight();
	// 隐藏腐蚀选项
	void HideMethodSevenOrEight();
	// 展示图像金字塔选项
	void ShowMethodNine();
	// 隐藏图像金字塔选项
	void HideMethodNine();
	void ShowMethodTen();
	// 隐藏阈值化选项
	void HideMethodTen();
	void ShowMethodEleven();
	void HideMethodEleven();
	// 显示边缘检测选项
	void ShowMethodTwelve();
	// 隐藏边缘检测选项
	void HideMethodTwelve();
	// 显示霍夫线变换选项
	void ShowMethodThirteen();
	// 隐藏霍夫线变换选项
	void HideMethodThirteen();
	// 显示霍夫圆变换选项
	void ShowMethodFourteen();
	// 隐藏霍夫圆变换选项
	void HideMethodFourteen();
	// 鼠标响应处理函数
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
	afx_msg void OnBnClickedButtonConfig();
private:
	// 显示重映射选项
	void ShowMethodFifteen();
	// 隐藏重映射选项
	void HideMethodFifteen();
	// 显示旋转图片选项
	void ShowMethodSeventeen();
	// 隐藏旋转图片选项
	void HideMethodSeventeen();
	// 显示直方图计算方法
	void ShowMethodNineteen();
	// 隐藏直方图计算方法
	void HideMethodNineteen();
	// 展示直方图对比方法
	void ShowMethodTwenty();
	// 隐藏直方图对比方法
	void HideMethodTwenty();
	// 展示反射投影方法
	void ShowMethodTwentyOne();
	// 隐藏反射投影方法
	void HideMethodTwentyOne();
	// 展示模板匹配方法
	void ShowMethodTwentyTwo();
	// 隐藏模板匹配方法
	void HideMethodTwentyTwo();
	// 打开图片文件夹
	CString PickImageFile();
	// 递归扫码文件夹下文件
	void TraverseDir(CString& strDir, std::vector<CString>& vecFiles);
	CButton m_multiple_blend;
	// 展示寻找轮廓方法
	void ShowMethodTwentyThree();
	// 隐藏寻找轮廓方法
	void HideMethodTwentyThree();
	// 展示寻找凸包方法
	void ShowMethodTwentyFour();
    // 隐藏寻找凸包方法
	void HideMethodTwentyFour();
	CButton m_check_is_draw_contours;
	// 展示多边形拟合方法
	void ShowMethodTwentyFive();
	// 隐藏多边形拟合方法
	void HideMethodTwentyFive();
	// 展示包覆图形方法
	void ShowMethodTwentySix();
	// 隐藏包覆图形方法
	void HideMethodTwentySix();
	// 展示寻找二维码方法
	void ShowMethodTwentyEight();
	// 隐藏寻找二维码方法
	void HideMethodTwentyEight();
	// 展示组合方法
	void ShowMethodTwentyNine();
	// 隐藏组合方法
	void HideMethodTwentyNine();
	// 点到线的最短距离
	static Point PointToLineDist(Point p, Point p1, Point p2);
	CButton m_config_bt;
	configForLine RLLineConfig;
	configForLine UDLineConfig;
	// 参数初始化
	void initLineConfig();
	configForScan ScanConfig;
	static Point FindMidPoint(Point p1, Point p2, int x_offset, int y_offset);
	static double GetLineLength(Point p1, Point p2, double x_proportion, double y_proportion);
	// 二维码判定
	void CodeJudgement(Mat& J);
	configForCode codeConfig;
	// 展示角点检测方法
	void ShowMethodThirty();
	// 隐藏角点检测方法
	void HideMethodThirty();
	// 暂时扫码选项
	void ShowMethodTwentySeven();
	// 隐藏扫码选项
	void HideMethodTwentySeven();
	CComboBox m_scan_code_type;
	configForProject projectConfig;
	// 处理二维码到边缘距离
	void handleCodeLine(const Mat& I, Mat& J, Point cp1, Point cp2, configForLine lineConfig, double& Len, double& CLen);
	// 两线角度
	void angleLine2Line(Point2f p1, Point2f p2, Point2f p3, Point2f p4);
	// 展示形态学变换方法
	void ShowMethodThirtyOne();
	// 隐藏形态学变换方法
	void HideMethodThirtyOne();
	std::map<std::string, int> camerasIndex;
	// 相机初始化
	void initCamera();
	// basler相机
	void ShowMethodThirtyTwo();
	// basler相机
	void HideMethodThirtyTwo();
	std::condition_variable openCamera;
	std::condition_variable grabImage;
	std::mutex mtxCamera;
	std::mutex mtxImage;
	bool cameraRunning = false;
	bool cameraIsOpen = false;
	// 相机线程
	void ThreadCamera();
public:
	afx_msg void OnBnClickedButtonOpenCamera();
	// 打开关闭相机
	CButton m_camera_bt;
	afx_msg void OnDestroy();
private:
	// 计算两线角度
	double GetTheta(Point p1, Point p2, Point p3, Point p4);
	void ShowMethodThirtyThree();
	void HideMethodThirtyThree();
	// 展示SVM支持向量机算法
	void ShowMethodThirtyFour();
	// 隐藏SVM支持向量机算法
	void HideMethodThirtyFour();
public:
	afx_msg void OnBnClickedButtonSave();
private:
	cv::Mat knnData;
	cv::Mat knnLabels;
	// 特征框架
	void ShowMethodThirtySix();
	// 特征框架
	void HideMethodThirtySix();
};
