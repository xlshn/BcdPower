#pragma once
#include <Windows.h>
#include <time.h>
#include "DiskIo.h"
class CDisk
{
public:
    CDisk()
    {
        m_hDisk = INVALID_HANDLE_VALUE;
    }
    bool OpenDisk(int diskIndex);
    //\Device\HarddiskVolume4
    bool OpenDisk(wchar_t pwszDiskDevice[MAX_PATH]);
    bool unInit();

    bool GetDiskSerialNumber(int diskIndex, char pwszSerialNum[256], char pwszModelNum[256], char pwszRevisionNum[256]);
    bool GetLetters(wchar_t wszLetters[MAX_PATH], int& letterCount);
private:
    enum Init_By
    {
        Init_By_Index,
        Init_By_Device
    };
    Init_By m_initBy;
    HANDLE m_hDisk;
    wchar_t m_pwszSymbolicLinkDisk[MAX_PATH] = { 0 };
    void PrintIdeInfo(DWORD* dwDiskData, char pwszSerialNum[256], char pwszModelNum[256], char pwszRevisionNum[256]);
    void ConvertToString(DWORD dwDiskData[256], int iFirstIndex, int iLastIndex, char* pcszBuf);
};