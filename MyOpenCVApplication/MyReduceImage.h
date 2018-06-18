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
	//look up table ����
	void UseLUT(const Mat& I, Mat& J);
	// ��ֵ
	void UseThreshold(const Mat& I, Mat& J, int threshold_value, int threshold_type);
	//������������
	void UseFilter2D(const Mat& I, Mat& J);
	//ͼ�����
	void UseAddWeighted(const Mat& I, Mat& K, double alpha, Mat& J);
	//�ı�ͼ��ԱȶȺ�����
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
	void UseHoughLines(const Mat& I, Mat& J, int method);
	// ����Բ�任
	void UseHoughCircles(const Mat& I, Mat& J);
	void UseRectangle(Mat& I, Point s, Point e);
};

