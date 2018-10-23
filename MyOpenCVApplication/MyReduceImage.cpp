#include "stdafx.h"
#include "MyReduceImage.h"
#include <iostream>
#include <sstream>
#include "zbar.h"
#include "MyOpenCVApplicationDlg.h"
#include "zxing/LuminanceSource.h"
#include "zxing/Reader.h" 
#include "zxing/common/GlobalHistogramBinarizer.h"  
#include "zxing/DecodeHints.h"  
#include "zxing/datamatrix/DataMatrixReader.h"  
#include "zxing/qrcode/QRCodeReader.h"
#include "MatSource.h"
#include "zxing/result.h"
#include "zxing/BinaryBitmap.h"
#include "zxing/Binarizer.h"
#include "zxing/Exception.h"
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "pylon/PylonIncludes.h"

using namespace std;
using namespace cv;
using namespace cv::ml;
using namespace cv::xfeatures2d;
using namespace zbar;  //添加zbar名称空间 
using namespace zxing; 
using namespace Pylon;

void testAdativeThreshold(int, void*);
void testCanny(int, void*);
int kernel_size = 1;
int offset_C = 0;
Mat i, j;

uchar table[256];

MyReduceImage::MyReduceImage()
{
	for (int i = 0; i < 256; ++i)
		table[i] = (uchar)i;
}

MyReduceImage::MyReduceImage(int inputDivideWith)
{
	for (int i = 0; i < 256; ++i)
		table[i] = (uchar)(inputDivideWith * (i / inputDivideWith));
}

MyReduceImage::~MyReduceImage()
{
	//delete []table;
}

void MyReduceImage::SetDivideWith(int inputDivideWith)
{
	for (int i = 0; i < 256; ++i)
		table[i] = (uchar)(inputDivideWith * (i / inputDivideWith));
}

void MyReduceImage::UseLUT(const Mat& I, Mat& J)
{
	// TODO: 在此处插入 return 语句
	//CV_Assert(I.depth() == CV_8U);  // 仅接受uchar图像
	Mat lookUpTable(1, 256, CV_8UC1, table);
	LUT(I, lookUpTable, J);
	//return J;
}


// 阈值
void MyReduceImage::UseThreshold(const Mat& I, Mat& J, int threshold_value, int threshold_type)
{
	Mat src_gray;
	if (I.channels() != 1)
		cvtColor(I, src_gray, COLOR_BGR2GRAY);
	else
		src_gray = I.clone();
	if (threshold_type < 5)
		threshold(src_gray, J, threshold_value, 255, threshold_type);
	else
	{
		i = src_gray;
		namedWindow("adaptiveThreshold Demo", WINDOW_AUTOSIZE);
		offset_C = 0;
		kernel_size = 1;
		createTrackbar("offset_C: ", "adaptiveThreshold Demo",
			&offset_C, 100,testAdativeThreshold);

		createTrackbar("Kernel size:\n 2n +1", "adaptiveThreshold Demo",
			&kernel_size, 100,testAdativeThreshold);
		testAdativeThreshold(0, 0);
		adaptiveThreshold(src_gray, J, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, threshold_value * 2 + 1, 0);
	}
}

void testAdativeThreshold(int, void*)
{
	adaptiveThreshold(i, j, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, kernel_size * 2 + 3, offset_C);
	imshow("adaptiveThreshold Demo", j);
}

void testCanny(int, void *)
{
	Canny(i, j, offset_C, kernel_size);
	imshow("Canny Demo", j);
}

// 矩阵的掩码操作
void MyReduceImage::UseFilter2D(const Mat& I, Mat& J)
{
	Mat kernel = (Mat_<char>(3, 3) << 0, -1, 0,
		-1, 5, -1,
		0, -1, 0);
	filter2D(I, J, I.depth(), kernel);
}

// 图像求和
void MyReduceImage::UseAddWeighted(const Mat& I, Mat& K, double alpha, Mat& J)
{
	addWeighted(I, alpha, K, (1.0 - alpha), 0.0, J);
}

// 改变亮度和对比度
void MyReduceImage::ChangeAlphaBeta(const Mat& I, Mat& J, double alpha, int beta)
{
	//执行运算new_image(i,j) = alpha * image(i,j) + beta
	/*for (int i = 0; i < I.rows; i++)
	{
		for (int j = 0; j < I.cols; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				J.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(alpha * (I.at<Vec3b>(i, j)[k]) + beta);
			}
		}
	}*/
	I.convertTo(J, -1, alpha, beta);
}


// 离散傅里叶变换
void MyReduceImage::UseDTF(const Mat& I, Mat& magI)
{
	if (I.empty()) {
		return;
	}
	//! [expand]
	Mat padded;                            //expand input image to optimal size
	int m = getOptimalDFTSize(I.rows);
	int n = getOptimalDFTSize(I.cols); // on the border add zero values
	copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0));
	//! [expand]
	//! [complex_and_real]
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexI;
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
										//! [complex_and_real]

										//! [dft]
	dft(complexI, complexI);            // this way the result may fit in the source matrix
										//! [dft]

										// compute the magnitude and switch to logarithmic scale
										// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
										//! [magnitude]
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
	magI = planes[0];
	//! [magnitude]

	//! [log]
	magI += Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);
	//! [log]

	//! [crop_rearrange]
	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);
	//! [crop_rearrange]

	//! [normalize]
	normalize(magI, magI, 0, 1, NORM_MINMAX);
	Mat J = Mat(magI.size(), CV_8UC1);
	magI.convertTo(J, CV_8UC1, 255, 0);
	magI = J.clone();
}


// 平滑处理
void MyReduceImage::UseBlur(const Mat& I, Mat& J, int filter, int size)
{
	switch (filter)
	{
	case 0:
		blur(I, J, Size(size, size), Point(-1, -1));
		break;
	case 1:
		GaussianBlur(I, J, Size(size, size), 0, 0);
		break;
	case 2:
		medianBlur(I, J, size);
		break;
	case 3:
		bilateralFilter(I, J, size, size * 2, size / 2);
		break;
	case 4:
	{
		Point anchor = Point(-1, -1);
		Mat kernel = Mat::ones(size, size, CV_32F) / (float)(size*size);
		filter2D(I, J, -1, kernel, anchor, 0, BORDER_DEFAULT);
	}
	break;
	default:
		break;
	}
}


// 腐蚀处理
void MyReduceImage::UseErosion(const Mat& I, Mat& J, int kernel_type, int kernel_size)
{
	Mat element = getStructuringElement(kernel_type,
		Size(2 * kernel_size + 1, 2 * kernel_size + 1),
		Point(kernel_size, kernel_size));
	// 腐蚀操作
	erode(I, J, element);
}

// 膨胀处理
void MyReduceImage::UseDilation(const Mat & I, Mat & J, int kernel_type, int kernel_size)
{
	Mat element = getStructuringElement(kernel_type,
		Size(2 * kernel_size + 1, 2 * kernel_size + 1),
		Point(kernel_size, kernel_size));
	///膨胀操作
	dilate(I, J, element);
}

// 图像金字塔
void MyReduceImage::UsePyr(const Mat& I, Mat& J, int up_or_down, int cycle)
{
	Mat temp;
	temp = I.clone();
	for (int i = 0; i < cycle; i++)
	{
		switch (up_or_down)
		{
		case 0:
			pyrUp(temp, J, Size(temp.cols * 2, temp.rows * 2));
			break;
		case 1:
			pyrDown(temp, J, Size(temp.cols / 2, temp.rows / 2));
			break;
		default:
			break;
		}
		temp = J;
	}
}

// 添加边界
void MyReduceImage::UseCopyMakeBorder(const Mat& I, Mat& J, Scalar value, int borderType)
{
	int size_tb = I.rows / 20, size_lr = I.cols / 20;
	/*int type;
	switch (borderType)
	{
	case 0:
		type = BORDER_CONSTANT;
		break;
	case 1:
		type = BORDER_REPLICATE;
		break;
	default:
		break;
	}*/
	copyMakeBorder(I, J, size_tb, size_tb, size_lr, size_lr, borderType, value);
}

// 边缘检测
void MyReduceImage::UseEdgeDetection(const Mat& I, Mat& J, int method)
{
	if (method == 2)
	{
		Mat temp;
		/// 使用Laplace函数
		Laplacian(I, temp, CV_16S, 3, 1, 0, BORDER_DEFAULT);
		convertScaleAbs(temp, J);
	}
	else if (method == 3)
	{
		Mat temp;
		/// 使用 3x3内核降噪
		blur(I, temp, Size(3, 3));
		/// 运行Canny算子
		Canny(temp, J, 80, 200, 3);
		namedWindow("Canny Demo", WINDOW_NORMAL);
		offset_C = 80;
		kernel_size = 200;
		i = temp;
		createTrackbar("low_thresh: ", "Canny Demo",
			&offset_C, 500, testCanny);

		createTrackbar("high_thresh:", "Canny Demo",
			&kernel_size, 800, testCanny);
		testCanny(0, 0);
	}
	else
	{
		/// 创建 grad_x 和 grad_y 矩阵
		Mat grad_x, grad_y;
		Mat abs_grad_x, abs_grad_y;
		if (method == 0) {
			Sobel(I, grad_x, CV_16S, 1, 0, 3, 1, 0, BORDER_DEFAULT);
			Sobel(I, grad_y, CV_16S, 0, 1, 3, 1, 0, BORDER_DEFAULT);
		}
		else if (method == 1)
		{
			Scharr(I, grad_x, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
			Scharr(I, grad_y, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
		}
		convertScaleAbs(grad_x, abs_grad_x);
		convertScaleAbs(grad_y, abs_grad_y);
		/// 合并梯度(近似)
		addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, J);
		//J = abs_grad_y - abs_grad_x;
	}
}

// 霍夫线变换
void MyReduceImage::UseHoughLines(const Mat& I, Mat& J, double rho, double theta, int threshold, int method, double minLinLength, double maxLineGap)
{
	J = I.clone();
	Mat temp;
	if (I.channels() != 1)
		UseEdgeDetection(I, temp, 3);//canny算子
	else
	{
		temp = J;
		cvtColor(J, J, COLOR_GRAY2BGR);
	}
	if (method == 0)
	{
		vector<Vec2f> lines;
		HoughLines(temp, lines, rho, theta, threshold, 0, 0);
		//double x = temp.size().width;
		//double y = temp.size().height;
		for (size_t i = 0; i < lines.size(); i++)
		{
			float r = lines[i][0], theta = lines[i][1];
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			//double x1 = r / b, x2 = (r - a*y) / b;
			//double y2 = r / a, y1 = (r - b*x) / a;
			/*if (x1 > x2)
			{
				if (x1 > x)
				{
					pt1.x = cvRound(x);
					pt2.x = cvRound(x2);
					pt1.y = cvRound((r - b*x) / a);
					pt2.y = cvRound((r - b*x2) / a);
				}
				else
				{
					pt1.x = cvRound(x1);
					pt2.x = cvRound(x);
					pt1.y = cvRound((r - b*x1) / a);
					pt2.y = cvRound((r - b*x) / a);
				}
			}
			else
			{
				if (x2 > x)
				{
					pt1.x = cvRound(x1);
					pt2.x = cvRound(x);
					pt1.y = cvRound((r - b*x1) / a);
					pt2.y = cvRound((r - b*x) / a);
				}
				else
				{
					pt1.x = cvRound(x);
					pt2.x = cvRound(x2);
					pt1.y = cvRound((r - b*x) / a);
					pt2.y = cvRound((r - b*x2) / a);
				}
			}*/
			double x0 = a*r, y0 = b*r;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			line(J, pt1, pt2, Scalar(0, 0, 255), 1, LINE_AA);
		}
	}
	else if (method == 1)
	{
		vector<Vec4i> lines;
		HoughLinesP(temp, lines, rho, theta, threshold, minLinLength, maxLineGap);
		Scalar color[3] = {Scalar(0,0,255),Scalar(0,255,0), Scalar(255,0,0)};
		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];
			line(J, Point(l[0], l[1]), Point(l[2], l[3]), color[i%3], 1, LINE_AA);
		}
	}
}


// 霍夫圆变换
void MyReduceImage::UseHoughCircles(const Mat& I, Mat& J)
{
	J = I.clone();
	Mat temp;
	if (I.channels() != 1)
	{
		cvtColor(I, temp, COLOR_BGR2GRAY);
	}
	else
		temp = J;
	/// 使用 3x3内核降噪
	blur(temp, temp, Size(3, 3));
	vector<Vec3f> circles;

	/// Apply the Hough Transform to find the circles
	HoughCircles(temp, circles, HOUGH_GRADIENT, 1, temp.rows / 8, 200, 100, 0, 0);

	/// Draw the circles detected
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// circle center
		circle(J, center, 3, Scalar(0, 0, 255), -1, 8, 0);
		// circle outline
		circle(J, center, radius, Scalar(0, 255, 0), 3, 8, 0);
	}
}

// 画矩形
void MyReduceImage::UseRectangle(Mat& I, Point s, Point e)
{
	rectangle(I, s, e, Scalar(0, 0, 255), 2, 8, 0);
}


// 重映射矩阵
void MyReduceImage::update_map(int type, float rows, float cols)
{
	for (int j = 0; j < rows; j++)
	{
		for (int i = 0; i < cols; i++)
		{
			switch (type)
			{
			case 0:
				if (i > cols*0.25 && i < cols*0.75 && j > rows*0.25 && j < rows*0.75)
				{
					map_x.at<float>(j, i) = 2 * (i - cols*0.25) + 0.5;
					map_y.at<float>(j, i) = 2 * (j - rows*0.25) + 0.5;
				}
				else
				{
					map_x.at<float>(j, i) = 0;
					map_y.at<float>(j, i) = 0;
				}
				break;
			case 1:
				map_x.at<float>(j, i) = i;
				map_y.at<float>(j, i) = rows - j;
				break;
			case 2:
				map_x.at<float>(j, i) = cols - i;
				map_y.at<float>(j, i) = j;
				break;
			case 3:
				map_x.at<float>(j, i) = cols - i;
				map_y.at<float>(j, i) = rows - j;
				break;
			} // end of switch
		}
	}
}

// 重映射
void MyReduceImage::UseRemap(const Mat& I, Mat& J, int type)
{
	map_x.create(I.size(), CV_32FC1);
	map_y.create(I.size(), CV_32FC1);
	update_map(type, I.rows, I.cols);
	remap(I, J, map_x, map_y, INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
}

// 仿射变换
void MyReduceImage::UseWarpAffine(const Mat& I, Mat& J)
{
	Point2f srcTri[3];
	Point2f dstTri[3];

	srcTri[0] = Point2f(0, 0);
	srcTri[1] = Point2f(I.cols - 1, 0);
	srcTri[2] = Point2f(0, I.rows - 1);

	dstTri[0] = Point2f(I.cols*0.0, I.rows*0.33);
	dstTri[1] = Point2f(I.cols*0.85, I.rows*0.25);
	dstTri[2] = Point2f(I.cols*0.15, I.rows*0.7);
	// (573,1279)
	// (1537, 1321)
	// (561, 1767)

	Mat warp_mat(2, 3, CV_32FC1);
	warp_mat = getAffineTransform(srcTri, dstTri);

	warpAffine(I, J, warp_mat, J.size());
}


// 旋转图片（放射变换法）
void MyReduceImage::RotateImage(const Mat& I, Mat& J, double angle)
{
	if (angle == 180.0)
	{
		Point center = Point(I.cols / 2, I.rows / 2);
		Mat rot_mat(2, 3, CV_32FC1);
		rot_mat = getRotationMatrix2D(center, 180, 1);
		warpAffine(I, J, rot_mat, I.size());
	}
	else if (angle == 90.0)
	{
		int c = I.channels();
		J = Mat(I.cols, I.rows, I.type());
		for (int i = 0; i < J.rows; i++)
		{
			for (int j = 0; j < J.cols; j++)
			{
				if (c == 1)
				{
					J.at<uchar>(i, J.cols - j - 1) = I.at<uchar>(j, i);
				}
				if (c == 3)
				{
					J.at<Vec3b>(i, J.cols - j - 1)[0] = I.at<Vec3b>(j, i)[0];
					J.at<Vec3b>(i, J.cols - j - 1)[1] = I.at<Vec3b>(j, i)[1];
					J.at<Vec3b>(i, J.cols - j - 1)[2] = I.at<Vec3b>(j, i)[2];
				}
			}
		}
	}
	else if (angle == 270.0)
	{
		int c = I.channels();
		J = Mat(I.cols, I.rows, I.type());
		for (int i = 0; i < J.rows; i++)
		{
			for (int j = 0; j < J.cols; j++)
			{
				if (c == 1)
				{
					J.at<uchar>(J.rows - i - 1, j) = I.at<uchar>(j, i);
				}
				if (c == 3)
				{
					J.at<Vec3b>(J.rows - i - 1, j)[0] = I.at<Vec3b>(j, i)[0];
					J.at<Vec3b>(J.rows - i - 1, j)[1] = I.at<Vec3b>(j, i)[1];
					J.at<Vec3b>(J.rows - i - 1, j)[2] = I.at<Vec3b>(j, i)[2];
				}
			}
		}
	}
	else
	{
		Point center = Point(I.cols / 2, I.rows / 2);
		Mat rot_mat(2, 3, CV_32FC1);
		rot_mat = getRotationMatrix2D(center, angle, 1);
		warpAffine(I, J, rot_mat, I.size());
	}
}


// 直方图均衡化
void MyReduceImage::UseEqualizeHist(const Mat& I, Mat& J)
{
	Mat temp;
	if (I.channels() != 1) 
	{
		cvtColor(I, temp, COLOR_BGR2GRAY);
	}
	else
	{
		I.copyTo(temp);
	}
	equalizeHist(temp, J);
}

// 直方图绘画
void MyReduceImage::UseCalcHistAndDraw(const Mat& I, Mat& J, int bins)
{
	int len = I.channels();

	UseCalcHist(I, J, 30, bins);

	switch (len)
	{
	case 1:
	{
		// 创建直方图画布
		int hist_w = 512; int hist_h = 512;
		int bin_w = cvRound((double)hist_w / bins);

		Mat histImage(hist_w, hist_h, CV_8UC3, Scalar(0, 0, 0));

		normalize(J, J, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		for (int i = 1; i < bins; i++)
		{
			line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(J.at<float>(i - 1))),
				Point(bin_w*(i), hist_h - cvRound(J.at<float>(i))),
				Scalar(0, 255, 0), 2, 8, 0);
		}
		histImage.copyTo(J);
	}
	break;
	case 3:
	{
		normalize(J, J, 0, 1, NORM_MINMAX, -1, Mat());
		int h_bins = 30, s_bins = bins;

		double maxVal = 0;
		minMaxLoc(J, 0, &maxVal, 0, 0);

		int scale = 10;
		Mat histImg = Mat::zeros(s_bins*scale, h_bins * 10, CV_8UC3);
		for (int h = 0; h < h_bins; h++)
			for (int s = 0; s < s_bins; s++)
			{
				float binVal = J.at<float>(h, s);
				int intensity = cvRound(binVal * 255 / maxVal);
				rectangle(histImg, Point(h*scale, s*scale),
					Point((h + 1)*scale - 1, (s + 1)*scale - 1),
					Scalar::all(intensity),
					FILLED);
			}
		histImg.copyTo(J);
	}
	break;
	default:
		break;
	}
}

// 直方图对比
double MyReduceImage::UseCompareHist(const Mat& I, const Mat& J, int method)
{
	if (I.channels() != J.channels())
		return 0.0;
	Mat hist_i, hist_j;
	UseCalcHist(I, hist_i, 30, 32);
	UseCalcHist(J, hist_j, 30, 32);
	return compareHist(hist_i, hist_j, method);
}


// 直方图计算
void MyReduceImage::UseCalcHist(const Mat& I, Mat& J, int hbins, int sbins)
{
	int len = I.channels();

	bool uniform = true, accumulate = false;

	switch (len)
	{
	case 1:
	{
		// 每个特征空间 子区段 的数目
		int histSize = sbins;
		// 设定取值范围
		float range_b[] = { 0, 255 };
		const float* histRange_gray = range_b;
		calcHist(&I, 1, 0, Mat(), J, 1, &histSize, &histRange_gray, uniform, accumulate);
	}
	break;
	case 3:
	{
		// 通道
		int channels[] = { 0, 1 };
		int h_bins = hbins, s_bins = sbins;
		int histSize[] = { h_bins, s_bins };
		// 设定取值范围
		float h_range[] = { 0, 180 };
		float s_range[] = { 0, 256 };
		const float* histRange_bgr[] = { h_range , s_range };
		Mat hsv;
		MatND hist;
		cvtColor(I, hsv, COLOR_BGR2HSV);
		calcHist(&hsv, 1, channels, Mat(), hist, 2, histSize, histRange_bgr, uniform, accumulate);

		//normalize(hist, hist, 0, 1, NORM_MINMAX, -1, Mat());

		hist.copyTo(J);
	}
	break;
	default:
		break;
	}
}

// 反射投影
void MyReduceImage::UseCalcBackProject(const Mat& I, const Mat& J, Mat& K, int hbins, int sbins)
{
	if (I.channels() != 3 || J.channels() != 3)
		return;
	MatND temp;
	UseCalcHist(I, temp, hbins, sbins);
	normalize(temp, temp, 0, 255, NORM_MINMAX, -1, Mat());
	// 通道
	int channels[] = { 0, 1 };
	int h_bins = hbins, s_bins = sbins;
	int histSize[] = { h_bins, s_bins };
	// 设定取值范围
	float h_range[] = { 0, 180 };
	float s_range[] = { 0, 256 };
	const float* histRange_bgr[] = { h_range , s_range };
	Mat hsv;
	MatND backproj;
	cvtColor(J, hsv, COLOR_BGR2HSV);
	calcBackProject(&hsv, 1, channels, temp, backproj, histRange_bgr, 1, true);
	backproj.copyTo(K);
}


// 模板匹配方法
void MyReduceImage::UseMatchTemplate(const Mat& I, const Mat& templ, Mat& J, int method, double& value, Point& p)
{
	I.copyTo(J);
	if (I.channels() != templ.channels())
		return;
	Mat result;
	int result_cols = I.cols - templ.cols + 1;
	int result_rows = I.rows - templ.rows + 1;

	result.create(result_cols, result_rows, CV_32FC1);

	matchTemplate(I, templ, result, method);

	//normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	Point minLoc;
	Point maxLoc;
	double minValue;
	double maxValue;

	minMaxLoc(result, &minValue, &maxValue, &minLoc, &maxLoc);
	if (method == TM_SQDIFF || method == TM_SQDIFF_NORMED)
	{
		p = minLoc;
		value = minValue;
	}
	else
	{
		p = maxLoc;
		value = maxValue;
	}

	rectangle(J, p, Point(p.x + templ.cols, p.y + templ.rows), Scalar::all(255), 10, 8, 0);
	rectangle(result, p, Point(p.x + templ.cols, p.y + templ.rows), Scalar::all(0), 2, 8, 0);

	imshow("result", result);
}


// 绘画轮廓
void MyReduceImage::FindAndDrawContours(const Mat& I, Mat& J, int thresh, int method)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh, true, method);
	RNG rng(12345);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//drawContours(J, contours, i, color, 2, 8, hierarchy, 0, Point());
		drawContours(J, contours, i, color, 2, 8, vector<Vec4i>(), 0, Point());
	}

}


// 寻找轮廓
void MyReduceImage::FindAllContours(const Mat& I, vector<vector<Point>>& contours, vector<Vec4i>& hierarchy, int thresh, bool cannyOrThresh, int method)
{
	Mat temp;
	if (I.channels() == 1)
	{
		I.copyTo(temp);
	}
	else
	{
		cvtColor(I, temp, COLOR_BGR2GRAY);
	}
	UseBlur(temp, temp, 1, 3);
	if (cannyOrThresh)
		Canny(temp, temp, thresh, thresh * 2, 3);
	else
		UseThreshold(temp, temp, thresh, THRESH_BINARY);

	/// 寻找轮廓
	switch (method)
	{
	case 0:
		findContours(temp, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
		break;
	case 1:
		findContours(temp, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		break;
	default:
		break;
	}
}

// 寻找凸包
void MyReduceImage::FindConvexHull(const vector<vector<Point>>& contours, vector<vector<Point>>& hull)
{
	for (int i = 0; i < contours.size(); i++)
	{
		convexHull(Mat(contours[i]), hull[i], false);
	}
}

// 绘画凸包
void MyReduceImage::FindAndDrawConvexHull(const Mat& I, Mat& J, int thresh, bool is_draw_contours)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh, false, 0);
	int len = contours.size();
	vector<vector<Point> > hull(len);
	FindConvexHull(contours, hull);
	RNG rng(12345);
	for (int i = 0; i < len; i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(J, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		if (is_draw_contours)
			drawContours(J, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	}

}


// 多边形拟合
void MyReduceImage::UseApproxPolyDP(const Mat& I, Mat& J, int thresh, bool closed)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh, false, 0);

	/// 多边形逼近轮廓 + 获取矩形和圆形边界框
	vector<vector<Point> > contours_poly(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, closed);
	}
	RNG rng(12345);
	for (int i = 0; i < contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(J, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	}
}


// 最小包覆正矩形
void MyReduceImage::UseBoundingRect(const Mat& I, Mat& J, int thresh)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh, false, 0);
	int len = contours.size();
	vector<Rect> boundRect(len);
	for (int i = 0; i < len; i++)
	{
		boundRect[i] = boundingRect(Mat(contours[i]));
		rectangle(J, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 0, 255), 2, 8, 0);
	}
}


// 最小包覆斜矩形
void MyReduceImage::UseMinAreaRect(const Mat& I, Mat& J, int thresh)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh, false, 0);
	int len = contours.size();

	vector<Point2f[4]> vertices(len);//定义矩形的4个顶点

	for (int i = 0; i < len; i++)
	{
		minAreaRect(Mat(contours[i])).points(vertices[i]); //计算矩形的4个顶点
		for (int j = 0; j < 4; j++)
			line(J, vertices[i][j], vertices[i][(j + 1) % 4], Scalar(0, 255, 0));//计算一个，画一个
	}
}



// 最小包覆圆形
void MyReduceImage::UseMinEnclosingCircle(const Mat& I, Mat& J, int thresh)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh, false, 0);
	vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());
	int len = contours.size();
	for (int i = 0; i < len; i++)
	{
		minEnclosingCircle(contours[i], center[i], radius[i]);
		circle(J, center[i], (int)radius[i], Scalar(0, 0, 255), 2, 8, 0);
	}
}

// 包覆矩形/圆形
void MyReduceImage::DrawRectOrCircle(const Mat& I, Mat& J, int thresh, int method)
{
	switch (method)
	{
	case 0:
		UseBoundingRect(I, J, thresh);
		break;
	case 1:
		UseMinAreaRect(I, J, thresh);
		break;
	case 2:
		UseMinEnclosingCircle(I, J, thresh);
		break;
	case 3:
		UseFitEllipse(I, J, thresh);
		break;
	case 4:
		FindMoments(I, J, thresh);
		break;
	default:
		break;
	}
}


// 最小包覆椭圆
void MyReduceImage::UseFitEllipse(const Mat& I, Mat& J, int thresh)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh, false, 0);
	int len = contours.size();

	for (int i = 0; i < len; i++)
	{
		// 超过5个点的轮廓才能计算包围其的最小椭圆
		if (contours[i].size() > 5)
			ellipse(J, fitEllipse(Mat(contours[i])), Scalar(0, 0, 255), 2, 8);
	}
}


// 轮廓矩
void MyReduceImage::FindMoments(const Mat& I, Mat& J, int thresh)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh, true, 0);
	int len = contours.size();
	vector<Moments> mu(len);
	vector<Point2f> mc(len);
	for (int i = 0; i < len; i++)
	{
		mu[i] = moments(contours[i], false);
		mc[i] = Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		drawContours(J, contours, i, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point());
		circle(J, mc[i], 4, Scalar(0, 0, 255), -1, 8, 0);
	}
}


// zbar扫码
void MyReduceImage::ScanBarCode(const Mat& I, string& type, string& data)
{
	Mat temp;
	if (I.channels() == 1)
	{
		I.copyTo(temp);
	}
	else
	{
		cvtColor(I, temp, COLOR_BGR2GRAY);
	}

	ImageScanner scanner;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	int width = temp.cols;
	int height = temp.rows;
	uchar *raw = (uchar *)temp.data;
	Image imageZbar(width, height, "Y800", raw, width * height);//zbar能识别的图像

	scanner.scan(imageZbar); //扫描条码 

	//imageZbar.

	Image::SymbolIterator symbol = imageZbar.symbol_begin();
	if (imageZbar.symbol_begin() == imageZbar.symbol_end())
	{
		//cout << "查询条码失败，请检查图片！" << endl;
		data = "查询条码失败,请检查图片！";

	}
	else
		for (; symbol != imageZbar.symbol_end(); ++symbol)
		{
			//cout << "类型：" << endl << symbol->get_type_name() << endl << endl;
			//cout << "条码：" << endl << symbol->get_data() << endl << endl;
			type = symbol->get_type_name();
			data = symbol->get_data();
		}
	imageZbar.set_data(NULL, 0);
}


// 寻找二维码轮廓
bool MyReduceImage::FindCodeCoutours(const Mat& I, Mat& J, configForCode config, RotatedRect& rotatedRect)
{
	Mat temp;
	PretreatmentForFindCode(I, temp, config);

	Scalar color = Scalar(1, 1, 255);
	vector<vector<Point> > contours, contours2;
	vector<Vec4i> hierarchy;
	Mat drawing = Mat::zeros(I.size(), CV_8UC3);
	Mat drawing2 = Mat::zeros(I.size(), CV_8UC3);
	//UseThreshold(temp, threshold_output, threshold, THRESH_BINARY);
	imshow("threshold_output", temp);
	findContours(temp, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));


	RNG rng(12345);
	int c = 0, ic = 0, area = 0;

	//程序的核心筛选
	int parentIdx = -1;
	for (int i = 0; i < contours.size(); i++)
	{
		if (hierarchy[i][2] != -1 && ic == 0)
		{
			parentIdx = i;
			ic++;
		}
		else if (hierarchy[i][2] != -1)
		{
			ic++;
		}
		else if (hierarchy[i][2] == -1)
		{
			ic = 0;
			parentIdx = -1;
		}


		if (ic >= 2)
		{
			contours2.push_back(contours[parentIdx]);
			drawContours(drawing, contours, parentIdx, Scalar(0, 255, 0), 1, 8);
			ic = 0;
			parentIdx = -1;
			//area = contourArea(contours[i]);//得出一个二维码定位角的面积，以便计算其边长（area_side）（数据覆盖无所谓，三个定位角中任意一个数据都可以）
		}
		imshow("提取前", drawing);
	}
	if (contours2.size() > 0)
	{
		for (int i = 0; i < contours2.size(); i++)
			drawContours(drawing2, contours2, i, Scalar(0, 0, 255), 1, 4, vector<Vec4i>(), 0, Point());


		vector<Point> point(contours2.size());
		for (int i = 0; i < contours2.size(); i++)
		{
			point[i] = Center_cal(contours2, i);
		}

		area = contourArea(contours2[1]);
		int area_side = cvRound(sqrt(double(area)));
		for (int i = 0; i < contours2.size(); i++)
		{
			line(drawing2, point[i%contours2.size()], point[(i + 1) % contours2.size()], color, area_side / 2, 8);
		}


		imshow("提取后", drawing2);


		Mat gray_all, threshold_output_all;
		vector<vector<Point> > contours_all;
		vector<Vec4i> hierarchy_all;
		cvtColor(drawing2, gray_all, COLOR_BGR2GRAY);


		UseThreshold(gray_all, threshold_output_all, 45, THRESH_BINARY);

		findContours(threshold_output_all, contours_all, hierarchy_all
			, RETR_EXTERNAL, CHAIN_APPROX_NONE);//RETR_EXTERNAL表示只寻找最外层轮廓


		//求最小包围矩形
		rotatedRect = minAreaRect(contours_all[0]);

		Point2f fourPoint2f[4];

		/** returns 4 vertices of the rectangle
		@param pts The points array for storing rectangle vertices. The order is bottomLeft, topLeft, topRight, bottomRight.
		*/
		//将rectPoint变量中存储的坐标值放到 fourPoint的数组中  
		rotatedRect.points(fourPoint2f);

		for (int i = 0; i < 4; i++)
		{
			line(J, fourPoint2f[i % 4], fourPoint2f[(i + 1) % 4], Scalar(20, 21, 237), 3);
		}
		return true;
	}
	return false;
}


// 寻找..点
Point MyReduceImage::Center_cal(vector<vector<Point> > contours, int i)
{
	int centerx = 0, centery = 0, n = contours[i].size();
	//在提取的小正方形的边界上每隔周长个像素提取一个点的坐标，求所提取四个点的平均坐标（即为小正方形的大致中心）
	centerx = (contours[i][n / 4].x + contours[i][n * 2 / 4].x + contours[i][3 * n / 4].x + contours[i][n - 1].x) / 4;
	centery = (contours[i][n / 4].y + contours[i][n * 2 / 4].y + contours[i][3 * n / 4].y + contours[i][n - 1].y) / 4;
	Point point1 = Point(centerx, centery);
	return point1;
}


// 寻找二维码前预处理
void MyReduceImage::PretreatmentForFindCode(const Mat& I, Mat& J, configForCode config)
{
	Mat temp;
	if (I.channels() == 1)
	{
		I.copyTo(temp);
	}
	else
	{
		cvtColor(I, temp, COLOR_BGR2GRAY);
	}
	UseThreshold(temp, temp, config.threshold, THRESH_BINARY);
	for (int i = 0; i < config.ErosionTimes; i++)
	{
		UseErosion(temp, temp, config.ErosionMethod, config.ErosionSize);
	}
	for (int i = 0; i < config.ErosionTimes; i++)
	{
		UseDilation(temp, temp, config.ErosionMethod, config.ErosionSize);
	}
	if(config.BlurSize)
		UseBlur(temp, temp, 1, config.BlurSize);
	UseEqualizeHist(temp, J);
}


// 寻找直线前预处理
bool MyReduceImage::PretreatmentForFindLine(const Mat& I, configForLine config, Vec4i& l)
{
	Mat temp;
	if (I.channels() == 1)
	{
		I.copyTo(temp);
	}
	else
	{
		cvtColor(I, temp, COLOR_BGR2GRAY);
	}
	//阈值
	UseThreshold(temp, temp, config.threshold, THRESH_BINARY);
	//边缘检测
	Canny(temp, temp, config.threshold1, config.threshold2, config.canny_size);
	vector<Vec4i> lines;
	HoughLinesP(temp, lines, 1, CV_PI/1800, 100, config.minLinLength, config.maxLineGap);
	//正、负方向寻找直线
	if (lines.size() == 0)
		return false;
	if (config.line_direction) 
	{
		l = lines[0];
	}
	else
	{
		l = lines.back();
	}
	return true;
}


// 扫码前预处理
void MyReduceImage::PretreatmentForScanCode(const Mat& I, Mat& J, configForScan config)
{
	if (I.channels() == 1)
	{
		I.copyTo(J);
	}
	else
	{
		cvtColor(I, J, COLOR_BGR2GRAY);
	}
	UseThreshold(J, J, config.threshold, 0);
	if (config.ErosionTimes)
		for (int i = 0; i < config.ErosionTimes; i++)
			UseMorphologyEx(I, J, 0, 0, config.ErosionSize);
}


// Harris角点检测
void MyReduceImage::UseCornerHarris(const Mat& I, Mat& dst_norm, Mat& dst_norm_scaled, int blockSize, int apertureSize, double k)
{
	Mat temp;
	if (I.channels() == 1)
	{
		I.copyTo(temp);
	}
	else
	{
		cvtColor(I, temp, COLOR_BGR2GRAY);
	}
	Mat dst = Mat::zeros(I.size(), CV_32FC1);

	/// Detector parameters

	/// Detecting corners
	cornerHarris(I, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

	/// Normalizing
	normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
	convertScaleAbs(dst_norm, dst_norm_scaled);
}


// 绘画角点
void MyReduceImage::DrawCornerHarris(const Mat& I, Mat& J, int blockSize, int apertureSize, double k, int thresh)
{
	Mat dst_norm;
	UseCornerHarris(I, dst_norm, J, blockSize, apertureSize, k);
	/// Drawing a circle around corners
	for (int j = 0; j < dst_norm.rows; j++)
	{
		for (int i = 0; i < dst_norm.cols; i++)
		{
			if ((int)dst_norm.at<float>(j, i) > thresh)
			{
				circle(J, Point(i, j), 5, Scalar(0), 2, 8, 0);
			}
		}
	}
}


// Shi-Tomasi角点检测
void MyReduceImage::UseGoodFeaturesToTrack(const Mat& I, vector<Point2f>& corners, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarrisDetector, double k)
{
	Mat temp;
	if (I.channels() == 1)
	{
		I.copyTo(temp);
	}
	else
	{
		cvtColor(I, temp, COLOR_BGR2GRAY);
	}
	if (maxCorners < 1)  
		maxCorners = 1; 
	goodFeaturesToTrack(temp,
		corners,
		maxCorners,
		qualityLevel,
		minDistance,
		Mat(),
		blockSize,
		useHarrisDetector,
		k);
}


// 绘画Shi-Tomasi角点检测
void MyReduceImage::DrawCorners(const Mat& I, Mat& J, int maxCorners, double qualityLevel, double minDistance, int blockSize, bool useHarrisDetector, double k)
{
	J = I.clone();
	if (J.channels() == 1)
		cvtColor(J, J, COLOR_GRAY2BGR);
	vector<Point2f> corners;
	RNG rng(12345);
	UseGoodFeaturesToTrack(I, corners, maxCorners, qualityLevel, minDistance, blockSize, useHarrisDetector, k);
	for (int i = 0; i < corners.size(); i++)
	{
		circle(J, corners[i], 4, Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
			rng.uniform(0, 255)), -1, 8, 0);
	}
}


// zxing解析二维码
bool MyReduceImage::ScanBarCodeForZxing(const Mat& I, int codeType, string& data, vector<Point>& codePoint)
{
	Mat temp;
	if (I.channels() == 1)
	{
		I.copyTo(temp);
	}
	else
	{
		cvtColor(I, temp, COLOR_BGR2GRAY);
	}
	try
	{
		Ref<LuminanceSource> source = MatSource::create(temp);
		Ref<Reader> reader;
		DecodeHints hints;
		switch (codeType)
		{
		case 0:
			reader.reset(new qrcode::QRCodeReader);
			hints = DecodeHints(DecodeHints::QR_CODE_HINT);
			break;
		case 1:
			reader.reset(new datamatrix::DataMatrixReader);
			hints = DecodeHints(DecodeHints::DATA_MATRIX_HINT);
			break;
		default:
			break;
		}
		Ref<Binarizer> binarizer(new GlobalHistogramBinarizer(source));
		Ref<BinaryBitmap> bitmap(new BinaryBitmap(binarizer));
		//开始解码
		Ref<Result> result(reader->decode(bitmap, hints));
		data = result->getText()->getText();

		// Get result point count
		int resultPointCount = result->getResultPoints()->size();
		codePoint = vector<Point>(resultPointCount);
		//Point* codePoint = new Point[resultPointCount];//动态分配

		for (int j = 0; j < resultPointCount; j++) 
			codePoint[j] = toCvPoint(result->getResultPoints()[j]);
		return true;
		//if (resultPointCount > 0)// Draw text
			//putText(temp, result->getText()->getText(), toCvPoint(result->getResultPoints()[0]), FONT_HERSHEY_PLAIN, 1, Scalar(110, 220, 0));

	}
	catch (zxing::Exception e)
	{
		data = "error decode";
		return false;
	}
}


Point MyReduceImage::toCvPoint(Ref<ResultPoint> resultPoint)
{
	return Point(resultPoint->getX(), resultPoint->getY());
}


// 绘画二维码点位
void MyReduceImage::DrawCodePoint(Mat& J, vector<Point> codePoint)
{
	int len = codePoint.size();
	if (!len > 1)
		return;
	if (J.channels() == 1)
		cvtColor(J, J, COLOR_GRAY2BGR);
	for (int j = 0; j < len; j++)// Draw circle
		circle(J, codePoint[j], 10, Scalar(110, 220, 0), 2);

	// Draw boundary on image
	if (len > 1)
		for (int j = 0; j < len; j++)
		{
			int previousIndex = (j > 0) ? j - 1 : len - 1;
			// Draw line
			//line(temp, toCvPoint(previousResultPoint), toCvPoint(result->getResultPoints()[j]), Scalar(110, 220, 0), 2, 8);
			line(J, codePoint[previousIndex], codePoint[j], Scalar(110, 220, 0), 2, 8);
		}
}


// 形态学变换操作
void MyReduceImage::UseMorphologyEx(const Mat& I, Mat& J, int method, int kernel_type, int kernel_size)
{
	// 由于 MORPH_X的取值范围是: 2,3,4,5 和 6
	int operation = method + 2;

	Mat element = getStructuringElement(kernel_type, Size(2 * kernel_size + 1, 2 * kernel_size + 1), Point(kernel_size, kernel_size));

	/// 运行指定形态学操作
	morphologyEx(I, J, operation, element);
}


// basler相机
void MyReduceImage::PylonFiveDemo(Mat& J)
{
}


void MyReduceImage::UseknnTrain(const Mat& I)
{
	Mat trainData, trainLabels;
	PrepareToTrain(I, trainData, trainLabels, 20);
	//使用KNN算法
	int K = 5;
	Ptr<TrainData> tData = TrainData::create(trainData, ROW_SAMPLE, trainLabels);
	Ptr<KNearest> model = KNearest::create();
	model->setDefaultK(K);
	model->setIsClassifier(true);
	model->train(tData);
	model->save("knn.xml");
}


// knn临近算法
void MyReduceImage::UseknnFindNearest(const Mat& I, string filePath, float& r, vector<float>& results, vector<float>& neighborResponses, vector<float>& dist)
{
	Ptr<KNearest> model = StatModel::load<KNearest>(filePath);
	Mat J, dst;
	if(I.channels() != 1)
		cvtColor(I, J, COLOR_BGR2GRAY);
	else
		I.copyTo(J);
	//threshold(J, J, 48, 255, CV_THRESH_BINARY);
	//Mat now;
	//J.copyTo(now);
	/*J.copyTo(dst);
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(J, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	cvtColor(J, J, COLOR_GRAY2BGR);
	drawContours(J, contours, -1, (255, 0, 255));
	imshow("Image", J);
	vector<Point> point = contours[0];
	Rect rect = boundingRect(Mat(point));

	for (int i = 0; i < contours.size(); i++)
	{
		Rect rect1 = boundingRect(Mat(contours[i]));
		rectangle(J, rect1.tl(), rect1.br(), Scalar(0, 0, 255), 1, 8, 0);
	}

	int x = rect.tl().x, y = rect.tl().y;
	int h = rect.br().x, w = rect.br().y;
	Mat now = dst(Range(y, w), Range(x, h));*/
	//dst(rect).copyTo(now);
	resize(J, J, Size(20, 20));

	imshow("now", J);

	Mat_<float>  nums;
	nums = J.reshape(0, 1);
	nums.convertTo(nums, CV_32F);

	//r = model->predict(nums);
	// Mat results, neighborResponses, dist;
	r = model->findNearest(nums, 8, results, neighborResponses, dist);

}


void MyReduceImage::UseSVMTrain(const Mat& I)
{
	Mat trainData, trainLabels;
	PrepareToTrain(I, trainData, trainLabels, 20);

	Ptr<TrainData> tData = TrainData::create(trainData, ROW_SAMPLE, trainLabels);
	Ptr<SVM> model = SVM::create();
	model->setType(SVM::C_SVC);
	model->setKernel(SVM::LINEAR);
	model->setC(1); 
	model->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER, 100, 1e-6));
	model->train(tData);
	model->save("SVMdata.xml");
}


void MyReduceImage::UseSVMPredict(const Mat& I, string filePath, float& r)
{
	Ptr<SVM> model = StatModel::load<SVM>(filePath);
	Mat J, dst;
	if (I.channels() != 1)
		cvtColor(I, J, COLOR_BGR2GRAY);
	else
		I.copyTo(J);
	threshold(J, J, 48, 255, THRESH_BINARY);
	resize(J, J, Size(20, 20));

	Mat_<float>  nums;
	nums = J.reshape(0, 1);
	nums.convertTo(nums, CV_32FC1);

	r = model->predict(nums);
}


void MyReduceImage::PrepareToTrain(const Mat& I, Mat& TrainData, Mat& Labels, int size)
{
	Mat gray;
	if (I.channels() != 1)
		cvtColor(I, gray, COLOR_BGR2GRAY);
	else
		I.copyTo(gray);
	//int b = 20;
	int m = gray.rows / size;   //原图为1000*2000
	int n = gray.cols / size;   //裁剪为5000个20*20的小图块
	Mat data, labels;   //特征矩阵
	for (int i = 0; i < n; i++)
	{
		int offsetCol = i*size; //列上的偏移量
		for (int j = 0; j < m; j++)
		{
			int offsetRow = j*size;  //行上的偏移量
								  //截取20*20的小块
			Mat tmp;
			gray(Range(offsetRow, offsetRow + size), Range(offsetCol, offsetCol + size)).copyTo(tmp);
			CString n;
			n.Format(_T("image\\%d\\%d_%d.png"), j / 5, i, j);
			imwrite(string(CW2A(n.GetString())), tmp);
			data.push_back(tmp.reshape(0, 1));  //序列化后放入特征矩阵
			labels.push_back((int)j / 5);  //对应的标注
		}
	}

	data.convertTo(data, CV_32FC1); //uchar型转换为cv_32f
									//labels.convertTo(labels, CV_32F);
	TrainData = data(Range(0, m*n), Range::all());
	Labels = labels(Range(0, m*n), Range::all());
}



bool MyReduceImage::readNumClassData(const string& filename, int var_count, Mat& _data, Mat& _responses)
{
	const int M = 1024;
	char buf[M + 2];

	Mat el_ptr(1, var_count, CV_32F);
	int i;
	vector<int> responses;

	_data.release();
	_responses.release();
	FILE* f;
	fopen_s(&f, filename.c_str(), "rt");
	if (!f)
	{
		cout << "Could not read the database " << filename << endl;
		return false;
	}

	for (;;)
	{
		char* ptr;
		if (!fgets(buf, M, f) || !strchr(buf, ','))
			break;
		responses.push_back((int)buf[0]);
		ptr = buf + 2;
		for (i = 0; i < var_count; i++)
		{
			int n = 0;
			sscanf_s(ptr, "%f%n", &el_ptr.at<float>(i), &n);
			ptr += n + 1;
		}
		if (i < var_count)
			break;
		_data.push_back(el_ptr);
	}
	fclose(f);
	Mat(responses).copyTo(_responses);
	return true;
}


Ptr<TrainData> MyReduceImage::prepareTrainData(const Mat& data, const Mat& responses, int ntrain_samples)
{
	Mat sample_idx = Mat::zeros(1, data.rows, CV_8U);
	Mat train_samples = sample_idx.colRange(0, ntrain_samples);
	train_samples.setTo(Scalar::all(1));

	int nvars = data.cols;
	Mat var_type(nvars + 1, 1, CV_8U);
	var_type.setTo(Scalar::all(VAR_ORDERED));
	var_type.at<uchar>(nvars) = VAR_CATEGORICAL;

	return TrainData::create(data, ROW_SAMPLE, responses,
		noArray(), sample_idx, noArray(), var_type);
}


void MyReduceImage::UseSVMTrain(const string data_filename)
{
	Mat data;
	Mat responses;
	readNumClassData(data_filename, 16, data, responses);

	int nsamples_all = data.rows;
	int ntrain_samples = (int)(nsamples_all*0.8);

	Ptr<SVM> model;
	Ptr<TrainData> tdata = prepareTrainData(data, responses, ntrain_samples);
	model = SVM::create();
	model->setType(SVM::C_SVC);
	model->setKernel(SVM::LINEAR);
	model->setC(1);
	model->train(tdata);
	model->save("SVMdata2.xml");
}


// 拉普拉斯变换
void MyReduceImage::UseLaplacian(Mat& I, Mat& J, int aperture)
{
	Mat temp;
	Laplacian(I, temp, CV_32F, aperture);
	temp.convertTo(J, CV_8U, 1.0, -50);
	J = I - J;
}


void MyReduceImage::findAllWord(const Mat& I, Mat& J, int offset, Mat& data, Mat& labels, vector<Rect>& rects)
{
	//J = Mat(I.cols, I.rows, I.type());
	int c = I.channels();

	if (c == 1)
	{
		I.copyTo(J);
		//cvtColor(I, J, CV_GRAY2BGR);
	}
	else if (c == 3)
	{
		//I.copyTo(J);
		cvtColor(I, J, COLOR_BGR2GRAY);
	}

	//Scalar color[3] = { Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255) };
	CString folder;
	//char ch[] = {'R', '-', '4', '1', '0', '3', '9', '6', '8', '2'};
	char* ch = "R-41039683";
	//vector<Rect> rects;
	if (rects.empty())
	{
		Mat temp;
		J.copyTo(temp);
		UseThreshold(temp, temp, 150, THRESH_BINARY_INV);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(temp, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);
		for (int i = 0; i < 10; i++)
		{
			Rect r = boundingRect(contours[i]);
			int index = i;
			for (int j = 0; j < i; j++)
			{
				if (r.br().x < rects[j].br().x)
				{
					index = j;
					break;
				}
			}
			rects.insert(rects.begin() + index, r);
		}
	}

	//Mat data, labels;   //特征矩阵
	for (int i = 0; i < 10; i++)
	{
		folder.Format(_T("imageBackUp\\%c\\%d_%d.png"), ch[i % 10], i, offset);
		//rectangle(J, r, color[i % 3], 1, 8, 0);
		Mat tmp = J(rects[i]);
		imwrite(string(CW2A(folder.GetString())), tmp);
		resize(tmp, tmp, Size(20, 20));
		data.push_back(tmp.reshape(0, 1));  //序列化后放入特征矩阵
		labels.push_back((int)ch[i]);  //对应的标注
	}

	/*int K = 5;
	Ptr<TrainData> tData = TrainData::create(data, ROW_SAMPLE, labels);
	Ptr<KNearest> model = KNearest::create();
	model->setDefaultK(K);
	model->setIsClassifier(true);
	model->train(tData);
	model->save("baterryKnn.xml");*/
}


// 端子检测
void MyReduceImage::UseBlocksChecker(const Mat& I, Mat& J, int offset)
{
	//(40,1161),(1216,1901)
	Mat temp = I(Rect(40, 1161, 1176, 740)).clone();
	//Mat temp = I(Rect(366, 892, 1362, 875)).clone();
	Mat tempa = temp.clone();
	threshold(temp, temp, 200, 255, THRESH_BINARY);

	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;
	Sobel(temp, grad_x, CV_16S, 1, 0);
	Sobel(temp, grad_y, CV_16S, 0, 1);
	//	Scharr(I, grad_x, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
	//	Scharr(I, grad_y, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);
	temp = abs_grad_y - abs_grad_x;

	vector<Vec4i> lines;
	HoughLinesP(temp, lines, 1, CV_PI / 900, 100, 750, 100);

	if (lines.empty())
		return;
	double x = lines[0][0] - lines[0][2];
	double y = lines[0][1] - lines[0][3];
	double t = atan(y / x) / CV_PI * 180;

	Point center = Point(tempa.cols / 2, tempa.rows / 2);
	Mat rot_mat(2, 3, CV_32FC1);
	Mat tempb;
	rot_mat = getRotationMatrix2D(center, t, 1);
	warpAffine(tempa, tempb, rot_mat, tempa.size());
	//(112,0),(115,375)左
	//(574,2),(580,372)右
	//(103,1),(585,137)上(295,6),(342,138)，(344,4),(398,137)(289,5),(398,141)
	//(103,1),(585,137)上
	//(110,220),(581,356)下
	//J = tempb.clone();
	imshow("tempb", tempb);

	Mat templ = imread("blockTemplate\\blocks.png");
	if (templ.channels() != 1)
		cvtColor(templ, templ, COLOR_BGR2GRAY);
	if (tempb.channels() != 1)
		cvtColor(tempb, tempb, COLOR_BGR2GRAY);
	Mat res;
	Point p;
	double d;
	UseMatchTemplate(tempb, templ, res, 0, d, p);

	J = tempb(Rect(p, templ.size()));

	Point centerb = Point(J.cols / 2, J.rows / 2);
	Mat tempc;
	rot_mat = getRotationMatrix2D(centerb, 180, 1);
	warpAffine(J, tempc, rot_mat, J.size());

	Rect rects[] = {Rect(12, 94, 94, 165), Rect(125, 2, 157, 140), Rect(299, 8, 39, 131), 
		Rect(351, 8, 39, 131), Rect(406, 2, 157, 140), Rect(115, 138, 462, 82), 
		Rect(11, 97, 94, 165), Rect(125, 4, 157, 140), Rect(300, 10, 39, 131), 
		Rect(351, 10, 39, 131), Rect(406, 4, 157, 140) };
	CString folder;
	for (int i = 0; i < 12; i++)
	{
		folder.Format(_T("blocksImage\\%d\\%d.png"), offset, i);
		Mat tmp;
		if (i < 6)
			tmp = J(rects[i]);
		else if (i < 11)
			tmp = tempc(rects[i]);
		else
			tmp = tempb;
		imwrite(string(CW2A(folder.GetString())), tmp);
	}
}


// 特征点检测
void MyReduceImage::UseFeatureDetector(const Mat& I, Mat& J)
{
	Mat temp;
	if (I.channels() == 3)
		cvtColor(I, temp, COLOR_BGR2GRAY);
	else
		I.copyTo(temp);

	Ptr<SURF> detector = SURF::create(400);
}
