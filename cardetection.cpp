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

	if (!car_cascade.load(car_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -1; };

	String file1="/home/aditi/Computer_Vision/HW2/PKLot-small/parking1a/cloudy/2012-12-12/2012-12-12_10_00_05.jpg";
	String file2="/home/aditi/Computer_Vision/HW2/PKLot-small/parking1a/rainy/2012-12-07/2012-12-07_16_42_25.jpg";
	String file3="/home/aditi/Computer_Vision/HW2/PKLot-small/parking1b/cloudy/2013-02-22/2013-02-22_17_10_11.jpg";
	String file4="/home/aditi/Computer_Vision/HW2/PKLot-small/parking1b/rainy/2013-02-26/2013-02-26_13_04_33.jpg";
	String file5="/home/aditi/Computer_Vision/HW2/PKLot-small/parking2/cloudy/2012-09-12/2012-09-12_07_23_35.jpg";
	String file6="/home/aditi/Computer_Vision/HW2/PKLot-small/parking2/rainy/2012-09-16/2012-09-16_07_57_59.jpg";

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
		std::vector<Point> Center;
		std::vector<Size> size1;
		std::vector<float> Angle;
		Spaceid.push_back(atoi(firstSpaceElement->Attribute("id")));
		Occupied.push_back(atoi(firstSpaceElement->Attribute("occupied")));
		Center.push_back(Point(atoi(centerpoints->Attribute("x")), atoi(centerpoints->Attribute("y"))));
		size1.push_back(Point(atoi(size->Attribute("w")), atoi(size->Attribute("h"))));
		Angle.push_back(atoi(angle->Attribute("d")));
}
	// XMLError eres = doc.loadFile(xml_file);
	// //XMLError eres = doc.LoadFile(argv[1]);
	// XMLNode* root = doc.FirstChildElement("parking");
	// XMLElement* space = root->FirstChildElement("space");
	
	// for (XMLElement* space = root->FirstChildElement("space"); space!=NULL; space = space->NextSiblingElement())
	// {
	// 	XMLElement* rotated = space->FirstChildElement("rotatedRect");
	// 	XMLElement* center = rotated->FirstChildElement("center");
	// 	XMLElement* size = rotated->FirstChildElement("size");
	// 	XMLElement* angle = rotated->FirstChildElement("angle");

	// 	center->QueryFloatAttribute("x", &x);
	// 	center->QueryFloatAttribute("y", &y);
	// 	size->QueryFloatAttribute("w", &w);
	// 	size->QueryFloatAttribute("h", &h);
	// 	angle->QueryFloatAttribute("d", &d);

	// 		//cout << x << "," << y << "," << w << "," << h << "," << d << endl;
		
	// 	/*if (d < -45) {
	// 		d += 90;
	// 		swap(w,h);
	// 	}*/

	// 	RotatedRect rRect = RotatedRect(Point2f(x, y), Size2f(w,h),d);

	waitKey();
	return 0;
}
