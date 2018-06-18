#pragma once
#include <opencv2/opencv.hpp> 

using namespace cv;

class MyReduceImage
{
public:
	MyReduceImage();
	MyReduceImage(int inputDivideWith);
	virtual ~MyReduceImage();
	void SetDivideWith(int inputDivideWith);
	//look up table 方法
	void UseLUT(const Mat& I, Mat& J);
	// 阈值
	void UseThreshold(const Mat& I, Mat& J, int threshold_value, int threshold_type);
	//矩阵的掩码操作
	void UseFilter2D(const Mat& I, Mat& J);
	//图像求和
	void UseAddWeighted(const Mat& I, Mat& K, double alpha, Mat& J);
	//改变图像对比度和亮度
	void ChangeAlphaBeta(const Mat& I, Mat& J, double alpha, int beta);
	// 离散傅里叶变换
	void UseDTF(const Mat& I, Mat& magI);
	// 平滑处理
	void UseBlur(const Mat& I, Mat& J, int filter, int size);
	// 腐蚀处理
	void UseErosion(const Mat& I, Mat& J, int erosion_type, int erosion_size);
	// 膨胀处理
	void UseDilation(const Mat& I, Mat& J, int dilation_type, int dilation_size);
	// 图像金字塔
	void UsePyr(const Mat& I, Mat& J, int up_or_down, int cycle);
	// 添加边界
	void UseCopyMakeBorder(const Mat& I, Mat& J, Scalar value, int borderType);
	// 边缘检测
	void UseEdgeDetection(const Mat& I, Mat& J, int method);
	// 霍夫线变换
	void UseHoughLines(const Mat& I, Mat& J, int method);
	// 霍夫圆变换
	void UseHoughCircles(const Mat& I, Mat& J);
	void UseRectangle(Mat& I, Point s, Point e);
};

