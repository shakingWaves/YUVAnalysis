/************************************************************************
 *
 *  Li Xiaobo  shakingWaves@hotmail.com
 *  
 *  May 28, 2013
 *
 ************************************************************************/

#include "stdafx.h"
#include "YUVProcess.h"
#include <cmath>
#include <iostream>
int dy[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
int dx[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};//used by sobel...self defines

using namespace  std;

YUVProcess::YUVProcess(void)
{
}

YUVProcess::~YUVProcess(void)
{
	if(sobelDst)
		free(sobelDst);
	
	if(MDFirst)
		free(MDFirst);
		//delete[] MDFirst;
		//MDFirst=NULL;
	if(MDSecond)
		free(MDSecond);
		//delete[] MDSecond;
		//MDSecond=NULL;
	/*if(MDDst)
		free(MDDst);*/
		//delete[] MDDst;
		MDDst=NULL;
	if(flag0)
		delete [] flag0;
	if(flag1)
		delete [] flag1;
	if(flag2)
		delete [] flag2;
	if(flag3)
		delete [] flag3;
}

YUVProcess::YUVProcess(int width,int height){
	
	this->width=width;
	this->height=height;
	sobelDst=NULL;
	MDDst=NULL;
	MDFirst=NULL;
	MDSecond=NULL;
	int cols=width>>6;
	int rows=height>>6;

	flag0=new bool[cols*rows];
	flag1=new bool[cols*rows*4];
	flag2=new bool[cols*rows*16];
	flag3=new bool[cols*rows*64];

	//img=cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	
	
}

//void YUVProcess::releaseData()
//{
//	if(sobelDst)
//		delete[] sobelDst;
//	if(sobelSrc)
//		delete[] sobelSrc;
//}


void YUVProcess::allocMemForMD(int width,int height)
{
	/*if (NULL== (MDFirst = (unsigned char *)malloc(width*height) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for MDFirst\n");
		return;
	}*/
	if (NULL== (MDSecond = (unsigned char *)malloc(width*height) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for MDSecond\n");
		return;
	}
	if (NULL== (MDDst = (unsigned char *)malloc(width*height) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for MDDst\n");
		return;
	}
	::memset(MDDst,0,width*height);
	//::memset(MDFirst,0,width*height);
	::memset(MDSecond,0,width*height);

}

void YUVProcess::allocMemForSobel(int width, int height)
{
	if (NULL== (sobelDst = (unsigned char *)malloc(width*height) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for sobelDst\n");
		return;
	}
}

void YUVProcess::YUVSobel(LPBYTE src,int xorder, int yorder, int aperture_size)
{
	int s;
	for (int i=1; i < height-1; i++)
				for (int j=1; j < width-1; j++)
				{
						// apply kernel in X ans Y direction
						int sum_x=0;
						int sum_y=0;
						for(int m=-1; m<=1; m++)
							for(int n=-1; n<=1; n++)
							{
								s=src[(i+m)*width+j+n];
								sum_x+=s*dx[m+1][n+1];
								sum_y+=s*dy[m+1][n+1];
							}
							sobelDst[i*width+j]=abs(sum_x)>abs(sum_y)?(abs(sum_x)>255?255:abs(sum_x)):(abs(sum_y)>255?255:abs(sum_y));
														
				}



}

/*
OTSU 算法可以说是自适应计算单阈值（用来转换灰度图像为二值图像）的简单高效方法。下面的代码最早由 Ryan Dibble提供，此后经过多人Joerg.Schulenburg, R.Z.Liu 等修改，补正。

算法对输入的灰度图像的直方图进行分析，将直方图分成两个部分，使得两部分之间的距离最大。划分点就是求得的阈值。

parameter: *image --- buffer for image
rows, cols --- size of image
x0, y0, dx, dy --- region of vector used for computing threshold
vvv --- debug option, is 0, no debug information outputed
*/
/*======================================================================*/
/* OTSU global thresholding routine */
/* takes a 2D unsigned char array pointer, number of rows, and */
/* number of cols in the array. returns the value of the threshold */
/*======================================================================*/
int YUVProcess::otsu (unsigned char *image, int rows, int cols, int x0, int y0, int dx, int dy, int vvv,int * maxValue)
{

	unsigned char *np; // 图像指针
	int thresholdValue=1; // 阈值
	int ihist[256]; // 图像直方图，256个点

	int i, j, k; // various counters
	int n, n1, n2, gmin, gmax;
	double m1, m2, sum, csum, fmax, sb;

	// 对直方图置零...
	memset(ihist, 0, sizeof(ihist));

	gmin=255; gmax=0;
	// 生成直方图
	for (i = y0; i < y0 + dy ; i++) {
		np = &image[i*cols+x0];
		for (j = x0 ; j < x0 + dx ; j++) {
			ihist[*np]++;
			if(*np > gmax) gmax=*np;
			if(*np < gmin) gmin=*np;
			np++; /* next pixel */
		}
	}

	// set up everything
	sum = csum = 0.0;
	n = 0;

	for (k = 0; k <= 255; k++) {
		sum += (double) k * (double) ihist[k]; /* x*f(x) 质量矩*/ 
		n += ihist[k]; /* f(x) 质量 */
	}

	if (!n) {
		// if n has no value, there is problems...
		fprintf (stderr, "NOT NORMAL thresholdValue = 160/n");
		return (160);
	}

	// do the otsu global thresholding method
	fmax = -1.0;
	n1 = 0;
	for (k = 0; k < 255; k++) {
		n1 += ihist[k];//n1 背景图像的像素值个数
		if (!n1) { continue; }
		n2 = n - n1;//n2背景图像的像素个数，n为总像素数
		if (n2 == 0) { break; }
		csum += (double) k *ihist[k];//csum 背景的像素值之和
		m1 = csum / n1;//m1灰度均值
		m2 = (sum - csum) / n2;//灰度均值
		sb = (double) n1 *(double) n2 *(m1 - m2) * (m1 - m2);//变形的类间方差
		/* bbg: note: can be optimized. */
		if (sb > fmax) {
			fmax = sb;
			thresholdValue = k;	

		}
	}
	// at this point we have our thresholding value

	// debug code to display thresholding values
	if ( vvv & 1 )
		fprintf(stderr,"# OTSU: thresholdValue = %d gmin=%d gmax=%d\n",
		thresholdValue, gmin, gmax);

	*maxValue =gmax;

	return(thresholdValue);
}

void YUVProcess::CUSplit(int depth,int addr,int row,int col)
{
	if(depth<4){
		int CUAdrr[4]={0};
		int size=64>>depth;
		
		bool flags=false;
		for(int i=0;i<4;i++){
			int subAddr=addr+(i/2)*width*size+(i%2)*size;
			CUAdrr[i]=subAddr;
			int sum=0;		

			for(int m=0;m<size;++m)
				for(int n=0;n<size;++n){
					sum+=MDDst[subAddr+m*width+n];					
				}//获取四叉树的像素值

			if(sum>0){
				flags=true;
				CUSplit(depth+1,subAddr,2*row+i/2,2*col+i%2);	

				if(depth==1)
					flag1[(2*row+i/2)*(width/64)*2+2*col+i%2]=true;
				else if (depth==2)
					flag2[(2*row+i/2)*(width/64)*4+2*col+i%2]=true;
				else if(depth==3)
					flag3[(2*row+i/2)*(width/64)*8+2*col+i%2]=true;				
			}						
		}
		
		if(flags){
				//for (int k=0;k<size;++k){
				//	MDDst[CUAdrr[p]+k]=255;//上
				//	MDDst[CUAdrr[p]+(size-1)*width+k]=255;//下							
				//	MDDst[CUAdrr[p]+k*width]=255;//左
				//	MDDst[CUAdrr[p]+k*width+size-1]=255;//右	
				//}	

				if(depth==1)
					//flag1[(2*row+p/2)*(width/64*2)+2*col+p%2]=true;
					flag0[row*width/64+col]=true;
				//else if (depth==2)
				//	//flag2[(2*row+p/2)*(width/64*4)+2*col+p%2]=true;
				//	flag1[row*(width/64)*2+col]=true;
				//else if(depth==3)
				//	//flag3[(2*row+p/2)*(width/64*8)+2*col+p%2]=true;
				//	flag2[row*(width/64)*4+col]=true;
				//else
				//	flag3[row*(width/64)*8+col]=true;
				

			}else{
				if(depth==1)
					flag0[row*width/64+col]=false;
				/*else if(depth==2)
					flag1[row*(width/64)*2+col]=false;
				else if(depth==3)
					flag2[row*(width/64)*4+col]=false;
				else
					flag3[row*(width/64)*8+col]=false;*/

				/*int CUSize=64>>(depth-1);
				for(int q=0;q<CUSize;++q){
					MDDst[addr+q]=255;
					MDDst[addr+(CUSize -1)*width+q]=255;
					MDDst[addr+q*width]=255;
					MDDst[addr+q*width+CUSize-1]=255;

				}*/

			}
		}

}




void YUVProcess::YUVMovementDetection()
{
	int threshold=0;
	int gmax=255;
	int k=0;
	int binarySize=2;
	int boundarySize=8;
	

			int s;
			for (int i=1; i < height-1; i++)
				for (int j=1; j < width-1; j++)
				{
					//TRACE("%d ",MDSecond[i*width+j]);
						// apply kernel in X ans Y direction
						int sum_x=0;
						int sum_y=0;
						for(int m=-1; m<=1; m++)
							for(int n=-1; n<=1; n++)
							{
								s=MDSecond[(i+m)*width+j+n];
								sum_x+=s*dx[m+1][n+1];
								sum_y+=s*dy[m+1][n+1];
							}
							MDDst[i*width+j]=abs(sum_x)>abs(sum_y)?(abs(sum_x)>255?255:abs(sum_x)):(abs(sum_y)>255?255:abs(sum_y));
							//MDDst[i*width+j]=abs(sum_x);	
							//MDDst[i*width+j]=sqrt(sum_x*sum_x*1.0+sum_y*sum_y*1.0);

							
				}



	//for(int i=0;i<secondFrame->height;i++)
	//	for(int j=0;j<secondFrame->width;j++){
	//		MDDst[k++]=data[i*step+j];
	//		//TRACE("%d\t",MDDst[k-1]);
	//	}

		/*int d=0;

		for(int a=0;a<height;a++)
			for(int  b=0;b<width;b++){
				MDDst[d++]=abs((uchar)MDFirst[a*width+b]-(uchar)MDSecond[a*width+b]);
			
			}*/
	
	//cvSobel(secondFrame,resultFrame,1,1);
	//		uchar* result=(uchar *)resultFrame->imageData;
	//		int step = resultFrame->widthStep/sizeof(uchar);

	//		int z=0;
	//		for(int i=0;i<resultFrame->height;i++)
	//			for(int j=0;j<resultFrame->width;j++)
	//				MDDst[z++]=result[i*step+j];

				for(int i=0;i<width;++i){
					MDDst[i]=0;
					//MDDst[i+width]=0;
					//MDDst[i+(height-2)*width]=0;
					MDDst[i+(height-1)*width]=0;
				}
				for(int i=0;i<height;++i){
					MDDst[i*width]=0;
					//MDDst[i*width+1]=0;
					//MDDst[i*width+width-2]=0;
					MDDst[i*width+width-1]=0;
				}
	threshold=otsu ((unsigned char *)MDDst,height,width, 0, 0, width, height, 1,&gmax);//大津法求最优阈值
	//cvThreshold(resultFrame,resultFrame,threshold,255,CV_THRESH_BINARY);
	//uchar* resultt=(uchar *)resultFrame->imageData;
	//int sttep = resultFrame->widthStep/sizeof(uchar);

	//int zz=0;
	//for(int i=0;i<resultFrame->height;i++)
	//	for(int j=0;j<resultFrame->width;j++)
	//		MDDst[zz++]=resultt[i*sttep+j];


	for(int i=0;i<height;i++)
		for(int j=0;j<width;j++)
			if(MDDst[i*width+j]<threshold){
				MDDst[i*width+j]=0;
			}else{
				//MDDst[i*width+j]=255;
			}

	

    int rows=(height/64);
	int cols=(width/64);
	memset(flag0,false,cols*rows);
	memset(flag1,false,cols*rows*4);
	memset(flag2,false,cols*rows*16);
	memset(flag3,false,cols*rows*64);

	for(int a=0;a<rows;a++)
		for(int b=0;b<cols;b++){				
			CUSplit(1,a*width*64+b*64,a,b);			
		}

		for(int a=0;a<rows;++a)//画64CU的边界线
			for(int b=0;b<cols;++b){
					for(int q=0;q<64;++q){
						MDDst[a*width*64+b*64+q]=255;
						MDDst[a*width*64+b*64+(64 -1)*width+q]=255;
						MDDst[a*width*64+b*64+q*width]=255;
						MDDst[a*width*64+b*64+q*width+64-1]=255;
					}		
	
			}


			for(int a=0;a<rows;++a)
				for(int b=0;b<cols;++b){
					
					if(flag0[a*cols+b]){

						for(int i=0;i<4;++i){
							for(int q=0;q<32;++q){
								MDDst[(2*a+i/2)*width*32+(2*b+i%2)*32+q]=255;
								MDDst[(2*a+i/2)*width*32+(2*b+i%2)*32+31*width+q]=255;
								MDDst[(2*a+i/2)*width*32+(2*b+i%2)*32+q*width]=255;
								MDDst[(2*a+i/2)*width*32+(2*b+i%2)*32+q*width+31]=255;
							}
						}
					}

				}

				for(int a=0;a<rows*2;++a)
					for(int b=0;b<cols*2;++b){
						
						if(flag1[a*cols*2+b]){

							for(int i=0;i<4;++i){
								for(int q=0;q<16;++q){
									MDDst[(2*a+i/2)*width*16+(2*b+i%2)*16+q]=255;
									MDDst[(2*a+i/2)*width*16+(2*b+i%2)*16+15*width+q]=255;
									MDDst[(2*a+i/2)*width*16+(2*b+i%2)*16+q*width]=255;
									MDDst[(2*a+i/2)*width*16+(2*b+i%2)*16+q*width+15]=255;
								}
							}
						}

					}

		for(int a=0;a<rows*4;++a)
			for(int b=0;b<cols*4;++b){
				
				if(flag2[a*cols*4+b]){
					
					for(int i=0;i<4;++i){
						for(int q=0;q<8;++q){
							MDDst[(2*a+i/2)*width*8+(2*b+i%2)*8+q]=255;
							MDDst[(2*a+i/2)*width*8+(2*b+i%2)*8+7*width+q]=255;
							MDDst[(2*a+i/2)*width*8+(2*b+i%2)*8+q*width]=255;
							MDDst[(2*a+i/2)*width*8+(2*b+i%2)*8+q*width+7]=255;
						}
					}
				}
			}

			for(int a=0;a<rows*8;++a)
				for(int b=0;b<cols*8;++b){
					
					if(flag3[a*cols*8+b]){

						/*for(int q=0;q<8;++q){
							for(int m=0;m<8;++m){
								MDDst[a*width*8+b*8+q*width+m]=255;
							}
						}*/

						for(int i=0;i<4;++i){
						for(int q=0;q<4;++q){
							MDDst[(2*a+i/2)*width*4+(2*b+i%2)*4+q]=255;
							MDDst[(2*a+i/2)*width*4+(2*b+i%2)*4+3*width+q]=255;
							MDDst[(2*a+i/2)*width*4+(2*b+i%2)*4+q*width]=255;
							MDDst[(2*a+i/2)*width*4+(2*b+i%2)*4+q*width+3]=255;
						}
					}
						
					}
				}


	//运动区域
	//int sum=0;
	//for(int m=0;m<height/binarySize;m++){
	//	for(int n=0;n<width/binarySize;n++){

	//		for(int p=0,t=0;/*(p<width*height)&&*/(t<binarySize);p+=width,t++){
	//			for(int q=0;q<binarySize;q++){
	//				sum+=MDDst[m*width*binarySize+n*binarySize+p+q];	
	//				//cout<<m*width+n*64+p+q<<"\t";
	//			}
	//			//cout<<endl;
	//		}
	//		sum/=(binarySize*binarySize);
	//	
	//		if(sum<threshold){

	//			for(int p=0,t=0;/*(p<width*height)&&*/(t<binarySize);p+=width,t++){
	//				for(int q=0;q<binarySize;q++){
	//					MDDst[m*width*binarySize+n*binarySize+p+q]=0;	

	//				}
	//			}

	//		}

	//	}
	//}

	//for(int m=0;m<height/boundarySize;m++){
	//	for(int n=0;n<width/boundarySize;n++){

	//		int sum2=0;

	//		for(int p=0,t=0;/*(p<width*height)&&*/(t<boundarySize);p+=width,t++){
	//			for(int q=0;q<boundarySize;q++){
	//				sum2+=MDDst[m*width*boundarySize+n*boundarySize+p+q];	
	//				//cout<<m*width+n*64+p+q<<"\t";
	//			}
	//			//cout<<endl;
	//		}

	//		if(sum2>0){

	//			for(int p=0,t=0;/*(p<width*height)&&*/(t<boundarySize);p+=width,t++){
	//				for(int q=0;q<boundarySize;q++){
	//					MDDst[m*width*boundarySize+n*boundarySize+p+q]=255;	

	//				}
	//			}
	//		}					
	//	}
	//}





//运动对象
	

	//cvMorphologyEx(    
	//	resultFrame,    
	//	resultFrame,    
	//	NULL,    
	//	NULL, //default 3*3    
	//	CV_MOP_CLOSE ,    
	//	1);   

	//cvMorphologyEx(    
	//	resultFrame,    
	//	resultFrame,    
	//	NULL,    
	//	NULL, //default 3*3    
	//	CV_MOP_OPEN ,    
	//	1);





	

	/*cvReleaseImage(&firstFrame);
	cvReleaseImage(&secondFrame);*/
	/*firstFrame=NULL;
	secondFrame=NULL;
	resultFrame=NULL;*/
	
}
