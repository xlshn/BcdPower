
// BcdDetailDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BcdDetail.h"
#include "BcdDetailDlg.h"
#include "afxdialogex.h"
#include "bcdstore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MENU_BCDSTORE_ITEM_OPENBCDSTROE 11000

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


// CBcdDetailDlg dialog



CBcdDetailDlg::CBcdDetailDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BCDDETAIL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBcdDetailDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_BCD_SOTRE, m_bcdStoreTree);
}

BEGIN_MESSAGE_MAP(CBcdDetailDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(MENU_BCDSTORE_ITEM_OPENBCDSTROE, OnOpenBcdStore)
END_MESSAGE_MAP()


// CBcdDetailDlg message handlers
BOOL CBcdDetailDlg::OnInitDialog()
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
	
	CMenu* pMenu = new CMenu;
	pMenu->CreateMenu();	
	CMenu menu;
	menu.CreateMenu();//新建菜单
	menu.AppendMenu(MF_STRING, MENU_BCDSTORE_ITEM_OPENBCDSTROE, L"OpenBcdStore");//新建子菜单的菜单项1
	pMenu->AppendMenu(MF_POPUP, (UINT)menu.m_hMenu, L"BcdStore");//增加子菜单 
	SetMenu(pMenu);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBcdDetailDlg::OnOpenBcdStore()
{
	wchar_t pwszBCDPath[ MAX_PATH * 2] = { 0 };
	OPENFILENAME openFileName;
	memset(&openFileName, 0, sizeof(openFileName));
	openFileName.lStructSize = sizeof(openFileName);
	openFileName.hwndOwner = m_hWnd;
	openFileName.lpstrFile = pwszBCDPath;
	openFileName.nMaxFile = sizeof(pwszBCDPath) / sizeof(wchar_t);
	openFileName.lpstrTitle = L"Open BCD File";
	openFileName.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
	openFileName.lpstrFilter = L"BCD File\0bcd*\0All Files\0*.*\0\0";
	if (!GetOpenFileName(&openFileName))
	{
		return;
	}
	BcdStore* pBcdStore = NULL;
	bool bRes = BcdStore::OpenStore(pwszBCDPath, pBcdStore);
	if (!bRes)
	{
		::MessageBox(m_hWnd, L"Open bcd fail fail", L"Error", MB_OK);
		return;
	}	
	pBcdStore->ExportStore(L"c:\\1.txt");
	vecBcdStore.push_back(pBcdStore);
	std::vector<BcdObject*> vecBcdObjects;
	pBcdStore->EnumerateObjects(0, vecBcdObjects);
	Sleep(1000);
	//m_bcdStoreTree.InsertItem();
}

void CBcdDetailDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBcdDetailDlg::OnPaint()
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
HCURSOR CBcdDetailDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

