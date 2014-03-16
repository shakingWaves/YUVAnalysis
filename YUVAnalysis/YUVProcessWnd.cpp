// YUVProcessWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "YUVviewer.h"
#include "YUVProcessWnd.h"
#include "ChildWindow.h"


// CYUVProcessWnd

IMPLEMENT_DYNCREATE(CYUVProcessWnd, CFrameWnd)

CYUVProcessWnd::CYUVProcessWnd()
{

}

CYUVProcessWnd::CYUVProcessWnd(CFrameWnd * childWindow)
{
	CChildWindow *childWnd=(CChildWindow*)childWindow;
	setHeight(childWnd->getHeight());
	setWidth(childWnd->getWidth());
	setZoom(childWnd->getZoom());
	setCurrFrame(childWnd->getCurrFrame());
	strcpy(m_aSeqName,childWnd->getSeqName());
	setTotalFrames(childWnd->getTotalFrames());
	setBColorImage(false);

	if (NULL== (Y = (unsigned char *)malloc(getHeight()*getWidth()) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for Y\n");
		return;
	}
	strcpy((char *)Y,(char *)childWnd->Y);

	/*if (NULL== (RGBbuf = (unsigned char *)malloc(getHeight()*getWidth()*3) ) ) 
	{
		AfxMessageBox("Couldn't allocate memory for RGBbuf\n");
		return;
	}*///��ʱδʹ��

	//m_cYUVProcess= new YUVProcess(Y,getWidth(),getHeight());//initialize YUVProcess


	hloc/*Sobel*/ = GlobalAlloc(GMEM_ZEROINIT | GMEM_MOVEABLE,
		sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
	BmpInfo = (LPBITMAPINFO) GlobalLock(hloc/*Sobel*/);

	DWORD AttrStyle = //WS_OVERLAPPEDWINDOW;
		WS_OVERLAPPED|WS_CAPTION|WS_MINIMIZEBOX/*|WS_SYSMENU*/;
	Create(NULL,NULL,AttrStyle,CRect(0,0,getZoom()*getWidth(),getZoom()*getHeight()),childWnd);
	
}

CYUVProcessWnd::~CYUVProcessWnd()
{
	/*Y=NULL;
	RGBbuf=NULL;*/
	if(Y)
		free(Y);
	/*if(RGBbuf)
		free(RGBbuf);*/
	RGBbuf=NULL;
	DestroyWindow();
}


BEGIN_MESSAGE_MAP(CYUVProcessWnd, CFrameWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//ON_WM_KEYDOWN()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CYUVProcessWnd ��Ϣ�������


void CYUVProcessWnd::ShowGrayImage(CDC *pDC,BYTE *lpImage){

	int i;         
	int nNum1,nNum2;
	//HANDLE hMem;
	//BYTE *lpBuf;
	BYTE *lpBuf=(BYTE *)malloc(iHeight*iWidth);

	BmpInfo->bmiHeader.biBitCount = 8;

	//hMem=GlobalAlloc(GHND,iWidth*iHeight);
	//lpBuf=(BYTE *)GlobalLock(hMem);

	//�����õ�ͼ����� 
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

	//GlobalUnlock(hMem);
	//GlobalFree(hMem);
	free(lpBuf);

}

void CYUVProcessWnd::ShowImage(CDC *pDC,BYTE *lpImage)
{
	BmpInfo->bmiHeader.biBitCount = 24;

	pDC->SetStretchBltMode(STRETCH_DELETESCANS);
	StretchDIBits(pDC->m_hDC,0,0,(int)(m_nZoom*iWidth),(int)(m_nZoom*iHeight),
		0,0,iWidth,iHeight,
		lpImage, BmpInfo, DIB_RGB_COLORS,SRCCOPY);
}


void CYUVProcessWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CFrameWnd::OnPaint()

	char msg[128];
	wsprintf(msg,"%d/%d - %s",getCurrFrame(),getTotalFrames(), getSeqName());
	SetWindowText(msg);
	if(bColorImage){
		ShowImage(&dc,RGBbuf);
	}else{
                   //�ڰ�	//Monochrome
		ShowGrayImage(&dc,Y);
	}
	
}

int CYUVProcessWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

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


void CYUVProcessWnd::Sobel(LPBYTE dst,int currentFrame,int xorder, int yorder, int aperture_size)
{
	//m_cYUVProcess->YUVSobel(src,xorder,yorder,aperture_size);
	//Y=m_cYUVProcess->getDst();
	Y=dst;
	setCurrFrame(currentFrame);
	InvalidateRect(NULL,FALSE);
	UpdateWindow ();
}
//void CYUVProcessWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//
//	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
//	switch(nChar){
//			case VK_ESCAPE:		
//				this->SendMessage(WM_CLOSE,0,0);
//				
//			
//				break;
//	}
//
//	
//}

void CYUVProcessWnd::MovementDetection(LPBYTE dst,int currentFrame)
{
	Y=dst;
	setCurrFrame(currentFrame);
	InvalidateRect(NULL,FALSE);
	UpdateWindow ();
}



void CYUVProcessWnd::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//((CChildWindow*)(this->GetParent()))->setBSobel(false);
	//this->GetWindowWord()
	CFrameWnd::OnClose();
}

void CYUVProcessWnd::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	
	CFrameWnd::PostNcDestroy();
}
