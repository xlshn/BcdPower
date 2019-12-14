#include "DeviceInfoDlg.h"

CDeviceInfoDlg::CDeviceInfoDlg(QWidget* parent):QDialog(parent)
{
	setWindowFlags( Qt::WindowCloseButtonHint | Qt::Dialog);
	m_mainVBL = new QVBoxLayout(this);
	QRadioButton* radioPartBtn = new QRadioButton(QString::fromWCharArray(L"Partition"));
	QRadioButton* radioRamdiskBtn = new QRadioButton(QString::fromWCharArray(L"RamDisk"));
	QRadioButton* radioVHDBtn = new QRadioButton(QString::fromWCharArray(L"VHD"));

	QLabel* lbRadioKey = new QLabel(QString::fromWCharArray(L"Device:"));
	QHBoxLayout* hblRadio = new QHBoxLayout;
	hblRadio->addWidget(lbRadioKey);
	hblRadio->addWidget(radioPartBtn);
	hblRadio->addWidget(radioRamdiskBtn);
	hblRadio->addWidget(radioVHDBtn);
	//hblRadio->addStretch();

	QHBoxLayout* hblBootDisk = new QHBoxLayout;
	QLabel* lbBootDisk = new QLabel(QString::fromWCharArray(L"Boot Disk:"));
	m_cbBootDisk = new QComboBox;
	hblBootDisk->addWidget(lbBootDisk);
	hblBootDisk->addWidget(m_cbBootDisk);
	//hblBootDisk->addStretch();

	QHBoxLayout* hblBootPart = new QHBoxLayout;
	QLabel* lbBootPart = new QLabel(QString::fromWCharArray(L"Boot Part:"));
	m_cbBootPart = new QComboBox;
	hblBootPart->addWidget(lbBootPart);
	hblBootPart->addWidget(m_cbBootPart);
	//hblBootPart->addStretch();

	QHBoxLayout* hblDeviceFile = new QHBoxLayout;
	QLabel* lbDeviceFile = new QLabel(QString::fromWCharArray(L"Device File"));
	m_leDeviceFile = new QLineEdit;
	hblDeviceFile->addWidget(lbDeviceFile);
	hblDeviceFile->addWidget(m_leDeviceFile);
	//hblDeviceFile->addStretch();

	QHBoxLayout* hblRamdisk = new QHBoxLayout;
	QLabel* lbRamdisk = new QLabel(QString::fromWCharArray(L"RamDisk"));
	m_cbRamdisk = new QComboBox;
	hblRamdisk->addWidget(lbRamdisk);
	hblRamdisk->addWidget(m_cbRamdisk);
	//hblRamdisk->addStretch();
	
	m_mainVBL->addLayout(hblRadio);
	m_mainVBL->addLayout(hblBootDisk);
	m_mainVBL->addLayout(hblBootPart);
	m_mainVBL->addLayout(hblDeviceFile);
	m_mainVBL->addLayout(hblRamdisk);

}
