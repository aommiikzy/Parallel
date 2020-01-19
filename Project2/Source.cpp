#include <stdio.h>
#include <iostream>
#include <ctime>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

/** @function main */
int main(int argc, char** argv)
{
	clock_t start = clock();
	Mat src;
	Mat grad;

	float scale = 0.7;
	float delta = 1.25;
	float ddepth = 3.0;

	int c;

	/// Load an image
	src = imread("bigdog.jpg");

	if (!src.data)
	{
		return -1;
	}



	/// Convert it to gray
	///cvtColor(src, gray, CV_RGB2GRAY);

	Mat gray(src.size(), CV_8UC1);

	int rows = src.rows;
	int cols = src.cols;


	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {

			Vec3b intensity = src.at<Vec3b>(i, j);

			int blue = intensity.val[0];
			int green = intensity.val[1];
			int red = intensity.val[2];

			gray.at<uchar>(i, j) = blue * 0.07 + green * 0.71 + red * 0.21;

		}

	}


	///// Create window
	//namedWindow("test", CV_WINDOW_AUTOSIZE);

	///// Generate grad_x and grad_y
	//Mat grad_x, grad_y;
	//Mat abs_grad_x, abs_grad_y;

	///// Gradient X
	////Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
	//Sobel(gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	//convertScaleAbs(grad_x, abs_grad_x);

	///// Gradient Y
	////Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
	//Sobel(gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	//convertScaleAbs(grad_y, abs_grad_y);


	/*imshow("Gray", gray);
	imshow("SobelX", abs_grad_x);
	imshow("SobelY", abs_grad_y);*/
	imwrite("Gray.jpg", gray);
	/*imwrite("SobelX.jpg", abs_grad_x);
	imwrite("SobelY.jpg", abs_grad_y);*/
	/*printf("Time taken: %.5f\n", (double)(clock() - tStart) / clock_t(1));*/
	clock_t end = clock();
	double elapsed_secs = double(end - start);
	fflush(stdout);
  std::cout <<"Time used: " << elapsed_secs << std::endl;

	waitKey(0);

	return 0;
}
