#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include "math.h"
#include "tinyxml2-master/tinyxml2.h"
#include "tinyxml2-master/tinyxml2.cpp"
#include "fstream"
#include "opencv2/objdetect.hpp"
#include <algorithm>
#include <string>

using namespace cv;
using namespace std;
using namespace tinyxml2;

int main()
{

	String car_cascade_name = "DS/lbp_1200/cascade.xml";
	CascadeClassifier car_cascade;
	Mat frame, gray;
	vector<Rect> cars;
	int SpaceNos ;
	std::vector<int> Occupied;
	int occupied_flag=0;
	float minimum_dist=2000;
	int occupied_count=0;
	int occupied_detected=0;

	if (!car_cascade.load(car_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -1; };

	String file1="/home/aditi/Computer_Vision/HW2/PKLot-small/parking1a/cloudy/2012-12-12/2012-12-12_10_00_05.jpg";
	String file2="/home/aditi/Computer_Vision/HW2/PKLot-small/parking1a/rainy/2012-12-07/2012-12-07_16_42_25.jpg";
	String file3="/home/aditi/Computer_Vision/HW2/PKLot-small/parking1b/cloudy/2013-02-22/2013-02-22_17_10_11.jpg";
	String file4="/home/aditi/Computer_Vision/HW2/PKLot-small/parking1b/rainy/2013-02-26/2013-02-26_13_04_33.jpg";
	String file5="/home/aditi/Computer_Vision/HW2/PKLot-small/parking2/cloudy/2012-09-12/2012-09-12_07_23_35.jpg";
	String file6="/home/aditi/Computer_Vision/HW2/PKLot-small/parking2/rainy/2012-09-16/2012-09-16_07_57_59.jpg";
	
	String file=file5;
	frame = imread(file);

	cvtColor(frame, gray, CV_RGB2GRAY);
	cout<<"Frame found"<<endl;
	car_cascade.detectMultiScale(gray, cars, 1.1, 10, 0, Size(40,40), Size(60,60));
	cout<<"Detection done"<<endl;
	cout <<"Cars found:"<<cars.size()<<endl;
	for (int i = 0; i < cars.size(); i++)
	{
		rectangle(frame, Point(cars[i].x, cars[i].y), Point(cars[i].x + cars[i].width, cars[i].y + cars[i].height), Scalar(255, 0, 0), 1);

	}
	cout<<"Rectangles drawn"<<endl;
	// imwrite("detect_lbp_1200_1_1.jpg",frame);
	cout<<"Image saved"<<endl;

	//PARKING LOT ANALYSIS
	std::string xml_file = file.operator std::string();
	std::string str1="xml";

	cout<<xml_file<<endl;
	unsigned length=xml_file.length();
	std::string str_new=xml_file.substr(0,length-3);  
	xml_file=str_new.append(str1);
	cout<<xml_file<<endl;
	
	char *cstr = new char[xml_file.length() + 1];
	strcpy(cstr, xml_file.c_str());

	//Loading XML file with error handling
	XMLDocument doc;

	XMLError e = doc.LoadFile(cstr);
	delete[] cstr;
	if (e != XML_SUCCESS)
	{
		cout<<"failed to load file"<<endl;

	}
	else
	{
		//Extracting root node
		XMLNode * pRoot = doc.FirstChildElement("parking");
		if (pRoot == NULL)
		{
			cout << "No root node" << endl;
		}
		//Accessing elements and storing them
		XMLElement * firstSpaceElement = pRoot->FirstChildElement("space");
		XMLElement * lastSpaceElement = pRoot->LastChildElement("space");
		XMLElement * nextSpaceElement = firstSpaceElement->NextSiblingElement("space");
		XMLElement * rotatedRect = firstSpaceElement->FirstChildElement("rotatedRect");
		XMLElement * centerpoints = rotatedRect->FirstChildElement("center");
		XMLElement * size = rotatedRect->FirstChildElement("size");
		XMLElement * angle = rotatedRect->FirstChildElement("angle");
		XMLElement * contour = firstSpaceElement->FirstChildElement("contour");
		XMLElement * point = contour->FirstChildElement("point");
		XMLElement * nextpoint = point->NextSiblingElement("point");

		//Storing necessary data for first space in vectors
		std::vector<int> Spaceid;
		std::vector<Point> Center;
		std::vector<Point> Coordinates;
		std::vector<Size> size1;
		std::vector<float> Angle;
		int x_coord=atoi(centerpoints->Attribute("x"));
		int y_coord=atoi(centerpoints->Attribute("y"));
		Spaceid.push_back(atoi(firstSpaceElement->Attribute("id")));	
		Occupied.push_back(atoi(firstSpaceElement->Attribute("occupied")));
		Center.push_back(Point(x_coord,y_coord));
		float width=atoi(size->Attribute("w"));
		float height=atoi(size->Attribute("h"));
		size1.push_back(Point(width,height));
		Angle.push_back(atoi(angle->Attribute("d")));

		// if(occ)//this might have issues, store in a variable and use
		circle(frame, Point(x_coord,y_coord), 3,Scalar(0,0,255), -1,8,0);

		Coordinates.push_back(Point(atoi(point->Attribute("x")), atoi(point->Attribute("y"))));
		Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
		nextpoint = nextpoint->NextSiblingElement("point");
		Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
		nextpoint = nextpoint->NextSiblingElement("point");
		Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
		vector<Point> gridpoints;
		SpaceNos = atoi(lastSpaceElement->Attribute("id"));
		// cout<<"Space ID: "<<0<<" "<<atoi(firstSpaceElement->Attribute("occupied"))<<endl;


		for (int i = 1; i < SpaceNos; i++)
		{	

			// // occupied_count+=atoi(firstSpaceElement->Attribute("occupied"));
			// minimum_dist=2000;
			// for (int j = 0; j < cars.size(); j++)
			// {
			// 	circle(frame, Point(cars[j].x+0.5*cars[j].width,cars[j].y+0.5*cars[j].height), 3,Scalar(0,255,0), -1,8,0);

			// 	float dist=sqrt(pow((cars[j].x+0.5*cars[j].width-x_coord),2)+pow((cars[j].y+0.5*cars[j].height-y_coord),2));
			// 	float h1=sqrt(pow(cars[j].width,2)+pow(cars[j].height,2));
			// 	float h2=sqrt(pow(width,2)+pow(height,2));
			// 	if((dist<minimum_dist)&&(dist<(h1+h2)))
			// 		{
			// 			minimum_dist=dist;occupied_flag=1; 
			// 			circle(frame, Point(cars[j].x+0.5*cars[j].width,cars[j].y+0.5*cars[j].height), 3,Scalar(0,255,0), -1,8,0);
			// 			// cout<<i<<" "<<dist<<" "<<minimum_dist<<" "<<h1+h2<<endl;
			// 		}
			// 	// if(diff<)
			// // 	//to find overlapping area, divide the detected rectangle in 2x2 size grid, and store the points of intersection
			// // 	for( int x_coord=cars[i].x;x_coord<cars[i].x+cars[i].width;x_coord=x_coord+2)
			// // 	{
			// // 		for( int y_coord=cars[i].y;y_coord<cars[i].y+cars[i].height;y_coord=y_coord+2)
			// // 		{
			// // 			gridpoints.push_back(Point(x_coord,y_coord));
			// // 			//for every point find if it lies in the other rectangle, dist is less than minimum side
			// // 		}		
			// // 	}	
			// }
			// if(occupied_flag)
			// 	occupied_detected++;

			// cout<<occupied_flag<<" "<<atoi(firstSpaceElement->Attribute("occupied"))<<endl;
			// Accessing the next space elements
			rotatedRect = nextSpaceElement->FirstChildElement("rotatedRect");
			centerpoints = rotatedRect->FirstChildElement("center");
			size = rotatedRect->FirstChildElement("size");
			angle = rotatedRect->FirstChildElement("angle");
			contour = nextSpaceElement->FirstChildElement("contour");
			point = contour->FirstChildElement("point");
			nextpoint = point->NextSiblingElement("point");

			//storing necessary data in vectors
			Spaceid.push_back(atoi(nextSpaceElement->Attribute("id")));

			if (nextSpaceElement->Attribute("occupied") != NULL)
				Occupied.push_back(atoi(firstSpaceElement->Attribute("occupied")));
			else
				Occupied.push_back(0);

			int x_coord=atoi(centerpoints->Attribute("x"));
			int y_coord=atoi(centerpoints->Attribute("y"));
			Center.push_back(Point(x_coord,y_coord));
			float width=atoi(size->Attribute("w"));
			float height=atoi(size->Attribute("h"));
			size1.push_back(Point(width,height));			
			Angle.push_back(atoi(angle->Attribute("d")));
			Coordinates.push_back(Point(atoi(point->Attribute("x")), atoi(point->Attribute("y"))));
			Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
			nextpoint = nextpoint->NextSiblingElement("point");
			Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
			nextpoint = nextpoint->NextSiblingElement("point");
			Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
			
			// circle(frame, Point(x_coord,y_coord), 3,Scalar(0,0,255), -1,8,0);
			// cout<<"Space ID: "<<i<<" "<<atoi(firstSpaceElement->Attribute("occupied"))<<endl;

			//loading the next space
			nextSpaceElement = nextSpaceElement->NextSiblingElement("space");
			//clear vectors after use

		}
	}

	int sum_of_elems=0;
	for(vector<int>::iterator it=Occupied.begin();it!=Occupied.end();++it)
		sum_of_elems+=*it;

	cout<<"Sum of occupied: "<<sum_of_elems<<endl;

	cout<<"Spaces :"<<SpaceNos<<" occupied total: "<<occupied_count<<", detected: "<<SpaceNos-occupied_detected<<" ,percentage: "<<100.0*occupied_detected/occupied_count<<endl;

	namedWindow("Frame", WINDOW_NORMAL);
    imshow("Frame",frame);
	waitKey();
	return 0;
}
