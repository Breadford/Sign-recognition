
#include "Enums.hpp"

#include <visualizer.hpp>

#include <iostream>
#include <cstdlib>
using namespace std;

#include <unistd.h>

#define DEBUG(x) do{ std::cout << #x << " = " << x << std::endl; }while(0)

#include "opencv2/opencv.hpp"
using namespace cv;


#define CRVENA_PLAVA 0

struct cropovanje {
	int x;
	int y;
	float sirina;
	float visina;


};

struct cropovanje getContours(Mat imgDil, Mat img) {

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	struct cropovanje abc;

	findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	drawContours(img, contours, -1, Scalar(161, 111, 245), 5);

	vector<vector<Point>> conPoly(contours.size());
	vector<Rect> boundRect(contours.size());
	 
	for (int i = 0; i < contours.size(); i++)
	{
		int area = contourArea(contours[i]);
		cout << " Area : " << area << endl<< endl;
		cout << area << endl;
		string objectType;

		if (area > 1000) 
		{
			float peri = arcLength(contours[i], true);
			approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);
			cout << conPoly[i].size() << endl;
			boundRect[i] = boundingRect(conPoly[i]);
		
			int objCor = (int)conPoly[i].size();

			if (objCor == 3) { objectType = "Tri"; }
			else if (objCor == 4)
			{ 
				float aspRatio = (float)boundRect[i].width / (float)boundRect[i].height;
				cout << aspRatio << endl;
				if (aspRatio> 0.95 && aspRatio< 1.05){ objectType = "Square"; }
				else { objectType = "Rect";}
			}
			else if (objCor == 8) { objectType = "Octagon"; }

			
			else if (objCor > 8) { objectType = "Circle"; }

			rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(161, 111, 245), 5);
			cout << "X koordinata: " << boundRect[i].x << endl;
			cout << "Y koordinata: " << boundRect[i].y << endl;



			abc.x = boundRect[i].x;
			abc.y = boundRect[i].y;
			abc.sirina = boundRect[i].width;
			abc.visina = boundRect[i].height;

			

			putText(img, objectType, { boundRect[i].x,boundRect[i].y - 5 }, FONT_HERSHEY_PLAIN,1, Scalar(0, 69, 255), 2);
		}
		
	}

	return abc;
}



int main() {

	Mat srcHSV, mask1, mask2, maskBlue, mask, cropHSV, mask3, mask4, mask_crop;	
	struct cropovanje cba;
	Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;

	Mat src = cv::imread("data/qwe.jpg");
	Mat src3 = cv::imread("data/stop_sign.jpg");
	Mat src2 = cv::imread("data/as.jpg");
	Mat src4 = cv::imread("data/2.jpg");
	cout << "Redovi originalne: " << src.rows << endl;
	cout << "Kolone originalne: " << src.cols << endl;
	if(src.empty()){
		throw runtime_error("Cannot open image!");
	}

	visualizer::load_cfg("data/main.visualizer.yaml");

	imshow("Pocetna", src);
	
	cvtColor(src, srcHSV, COLOR_BGR2HSV);



	inRange(srcHSV, Scalar(0, 95, 50), Scalar(10, 255, 255), mask1);
	inRange(srcHSV, Scalar(170, 95, 50), Scalar(180, 255, 255), mask2);

	//inRange(srcHSV, Scalar(100, 158, 0), Scalar(138, 255, 255), maskBlue);

	mask = mask1 | mask2;
	
	imshow("Mask-red", mask);

	//imshow("MASK-blue", maskBlue);

	GaussianBlur(mask, imgBlur, Size(3, 3), 3, 0);
	
	Canny(imgBlur, imgCanny, 25, 75);

	Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
	
	dilate(imgCanny, imgDil, kernel);

	cba = getContours(imgDil, src);

	imshow("Konture", src);


	Mat crop_img;

	cv::Rect crop_region(cba.x,cba.y, cba.sirina, cba.visina);
		
	crop_img = src(crop_region);
			



	cvtColor(crop_img, cropHSV, COLOR_BGR2HSV);

	inRange(cropHSV, Scalar(0, 95, 50), Scalar(10, 255, 255), mask3);
	inRange(cropHSV, Scalar(170, 95, 50), Scalar(180, 255, 255), mask4);

	mask_crop = mask3 | mask4;
	


	Mat im_floodfill = mask_crop.clone();
	floodFill(im_floodfill, cv::Point(0, 0), Scalar(255));

	Mat im_floodfill_inv;
	bitwise_not(im_floodfill, im_floodfill_inv);
	Mat im_out = (mask_crop | im_floodfill_inv);

	
	imshow("Inverzna", im_out);

	imshow("Kropovana", crop_img);
       
	int io, oj;

	for (io = 0; io < crop_img.rows; io++)
	{
		for (oj = 0; oj < crop_img.cols/3; oj++)
		{
			if (im_out.at<cv::Vec3b>(io,oj)[0] == 0 && im_out.at<cv::Vec3b>(io,oj+1)[0] == 0 && im_out.at<cv::Vec3b>(io,oj+2)[0] == 0)
			{
				crop_img.at<cv::Vec3b>(io,oj*3) = {161, 111, 245};
			}
			if (im_out.at<cv::Vec3b>(io,oj)[1] == 0 && im_out.at<cv::Vec3b>(io,oj+1)[1] == 0 && im_out.at<cv::Vec3b>(io,oj+2)[1] == 0)
			{
				crop_img.at<cv::Vec3b>(io,oj*3+1) = {161, 111, 245};
			}
			if (im_out.at<cv::Vec3b>(io,oj)[2] == 0 && im_out.at<cv::Vec3b>(io,oj+2)[2] == 0 && im_out.at<cv::Vec3b>(io,oj+2)[2] == 0)
			{
				crop_img.at<cv::Vec3b>(io,oj*3+2) = {161, 111, 245};
			}
		}

	}

	//cv::namedWindow("ZNAK");
	imshow("Finalna", crop_img);

	cv::waitKey();

	const uint16_t width = 10;
	const uint16_t height = 20;
	static uint8_t pix[width*height*3];
	for(uint16_t y = 0; y < height; y++){
		for(uint16_t x = 0; x < width; x++){
			uint32_t i = (y*width + x)*3;
			// Red.
			pix[i+0] = 0;
			pix[i+1] = 0;
			pix[i+2] = 255;
		}
	}

	for(uint16_t y = 3; y < height-3; y++){
		for(uint16_t x = 3; x < width-3; x++){
			uint32_t i = (y*width + x)*3;
			// Blue.
			pix[i+0] = 255;
			pix[i+1] = 0;
			pix[i+2] = 0;
		}
	}

	visualizer::img::show(
		"src",
		pix,
		width,
		height
	);


	visualizer::img::show(
		"h",
		src
	);

	visualizer::img::show(
		"r",
		src2
	);

	int th_start_h0;

	while(true){
		th_start_h0 = 20;
		visualizer::slider::slider(
			"/win0/upper_half/upper_rigth_corner/th_start_h0",
			th_start_h0,
			[&](int& value){
				DEBUG(th_start_h0);
			}
		);

		for(int i = 0; i < 3; i++){
			visualizer::slider::update();
			DEBUG(th_start_h0);
			sleep(1);
		}
	}

	return 0;
}
