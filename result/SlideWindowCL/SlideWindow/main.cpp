#include <iostream>
#include <string>
#include <fstream>
#include <time.h>

#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

#include "SVMDetector.h"
#include "rectangles.h"

using namespace std;
using namespace cv;

int main()
{
	//	Calculate Execution Time.
	clock_t totalStart = clock();
	double excTime;
	clock_t start, end;

	SVMDetector svmDetect;
	start = clock();
	svmDetect.initDetector("vehicle_detect.model");
	end = clock();
	excTime = (double)(end - start);
	cout << "Init Detector Cost: " << excTime << "(ms)" << endl;

	Para param;
	param.sw = 0.2; //����
	param.sh = 0.2; //����
	param.ss = 1.3; //����
	param.th = 0.9999; //���Ըĳ�0.9 0.95 0.99
	//param.th = 0.9;
	
	ifstream filelist("vehicle_data/filelist.txt");
	while(filelist)
	{
		string line;
		getline(filelist, line);
		if(line.size() == 0)
			break;

		Mat img = imread("vehicle_data/" + line);

		Mat img_g = img.clone();
		Mat img_r = img.clone();

		vector<Rect> rects;
		vector<double> scores; 

		vector<Rect_<double> > double_rects;
		vector<Rect_<double> > drects; // ����Ŀ������λ��
		vector<double> dscores;
		vector<rectw> rectws; // rectws��������

		start = clock();
		svmDetect.detectInRectUsingOpenCVHog(img, rects, scores); // ��detection
		end = clock();
		excTime = (double)(end - start);
		cout << line << "\'s detectInRectUsingOpenCVHog Cost: " << excTime << "(ms)" << endl;

		for(int i = 0 ; i < rects.size(); i++)
		{
			rectw temp;
			temp.r = rects[i];
			temp.w = scores[i];
			rectws.push_back(temp);
		}
		//	���յ÷�����
		sort(rectws.begin(), rectws.end(), cmp);
		scores.clear();

		for(int i = 0 ; i < rects.size(); i++)
		{
			double_rects.push_back(Rect_<double>(rectws[i].r));   //��vector<Rect>ת��Ϊvector<Rect_<double> >
			scores.push_back(rectws[i].w);
		}
		
		start = clock();
		non_max_sp(double_rects, scores, param, drects, dscores); //����,NMS
		end = clock();
		excTime = (double)(end - start);
		cout << line << "\'s Non Max Sp Cost: " << excTime << "(ms)" << endl;

		start = clock();
		for(int i = 0 ; i < rects.size(); i++)
		{
			rectangle(img_g, rects[i], Scalar(0, 255, 0)); //���̿�
		}
		for(int i = 0 ;i < drects.size(); i++)
		{
			rectangle(img_r, drects[i], Scalar(0, 0, 255), 2);  //�����ĺ��
		}
		imwrite(string("test_result") + "/red/" + line, img_r);
		imwrite(string("test_result") + "/green/" + line, img_g);
		end = clock();
		excTime = (double)(end - start);
		cout << line << "\'s Drawing Cost: " << excTime << "(ms)" << endl;

		cout << "Finish Processing " << line << endl << endl << endl;
	}

	clock_t totalEnd = clock();
	cout << "Total Cost: " << (double)(totalEnd - totalStart) << "ms" << endl;
	return 0;
}
