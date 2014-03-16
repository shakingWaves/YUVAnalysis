/************************************************************************
 *
 *  Li Xiaobo  shakingWaves@hotmail.com
 *  
 *  May 28, 2013
 *
 ************************************************************************/


#pragma once

//#include "opencv\cv.h"
//#include "opencv\cxcore.h"
//#include "opencv\highgui.h"
using namespace std;
class YUVProcess
{

public:

	LPBYTE /*sobelSrc,*/sobelDst;
	int width,height;
	LPBYTE MDFirst,MDSecond,MDDst;
	bool *flag0,*flag1,*flag2,*flag3;


public:
	YUVProcess(void);
	~YUVProcess(void);
	YUVProcess(int width,int height);

	
	/*LPBYTE  getSrc(){return sobelSrc;}
	void setSrc(LPBYTE src){this->sobelSrc=src;}*/
	LPBYTE  getDst(){return sobelDst;}
	void setDst(LPBYTE dst){this->sobelDst=dst;}

	LPBYTE getMDDst(){return MDDst;}
	void setMDFirst(LPBYTE First){strcpy((char *)MDFirst,(char *)First);}
	void setMDSecond(LPBYTE Second){strcpy((char *)MDSecond,(char *)Second);}
	LPBYTE getMDFirst(){return MDFirst;}
	LPBYTE getMDSecond(){return MDSecond;}
	void allocMemForMD(int width,int height);

	void YUVSobel(LPBYTE src,int xorder, int yorder, int aperture_size=3);
	int otsu (unsigned char *image, int rows, int cols, int x0, int y0, int dx, int dy, int vvv,int * maxValue);
	void CUSplit(int depth,int addr,int row,int col);
	void YUVMovementDetection();

	//IplImage *img;
	
	//void releaseData();


	void allocMemForSobel(int width, int height);
};
