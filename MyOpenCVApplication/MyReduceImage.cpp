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
	CV_Assert(I.depth() == CV_8U);  // 仅接受uchar图像
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


void MyReduceImage::UseFilter2D(const Mat& I, Mat& J)
{
	Mat kernel = (Mat_<char>(3, 3) << 0, -1,  0,
									 -1,  5, -1,
									  0, -1,  0);
	filter2D(I, J, I.depth(), kernel);
}


void MyReduceImage::UseAddWeighted(const Mat& I, Mat& K, double alpha, Mat& J)
{
	addWeighted(I,alpha,K,(1.0 - alpha),0.0,J);
}


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

void MyReduceImage::UseDilation(const Mat & I, Mat & J, int dilation_type, int dilation_size)
{
	Mat element = getStructuringElement(dilation_type,
		Size(2 * dilation_size + 1, 2 * dilation_size + 1),
		Point(dilation_size, dilation_size));
	///膨胀操作
	dilate(I, J, element);
}


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

//画矩形
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

//重映射
void MyReduceImage::UseRemap(const Mat& I, Mat& J, int type)
{
	map_x.create(I.size(), CV_32FC1);
	map_y.create(I.size(), CV_32FC1);
	update_map(type,I.rows,I.cols);
	remap(I, J, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));
}
