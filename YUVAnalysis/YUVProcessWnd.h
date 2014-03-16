#pragma once


// CYUVProcessWnd 框架

class CYUVProcessWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CYUVProcessWnd)

private:
	int iWidth,iHeight;
	double m_nZoom;
	HANDLE hloc;
	LPBITMAPINFO BmpInfo;
	int totalFrames;
	int currFrame;
	char  m_aSeqName[64];
	LPBYTE Y,RGBbuf;
	//YUVProcess *m_cYUVProcess;//YUVProcess
	char *usedInfo;
	bool bColorImage;


public:
	CYUVProcessWnd();           // 动态创建所使用的受保护的构造函数
	virtual ~CYUVProcessWnd();
	CYUVProcessWnd(CFrameWnd * childWindow); 


	//YUVProcess *getYUVProcess(){return m_cYUVProcess;}

	char *getUsedInfo(){return usedInfo; }
	void setUsedInfo(char *uInfo){usedInfo=uInfo;}

	LPBYTE getY(){return Y;}
	void setY(LPBYTE Luma){Y=Luma;}
	LPBYTE getRGBbuf(){return RGBbuf;}
	void setRGBbuf(LPBYTE rgb){RGBbuf=rgb;}

	void setBColorImage(bool bC){bColorImage=bC;}
	bool getBColorImage(){return bColorImage;}

	int getWidth(){return iWidth;}
	void setWidth(int width){iWidth=width;}

	int getHeight(){return iHeight;}
	void setHeight(int height){iHeight=height;}

	double getZoom(){return m_nZoom;}
	void setZoom(double zoom){m_nZoom=zoom;}
	int getCurrFrame(){return currFrame;}
	void setCurrFrame(int currentFrame){currFrame=currentFrame;}
	int getTotalFrames(){return totalFrames;}
	void setTotalFrames(int Frames){totalFrames=Frames;}
	char * getSeqName(){return m_aSeqName;}

	void ShowImage(CDC *pDC,BYTE *lpImage);
	void ShowGrayImage(CDC *pDC,BYTE *lpImage);
	void Sobel(LPBYTE dst,int currentFrame,int xorder, int yorder, int aperture_size=3);
	void MovementDetection(LPBYTE dst,int currentFrame);

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
protected:
	virtual void PostNcDestroy();
};


