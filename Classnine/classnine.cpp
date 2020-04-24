#include <opencv2/opencv.hpp>
#include <iostream>
#include <math.h>

constexpr auto cell_size =  16;
constexpr auto angle_size = 8;

using namespace std;
using namespace cv;

bool cal_gxy(Mat src,Mat &angle,Mat &mag)
{
	Mat gx, gy;
	Sobel(src, gx, CV_32F, 1, 0, 1);
	Sobel(src, gy, CV_32F, 0, 1, 1);
	cartToPolar(gx, gy, mag, angle, true);

	float s = 360 / angle_size;
	for (int i = 0; i < angle.rows; i++)
	{
		for (int j = 0; j < angle.cols; j++)
		{
			angle.at<Vec3f>(i, j)[0] = (angle.at<Vec3f>(i, j)[0]/ s);
			angle.at<Vec3f>(i, j)[1] = (angle.at<Vec3f>(i, j)[1]/ s);
			angle.at<Vec3f>(i, j)[2] = (angle.at<Vec3f>(i, j)[2]/ s);
		}
	}
	return true;
}

bool divid_img(Mat src, vector<Mat> &cells)
{
	int nX = src.cols / cell_size;
	int nY = src.rows / cell_size;
	
	for (int i = 0; i < nY; i++)
	{
		for (int j = 0; j < nX; j++)
		{
			Rect rect(j*cell_size, i*cell_size, cell_size, cell_size);
			cells.push_back(Mat(src, rect));
		}
	}

	return  true;
}

bool creat_hist(Mat src,vector<float> &hist)
{
	vector<Mat> cells;
	vector<Mat> mag_cells;
	Mat angle, mag;

	cal_gxy(src, angle, mag);
	divid_img(angle, cells);
	divid_img(mag, mag_cells);

	int cells_size = cells.size();
	vector<vector<float>> hist_part(cells_size,vector<float> (angle_size,0));
	for (int i = 0; i < cells_size; i++)
	{
		for (int m = 0; m < cells[i].rows; m++)
		{
			for (int n = 0; n < cells[i].cols; n++)
			{
				hist_part[i][cells[i].at<Vec3f>(m, n)[0]] += mag_cells[i].at<Vec3f>(m, n)[0];
				hist_part[i][cells[i].at<Vec3f>(m, n)[1]] += mag_cells[i].at<Vec3f>(m, n)[1];
				hist_part[i][cells[i].at<Vec3f>(m, n)[2]] += mag_cells[i].at<Vec3f>(m, n)[2];
			}
		}
	}

	
	for (int j = 0; j < hist_part.size(); j++)
	{
		for (int k = 0; k < hist_part[0].size(); k++)
		{
			hist[k] += hist_part[j][k];
		}
	}

	return true;
}


float calculation(vector<float> hist1,vector<float> hist2)
{
	int a = hist1.size();
	int b = hist2.size();
	int len = min(a, b);
	float res = 0;
	for (int i = 0; i < len; i++)
	{
		res += (hist1[i] - hist2[i])*(hist1[i] - hist2[i]);
	}
	res = sqrt(res);
	return res;
}
int main()
{
	Mat train_src = imread("hogTemplate.jpg");
	Mat img1 = imread("img1.jpg");
	Mat img2 = imread("img2.jpg");

	vector<float> train_hist(angle_size);
	vector<float> hist1(angle_size);
	vector<float> hist2(angle_size);

	creat_hist(train_src, train_hist);
	creat_hist(img1, hist1);
	creat_hist(img2, hist2);

	float res1 = calculation(hist1, train_hist);
	float res2 = calculation(hist2, train_hist);

	if (res1 < res2)
		cout << "img1ÏàËÆ" << endl;
	else
		cout << "img2ÏàËÆ" << endl;

	return 0;
}