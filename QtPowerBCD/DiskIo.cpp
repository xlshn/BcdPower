#include "DiskIo.h"


HANDLE DiskIO::OpenDisk(int diskIndex)
{
	wchar_t pwszDiksPath[MAX_PATH] = { 0 };
	wsprintf(pwszDiksPath, L"\\\\.\\PhysicalDrive%d", diskIndex);
	return CreateFile(pwszDiksPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
}

HANDLE DiskIO::OpenDisk(wchar_t pwszDiskDevice[MAX_PATH])
{
    return CreateFile(pwszDiskDevice, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

}

