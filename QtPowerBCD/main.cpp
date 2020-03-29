#include <QApplication>
#include "MainWdg.h"
#include "Disk.h"
int main(int argc, char *argv[])
{
	char pwszSerialNum[256];
	char pwszModelNum[256];
	char pwszRevisionNum[256];
	CDisk disk;
	disk.OpenDisk(L"\\Device\\HarddiskVolume4");
	disk.GetDiskSerialNumber(0, pwszSerialNum,  pwszModelNum,  pwszRevisionNum);
	wchar_t wszLetters[MAX_PATH] = { 0 };
	int letterCount = 0;
	disk.GetLetters(wszLetters, letterCount);
	QApplication a(argc, argv);
	CWimBase* pWimBase = getWimBase();
	HRESULT hres = pWimBase->init();
	CMainWdg mainWdg;
	mainWdg.show();
	return a.exec();
}
