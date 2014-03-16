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


// YUVviewerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "YUVviewer.h"
#include "ChildWindow.h"
#include "YUVviewerDlg.h"
#include <vector>
#include <algorithm>
#include "commonDefs.h"


using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void getSeqName(char *inseqpath, char *seqname);

//int nImgWidth, nImgHeight;
UINT PlayVideo( LPVOID pParam )
{
	int i,j;
	vector<int>vecFrameLeft;
	UINT picsize;
	int timespan;
	CYUVviewerDlg *pWin = (CYUVviewerDlg *)pParam;

	for(j=0;j<=CChildWindow::getCount();++j){
		vecFrameLeft.push_back(pWin->m_pWnd[j]->getTotalFrames()-pWin->m_pWnd[j]->getCurrFrame());
	}

	int maxFrames=*max_element(vecFrameLeft.begin(),vecFrameLeft.end());

	HANDLE hPlayTemp1 = OpenMutex(MUTEX_ALL_ACCESS,FALSE,"Play");
	
	while(maxFrames>0)
	{
		DWORD t2=GetTickCount();
 
		if ( WAIT_OBJECT_0 == WaitForSingleObject(hPlayTemp1,INFINITE) )
			ReleaseMutex( hPlayTemp1 );
		
		for(i=0; i<=CChildWindow::getCount(); i++)
		{
			if(pWin->m_pWnd[i]->getCurrFrame()<pWin->m_pWnd[i]->getTotalFrames()){

				timespan = (int)(1000.0/pWin->m_pWnd[i]->getFrameRate());
				picsize=pWin->m_pWnd[i]->getFrameSize();
				
				pWin->m_pWnd[i]->readYUVData(picsize);
				pWin->m_pWnd[i]->refreshChildWnd(pWin->m_pWnd[i]->getCurrFrame()+1);

				if(pWin->m_pWnd[i]->getBSobel()){
					pWin->m_pWnd[i]->getYUVProcess()->YUVSobel(pWin->m_pWnd[i]->Y,pWin->Xorder,pWin->Yorder,pWin->ApertureSize);
					pWin->m_pWnd[i]->getYUVProcessWnd(WT_SOBEL)->Sobel(pWin->m_pWnd[i]->getYUVProcess()->getDst(),pWin->m_pWnd[i]->getCurrFrame(),pWin->Xorder,pWin->Yorder,pWin->ApertureSize);
				}

				if(pWin->m_pWnd[i]->getBMovementDetection()){
					
					//pWin->m_pWnd[i]->getYUVProcess()->setMDFirst(pWin->m_pWnd[i]->getYUVProcess()->getMDSecond());
					pWin->m_pWnd[i]->getYUVProcess()->setMDSecond(pWin->m_pWnd[i]->Y);					
					pWin->m_pWnd[i]->getYUVProcess()->YUVMovementDetection();
					pWin->m_pWnd[i]->getYUVProcessWnd(WT_MOVEMENTDETECTION)->MovementDetection(pWin->m_pWnd[i]->getYUVProcess()->getMDDst(),pWin->m_pWnd[i]->getCurrFrame());			

				}

				int t1=GetTickCount()-t2;
				if(t1 < timespan) 
					Sleep(timespan - t1); // sleep time in milliseconds
			}
		}
		

		maxFrames--;
	}	
	pWin->m_pWinThread = NULL;
	AfxEndThread(0);

	return 0;

}



void CYUVviewerDlg::OnPauseplay() 
{
	// create a new thread
	if (m_bPlay)
	{
		m_buttonPausePlay.SetWindowText("Pause");
		m_bPlay = false;

		GetDlgItem(IDC_NEXT)->EnableWindow(false);
		GetDlgItem(IDC_NEXT5)->EnableWindow(false);
		GetDlgItem(IDC_PREVIOUS)->EnableWindow(false);
		GetDlgItem(IDC_PREVIOUS5)->EnableWindow(false);
		GetDlgItem(IDC_REPLAY)->EnableWindow(false);	
		GetDlgItem(IDC_ORDER)->EnableWindow(false);
		GetDlgItem(IDC_OPENFILE)->EnableWindow(false);
		GetDlgItem(IDC_CLOSEALL)->EnableWindow(false);
		GetDlgItem(IDCANCEL)->EnableWindow(false);
		GetDlgItem(IDC_BUTTONMovementDetection)->EnableWindow(false);
		GetDlgItem(IDC_BUTTONSobel)->EnableWindow(false);

	}
	else
	{
		m_buttonPausePlay.SetWindowText("Play");
		m_bPlay = true;

		GetDlgItem(IDC_NEXT)->EnableWindow(true);
		GetDlgItem(IDC_NEXT5)->EnableWindow(true);
		GetDlgItem(IDC_PREVIOUS)->EnableWindow(true);
		GetDlgItem(IDC_PREVIOUS5)->EnableWindow(true);
		GetDlgItem(IDC_REPLAY)->EnableWindow(true);	
		GetDlgItem(IDC_ORDER)->EnableWindow(true);
		GetDlgItem(IDC_OPENFILE)->EnableWindow(true);
		GetDlgItem(IDC_CLOSEALL)->EnableWindow(true);
		GetDlgItem(IDCANCEL)->EnableWindow(true);
		//GetDlgItem(IDC_BUTTONMovementDetection)->EnableWindow(true);
	}

	char chTitle[10];
	m_buttonPausePlay.GetWindowText(chTitle,10);
	hPlayTemp = NULL;
	hPlayTemp=OpenMutex(MUTEX_ALL_ACCESS,FALSE,"Play");
	if ( strcmp( chTitle,"Play" ) == 0 )
	{
		WaitForSingleObject( hPlayTemp,0);
		
	}
	else
		ReleaseMutex(hPlayTemp);

	if ( m_pWinThread == NULL)
		m_pWinThread = AfxBeginThread( (AFX_THREADPROC)PlayVideo , (void*)this);

}
void CYUVviewerDlg::OnOrder() 
{
	int i,j;
	int picsize;
	vector<int> vec;

	for(j=0; j<=CChildWindow::getCount(); ++j){		
		vec.push_back(m_pWnd[j]->getCurrFrame());
	}
	int maxFrames=*max_element(vec.begin(),vec.end());
	while(maxFrames>0){
		for(i=0; i<=CChildWindow::getCount(); i++){		

			if(m_pWnd[i]->getCurrFrame()!=1){
				picsize = m_pWnd[i]->getWidth()*m_pWnd[i]->getHeight();				
				m_pWnd[i]->m_pFile->Seek((m_pWnd[i]->getCurrFrame()-1)*picsize*3/2, SEEK_SET);
				m_pWnd[i]->readYUVData(picsize);	
				m_pWnd[i]->refreshChildWnd(m_pWnd[i]->getCurrFrame()-1);

				if(m_pWnd[i]->getBSobel()){
					m_pWnd[i]->getYUVProcess()->YUVSobel(m_pWnd[i]->Y,Xorder,Yorder,ApertureSize);
					m_pWnd[i]->getYUVProcessWnd(WT_SOBEL)->Sobel(m_pWnd[i]->getYUVProcess()->getDst(),m_pWnd[i]->getCurrFrame(),Xorder,Yorder,ApertureSize);
				}
				if(m_pWnd[i]->getBMovementDetection()){			
					//m_pWnd[i]->getYUVProcess()->setMDFirst(m_pWnd[i]->getYUVProcess()->getMDSecond());
					m_pWnd[i]->getYUVProcess()->setMDSecond(m_pWnd[i]->Y);			
					m_pWnd[i]->getYUVProcess()->YUVMovementDetection();
					m_pWnd[i]->getYUVProcessWnd(WT_MOVEMENTDETECTION)->MovementDetection(m_pWnd[i]->getYUVProcess()->getMDDst(),m_pWnd[i]->getCurrFrame());		
				}

			}
		}
		maxFrames--;
		//if(m_iSobel!=0)
		//	OnBnClickedButtonsobel();//sobel边缘滤波
		Sleep(20);
	}

}
void CYUVviewerDlg::OnOpenFile() 
{	

	CChildWindow::setCount();
	UpdateData(TRUE);	

	CFile *file = new CFile();

	char BASED_CODE szFilter[] = "All Files (*.*)|*.*||";
	CFileDialog dlg( TRUE, "yuv", NULL, OFN_HIDEREADONLY,szFilter);

	if(dlg.DoModal()!=IDOK) {
		CChildWindow::setCount(-1);
		return ;
	}

	
	char inSeqence[_MAX_PATH];//存储路径
	char inSeqName[64];//存储文件名

	sprintf( inSeqence, "%s", dlg.GetPathName() );
	getSeqName(inSeqence, inSeqName);

	if(!file->Open(inSeqence, CFile::modeRead)) 
	{
		AfxMessageBox("Can't open input file");
		return ;
	}


	m_pWnd[CChildWindow::getCount()]=new CChildWindow((CFrameWnd*)this,file,inSeqName);

	m_pWnd[CChildWindow::getCount()]->readYUVData(m_pWnd[CChildWindow::getCount()]->getFrameSize());


	m_pWnd[CChildWindow::getCount()]->ShowWindow(SW_SHOW);

	m_pWnd[CChildWindow::getCount()]->CenterWindow((int)(m_pWnd[CChildWindow::getCount()]->getWidth()*m_pWnd[CChildWindow::getCount()]->getZoom()),
		(int)(m_pWnd[CChildWindow::getCount()]->getHeight()*m_pWnd[CChildWindow::getCount()]->getZoom()));

	GetDlgItem(IDC_NEXT)->EnableWindow(TRUE);
	GetDlgItem(IDC_NEXT5)->EnableWindow(TRUE);
	GetDlgItem(IDC_PREVIOUS)->EnableWindow(TRUE);
	GetDlgItem(IDC_PREVIOUS5)->EnableWindow(TRUE);
	GetDlgItem(IDC_PAUSEPLAY)->EnableWindow(TRUE);
	GetDlgItem(IDC_REPLAY)->EnableWindow(TRUE);
	GetDlgItem(IDC_CLOSEALL)->EnableWindow(TRUE);	
	GetDlgItem(IDC_ORDER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTONSobel)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTONMovementDetection)->EnableWindow(true);
	
}

void CYUVviewerDlg::OnNext() //读取下一帧
{
	int i;
	//UINT picsize;
 
	for(i=0; i<=CChildWindow::getCount(); i++)
	{
		//picsize = m_pWnd[i]->getHeight()*m_pWnd[i]->getWidth();
		m_pWnd[i]->readYUVData(m_pWnd[i]->getFrameSize());
		m_pWnd[i]->refreshChildWnd(m_pWnd[i]->getCurrFrame()+1);

		if(m_pWnd[i]->getBSobel()){
			m_pWnd[i]->getYUVProcess()->YUVSobel(m_pWnd[i]->Y,Xorder,Yorder,ApertureSize);
			m_pWnd[i]->getYUVProcessWnd(WT_SOBEL)->Sobel(m_pWnd[i]->getYUVProcess()->getDst(),m_pWnd[i]->getCurrFrame(),Xorder,Yorder,ApertureSize);
		}

		if(m_pWnd[i]->getBMovementDetection()){			
			//m_pWnd[i]->getYUVProcess()->setMDFirst(m_pWnd[i]->getYUVProcess()->getMDSecond());
			m_pWnd[i]->getYUVProcess()->setMDSecond(m_pWnd[i]->Y);
			m_pWnd[i]->getYUVProcess()->YUVMovementDetection();
			m_pWnd[i]->getYUVProcessWnd(WT_MOVEMENTDETECTION)->MovementDetection(m_pWnd[i]->getYUVProcess()->getMDDst(),m_pWnd[i]->getCurrFrame());		
		}

	}

}

void CYUVviewerDlg::OnNext5() //读取下5帧
{
	int i;
	UINT picsize;	
	int count=5;
	while((count--)>0){
		for(i=0; i<=CChildWindow::getCount(); i++){			
			picsize = m_pWnd[i]->getWidth()*m_pWnd[i]->getHeight();
			m_pWnd[i]->readYUVData(picsize);
			m_pWnd[i]->refreshChildWnd(m_pWnd[i]->getCurrFrame()+1);


			if(m_pWnd[i]->getBSobel()){
				m_pWnd[i]->getYUVProcess()->YUVSobel(m_pWnd[i]->Y,Xorder,Yorder,ApertureSize);
				m_pWnd[i]->getYUVProcessWnd(WT_SOBEL)->Sobel(m_pWnd[i]->getYUVProcess()->getDst(),m_pWnd[i]->getCurrFrame(),Xorder,Yorder,ApertureSize);
			}
			if(m_pWnd[i]->getBMovementDetection()){			
				//m_pWnd[i]->getYUVProcess()->setMDFirst(m_pWnd[i]->getYUVProcess()->getMDSecond());
				m_pWnd[i]->getYUVProcess()->setMDSecond(m_pWnd[i]->Y);			
				m_pWnd[i]->getYUVProcess()->YUVMovementDetection();
				m_pWnd[i]->getYUVProcessWnd(WT_MOVEMENTDETECTION)->MovementDetection(m_pWnd[i]->getYUVProcess()->getMDDst(),m_pWnd[i]->getCurrFrame());		
			}

		}
		Sleep(200);
	}
}


void CYUVviewerDlg::OnPrevious() //读取前一帧
{
	int i;
	int picsize;
	
		for(i=0; i<=CChildWindow::getCount(); i++)
		{
			if(m_pWnd[i]->getCurrFrame()!=1){

				picsize = m_pWnd[i]->getWidth()*m_pWnd[i]->getHeight();
				m_pWnd[i]->m_pFile->Seek((m_pWnd[i]->getCurrFrame()-1)*picsize*3/2, SEEK_SET);
				m_pWnd[i]->readYUVData(picsize);	
				m_pWnd[i]->refreshChildWnd(m_pWnd[i]->getCurrFrame()-1);

				if(m_pWnd[i]->getBSobel()){
					m_pWnd[i]->getYUVProcess()->YUVSobel(m_pWnd[i]->Y,Xorder,Yorder,ApertureSize);
					m_pWnd[i]->getYUVProcessWnd(WT_SOBEL)->Sobel(m_pWnd[i]->getYUVProcess()->getDst(),m_pWnd[i]->getCurrFrame(),Xorder,Yorder,ApertureSize);
				}
				if(m_pWnd[i]->getBMovementDetection()){			
					//m_pWnd[i]->getYUVProcess()->setMDFirst(m_pWnd[i]->getYUVProcess()->getMDSecond());
					m_pWnd[i]->getYUVProcess()->setMDSecond(m_pWnd[i]->Y);			
					m_pWnd[i]->getYUVProcess()->YUVMovementDetection();
					m_pWnd[i]->getYUVProcessWnd(WT_MOVEMENTDETECTION)->MovementDetection(m_pWnd[i]->getYUVProcess()->getMDDst(),m_pWnd[i]->getCurrFrame());		
				}

			
			}else{
				MessageBox("Get to begining of file");
			}
		}
		//if(m_iSobel!=0)
		//	OnBnClickedButtonsobel();//sobel边缘滤波
}

void CYUVviewerDlg::OnPrevious5() //读取前5帧
{
	int i;
	int picsize;
	int count=5;
	while((count--)>0){
		for(i=0; i<=CChildWindow::getCount(); i++){			
			if(m_pWnd[i]->getCurrFrame()!=1){
				picsize = m_pWnd[i]->getWidth()*m_pWnd[i]->getHeight();				
				m_pWnd[i]->m_pFile->Seek((m_pWnd[i]->getCurrFrame()-1)*picsize*3/2, SEEK_SET);
				m_pWnd[i]->readYUVData(picsize);
				m_pWnd[i]->refreshChildWnd(m_pWnd[i]->getCurrFrame()-1);

				if(m_pWnd[i]->getBSobel()){
					m_pWnd[i]->getYUVProcess()->YUVSobel(m_pWnd[i]->Y,Xorder,Yorder,ApertureSize);
					m_pWnd[i]->getYUVProcessWnd(WT_SOBEL)->Sobel(m_pWnd[i]->getYUVProcess()->getDst(),m_pWnd[i]->getCurrFrame(),Xorder,Yorder,ApertureSize);
				}
				if(m_pWnd[i]->getBMovementDetection()){			
					//m_pWnd[i]->getYUVProcess()->setMDFirst(m_pWnd[i]->getYUVProcess()->getMDSecond());
					m_pWnd[i]->getYUVProcess()->setMDSecond(m_pWnd[i]->Y);			
					m_pWnd[i]->getYUVProcess()->YUVMovementDetection();
					m_pWnd[i]->getYUVProcessWnd(WT_MOVEMENTDETECTION)->MovementDetection(m_pWnd[i]->getYUVProcess()->getMDDst(),m_pWnd[i]->getCurrFrame());		
				}

			}/*else{
				MessageBox("Get to begining of file");
			}*/
		}
		//if(m_iSobel!=0)
		//	OnBnClickedButtonsobel();//sobel边缘滤波
		Sleep(200);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYUVviewerDlg dialog

CYUVviewerDlg::CYUVviewerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CYUVviewerDlg::IDD, pParent)
	, Xorder(0)
	, Yorder(0)
	, ApertureSize(0)
{
	//{{AFX_DATA_INIT(CYUVviewerDlg)

	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CYUVviewerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CYUVviewerDlg)
	DDX_Control(pDX, IDC_COMBOSample, m_cCOMBOSample);
	DDX_Control(pDX, IDC_COMBOResolution, m_cCOMBOresolution);
	DDX_Control(pDX, IDC_COMBOResize, m_cCOMBOResize);
	DDX_Text(pDX, IDC_EDITFrameRate, m_iFrameRate);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PAUSEPLAY, m_buttonPausePlay);
	DDX_Text(pDX, IDC_EDITXorder, Xorder);
	DDX_Text(pDX, IDC_EDITYorder, Yorder);
	DDX_Text(pDX, IDC_EDITApertureSize, ApertureSize);
}

BEGIN_MESSAGE_MAP(CYUVviewerDlg, CDialog)
	//{{AFX_MSG_MAP(CYUVviewerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENFILE, OnOpenFile)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PAUSEPLAY, OnPauseplay)
	ON_BN_CLICKED(IDC_PREVIOUS, OnPrevious)
	ON_BN_CLICKED(IDC_NEXT5, OnNext5)
	ON_BN_CLICKED(IDC_ORDER, OnOrder)
	ON_BN_CLICKED(IDC_PREVIOUS5, OnPrevious5)
	ON_BN_CLICKED(IDC_CLOSEALL, &CYUVviewerDlg::OnCloseAll)
	//}}AFX_MSG_MAP


	
	ON_BN_CLICKED(IDC_REPLAY, &CYUVviewerDlg::OnBnClickedReplay)
	ON_BN_CLICKED(IDC_BUTTONSobel, &CYUVviewerDlg::OnBnClickedButtonsobel)
	ON_BN_CLICKED(IDC_BUTTONMovementDetection, &CYUVviewerDlg::OnBnClickedButtonmovementdetection)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYUVviewerDlg message handlers

BOOL CYUVviewerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//	CChildWindow.getCount()=0;
	m_bPlay = true;
	m_pWinThread = NULL;
	m_cCOMBOSample.SetCurSel(0);
	m_cCOMBOresolution.SetCurSel(0);
	m_cCOMBOResize.SetCurSel(2);
	m_iFrameRate=30;
	Xorder=1;
	Yorder=1;
	ApertureSize=3;
	UpdateData(false);

	for(int i=0;i<36;i++){
		m_pWnd[i]=NULL;
	}
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	//Disable(IDC_SIZE_WIDTH);
	//Disable(IDC_SIZE_HEIGHT);
	//Disable(IDC_STATIC_H);
	//Disable(IDC_STATIC_W);

	HANDLE hPlay = NULL;
	if( (hPlay=OpenMutex(MUTEX_ALL_ACCESS,FALSE,"Play"))==NULL)
	{
		//如果没有其他进程创建这个互斥量，则重新创建
		hPlay = CreateMutex(NULL,FALSE,"Play");
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CYUVviewerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CYUVviewerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CYUVviewerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CYUVviewerDlg::Disable(int nID)
{
	CWnd *pObject1;
	pObject1 = GetDlgItem(nID);
	pObject1->EnableWindow(FALSE);
}
void CYUVviewerDlg::Enable(int nID)
{
	CWnd *pObject1;
	pObject1 = GetDlgItem(nID);
	pObject1->EnableWindow(TRUE);
}
BOOL CYUVviewerDlg::Enabled(int nID)
{
	CWnd *pObject1;
	pObject1 = GetDlgItem(nID);
	return (pObject1->IsWindowEnabled());
}


void getSeqName(char *inseqpath, char *seqname)
{
  int lastSlashPos, lastDotPos; // the last dot is located after the last slash "\"
  int lastNonZeroPos; // last pos that tmp != 0
  int i=0;
  char tmp = '0';

  while(tmp != 0)
  {
    tmp = inseqpath[i++];
    if(tmp == '\\')
      lastSlashPos = i-1;
    if(tmp == '.')
      lastDotPos = i-1;
  }
  lastNonZeroPos = i-1;

  if(lastDotPos < lastSlashPos)
    lastDotPos = -1; // that means the file name with no extention, such as "c:\seq\forman".

  if(lastDotPos != -1)
  {
    for(i=lastSlashPos+1; i<lastDotPos; i++)
      seqname[i-lastSlashPos-1] = inseqpath[i];
    seqname[lastDotPos-lastSlashPos-1] = 0;
  }
  else
  {
    for(i=lastSlashPos+1; i<lastNonZeroPos+1; i++)
      seqname[i-lastSlashPos-1] = inseqpath[i];
    seqname[lastNonZeroPos-lastSlashPos] = 0;
  }
}


void CYUVviewerDlg::OnCloseAll()
{
	// TODO: 在此添加控件通知处理程序代码

	for(int i=CChildWindow::getCount();i>-1;i--){
		//if(m_pWnd[i]){	
			//m_pWnd[i]->setYUVProcess(NULL);
			for(int j=0;j<4;j++){
				m_pWnd[i]->setYUVProcessWnd(j,NULL);
			}
			m_pWnd[i]->SendMessage(WM_CLOSE,0,0);
			m_pWnd[i]=NULL;
		//}
		
	}	
}

void CYUVviewerDlg::OnBnClickedReplay()
{
	// TODO: 在此添加控件通知处理程序代码
	CYUVviewerDlg::OnOrder() ;
	CYUVviewerDlg::OnPauseplay();
}
//int CYUVviewerDlg::m_iSobel=0;
void CYUVviewerDlg::OnBnClickedButtonsobel()
{
	// TODO: 在此添加控件通知处理程序代码

	int wndCount=CChildWindow::getCount();

	for(int i=0; i<=wndCount; i++){	
		
		if(m_pWnd[i]->getBSobel()==false){
			
			m_pWnd[i]->setBSobel(true);	
			m_pWnd[i]->setYUVProcessWnd(WT_SOBEL,new CYUVProcessWnd(m_pWnd[i]));	
			m_pWnd[i]->getYUVProcessWnd(WT_SOBEL)->setBColorImage(false);
			m_pWnd[i]->getYUVProcess()->allocMemForSobel(m_pWnd[i]->getWidth(),m_pWnd[i]->getHeight());
			m_pWnd[i]->getYUVProcess()->YUVSobel(m_pWnd[i]->Y,Xorder,Yorder,ApertureSize);			
			m_pWnd[i]->getYUVProcessWnd(WT_SOBEL)->setY(m_pWnd[i]->getYUVProcess()->getDst());
			m_pWnd[i]->getYUVProcessWnd(WT_SOBEL)->ShowWindow(SW_SHOW);
		}	
	}

}

void CYUVviewerDlg::OnBnClickedButtonmovementdetection()
{
	// TODO: 在此添加控件通知处理程序代码

	int wndCount=CChildWindow::getCount();

	for(int i=0; i<=wndCount; i++){	

		if(m_pWnd[i]->getBMovementDetection()==false){

			m_pWnd[i]->setBMovementDetection(true);	
			m_pWnd[i]->setYUVProcessWnd(WT_MOVEMENTDETECTION,new CYUVProcessWnd(m_pWnd[i]));
			m_pWnd[i]->getYUVProcessWnd(WT_MOVEMENTDETECTION)->setBColorImage(false);
			m_pWnd[i]->getYUVProcess()->allocMemForMD(m_pWnd[i]->getWidth(),m_pWnd[i]->getHeight());
			m_pWnd[i]->getYUVProcess()->setMDSecond(m_pWnd[i]->Y);
			m_pWnd[i]->getYUVProcessWnd(WT_MOVEMENTDETECTION)->setY(m_pWnd[i]->Y);
			m_pWnd[i]->getYUVProcessWnd(WT_MOVEMENTDETECTION)->ShowWindow(SW_SHOW);
		}
	//WT_MOVEMENTDETECTION
	
	}
}
