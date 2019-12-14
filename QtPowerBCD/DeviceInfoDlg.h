#pragma once

#include <QtWidgets>

class CDeviceInfoDlg : public QDialog
{
	Q_OBJECT;
public:
	CDeviceInfoDlg(QWidget* parent = NULL);
private:
	enum DeviceBtnId
	{
		DeviceBtnId_Part = 0,
		DeviceBtnId_RamDisk = 1,
		DeviceBtnId_VHD = 2
	};
	QButtonGroup m_btnGroup;
	QComboBox* m_cbBootDisk;
	QComboBox* m_cbBootPart;

	QLineEdit* m_leDeviceFile;
	QComboBox* m_cbRamdisk;
	QVBoxLayout* m_mainVBL;
};
