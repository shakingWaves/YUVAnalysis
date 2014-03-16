/************************************************************************
 *
 *  Li Xiaobo  shakingWaves@hotmail.com
 *  
 *  May 25, 2013
 *
 ************************************************************************/

/*
 * Disclaimer of Warranty
 *
 * These software programs are available to the user without any
 * license fee or royalty on an "as is" basis.  The developers disclaim 
 * any and all warranties, whether express, implied, or statuary, including 
 * any implied warranties or merchantability or of fitness for a particular 
 * purpose.  In no event shall the copyright-holder be liable for any incidental,
 * punitive, or consequential damages of any kind whatsoever arising from 
 * the use of these programs.
 *
 * This disclaimer of warranty extends to the user of these programs
 * and user's customers, employees, agents, transferees, successors,
 * and assigns.
 *
 * The developers does not represent or warrant that the programs furnished 
 * hereunder are free of infringement of any third-party patents.
 *
 * */


// ChildWindow.cpp : implementation file
//

#include "stdafx.h"
#include "YUVviewer.h"
#include "ChildWindow.h"
#include "YUVviewerDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildWindow

IMPLEMENT_DYNCREATE(CChildWindow, CFrameWnd)

int CChildWindow::m_iCount=-1;//静态成员变量赋初值
//int CChildWindow::m_iTotalCount=-1;

CChildWindow::CChildWindow()
{
}
CChildWindow::~CChildWindow()
{
	GlobalUnlock(hloc);
	GlobalFree(hloc);  
	/*GlobalUnlock(hlocSobel);
	GlobalFree(hlocSobel);*/
	if(this->getBColorImage()!=0){
		free(RGBbuf);
		free(Y);
		free(Cb);
		free(Cr);
	}else{
		free(Y);
	}
	this->m_cYUVProcess->~YUVProcess();
	this->m_cYUVProcess=NULL;
	for(int i=0;i<4;i++){
		this->m_cYUVProcessWnd[i]=NULL;
		
	}
}

CChildWindow::CChildWindow(CChildWindow  * childWnd)
{	
	setHeight(childWnd->getHeight());
	setWidth(childWnd->getWidth());
	setZoom(childWnd->getZoom());
	setCurrFrame(childWnd->getCurrFrame());
	setFrameSize(childWnd->getFrameSize());
	setFrameRate(childWnd->getFrameRate());
	strcpy(m_aSeqName,childWnd->getSeqName());
	setTotalFrames(childWnd->getTotalFrames());
	
	m_pFile=NULL;
	Cb=NULL;
	Cr=NULL;
	//Y=childWnd->Y;
	
	if (NULL== (Y = (unsigned char *)malloc(getFrameSize()) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for Y\n");
		return;
	}
	/*if (NULL== (RGBbuf = (unsigned char *)malloc(getFrameSize()*3) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}*/

	strcpy((char *)Y,(char *)childWnd->Y);
	setBColorImage(0);
	//m_cYUVProcess= new YUVProcess(Y,getWidth(),getHeight());//initialize YUVProcess

	DWORD AttrStyle;

	hloc/*Sobel*/ = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
		sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
	BmpInfo = (LPBITMAPINFO) GlobalLock(hloc/*Sobel*/);


	AttrStyle = //WS_OVERLAPPEDWINDOW;
		WS_OVERLAPPED|WS_CAPTION|WS_MINIMIZEBOX;
	Create(NULL,NULL,AttrStyle,rectDefault,childWnd->GetParent());


}

void CChildWindow::setYUVProcessWnd(int type,CYUVProcessWnd * yuvProcessWnd)//not used just now...
{
	m_cYUVProcessWnd[type]=yuvProcessWnd;
	/*switch(type){
		case WT_SOBEL:
			m_cYUVProcessWnd[type]->setUsedInfo("SOBEL");
			break;
	
	}*/
}




CChildWindow::CChildWindow( CFrameWnd *pParentWnd,CFile* file,char * seqName)
{    
	
	switch(((CYUVviewerDlg *)pParentWnd)->m_cCOMBOresolution.GetCurSel()){//获取视频的分辨率
		case 0:
			setHeight(1600);
			setWidth(2560);
			break;
		case 1:
			setHeight(1080);
			setWidth(1920);
			break;
		case 2:
			setHeight(480);
			setWidth(832);	
			break;
		case 3:
			setHeight(240);
			setWidth(416);	
			break;
		case 4:
			setHeight(720);
			setWidth(1280);	
			break;
		case 5:
			setHeight(768);
			setWidth(1024);	
			break;
		case 6:
			setHeight(240);
			setWidth(320);
			break;
		case 7:
			setHeight(480);
			setWidth(640);
			break;
	}

	switch(((CYUVviewerDlg *)pParentWnd)->m_cCOMBOResize.GetCurSel()){//获取视频的缩放比例

		case 0:
			setZoom(0.25);
			break;
		case 1:
			setZoom(0.5);
			break;
		case 2:
			setZoom(1.0);
			break;
		case 3:
			setZoom(2.0);
			break;
		case 4:
			setZoom(4.0);
			break;
	}	

	setCurrFrame(1);
	setFrameSize(getHeight()*getWidth());
	setBSobel(false);
	setBMovementDetection(false);
	CString  cstr;
	((CEdit*)(AfxGetMainWnd()->GetDlgItem(IDC_EDITFrameRate)))->GetWindowText(cstr);
	char str[6];
	sprintf(str,cstr);
	setFrameRate(atoi(str));
	//m_cYUVProcess=NULL;
	m_pFile=file;
	strcpy(m_aSeqName,seqName);

	m_cYUVProcess= new YUVProcess(getWidth(),getHeight());//initialize YUVProcess

	//获取总的帧数
		totalFrames=(int)(2.0/3*m_pFile->GetLength()/getFrameSize());
	
	if (NULL== (Y = (unsigned char *)malloc(getFrameSize()) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	if (NULL== (Cb = (unsigned char *)malloc(getFrameSize()/4) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	if (NULL== (Cr = (unsigned char *)malloc(getFrameSize()/4) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	if (NULL== (RGBbuf = (unsigned char *)malloc(getFrameSize()*3) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}
	
	DWORD AttrStyle;
    
	hloc = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
   		sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
	BmpInfo = (LPBITMAPINFO) GlobalLock(hloc);

	
	AttrStyle = //WS_OVERLAPPEDWINDOW;
		WS_OVERLAPPED|WS_CAPTION|WS_MINIMIZEBOX/*|WS_SYSMENU*/;
    Create(NULL,NULL,AttrStyle,rectDefault,pParentWnd);
}

void CChildWindow::ShowGrayImage(CDC *pDC,BYTE *lpImage)
{
	int i;         
	int nNum1,nNum2;
	HANDLE hMem;
	BYTE *lpBuf;

	BmpInfo->bmiHeader.biBitCount = 8;

    hMem=GlobalAlloc(GHND,iWidth*iHeight);
	lpBuf=(BYTE *)GlobalLock(hMem);
	
    //将倒置的图象放正 
	//Make the inverse image normal
    for(i=0;i<iHeight;i++){
		nNum1=(iHeight-i-1)*iWidth;
		nNum2=i*iWidth;
		memcpy(lpBuf+nNum1,lpImage+nNum2,iWidth);
	}

	pDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pDC->m_hDC,0,0,(int)(m_nZoom*iWidth),(int)(m_nZoom*iHeight),
		0,0,iWidth,iHeight,
		lpBuf, BmpInfo, DIB_RGB_COLORS,SRCCOPY);

	GlobalUnlock(hMem);
	GlobalFree(hMem);


	//IplImage *dstImg2=  cvCreateImage(cvSize(iWidth,iHeight), IPL_DEPTH_8U, 1);
	//cvSetData(dstImg2, (uchar *)(lpImage), 832);
	//cvNamedWindow("test",1);
	//cvShowImage("test",dstImg2);

}

void CChildWindow::ShowImage(CDC *pDC,BYTE *lpImage)
{
	BmpInfo->bmiHeader.biBitCount = 24;

	pDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pDC->m_hDC,0,0,(int)(m_nZoom*iWidth),(int)(m_nZoom*iHeight),
							 0,0,iWidth,iHeight,
					  lpImage, BmpInfo, DIB_RGB_COLORS,SRCCOPY);
}

void CChildWindow::CenterWindow(int width,int height)
{
    RECT rc;
    RECT rw;
    
    int cyBorder, cxBorder;
    int cyTotal,cxTotal;
    int cyMenuAndCaption;

    int cw, ch;

  	RECT r;
	  int nCx=GetSystemMetrics(SM_CXSCREEN),nCy=GetSystemMetrics(SM_CYSCREEN);

    cyBorder = GetSystemMetrics(SM_CYBORDER);
    cxBorder = GetSystemMetrics(SM_CXBORDER);

    // Figure out the height of the menu, toolbar, and caption
    GetWindowRect(&rw);
    GetClientRect(&rc);

    ClientToScreen ((LPPOINT) &rc);
    cyMenuAndCaption = (rc.top - rw.top) ;
  
    cyTotal =height  +
              cyMenuAndCaption +
              cyBorder * 2 ;
    cxTotal=width+cxBorder * 2;            

	r.left=(nCx-cxTotal)/2;r.top=(nCy-cyTotal)/2;
	r.right=(nCx+cxTotal)/2;r.bottom=(nCy+cyTotal)/2;
	
	MoveWindow(&r);//,FALSE);

  GetClientRect(&rc);
  while( (rc.right-rc.left != width-1) || (rc.bottom-rc.top != height-1) )
  {
  
    if(rc.right-rc.left < width-1)
      cxTotal ++;
    else if(rc.right-rc.left > width-1)
      cxTotal --;
    if(rc.bottom-rc.top < height-1)
      cyTotal ++;
    else if(rc.bottom-rc.top > height-1)
      cyTotal --;

	  r.left=(nCx-cxTotal)/2;r.top=(nCy-cyTotal)/2;
	  r.right=(nCx+cxTotal)/2;r.bottom=(nCy+cyTotal)/2;
	
    cw = rc.right-rc.left+1;
    ch = rc.bottom-rc.top+1;

    MoveWindow(&r);//,FALSE);
    GetClientRect(&rc);

    if( (cw == rc.right-rc.left+1) && (ch == rc.bottom-rc.top+1) )  // client size unchanged
      break;
  }
}

void CChildWindow::readYUVData(UINT picsize)//读取YUV数据作为前进后退操作的子函数
{
	if(picsize != m_pFile->Read(Y,picsize))
	{
		//MessageBox("Get to end of file");
		return;
	}
	if(1)//bColorImage) 
	{
		if(picsize/4 != m_pFile->Read(Cb,picsize/4))
		{
			//MessageBox("Get to end of file");
			return;
		}
		if(picsize/4 !=m_pFile->Read(Cr,picsize/4))
		{
			//MessageBox("Get to end of file");
			return;
		}
	}	
}

void CChildWindow::refreshChildWnd(int frameNum)//和readYUVData联合使用，大多数情况下
{
	setCurrFrame(frameNum);
	InvalidateRect (NULL,FALSE);
	UpdateWindow ();
}


BEGIN_MESSAGE_MAP(CChildWindow, CFrameWnd)
	//{{AFX_MSG_MAP(CChildWindow)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_KEYDOWN()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildWindow message handlers

void CChildWindow::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if(!Y || !RGBbuf) return;
	
	char msg[128];
	wsprintf(msg,"%d/%d - %s",getCurrFrame(),getTotalFrames(), getSeqName());
	SetWindowText(msg);
	
	if(bColorImage){    //彩色	//Colorful
		conv.YV12_to_RGB24(Y,Cb,Cr,RGBbuf,iWidth,iHeight);
		ShowImage(&dc,RGBbuf);
	}
	else {                    //黑白	//Monochrome
		ShowGrayImage(&dc,Y);
	} 

	// Do not call CFrameWnd::OnPaint() for painting messages
}

int CChildWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
    int i;
	HANDLE hloc1;
	RGBQUAD *argbq;

	hloc1 = LocalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,(sizeof(RGBQUAD) * 256));
	argbq = (RGBQUAD *) LocalLock(hloc1);

	for(i=0;i<256;i++) {
		argbq[i].rgbBlue=i;
		argbq[i].rgbGreen=i;
		argbq[i].rgbRed=i;
		argbq[i].rgbReserved=0;
	}

	BmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	BmpInfo->bmiHeader.biPlanes = 1;
	if(bColorImage) 
		BmpInfo->bmiHeader.biBitCount = 24;
	else BmpInfo->bmiHeader.biBitCount = 8;
	BmpInfo->bmiHeader.biCompression = BI_RGB;
	BmpInfo->bmiHeader.biWidth = iWidth;
	BmpInfo->bmiHeader.biHeight = iHeight;

	memcpy(BmpInfo->bmiColors, argbq, sizeof(RGBQUAD) * 256);

	LocalUnlock(hloc1);
	LocalFree(hloc1);

	return 0;
}

void CChildWindow::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	
	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
		switch(nChar){
			case VK_ESCAPE:		
				//if(this->getBColorImage()!=0){
					this->SendMessage(WM_CLOSE,0,0);
				//}//else{
				//	this->DestroyWindow();
				//	//this->m_cYUVProcess=NULL;
				//}
				break;
		}
	
}

void CChildWindow::OnClose()
{	
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//if(this->getBColorImage()!=0){
		m_iCount--;
		if(this->m_pFile)
			this->m_pFile->Close();	

		if(m_iCount<0){	
			AfxGetMainWnd()->GetDlgItem(IDC_NEXT)->EnableWindow(false);	
			AfxGetMainWnd()->GetDlgItem(IDC_NEXT5)->EnableWindow(false);
			AfxGetMainWnd()->GetDlgItem(IDC_PREVIOUS)->EnableWindow(false);
			AfxGetMainWnd()->GetDlgItem(IDC_PREVIOUS5)->EnableWindow(false);
			AfxGetMainWnd()->GetDlgItem(IDC_PAUSEPLAY)->EnableWindow(false);
			AfxGetMainWnd()->GetDlgItem(IDC_REPLAY)->EnableWindow(false);
			AfxGetMainWnd()->GetDlgItem(IDC_CLOSEALL)->EnableWindow(false);
			AfxGetMainWnd()->GetDlgItem(IDC_ORDER)->EnableWindow(false);
			AfxGetMainWnd()->GetDlgItem(IDC_BUTTONSobel)->EnableWindow(false);
		}
	//}
	CFrameWnd::OnClose();	
}
