#pragma once
#include <opencv2/opencv.hpp> 

using namespace cv;

struct configForLine {
	int threshold;
	double threshold1;
	double threshold2;
	int canny_size;
	double minLinLength;
	double maxLineGap;
	bool line_direction;
};

struct configForScan {
	int threshold;
	int ErosionTimes;
	int ErosionSize;
	int ErosionMethod;
};

struct configForCode {
	int threshold;
	int ErosionTimes;
	int ErosionSize;
	int ErosionMethod;
	int BlurSize;
};

struct configForRect {
	int topLeft;
	int topRight;
	int bottomLeft;
	int bottomRight;
};

class MyReduceImage
{
private:
	// 重映射矩阵
	void update_map(int type, float rows, float cols);
	Mat map_x;
	Mat map_y;
	// 直方图计算
	void UseCalcHist(const Mat& I, Mat& J, int hbins, int sbins);
	// 寻找轮廓
	void FindAllContours(const Mat& I, std::vector<std::vector<Point>>& contours, std::vector<Vec4i>& hierarchy, int thresh, bool cannyOrThresh);
	// 寻找凸包
	void FindConvexHull(const std::vector<std::vector<Point>>& contours, std::vector<std::vector<Point>>& hull);
	// 最小包覆正矩形
	void UseBoundingRect(const Mat& I, Mat& J, int thresh);
	// 最小包覆斜矩形
	void UseMinAreaRect(const Mat& I, Mat& J, int thresh);
	// 最小包覆圆形
	void UseMinEnclosingCircle(const Mat& I, Mat& J, int thresh);
public:
	MyReduceImage();
	MyReduceImage(int inputDivideWith);
	virtual ~MyReduceImage();
	void SetDivideWith(int inputDivideWith);
	//look up table 方法
	void UseLUT(const Mat& I, Mat& J);
	// 阈值
	void UseThreshold(const Mat& I, Mat& J, int threshold_value, int threshold_type);
	// 矩阵的掩码操作
	void UseFilter2D(const Mat& I, Mat& J);
	// 图像求和
	void UseAddWeighted(const Mat& I, Mat& K, double alpha, Mat& J);
	// 改变图像对比度和亮度
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
	void UseHoughLines(const Mat& I, Mat& J, double rho, double theta, int threshold, int method, double minLinLength = 0.0, double maxLineGap = 0.0);
	// 霍夫圆变换
	void UseHoughCircles(const Mat& I, Mat& J);
	// 画矩形
	void UseRectangle(Mat& I, Point s, Point e);
	// 重映射
	void UseRemap(const Mat& I, Mat& J, int type);
	// 仿射变换
	void UseWarpAffine(const Mat& I, Mat& J);
	// 旋转图片（放射变换法）
	void RotateImage(const Mat& I, Mat& J, double angle);
	// 直方图均衡化
	void UseEqualizeHist(const Mat& I, Mat& J);
	// 直方图绘画
	void UseCalcHistAndDraw(const Mat& I, Mat& J, int bins);
	// 直方图对比
	double UseCompareHist(const Mat& I, const Mat& J, int method);
	// 反射投影
	void UseCalcBackProject(const Mat& I, const Mat& J, Mat& K, int hbins, int sbins);
	// 模板匹配方法
	void UseMatchTemplate(const Mat& I, const Mat& templ, Mat& J, int method);
	// 绘画轮廓
	void FindAndDrawContours(const Mat& I, Mat& J, int thresh);
	// 绘画凸包
	void FindAndDrawConvexHull(const Mat& I, Mat& J, int thresh, bool is_draw_contours);
	// 多边形拟合
	void UseApproxPolyDP(const Mat& I, Mat& J, int thresh, bool closed);
	// 包覆矩形/圆形
	void DrawRectOrCircle(const Mat& I, Mat& J, int thresh, int method);
	// 最小包覆椭圆
	void UseFitEllipse(const Mat& I, Mat& J, int thresh);
	// 轮廓矩
	void FindMoments(const Mat& I, Mat& J, int thresh);
	// zbar扫码
	void ScanBarCode(const Mat& I, std::string& type, std::string& data);
	// 寻找二维码轮廓
	bool FindCodeCoutours(const Mat& I, Mat& J, configForCode config, RotatedRect& rotatedRect);
	// 寻找..点
	Point Center_cal(std::vector<std::vector<Point> > contours, int i);
private:
	// 寻找二维码前预处理
	void PretreatmentForFindCode(const Mat& I, Mat& J, configForCode config);
public:
	// 寻找直线前预处理
	bool PretreatmentForFindLine(const Mat& I, configForLine config, Vec4i& l);
	// 扫码前预处理
	void PretreatmentForScanCode(const Mat& I, Mat& J, configForScan config);
	// Harris角点检测
	void UseCornerHarris(const Mat& I, Mat& dst_norm, Mat& dst_norm_scaled, int blockSize, int apertureSize, double k);
	// 绘画角点
	void DrawCornerHarris(const Mat& I, Mat& J, int blockSize, int apertureSize, double k, int thresh);
	// Shi-Tomasi角点检测
	void UseGoodFeaturesToTrack(const Mat& I, std::vector<Point2f>& corners, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarrisDetector, double k);
	// 绘画Shi-Tomasi角点检测
	void DrawCorners(const Mat& I, Mat& J, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarrisDetector, double k);
	// zxing解析二维码
	void ScanBarCodeForZxing(const Mat& I, int codeType, std::string& data);
};

