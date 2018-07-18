#include "stdafx.h"
#include "MyReduceImage.h"
#include <opencv2/opencv.hpp> 
#include <iostream>
#include <sstream>

using namespace std;
using namespace cv;

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
	Mat lookUpTable(1, 256, CV_8UC1,table);
	LUT(I, lookUpTable, J);
	//return J;
}


// 阈值
void MyReduceImage::UseThreshold(const Mat& I,Mat& J,int threshold_value, int threshold_type)
{
	Mat src_gray;
	if (I.channels() != 1)
		cvtColor(I, src_gray, CV_RGB2GRAY);
	else
		src_gray = I.clone();
	threshold(src_gray, J, threshold_value, 255, threshold_type);
}

// 矩阵的掩码操作
void MyReduceImage::UseFilter2D(const Mat& I, Mat& J)
{
	Mat kernel = (Mat_<char>(3, 3) << 0, -1,  0,
									 -1,  5, -1,
									  0, -1,  0);
	filter2D(I, J, I.depth(), kernel);
}

// 图像求和
void MyReduceImage::UseAddWeighted(const Mat& I, Mat& K, double alpha, Mat& J)
{
	addWeighted(I,alpha,K,(1.0 - alpha),0.0,J);
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
void MyReduceImage::UseErosion(const Mat& I, Mat& J, int erosion_type, int erosion_size)
{
	Mat element = getStructuringElement(erosion_type,
		Size(2 * erosion_size + 1, 2 * erosion_size + 1),
		Point(erosion_size, erosion_size));
	// 腐蚀操作
	erode(I, J, element);
}

// 膨胀处理
void MyReduceImage::UseDilation(const Mat & I, Mat & J, int dilation_type, int dilation_size)
{
	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
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
		else if(method == 1)
		{
			Scharr(I, grad_x, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT );
			Scharr(I, grad_y, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT );
		}
		convertScaleAbs(grad_x, abs_grad_x);
		convertScaleAbs(grad_y, abs_grad_y);
		/// 合并梯度(近似)
		addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, J);
	}
}

// 霍夫线变换
void MyReduceImage::UseHoughLines(const Mat& I, Mat& J, int method)
{
	J = I.clone();
	Mat temp;
	if (I.channels() != 1)
		UseEdgeDetection(I, temp, 3);//canny算子
	else
		temp = J;
	if (method == 0)
	{
		vector<Vec2f> lines;
		HoughLines(temp, lines, 1, CV_PI / 180, 100, 0, 0);

		for (size_t i = 0; i < lines.size(); i++)
		{
			float rho = lines[i][0], theta = lines[i][1];
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000 * (-b));
			pt1.y = cvRound(y0 + 1000 * (a));
			pt2.x = cvRound(x0 - 1000 * (-b));
			pt2.y = cvRound(y0 - 1000 * (a));
			line(J, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
		}
	}
	else
	{
		vector<Vec4i> lines;
		HoughLinesP(temp, lines, 1, CV_PI / 180, 50, 50, 10);
		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];
			line(J, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
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
		cvtColor(I, temp, CV_BGR2GRAY);
	}
	else
		temp = J;
	/// 使用 3x3内核降噪
	blur(temp, temp, Size(3, 3));
	vector<Vec3f> circles;

	/// Apply the Hough Transform to find the circles
	HoughCircles(temp, circles, CV_HOUGH_GRADIENT, 1, temp.rows / 8, 200, 100, 0, 0);

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
	rectangle(I, s, e, Scalar(0, 0, 255),2,8,0);
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
	update_map(type,I.rows,I.cols);
	remap(I, J, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
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

	Mat warp_mat(2, 3, CV_32FC1);
	warp_mat = getAffineTransform(srcTri, dstTri);

	warpAffine(I, J, warp_mat, J.size());
}


// 旋转图片（放射变换法）
void MyReduceImage::RotateImage(const Mat& I, Mat& J, double angle)
{
	Point center = Point(I.cols / 2, I.rows / 2);
	Mat rot_mat(2, 3, CV_32FC1);
	rot_mat = getRotationMatrix2D(center, angle, 1);
	warpAffine(I, J, rot_mat, I.size());
}


// 直方图均衡化
void MyReduceImage::UseEqualizeHist(const Mat& I, Mat& J)
{
	Mat temp;
	if (I.channels() != 1) {
		cvtColor(I, temp, CV_BGR2GRAY);
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
					CV_FILLED);
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
	if(I.channels() != J.channels())
		return 0.0;
	Mat hist_i, hist_j;
	UseCalcHist(I, hist_i, 30,32);
	UseCalcHist(J, hist_j, 30,32);
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
		cvtColor(I, hsv, CV_BGR2HSV);
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
	cvtColor(J, hsv, CV_BGR2HSV);
	calcBackProject(&hsv, 1, channels, temp, backproj, histRange_bgr, 1, true);
	backproj.copyTo(K);
}


// 模板匹配方法
void MyReduceImage::UseMatchTemplate(const Mat& I, const Mat& templ, Mat& J, int method)
{
	I.copyTo(J);
	if (I.channels() != templ.channels())
		return;
	Mat result;
	int result_cols = I.cols - templ.cols + 1;
	int result_rows = I.rows - templ.rows + 1;

	result.create(result_cols, result_rows, CV_32FC1);

	matchTemplate(I, templ, result, method);

	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

	Point minLoc; 
	Point maxLoc;
	Point matchLoc;

	minMaxLoc(result, 0, 0, &minLoc, &maxLoc);
	if (method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED)
		matchLoc = minLoc;
	else
		matchLoc = maxLoc;

	rectangle(J, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

	imshow("result", result);
}


// 绘画轮廓
void MyReduceImage::FindAndDrawContours(const Mat& I, Mat& J, int thresh)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh);
	RNG rng(12345);
	for (int i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//drawContours(J, contours, i, color, 2, 8, hierarchy, 0, Point());
		drawContours(J, contours, i, color, 2, 8, vector<Vec4i>(), 0, Point());
	}

}


// 寻找轮廓
void MyReduceImage::FindAllContours(const Mat& I, vector<vector<Point>>& contours, vector<Vec4i>& hierarchy, int thresh)
{
	Mat temp;
	if (I.channels() == 1)
	{
		I.copyTo(temp);
	}
	else
	{
		cvtColor(I, temp, CV_BGR2GRAY);
	}
	UseBlur(temp, temp, 1, 3);
	Canny(temp, temp, thresh, thresh * 2, 3);

	/// 寻找轮廓
	findContours(temp, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
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
	FindAllContours(I, contours, hierarchy, thresh);
	int len = contours.size();
	vector<vector<Point> > hull(len);
	FindConvexHull(contours, hull);
	RNG rng(12345);
	for (int i = 0; i< len; i++)
	{
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		drawContours(J, hull, i, color, 1, 8, vector<Vec4i>(), 0, Point());
		if(is_draw_contours)
			drawContours(J, contours, i, color, 1, 8, vector<Vec4i>(), 0, Point());
	}

}


// 多边形拟合
void MyReduceImage::UseApproxPolyDP(const Mat& I, Mat& J, int thresh, bool closed)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh);

	/// 多边形逼近轮廓 + 获取矩形和圆形边界框
	vector<vector<Point> > contours_poly(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, closed);
	}
	RNG rng(12345);
	for (int i = 0; i< contours.size(); i++)
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
	FindAllContours(I, contours, hierarchy, thresh);
	int len = contours.size();
	vector<Rect> boundRect(len);
	for (int i = 0; i < len; i++)
	{
		boundRect[i] = boundingRect(Mat(contours[i]));
	}
	for (int i = 0; i< len; i++)
	{
		rectangle(J, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 0, 255), 2, 8, 0);
	}
}


// 最小包覆斜矩形
void MyReduceImage::UseMinAreaRect(const Mat& I, Mat& J, int thresh)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh);
	int len = contours.size();

	vector<Point2f[4]> vertices(len);//定义矩形的4个顶点

	for (int i = 0; i < len; i++)
		minAreaRect(Mat(contours[i])).points(vertices[i]); //计算矩形的4个顶点

	for (int i = 0; i< len; i++)
		for (int j = 0; j < 4; j++)
			line(J, vertices[i][j], vertices[i][(j+1)%4], Scalar(0, 255, 0));
}



// 最小包覆圆形
void MyReduceImage::UseMinEnclosingCircle(const Mat& I, Mat& J, int thresh)
{
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	FindAllContours(I, contours, hierarchy, thresh);
	vector<Point2f>center(contours.size());
	vector<float>radius(contours.size());
	int len = contours.size();
	for (int i = 0; i < len; i++)
	{
		minEnclosingCircle(contours[i], center[i], radius[i]);
	}
	for (int i = 0; i< len; i++)
	{
		circle(J, center[i], (int)radius[i], Scalar(0, 0, 255), 2, 8, 0);
	}
}


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
	default:
		break;
	}
}
