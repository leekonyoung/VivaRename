
// BatchRenameDlg.h : header file
//

#pragma once

#include "KRenameWorker.h"

// CBatchRenameDlg dialog
class CBatchRenameDlg : public CDialogEx
{
// Construction
public:
	CBatchRenameDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BATCHRENAME_DIALOG };
#endif

	CString mTargetFolder;
	CString mFindStr;
	CString mReplaceStr;
	KRenameWorker* mpWorker;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit mEditTargetFolder;
	CEdit mEditResult;
	CEdit mEditReplace;
	CEdit mEditFind;
	afx_msg void OnClickedButtonSelectFolder();
	afx_msg void OnClickedReplace();
	afx_msg LRESULT OnRenameWorkerMessage(WPARAM wParam, LPARAM lParam);

	CButton mCheckRecurr;
};
