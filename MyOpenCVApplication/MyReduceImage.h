#pragma once
#include "zxing/result.h"
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
	Rect rect;
	bool same;
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

struct configForProject {
	Rect rect;
	int codeType;
	double pixel_scale_x;
	double pixel_scale_y;
	bool isDebug;
	int corner;
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
	void FindAllContours(const Mat& I, std::vector<std::vector<Point>>& contours, std::vector<Vec4i>& hierarchy, int thresh, bool cannyOrThresh, int method);
	// Ѱ��͹��
	void FindConvexHull(const std::vector<std::vector<Point>>& contours, std::vector<std::vector<Point>>& hull);
	// ��С����������
	void UseBoundingRect(const Mat& I, Mat& J, int thresh);
	// ��С����б����
	void UseMinAreaRect(const Mat& I, Mat& J, int thresh);
	// ��С����Բ��
	void UseMinEnclosingCircle(const Mat& I, Mat& J, int thresh);
	Point toCvPoint(zxing::Ref<zxing::ResultPoint> resultPoint);
	// Ѱ�Ҷ�ά��ǰԤ����
	void PretreatmentForFindCode(const Mat& I, Mat& J, configForCode config);
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
	void UseErosion(const Mat& I, Mat& J, int kernel_type, int kernel_size);
	// ���ʹ���
	void UseDilation(const Mat& I, Mat& J, int kernel_type, int kernel_size);
	// ͼ�������
	void UsePyr(const Mat& I, Mat& J, int up_or_down, int cycle);
	// ���ӱ߽�
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
	void UseMatchTemplate(const Mat& I, const Mat& templ, Mat& J, int method, double& value, Point& p);
	// �滭����
	void FindAndDrawContours(const Mat& I, Mat& J, int thresh, int method);
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
	bool ScanBarCodeForZxing(const Mat& I, int codeType, std::string& data, std::vector<Point>& codePoint);
	// �滭��ά���λ
	void DrawCodePoint(Mat& J, std::vector<Point> codePoint);
	// ��̬ѧ�任����
	void UseMorphologyEx(const Mat& I, Mat& J, int method, int kernel_type, int kernel_size);
	// basler���
	void PylonFiveDemo(Mat& J);
	// knn�㷨
	void UseknnTrain(const Mat& I);
	// knn�ٽ��㷨
	void UseknnFindNearest(const Mat& I, std::string filePath, float& r, std::vector<float>& results, std::vector<float>& neighborResponses, std::vector<float>& dist);
	void UseSVMTrain(const Mat& I);
	void UseSVMPredict(const Mat& I, std::string filePath, float& r);
private:
	void PrepareToTrain(const Mat& I, Mat& TrainData, Mat& Labels, int size);
	bool readNumClassData(const std::string& filename, int var_count, Mat& _data, Mat& _responses);
	Ptr<ml::TrainData> prepareTrainData(const Mat& data, const Mat& responses, int ntrain_samples);
public:
	void UseSVMTrain(const std::string data_filename);
	// ������˹�任
	void UseLaplacian(Mat& I, Mat& J, int aperture);
	void findAllWord(const Mat& I, Mat& J, int offset, Mat& data, Mat& labels, std::vector<Rect>& rects);
	// ���Ӽ��
	void UseBlocksChecker(const Mat& I, Mat& J, int offset);
	// ��������
	void UseFeatureDetector(const Mat& I, Mat& K, Mat& J, int method);
	// ͸�ӱ任
	void UsePerspectiveTransform(const Mat& I, Mat& J, double& t);
	void UseBlocksChecker2(const Mat& I, Mat& J, Mat& templ);
};

