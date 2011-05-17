// include what is needed from cisst
//#include <cisstConfig.h>
//#include <cisstCommon.h>
#include <cisstVector.h>
//#include <cisstNumerical.h>

// system includes
#include <iostream>

// to simplify the code
using namespace std;

#include "ccalFileIO.h"

//================== Subclass of ccalFileIO for the points file (DRL .pts) type ==================//
ccalPointsFileIO::ccalPointsFileIO(const char* filename, int fileFormat)
		:ccalFileIO(filename)
{
	this->fileFormat = fileFormat;
	worldToTCP = cvCreateMat(4,4,CV_64F);

	switch(fileFormat)
	{
		case ORIGINAL:
			sections[0] = new SectionFormat(2,"######\n");
			sections[1] = new SectionFormat(7,"##\n");
			sections[2] = new SectionFormat(10,"#\n");
			sections[3] = new SectionFormat(END,"#,#,#\n");
			break;
		case IMPROVED:
			sections[0] = new SectionFormat(3,"######\n");
			sections[1] = new SectionFormat(6,"##\n");
			sections[2] = new SectionFormat(9,"#\n");
			sections[3] = new SectionFormat(END,"#,#,#\n");
			break;
		default:
			printf("Unknown poitns file format: %d\n", fileFormat);
			break;
	}

}

// repack data
void ccalPointsFileIO::repackData(IplImage* iplImage)
{
	int offset, worldToTCPOffset;
    static CvScalar colors[] =
    {
        {{0,0,255}},
        {{0,128,255}},
        {{0,255,255}},
        {{0,255,0}},
        {{255,128,0}},
        {{255,255,0}},
		{{255,0,0}},
        {{255,0,255}},
        {{255,255,255}}
    };

	switch(fileFormat)
	{
		case ORIGINAL:
			pointsCount = data[10][0];
			cv::Size(data[7][0],data[7][1]);
			worldToTCPOffset = 2;
			offset = 13;
			break;
		case IMPROVED:
			pointsCount = data[9][0];
			imageSize = cv::Size(data[6][0],data[6][1]);
			worldToTCPOffset = 3;
			offset = 12;
			break;
		default:
			printf("Unknown points file format: %d\n", fileFormat);
			return;
	}

	//World to TCP
	float worldToTCPRVector[1][3] = {data[worldToTCPOffset][0],data[worldToTCPOffset][1],data[worldToTCPOffset][2]};
	float worldToTCPTVector[1][3] = {data[worldToTCPOffset][3],data[worldToTCPOffset][4],data[worldToTCPOffset][5]};	
	cv::Mat rvect(1,3,CV_64F,worldToTCPRVector);
	cv::Mat rmatrix;
	cv::Rodrigues(rvect,rmatrix);
	float dataMatrix[4][4] = {{rmatrix.at<double>(0,0),rmatrix.at<double>(0,1),rmatrix.at<double>(0,2),worldToTCPTVector[0][0]},
							 {rmatrix.at<double>(1,0),rmatrix.at<double>(1,1),rmatrix.at<double>(1,2),worldToTCPTVector[0][1]},
							 {rmatrix.at<double>(2,0),rmatrix.at<double>(2,1),rmatrix.at<double>(2,2),worldToTCPTVector[0][2]},
							 {0,0,0,1}};
	worldToTCP = cvCreateMat(4,4,CV_64F);
	worldToTCP->data.fl[0] = dataMatrix[0][0];
	worldToTCP->data.fl[1] = dataMatrix[0][1];
	worldToTCP->data.fl[2] = dataMatrix[0][2];
	worldToTCP->data.fl[3] = dataMatrix[0][3];
	worldToTCP->data.fl[4] = dataMatrix[1][0];
	worldToTCP->data.fl[5] = dataMatrix[1][1];
	worldToTCP->data.fl[6] = dataMatrix[1][2];
	worldToTCP->data.fl[7] = dataMatrix[1][3];
	worldToTCP->data.fl[8] = dataMatrix[2][0];
	worldToTCP->data.fl[9] = dataMatrix[2][1];
	worldToTCP->data.fl[10] = dataMatrix[2][2];
	worldToTCP->data.fl[11] = dataMatrix[2][3];
	worldToTCP->data.fl[12] = dataMatrix[3][0];
	worldToTCP->data.fl[13] = dataMatrix[3][1];
	worldToTCP->data.fl[14] = dataMatrix[3][2];
	worldToTCP->data.fl[15] = dataMatrix[3][3];

	//imagePoints;
	//calibrationGridPoints;
	int index;
	for(int i=0;i<pointsCount;i++)
	{
		index = i + offset;
		//calibrationGridPoints.push_back(cv::Point3f(imageSize.width/2+data[index][0]/20*30,imageSize.height/2+data[index][1]/20*30,data[index][2]));
		calibrationGridPoints.push_back(cv::Point3f(data[index][0],data[index][1],data[index][2]));
		imagePoints.push_back(cv::Point2f(data[index][3],data[index][4]));

		if(debug)
		{
			//cvCircle( iplImage, cv::Point2f(imageSize.width/2+data[index][0]/20*30,imageSize.height/2+data[index][1]/20*30), 5, colors[1], 1, 8, 0 );
			cvCircle( iplImage, cv::Point2f(data[index][0],data[index][1]), 5, colors[1], 1, 8, 0 );
			cvCircle( iplImage, cv::Point2f(data[index][3],data[index][4]), 5, colors[6], 1, 8, 0 );
			cout << "repackData at point: " << i << " (" << data[index][0] << ",";
			cout << data[index][1] << ")" << " : (" << data[index][3] << ",";
			cout << data[index][4] << ")" << endl;
		}

	}
	if(debug)
		cout << "read " << pointsCount << " points" <<endl;

	// Free memory
	rvect.~Mat();
	rmatrix.~Mat();
}

// repack data
void ccalPointsFileIO::repackData()
{
	//see overloaded repackData(IplImage* iplImage);
}

void ccalPointsFileIO::showData()
{
	if(debug)
	{
		cout << "pointsCount = " << pointsCount << endl;
		cout << "Image size: " << imageSize.width << "," << imageSize.height << endl;
	}
}


//================== Subclass of ccalFileIO for the DLR cameraCalibration.m type ==================//
ccalDLRCalibrationFileIO::ccalDLRCalibrationFileIO(const char* filename)
		:ccalFileIO(filename)
{

	sections[0] = new SectionFormat(5,"[##\n");
	sections[1] = new SectionFormat(8,"[##\n");
	sections[2] = new SectionFormat(11,"[#\n");
	sections[3] = new SectionFormat(14,"[###\n");
	sections[4] = new SectionFormat(15,"[###\n");
	sections[5] = new SectionFormat(16,"[###\n");
	sections[6] = new SectionFormat(20,"####\n");
	sections[7] = new SectionFormat(END,"#,#,#\n");

}

void ccalDLRCalibrationFileIO::repackData(int numImages)
{
	focalLength = cv::Point2f(data[5][0],data[5][1]);
	principalPoint = cv::Point2f(data[8][0],data[8][1]);
	alpha = data[11][0];
	double distortionParameters[5][1] = {data[14][0],data[14][1],data[14][2], 0.0 , 0.0};
	double tcpToCamera1[3][4] = {{data[20][0], data[20][1],data[20][2], data[20][3]},{data[21][0], data[21][1],data[21][2], data[21][3]},{data[22][0], data[22][1],data[22][2], data[22][3]}};
	distCoeffs = cv::Mat(5,1,CV_64F, distortionParameters);
	tcpToCamera = cv::Mat(3,4,CV_64F, tcpToCamera1);
	this->numImages = numImages;	

	//cameraMatrix;
	cout<<"focal length: " << focalLength.x<<","<<focalLength.y << endl;
	cout<<"pricipal point: " << principalPoint.x<<","<<principalPoint.y << endl;
	cout<<"alpha " << alpha << endl;
	cout<<"distortion coeffs " << distCoeffs.at<double>(0,0) <<","<< distCoeffs.at<double>(1,0) <<","<< distCoeffs.at<double>(2,0) << endl;
	cout<<"data 15: " << data[15][0] <<","<< data[15][1] <<","<< data[15][2] << endl;
	cout<<"data 16: " << data[16][0] <<","<< data[16][1] <<","<< data[16][2] << endl;

	cout<<"tcpToCamera1 " << tcpToCamera.at<double>(0,0) <<","<< tcpToCamera.at<double>(0,1) <<","<< tcpToCamera.at<double>(0,2) <<","<< tcpToCamera.at<double>(0,3)<< endl;
	cout<<"tcpToCamera1 " << tcpToCamera.at<double>(1,0) <<","<< tcpToCamera.at<double>(1,1) <<","<< tcpToCamera.at<double>(1,2) <<","<< tcpToCamera.at<double>(1,3)<< endl;
	cout<<"tcpToCamera1 " << tcpToCamera.at<double>(2,0) <<","<< tcpToCamera.at<double>(2,1) <<","<< tcpToCamera.at<double>(2,2) <<","<< tcpToCamera.at<double>(2,3)<< endl;

	int index = 28;
	for(int i=0;i<numImages;i++)
	{
		float cameraParameters[3][4] = {{data[index][0], data[index][1],data[index][2], data[index][3]},
									 {data[index+1][0], data[index+1][1],data[index+1][2], data[index+1][3]},
									 {data[index+2][0], data[index+2][1],data[index+2][2], data[index+2][3]}};

		CvMat* myCameraMatrix = cvCreateMat(3,4,CV_64F);
		myCameraMatrix->data.fl[0] = cameraParameters[0][0];
		myCameraMatrix->data.fl[1] = cameraParameters[0][1];
		myCameraMatrix->data.fl[2] = cameraParameters[0][2];
		myCameraMatrix->data.fl[3] = cameraParameters[0][3];
		myCameraMatrix->data.fl[4] = cameraParameters[1][0];
		myCameraMatrix->data.fl[5] = cameraParameters[1][1];
		myCameraMatrix->data.fl[6] = cameraParameters[1][2];
		myCameraMatrix->data.fl[7] = cameraParameters[1][3];
		myCameraMatrix->data.fl[8] = cameraParameters[2][0];
		myCameraMatrix->data.fl[9] = cameraParameters[2][1];
		myCameraMatrix->data.fl[10] = cameraParameters[2][2];
		myCameraMatrix->data.fl[11] = cameraParameters[2][3];


		cameraMatrix.push_back(myCameraMatrix);

		index+=5;
	}

	printCameraMatrix();
	
}

void ccalDLRCalibrationFileIO::printCameraMatrix()
{
	for(int i=0;i<numImages;i++)
	{
		CvMat* m = (CvMat*) cameraMatrix[i];
		cout<<"CameraMatrix # " << i << " " << m->data.fl[0]<<","<< m->data.fl[1] <<","<< m->data.fl[2] <<","<< m->data.fl[3]<< endl;
		cout<<"CameraMatrix # " << i << " " << m->data.fl[4]<<","<< m->data.fl[5] <<","<< m->data.fl[6] <<","<< m->data.fl[7]<< endl;
		cout<<"CameraMatrix # " << i << " " << m->data.fl[8]<<","<< m->data.fl[9] <<","<< m->data.fl[10] <<","<< m->data.fl[11]<<endl<<endl;
	}
}

//================== Subclass of ccalFileIO for the tracking .coords type ==================//
ccalTrackerCoordsFileIO::ccalTrackerCoordsFileIO(const char *filename)
		:ccalFileIO(filename)
{
	sections[0] = new SectionFormat(0,"############\n");
	sections[1] = new SectionFormat(END,"#,#,#\n");
}

void ccalTrackerCoordsFileIO::repackData()
{

	worldToTCP = cvCreateMat(4,4,CV_64F);

	worldToTCP->data.fl[0] = data[0][0];
	worldToTCP->data.fl[1] = data[0][1];
	worldToTCP->data.fl[2] = data[0][2];
	worldToTCP->data.fl[3] = data[0][3];
	worldToTCP->data.fl[4] = data[0][4];
	worldToTCP->data.fl[5] = data[0][5];
	worldToTCP->data.fl[6] = data[0][6];
	worldToTCP->data.fl[7] = data[0][7];
	worldToTCP->data.fl[8] = data[0][8];
	worldToTCP->data.fl[9] = data[0][9];
	worldToTCP->data.fl[10] = data[0][10];
	worldToTCP->data.fl[11] = data[0][11];
	worldToTCP->data.fl[12] = 0;
	worldToTCP->data.fl[13] = 0;
	worldToTCP->data.fl[14] = 0;
	worldToTCP->data.fl[15] = 1;
}