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



// YUVviewerDlg.h : header file
//

#include "afxwin.h"
#if !defined(AFX_YUVVIEWERDLG_H__204D3CBB_8AA7_4546_BED8_CA2B93D08BC8__INCLUDED_)
#define AFX_YUVVIEWERDLG_H__204D3CBB_8AA7_4546_BED8_CA2B93D08BC8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CYUVviewerDlg dialog

class CYUVviewerDlg : public CDialog
{
// Construction
public:
	BOOL m_bPlay;
	//static int  m_iSobel;
	CWinThread* m_pWinThread;
	CChildWindow *m_pWnd[36];
	HANDLE hPlayTemp;
	CYUVviewerDlg(CWnd* pParent = NULL);	// standard constructor

	

// Dialog Data
	//{{AFX_DATA(CYUVviewerDlg)

  CButton	m_buttonPrev5;
  CButton	m_buttonOrder;
  CButton	m_buttonNext5;
  CButton	m_buttonPrev;
  CButton	m_buttonNext;
  CButton	m_buttonOpen;
  CComboBox m_cCOMBOSample;
  CComboBox m_cCOMBOresolution;
  CComboBox m_cCOMBOResize;
  CButton m_buttonPausePlay;
  int m_iFrameRate;
	enum { IDD = IDD_YUVVIEWER_DIALOG };
	int		m_nFrameFrom;
	int		m_nFrameTo;
	int		m_nFrameSize;
	
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYUVviewerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	void Enable(int nID);
	void Disable(int nID);
	BOOL Enabled(int nID);
	// Generated message map functions
	//{{AFX_MSG(CYUVviewerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpenFile();
	afx_msg void OnNext();
	afx_msg void OnPauseplay();
	afx_msg void OnPrevious();
	afx_msg void OnNext5();
	afx_msg void OnOrder();
	afx_msg void OnPrevious5();
	afx_msg void OnCloseAll();
	afx_msg void OnBnClickedReplay();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	
public:
	afx_msg void OnBnClickedButtonsobel();
	int Xorder;
	int Yorder;
	int ApertureSize;
	afx_msg void OnBnClickedButtonmovementdetection();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YUVVIEWERDLG_H__204D3CBB_8AA7_4546_BED8_CA2B93D08BC8__INCLUDED_)
