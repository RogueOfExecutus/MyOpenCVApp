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
	// ��ӳ�����
	void update_map(int type, float rows, float cols);
	Mat map_x;
	Mat map_y;
	// ֱ��ͼ����
	void UseCalcHist(const Mat& I, Mat& J, int hbins, int sbins);
	// Ѱ������
	void FindAllContours(const Mat& I, std::vector<std::vector<Point>>& contours, std::vector<Vec4i>& hierarchy, int thresh, bool cannyOrThresh);
	// Ѱ��͹��
	void FindConvexHull(const std::vector<std::vector<Point>>& contours, std::vector<std::vector<Point>>& hull);
	// ��С����������
	void UseBoundingRect(const Mat& I, Mat& J, int thresh);
	// ��С����б����
	void UseMinAreaRect(const Mat& I, Mat& J, int thresh);
	// ��С����Բ��
	void UseMinEnclosingCircle(const Mat& I, Mat& J, int thresh);
public:
	MyReduceImage();
	MyReduceImage(int inputDivideWith);
	virtual ~MyReduceImage();
	void SetDivideWith(int inputDivideWith);
	//look up table ����
	void UseLUT(const Mat& I, Mat& J);
	// ��ֵ
	void UseThreshold(const Mat& I, Mat& J, int threshold_value, int threshold_type);
	// ������������
	void UseFilter2D(const Mat& I, Mat& J);
	// ͼ�����
	void UseAddWeighted(const Mat& I, Mat& K, double alpha, Mat& J);
	// �ı�ͼ��ԱȶȺ�����
	void ChangeAlphaBeta(const Mat& I, Mat& J, double alpha, int beta);
	// ��ɢ����Ҷ�任
	void UseDTF(const Mat& I, Mat& magI);
	// ƽ������
	void UseBlur(const Mat& I, Mat& J, int filter, int size);
	// ��ʴ����
	void UseErosion(const Mat& I, Mat& J, int erosion_type, int erosion_size);
	// ���ʹ���
	void UseDilation(const Mat& I, Mat& J, int dilation_type, int dilation_size);
	// ͼ�������
	void UsePyr(const Mat& I, Mat& J, int up_or_down, int cycle);
	// ��ӱ߽�
	void UseCopyMakeBorder(const Mat& I, Mat& J, Scalar value, int borderType);
	// ��Ե���
	void UseEdgeDetection(const Mat& I, Mat& J, int method);
	// �����߱任
	void UseHoughLines(const Mat& I, Mat& J, double rho, double theta, int threshold, int method, double minLinLength = 0.0, double maxLineGap = 0.0);
	// ����Բ�任
	void UseHoughCircles(const Mat& I, Mat& J);
	// ������
	void UseRectangle(Mat& I, Point s, Point e);
	// ��ӳ��
	void UseRemap(const Mat& I, Mat& J, int type);
	// ����任
	void UseWarpAffine(const Mat& I, Mat& J);
	// ��תͼƬ������任����
	void RotateImage(const Mat& I, Mat& J, double angle);
	// ֱ��ͼ���⻯
	void UseEqualizeHist(const Mat& I, Mat& J);
	// ֱ��ͼ�滭
	void UseCalcHistAndDraw(const Mat& I, Mat& J, int bins);
	// ֱ��ͼ�Ա�
	double UseCompareHist(const Mat& I, const Mat& J, int method);
	// ����ͶӰ
	void UseCalcBackProject(const Mat& I, const Mat& J, Mat& K, int hbins, int sbins);
	// ģ��ƥ�䷽��
	void UseMatchTemplate(const Mat& I, const Mat& templ, Mat& J, int method);
	// �滭����
	void FindAndDrawContours(const Mat& I, Mat& J, int thresh);
	// �滭͹��
	void FindAndDrawConvexHull(const Mat& I, Mat& J, int thresh, bool is_draw_contours);
	// ��������
	void UseApproxPolyDP(const Mat& I, Mat& J, int thresh, bool closed);
	// ��������/Բ��
	void DrawRectOrCircle(const Mat& I, Mat& J, int thresh, int method);
	// ��С������Բ
	void UseFitEllipse(const Mat& I, Mat& J, int thresh);
	// ������
	void FindMoments(const Mat& I, Mat& J, int thresh);
	// zbarɨ��
	void ScanBarCode(const Mat& I, std::string& type, std::string& data);
	// Ѱ�Ҷ�ά������
	bool FindCodeCoutours(const Mat& I, Mat& J, configForCode config, RotatedRect& rotatedRect);
	// Ѱ��..��
	Point Center_cal(std::vector<std::vector<Point> > contours, int i);
private:
	// Ѱ�Ҷ�ά��ǰԤ����
	void PretreatmentForFindCode(const Mat& I, Mat& J, configForCode config);
public:
	// Ѱ��ֱ��ǰԤ����
	bool PretreatmentForFindLine(const Mat& I, configForLine config, Vec4i& l);
	// ɨ��ǰԤ����
	void PretreatmentForScanCode(const Mat& I, Mat& J, configForScan config);
	// Harris�ǵ���
	void UseCornerHarris(const Mat& I, Mat& dst_norm, Mat& dst_norm_scaled, int blockSize, int apertureSize, double k);
	// �滭�ǵ�
	void DrawCornerHarris(const Mat& I, Mat& J, int blockSize, int apertureSize, double k, int thresh);
	// Shi-Tomasi�ǵ���
	void UseGoodFeaturesToTrack(const Mat& I, std::vector<Point2f>& corners, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarrisDetector, double k);
	// �滭Shi-Tomasi�ǵ���
	void DrawCorners(const Mat& I, Mat& J, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarrisDetector, double k);
	// zxing������ά��
	void ScanBarCodeForZxing(const Mat& I, int codeType, std::string& data);
};

