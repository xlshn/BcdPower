
// BcdDetailDlg.h : header file
//

#pragma once
#include <vector>

// CBcdDetailDlg dialog
class CBcdDetailDlg : public CDialogEx
{
// Construction
public:
	CBcdDetailDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BCDDETAIL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg  void OnOpenBcdStore();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_bcdStoreTree;
private:
	std::vector<BcdStore*> vecBcdStore;
	void ExpandAllTree(HTREEITEM hTreeItem);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
public:
	CListCtrl m_listBcdObjectDetail;	
	afx_msg void OnTvnSelchangedTreeBcdSotre(NMHDR *pNMHDR, LRESULT *pResult);
};
