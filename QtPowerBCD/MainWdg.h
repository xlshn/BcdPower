#pragma once
#include <qtwidgets>
#include <vector>
#include "bcdstore.h"

class CMainWdg : public QWidget
{
	Q_OBJECT;
public:
	CMainWdg(QWidget* parent = NULL);
private:
	QMenuBar* m_mainMenuBar = nullptr;
	QVBoxLayout* m_mainVBL;
	QTreeWidget* m_bcdStoreTree;
	QTreeWidget* m_bcdElementTree;
	QTreeWidgetItem* m_itemBcdStore;
	QTreeWidgetItem* m_itemAppObjs;
	QTreeWidgetItem* m_itemInheritableObj;
	QTreeWidgetItem* m_itemDeviceObj;

	QLabel* m_lbBcdObjGuid;
	std::vector<BcdStore*> vecBcdStore;
	/////////////////////////////////////////////////////
	QString GetAppFriendNameByAppGuid(BcdStore* pBcdStore, const std::wstring& wstrGuid);
	/////////////////////////////////////////////////////
	QMap<QString, BcdStore*> m_mapBcdStore;
	void onOpenSystemBcd();
	bool openBcdStore(const std::wstring& wstrBcdPath);
	enum ActType
	{
		ActTypeSysBcd = 0,
		ActTypeBcd = 1
	};
private slots:
	void onMenuTriggered(QAction*); 
	void onAppItemClicked(QTreeWidgetItem*, int);
	void onElementItemClicked(QTreeWidgetItem*, int);
};