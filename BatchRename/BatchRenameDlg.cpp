
// BatchRenameDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "BatchRename.h"
#include "BatchRenameDlg.h"
#include "afxdialogex.h"

#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBatchRenameDlg dialog



CBatchRenameDlg::CBatchRenameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BATCHRENAME_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	mpWorker = NULL;
}

void CBatchRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TARGET_FOLDER, mEditTargetFolder);
	DDX_Control(pDX, IDC_EDIT_RESULT, mEditResult);
	DDX_Control(pDX, IDC_EDIT_REPLACE, mEditReplace);
	DDX_Control(pDX, IDC_EDIT_FIND, mEditFind);
	DDX_Control(pDX, IDC_CHECK_RECURR, mCheckRecurr);
}

BEGIN_MESSAGE_MAP(CBatchRenameDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FOLDER, &CBatchRenameDlg::OnClickedButtonSelectFolder)
	ON_BN_CLICKED(ID_REPLACE, &CBatchRenameDlg::OnClickedReplace)
	ON_MESSAGE(WM_RENAME_PROCESS, &CBatchRenameDlg::OnRenameWorkerMessage)
END_MESSAGE_MAP()


// CBatchRenameDlg message handlers

BOOL CBatchRenameDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	mEditTargetFolder.SetWindowText(mTargetFolder);
	mEditFind.SetWindowText(mFindStr);
	mEditReplace.SetWindowText(mReplaceStr);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBatchRenameDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBatchRenameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBatchRenameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define KMAX_PATH 260

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	switch (uMsg) {
		// 폴더선택 다이얼로그의 초기화가 끝난 경우
	case BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
		break;
	}
	return 0;
}

BOOL SelectLocalFolder(HWND hwnd, LPTSTR folderPath, BOOL includeFile, LPTSTR pTitle)
{
	TCHAR szwNewPath[KMAX_PATH] = { 0 };
	CString title;

	if (pTitle != NULL)
		title = pTitle;
	else
		title.LoadString(IDS_SELECT_TARGET_FOLDER);

	StringCchCopy(szwNewPath, KMAX_PATH, folderPath);

	BROWSEINFO bInfo;
	bInfo.hwndOwner = hwnd;
	bInfo.pidlRoot = NULL;
	bInfo.pszDisplayName = szwNewPath;
	bInfo.lpszTitle = title;
	bInfo.lpfn = BrowseCallbackProc;
	bInfo.ulFlags = BIF_NEWDIALOGSTYLE |
		// | BIF_NONEWFOLDERBUTTON
		// | BIF_EDITBOX
		// | BIF_SHAREABLE
		// | BIF_RETURNFSANCESTORS
		BIF_RETURNONLYFSDIRS;
	if (includeFile)
		bInfo.ulFlags |= (BIF_EDITBOX | BIF_BROWSEINCLUDEFILES);
	bInfo.lParam = (LPARAM)folderPath;

	LPITEMIDLIST lpItem = SHBrowseForFolder(&bInfo);

	if (lpItem != NULL)
	{
		SHGetPathFromIDList(lpItem, folderPath);
		CoTaskMemFree(lpItem);
		return TRUE;
	}
	return FALSE;
}

void CBatchRenameDlg::OnClickedButtonSelectFolder()
{
	TCHAR pathName[KMAX_PATH] = _T("");

	StringCchCopy(pathName, KMAX_PATH, mTargetFolder);
	if (SelectLocalFolder(GetSafeHwnd(), pathName, FALSE, NULL))
	{
		mTargetFolder.SetString(pathName);
		mEditTargetFolder.SetWindowText(pathName);
	}
}

void CBatchRenameDlg::OnClickedReplace()
{
	if (mpWorker != NULL)
	{
		AfxMessageBox(_T("Previous job is progressing.\nPlease wait until it ended."));
		return;
	}

	mEditFind.GetWindowText(mFindStr);
	mEditReplace.GetWindowText(mReplaceStr);
	if (mFindStr.GetLength() == 0)
	{
		AfxMessageBox(_T("No find string.\nPlease enter find string."));
		return;
	}

	if (mTargetFolder.GetLength() == 0)
	{
		AfxMessageBox(_T("No Target folder.\nPlease enter or select target folder."));
		return;
	}

	BOOL recurr = mCheckRecurr.GetCheck();

	mpWorker = new KRenameWorker(GetSafeHwnd());
	mpWorker->Start(mTargetFolder, mFindStr, mReplaceStr, recurr);
}

LRESULT CBatchRenameDlg::OnRenameWorkerMessage(WPARAM wParam, LPARAM lParam)
{
	if (lParam != NULL)
	{
		int len = mEditResult.GetWindowTextLength();
		mEditResult.SetSel(len, len);
		mEditResult.ReplaceSel((LPCTSTR)lParam);
	}

	if (wParam == 1)
	{
		delete mpWorker;
		mpWorker = NULL;
	}
	return 0;
}


