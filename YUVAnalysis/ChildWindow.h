
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



#if !defined(AFX_CHILDWINDOW_H__964DA0F6_4CD2_11D4_B1E9_0000B43F34EC__INCLUDED_)
#define AFX_CHILDWINDOW_H__964DA0F6_4CD2_11D4_B1E9_0000B43F34EC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChildWindow.h : header file
//

#include "Convert.h"

/////////////////////////////////////////////////////////////////////////////
// CChildWindow frame
#include "YUVProcess.h"
#include "YUVProcessWnd.h"
#include "commonDefs.h"
class CChildWindow : public CFrameWnd
{
	DECLARE_DYNCREATE(CChildWindow)
protected:
	CChildWindow();           // protected constructor used by dynamic creation

private:
	BOOL bColorImage;
	int iWidth,iHeight;
	double m_nZoom;
	int totalFrames;
	UINT frameSize;
	int currFrame;
	int m_iFrameRate;
	char  m_aSeqName[64];
	HANDLE hloc;
	LPBITMAPINFO BmpInfo;
	ColorSpaceConversions conv;
	static int m_iCount;	
	bool m_bSobel;
	bool m_bMovementDetection;
	CYUVProcessWnd * m_cYUVProcessWnd[4]; 
	YUVProcess *m_cYUVProcess;//YUVProcess
	

public: 
	CChildWindow(CFrameWnd* pParentWnd,CFile *file,char *seqName);
	~CChildWindow();
	CChildWindow(CChildWindow * childWnd);

	CFile *m_pFile;
	LPBYTE Y,Cb,Cr,RGBbuf;


	YUVProcess *getYUVProcess(){return m_cYUVProcess;}
	void setYUVProcess(YUVProcess *yp){m_cYUVProcess=yp;}

	void setYUVProcessWnd(int type,CYUVProcessWnd * yuvProcessWnd);
	CYUVProcessWnd * getYUVProcessWnd(int type){return m_cYUVProcessWnd[type];}	
	
	void setBColorImage(BOOL bC){bColorImage=bC;}
	BOOL getBColorImage(){return bColorImage;}

	void setBSobel(bool sobelFlags){m_bSobel=sobelFlags;}
	bool getBSobel(){return m_bSobel;}

	void setBMovementDetection(bool BMD){m_bMovementDetection=BMD;}
	bool getBMovementDetection(){return m_bMovementDetection;}


	char * getSeqName(){return m_aSeqName;}

	static int getCount(){return m_iCount;}//return the value of m_iCount
	static void setCount(int defValue=1){m_iCount+=defValue;}

	int getWidth(){return iWidth;}
	void setWidth(int width){iWidth=width;}

	int getHeight(){return iHeight;}
	void setHeight(int height){iHeight=height;}

	double getZoom(){return m_nZoom;}
	void setZoom(double zoom){m_nZoom=zoom;}

	int getTotalFrames(){return totalFrames;}
	void setTotalFrames(int Frames){totalFrames=Frames;}

	UINT getFrameSize(){return frameSize;}
	void setFrameSize(UINT pictureSize){frameSize=pictureSize;}

	int getCurrFrame(){return currFrame;}
	void setCurrFrame(int currentFrame){currFrame=currentFrame;}

	int getFrameRate(){return m_iFrameRate;}
	void setFrameRate(int frameRate){m_iFrameRate=frameRate;}

	void readYUVData(UINT pictureSize);
	void refreshChildWnd(int frameNum=1);	
	
	void ShowImage(CDC *dc,BYTE  *lpImage);
	void ShowGrayImage(CDC *dc,BYTE  *lpImage);
	void CenterWindow(int width,int height);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildWindow)
	//}}AFX_VIRTUAL


	// Generated message map functions
	//{{AFX_MSG(CChildWindow)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDWINDOW_H__964DA0F6_4CD2_11D4_B1E9_0000B43F34EC__INCLUDED_)
