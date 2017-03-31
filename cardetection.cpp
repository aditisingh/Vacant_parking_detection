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

	String car_cascade_name = "cascade.xml";
	CascadeClassifier car_cascade;
	Mat frame, gray;
	vector<Rect> cars;

	if (!car_cascade.load(car_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -1; };

	String file1="2012-12-12_10_00_05.jpg";
	String file2="2012-12-07_16_42_25.jpg";
	String file3="2013-02-22_17_10_11.jpg";
	String file4="2013-02-26_13_04_33.jpg";
	String file5="2012-09-12_06_05_16.jpg";
	String file6="2012-09-16_06_22_55.jpg ";

	String file=file1;
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
		std::vector<int> Occupied;
		std::vector<Point> Center, Coordinates, nextpoint;
		std::vector<Size> size1;
		std::vector<float> Angle;

		Spaceid.push_back(atoi(firstSpaceElement->Attribute("id")));
		Occupied.push_back(atoi(firstSpaceElement->Attribute("occupied")));
		Center.push_back(Point(atoi(centerpoints->Attribute("x")), atoi(centerpoints->Attribute("y"))));
		size1.push_back(Point(atoi(size->Attribute("w")), atoi(size->Attribute("h"))));
		Angle.push_back(atoi(angle->Attribute("d")));

		Coordinates.push_back(Point(atoi(point->Attribute("x")), atoi(point->Attribute("y"))));
		Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
		nextpoint = nextpoint->NextSiblingElement("point");
		Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
		nextpoint = nextpoint->NextSiblingElement("point");
		Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
		vector<Point> gridpoints;
		int SpaceNos = atoi(lastSpaceElement->Attribute("id"));
		for (int i = 1; i < SpaceNos; i++)
		{
			for (int i = 0; i < cars.size(); i++)
			{
				float diff=sqrt(pow((cars[i].x-centerpoints.x),2)+pow((cars[i].y-centerpoints.y),2));
				//to find overlapping area, divide the detected rectangle in 2x2 size grid, and store the points of intersection
				for( int x_coord=cars[i].x;x_coord<cars[i].x+cars[i].width;x_coord=x_coord+2)
				{
					for( int y_coord=cars[i].y;y_coord<cars[i].y+cars[i].height;y_coord=y_coord+2)
					{
						gridpoints.push_back(Point(x_coord,y_coord));
						//for every point find if it lies in the other rectangle, dist is less than minimum side
					}		
				}
				
					

			}
			//Accessing the next space elements
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
				Occupied.push_back(atoi(nextSpaceElement->Attribute("occupied")));

			else
				Occupied.push_back(-1);


			Center.push_back(Point(atoi(centerpoints->Attribute("x")), atoi(centerpoints->Attribute("y"))));
			Size.push_back(Point(atoi(size->Attribute("w")), atoi(size->Attribute("h"))));
			Angle.push_back(atoi(angle->Attribute("d")));
			Coordinates.push_back(Point(atoi(point->Attribute("x")), atoi(point->Attribute("y"))));
			Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
			nextpoint = nextpoint->NextSiblingElement("point");
			Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));
			nextpoint = nextpoint->NextSiblingElement("point");
			Coordinates.push_back(Point(atoi(nextpoint->Attribute("x")), atoi(nextpoint->Attribute("y"))));

			//loading the next space
			nextSpaceElement = nextSpaceElement->NextSiblingElement("space");

		}
	}
	waitKey();
	return 0;
}
