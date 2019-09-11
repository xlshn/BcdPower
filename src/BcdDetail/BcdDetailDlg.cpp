
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
	LONG lTreeLong = ::GetWindowLong(m_bcdStoreTree.m_hWnd, GWL_STYLE);
	lTreeLong |= (TVS_HASBUTTONS | TVS_LINESATROOT | TVS_HASLINES);
	::SetWindowLong(m_bcdStoreTree.m_hWnd, GWL_STYLE, lTreeLong);
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
	vecBcdStore.push_back(pBcdStore);
	std::vector<BcdObject*> vecBcdObjects;
	pBcdStore->EnumerateObjects(0, vecBcdObjects);
	TVINSERTSTRUCT treeItem;
	memset(&treeItem, 0, sizeof(treeItem));	
	treeItem.item.mask = TVIF_TEXT | TVIF_PARAM;// | TVIF_CHILDREN;
	//treeItem.item.cChildren = I_CHILDRENCALLBACK;
	treeItem.item.pszText = L"BcdStore";
	treeItem.item.cchTextMax = wcslen(treeItem.item.pszText);
	treeItem.item.lParam = (LPARAM)pBcdStore;
	HTREEITEM hRootItem = m_bcdStoreTree.InsertItem(&treeItem);

	TVINSERTSTRUCT treeItemAppObjs;
	memset(&treeItemAppObjs, 0, sizeof(treeItemAppObjs));
	treeItemAppObjs.item.mask = TVIF_TEXT | TVIF_PARAM;// | TVIF_CHILDREN;
	//treeItem.item.cChildren = I_CHILDRENCALLBACK;
	treeItemAppObjs.item.pszText = L"Application Objects";
	treeItemAppObjs.item.cchTextMax = wcslen(treeItem.item.pszText);
	treeItemAppObjs.item.lParam = (LPARAM)nullptr;
	//treeItemAppObjs.hInsertAfter = hRootItem;
	treeItemAppObjs.hParent = hRootItem;
	HTREEITEM hRootAppItem = m_bcdStoreTree.InsertItem(&treeItemAppObjs);

	TVINSERTSTRUCT itemInheritableObj;
	memset(&itemInheritableObj, 0, sizeof(itemInheritableObj));
	itemInheritableObj.item.mask = TVIF_TEXT | TVIF_PARAM;// | TVIF_CHILDREN;
	//treeItem.item.cChildren = I_CHILDRENCALLBACK;
	itemInheritableObj.item.pszText = L"Inheritable Objects";
	itemInheritableObj.item.cchTextMax = wcslen(treeItem.item.pszText);
	itemInheritableObj.item.lParam = (LPARAM)nullptr;
	itemInheritableObj.hInsertAfter = hRootAppItem;
	itemInheritableObj.hParent = hRootItem;
	HTREEITEM hRootInheritableItem = m_bcdStoreTree.InsertItem(&itemInheritableObj);

	TVINSERTSTRUCT itemDeviceObj;
	memset(&itemDeviceObj, 0, sizeof(itemDeviceObj));
	itemDeviceObj.item.mask = TVIF_TEXT | TVIF_PARAM;// | TVIF_CHILDREN;
	//treeItem.item.cChildren = I_CHILDRENCALLBACK;
	itemDeviceObj.item.pszText = L"Device Objects";
	itemDeviceObj.item.cchTextMax = wcslen(treeItem.item.pszText);
	itemDeviceObj.item.lParam = (LPARAM)nullptr;
	itemDeviceObj.hInsertAfter = hRootInheritableItem;
	itemDeviceObj.hParent = hRootItem;
	HTREEITEM hRootDeviceItem = m_bcdStoreTree.InsertItem(&itemDeviceObj);

	for (int i = 0; i < vecBcdObjects.size(); i++)
	{
		BcdObject* pBcdObject = vecBcdObjects[i];
		std::wstring wstrBcdObjectId;
		if (!pBcdObject->GetBcdObjectId(wstrBcdObjectId))
		{
			continue;
		}
		TVINSERTSTRUCT treeItemTmp;
		memset(&treeItemTmp, 0, sizeof(treeItemTmp));
		treeItemTmp.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_CHILDREN;
		//treeItemTmp.item.cChildren = I_CHILDRENCALLBACK;
		if (!wcscmp(wstrBcdObjectId.c_str(), Windows_Boot_Manager_GUID))
		{
			treeItemTmp.item.pszText = L"Windows Boot Manager";
			treeItemTmp.item.cchTextMax = wcslen(treeItem.item.pszText);
			treeItemTmp.item.lParam = (LPARAM)pBcdObject;
			treeItemTmp.hParent = hRootItem;
			treeItemTmp.hInsertAfter = TVI_FIRST;
			m_bcdStoreTree.InsertItem(&treeItemTmp);
			continue;
		}
		else if (!wcscmp(wstrBcdObjectId.c_str(), Firmware_Boot_Manager_GUID))
		{
			treeItemTmp.item.pszText = L"Firmware Boot Manager";
			treeItemTmp.item.cchTextMax = wcslen(treeItem.item.pszText);
			treeItemTmp.item.lParam = (LPARAM)pBcdObject;
			treeItemTmp.hParent = hRootItem;
			treeItemTmp.hInsertAfter = TVI_FIRST;
			m_bcdStoreTree.InsertItem(&treeItemTmp);
			continue;
		}
		
		wchar_t* pwszBcdObjectDescription = (wchar_t*)wstrBcdObjectId.c_str();
		if (!_wcsicmp(pwszBcdObjectDescription, Standard_Inheritable_badmemory_GUID))
		{
			pwszBcdObjectDescription = L"badmemory";
		}
		else if (!_wcsicmp(pwszBcdObjectDescription, Standard_Inheritable_bootloadersettings_GUID))
		{
			pwszBcdObjectDescription = L"bootloadersettings";
		}
		else if (!_wcsicmp(pwszBcdObjectDescription, Standard_Inheritable_dbgsettings_GUID))
		{
			pwszBcdObjectDescription = L"dbgsettings";
		}
		else if (!_wcsicmp(pwszBcdObjectDescription, Standard_Inheritable_emssetting_GUID))
		{
			pwszBcdObjectDescription = L"emssetting";
		}
		else if (!_wcsicmp(pwszBcdObjectDescription, Standard_Inheritable_globalsettings_GUID))
		{
			pwszBcdObjectDescription = L"globalsettings";
		}
		else if (!_wcsicmp(pwszBcdObjectDescription, Standard_Inheritable_resumeloadersettings_GUID))
		{
			pwszBcdObjectDescription = L"resumeloadersettings";
		}
		else if (!_wcsicmp(pwszBcdObjectDescription, Standard_Inheritable_hypervisorsettings_GUID))
		{
			pwszBcdObjectDescription = L"hypervisorsettings";
		}
		treeItemTmp.item.pszText = pwszBcdObjectDescription;
		std::wstring wstrDescription;
		if (pBcdObject->GetBcdObjectDescription(wstrDescription))
		{
			treeItemTmp.item.pszText = (wchar_t*)wstrDescription.c_str();
		}
		
		treeItemTmp.item.cchTextMax = wcslen(treeItem.item.pszText);
		treeItemTmp.item.lParam = (LPARAM)pBcdObject;
		//treeItemTmp.hInsertAfter = TVI_FIRST;		
		ObjectCode objCode = pBcdObject->GetObjectCode();
		if (objCode != Application && objCode != Inheritable && objCode != Device)
		{
			//todo: 还需要研究
			continue;
		}
		if (objCode == Application)
		{
			treeItemTmp.hParent = hRootAppItem;
		}
		else if (objCode == Inheritable)
		{
			treeItemTmp.hParent = hRootInheritableItem;
		}
		else if (objCode == Device)
		{			
			treeItemTmp.hParent = hRootDeviceItem;
		}		
		m_bcdStoreTree.InsertItem(&treeItemTmp);
	}
	
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

