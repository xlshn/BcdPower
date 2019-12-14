#include "MainWdg.h"
#include "DeviceInfoDlg.h"

#define BCDOBJDATA		Qt::UserRole + 1
#define BCDSTOREDATA	Qt::UserRole + 2
#define BCDITEMEDATA	Qt::UserRole + 3
#define BCD_ELE_TYPE_COLUMN 0
#define BCD_ELE_NAME_COLUMN 1
#define BCD_ELE_VALUE_COLUMN 2

CMainWdg::CMainWdg(QWidget* parent):QWidget(parent)
{
	setMinimumSize(915, 550);
	m_mainVBL = new QVBoxLayout(this);
	m_mainVBL->setContentsMargins(0, 0, 0, 0);
	m_mainVBL->setSpacing(0);

	m_mainMenuBar = new QMenuBar(this);
	QMenu* pMenu = m_mainMenuBar->addMenu(QString::fromWCharArray(L"BcdStore"));

	QAction *actOpenSysBcd = pMenu->addAction(QString::fromWCharArray(L"OpenCurrentSystemBcdStore"));
	actOpenSysBcd->setData(ActTypeSysBcd);
	QAction* actOpenBcd = pMenu->addAction(QString::fromWCharArray(L"OpenBcdStore"));
	actOpenBcd->setData(ActTypeBcd);

	m_bcdStoreTree = new QTreeWidget;	
	m_bcdStoreTree->setHeaderHidden(true);

	m_bcdElementTree = new QTreeWidget;
	m_bcdElementTree->setHeaderLabels(QStringList() << QString::fromWCharArray(L"Element Type") << QString::fromWCharArray(L"Element Name") << QString::fromWCharArray(L"Element Value"));
	m_bcdElementTree->setRootIsDecorated(false);
	//m_bcdElementTree->setStyleSheet("QTreeWidget::item{color: black;border: none; border-top: 1px solid gray;min-height: 30px}::item:selected{background-color: gray}");
	m_bcdElementTree->setStyleSheet("QTreeWidget::item{min-height: 30px}");

	//m_bcdElementTree->setHit
	QHBoxLayout* hblBcd = new QHBoxLayout;
	hblBcd->setContentsMargins(0, 0, 0, 0);
	hblBcd->setSpacing(0);
	hblBcd->addWidget(m_bcdStoreTree);
	hblBcd->addSpacing(2);
	hblBcd->addWidget(m_bcdElementTree);
	//hblBcd->addStretch();
	hblBcd->setStretch(0, 305);
	hblBcd->setStretch(2, 600);

	m_lbBcdObjGuid = new QLabel;
	m_lbBcdObjGuid->setTextInteractionFlags(Qt::TextSelectableByMouse);
	QLabel* lbBcdObjGuidKey = new QLabel(QString::fromWCharArray(L"Bcd Object Guid:"));
	QHBoxLayout* hblBcdObjGuid = new QHBoxLayout;
	hblBcdObjGuid->addWidget(lbBcdObjGuidKey);
	hblBcdObjGuid->addWidget(m_lbBcdObjGuid);
	hblBcdObjGuid->addStretch();

	m_mainVBL->addWidget(m_mainMenuBar);
	m_mainVBL->addLayout(hblBcd);
	m_mainVBL->addSpacing(10);
	m_mainVBL->addLayout(hblBcdObjGuid);
	m_mainVBL->addSpacing(10);
	//m_mainVBL->addWidget(m_lBcdObjInfo);
	//m_mainVBL->addStretch();

	connect(pMenu, SIGNAL(triggered(QAction*)), this, SLOT(onMenuTriggered(QAction*)));
	connect(m_bcdStoreTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onAppItemClicked(QTreeWidgetItem*, int)));	
	connect(m_bcdElementTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onElementItemClicked(QTreeWidgetItem*, int)));
}
QString CMainWdg::GetAppFriendNameByAppGuid(BcdStore* pBcdStore, const std::wstring& wstrGuid)
{
	std::vector<BcdObject*> vecApp = pBcdStore->getBcdObject();
	for (size_t i = 0; i < vecApp.size(); i++)
	{
		BcdObject* app = vecApp[i];
		if (app->GetBcdObjectId() != wstrGuid)
		{
			continue;
		}
		std::wstring appDescrib;
		app->GetBcdObjectDescription(appDescrib);
		if (appDescrib.length() == 0)
		{
			return QString::fromWCharArray(wstrGuid.c_str());
		}

		return QString::fromWCharArray(appDescrib.c_str());
	}

	return QString::fromWCharArray(wstrGuid.c_str());
}
void CMainWdg::onOpenSystemBcd()
{
	openBcdStore(L"");
}
bool CMainWdg::openBcdStore(const std::wstring& wstrBcdPath)
{
	BcdStore* pBcdStore = NULL;
	bool bRes = BcdStore::OpenStore(wstrBcdPath, pBcdStore);
	if (!bRes)
	{
		//::MessageBox(m_hWnd, L"Open bcd fail fail", L"Error", MB_OK);
		return false;
	}
	m_mapBcdStore[QString::fromWCharArray(wstrBcdPath.c_str())] = pBcdStore;
	vecBcdStore.push_back(pBcdStore);
	std::vector<BcdObject*> vecBcdObjects;
	pBcdStore->EnumerateObjects(0, vecBcdObjects);
	m_itemBcdStore = new QTreeWidgetItem;
	m_itemBcdStore->setText(0, QString::fromWCharArray(L"BcdStore"));
	m_bcdStoreTree->addTopLevelItem(m_itemBcdStore);

	m_itemAppObjs = new QTreeWidgetItem;
	m_itemAppObjs->setText(0, QString::fromWCharArray(L"Application Objects"));
	m_itemBcdStore->addChild(m_itemAppObjs);

	m_itemInheritableObj = new QTreeWidgetItem;
	m_itemInheritableObj->setText(0, QString::fromWCharArray(L"Inheritable Objects"));
	m_itemBcdStore->addChild(m_itemInheritableObj);

	m_itemDeviceObj = new QTreeWidgetItem;
	m_itemDeviceObj->setText(0, QString::fromWCharArray(L"Device Objects"));
	m_itemBcdStore->addChild(m_itemDeviceObj);

	for (int i = 0; i < vecBcdObjects.size(); i++)
	{
		BcdObject* pBcdObject = vecBcdObjects[i];
		std::wstring wstrBcdObjectId = pBcdObject->GetBcdObjectId();
		if (wstrBcdObjectId.length() == 0)
		{
			continue;
		}
		//WM_SETFONT
		std::wstring wstrDescription;
		if (!pBcdObject->GetBcdObjectDescription(wstrDescription))
		{
			continue;
		}

		QTreeWidgetItem* itemObj = new QTreeWidgetItem;
		QVariant varBcdObj = QVariant::fromValue((void*)pBcdObject);
		QVariant varBcdStore = QVariant::fromValue((void*)pBcdStore);

		itemObj->setData(0, BCDOBJDATA, QVariant(varBcdObj));
		itemObj->setData(0, BCDSTOREDATA, QVariant(varBcdStore));
		if (!wcscmp(wstrBcdObjectId.c_str(), Windows_Boot_Manager_GUID))
		{
			wchar_t* pszText = (wchar_t*)wstrDescription.c_str();
			itemObj->setText(0, QString::fromWCharArray(pszText));
			m_itemBcdStore->insertChild(0, itemObj);
			continue;
		}
		else if (!wcscmp(wstrBcdObjectId.c_str(), Firmware_Boot_Manager_GUID))
		{
			wchar_t* pszText = (wchar_t*)wstrDescription.c_str();
			itemObj->setText(0, QString::fromWCharArray(pszText));
			m_itemBcdStore->insertChild(0, itemObj);
			continue;
		}
		wchar_t* pszText = (wchar_t*)wstrDescription.c_str();
		itemObj->setText(0, QString::fromWCharArray(pszText));	
		ObjectCode objCode = pBcdObject->GetObjectCode();
		if (objCode != Application && objCode != Inheritable && objCode != Device)
		{
			//todo: 还需要研究
			continue;
		}
		if (objCode == Application)
		{
			m_itemAppObjs->insertChild(0, itemObj);
		}
		else if (objCode == Inheritable)
		{
			m_itemInheritableObj->insertChild(0, itemObj);
		}
		else if (objCode == Device)
		{
			m_itemDeviceObj->insertChild(0, itemObj);
		}
	}
}


void CMainWdg::onMenuTriggered(QAction* act)
{
	ActType actType = (ActType)act->data().toInt();
	switch (actType)
	{
	case CMainWdg::ActTypeSysBcd:
		onOpenSystemBcd();
		break;
	case CMainWdg::ActTypeBcd:
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", "bcd file (*bcd*)");
		if (fileName.length() == 0)
		{
			return;
		}
		wchar_t pwszFileName[MAX_PATH] = { 0 };
		fileName.toWCharArray(pwszFileName);
		openBcdStore(pwszFileName);
		break;
	}
	default:
		break;
	}

	m_bcdStoreTree->expandAll();
}

void CMainWdg::onAppItemClicked(QTreeWidgetItem* item, int column)
{
	while(QTreeWidgetItem* itemTmp = m_bcdElementTree->takeTopLevelItem(0))
	{
		BcdElement* pEle = (BcdElement*)item->data(0, BCDITEMEDATA).value<void*>();
		if (pEle != NULL)
		{
			delete pEle;
		}
	}
	m_bcdElementTree->clear();
	m_lbBcdObjGuid->setText("");
	BcdObject* pBcdObj = (BcdObject*)item->data(0, BCDOBJDATA).value<void*>();
	if (pBcdObj == NULL)
	{
		return;
	}
	BcdStore* pBcdStore = (BcdStore*)item->data(0, BCDSTOREDATA).value<void*>();

	std::vector<BcdElement*> vecElement;
	pBcdObj->EnumerateElements(vecElement);
	for (size_t i = 0; i < vecElement.size(); i++)
	{
		BcdElement* pEle = vecElement[i];
		QTreeWidgetItem* eleItem = new QTreeWidgetItem;
		QVariant varBcdObj = QVariant::fromValue((void*)pBcdObj);
		QVariant varBcdStore = QVariant::fromValue((void*)pBcdStore);
		QVariant varBcdElement = QVariant::fromValue((void*)pEle);
		eleItem->setData(0, BCDOBJDATA, QVariant(varBcdObj));
		eleItem->setData(0, BCDSTOREDATA, QVariant(varBcdStore));
		eleItem->setData(0, BCDITEMEDATA, QVariant(varBcdElement));
		/*eleItem->setSizeHint(BCD_ELE_TYPE_COLUMN, QSize(20, 0));*/
		
		wchar_t pwszEleType[MAX_PATH] = { 0 };
		swprintf_s(pwszEleType, L"%08x", pEle->Type);
		eleItem->setText(BCD_ELE_TYPE_COLUMN, QString::fromWCharArray(pwszEleType));
		eleItem->setText(BCD_ELE_NAME_COLUMN, QString::fromWCharArray(BcdObject::getEleDisplay(pEle->Type).c_str()));
		
		switch (pEle->valueType)
		{
			case EleValueType_BcdObjectId:
			{
				BcdObjectElement* objEle = (BcdObjectElement*)pEle;
				QString qstrAppDescribe = GetAppFriendNameByAppGuid(pBcdStore, objEle->Id);
				eleItem->setText(BCD_ELE_VALUE_COLUMN, qstrAppDescribe);
				break;
			}
			
			case EleValueType_BcdObjectIdArray:
			{
				BcdObjectListElement* objEleList = (BcdObjectListElement*)pEle;
				std::vector<std::wstring> vecAppGuid = objEleList->getObjectListElement();
				QString qstrAppDescribe;
				for (size_t i = 0; i < vecAppGuid.size(); i++)
				{
					std::wstring wstrGuid = vecAppGuid[i];
					QString qstrAppDescribeTmp = GetAppFriendNameByAppGuid(pBcdStore, wstrGuid);
					if (i != 0)
					{
						qstrAppDescribe += ", ";
					}
					qstrAppDescribe += qstrAppDescribeTmp;
				}
				eleItem->setText(BCD_ELE_VALUE_COLUMN, qstrAppDescribe);
				break;
			}
			default:
			{
				eleItem->setText(BCD_ELE_VALUE_COLUMN, QString::fromWCharArray(pEle->getValueString().c_str()));
			}
				break;
		}
		//pEle->release();
		
		m_bcdElementTree->addTopLevelItem(eleItem);
	}
	m_lbBcdObjGuid->setText(QString::fromWCharArray(pBcdObj->GetBcdObjectId().c_str()));
	m_bcdElementTree->resizeColumnToContents(BCD_ELE_TYPE_COLUMN);
	m_bcdElementTree->resizeColumnToContents(BCD_ELE_NAME_COLUMN);
	m_bcdElementTree->resizeColumnToContents(BCD_ELE_VALUE_COLUMN);

}

void CMainWdg::onElementItemClicked(QTreeWidgetItem* eleItem, int column)
{
	BcdElement* pEle = (BcdElement*)eleItem->data(0, BCDITEMEDATA).value<void*>();
	if (pEle == NULL)
	{
		return;
	}

	switch (pEle->valueType)
	{
	case EleValueType_Device:
	{
		CDeviceInfoDlg deviceDlg(this);
		deviceDlg.exec();
		break;
	}
	default:
		break;
	}

}