#include "DiskIo.h"
#include <time.h>

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

bool CDisk::GetDiskSerialNumber(int diskIndex, char pwszSerialNum[256], char pwszModelNum[256], char pwszRevisionNum[256])
{	
	if (m_hDisk == INVALID_HANDLE_VALUE)
	{
		return false;
	}

    bool bResult = false;
    do
    { 
        GETVERSIONINPARAMS GetVersionParams;
        DWORD dwBytesReturned = 0;

        // Get the version, etc of PhysicalDrive IOCTL
        memset((void*)&GetVersionParams, 0, sizeof(GetVersionParams));

        if (!DeviceIoControl(m_hDisk, SMART_GET_VERSION,
            NULL,
            0,
            &GetVersionParams, sizeof(GETVERSIONINPARAMS),
            &dwBytesReturned, NULL))
        {
            break;
        }
        ULONG CommandSize = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
        PSENDCMDINPARAMS Command = (PSENDCMDINPARAMS)malloc(CommandSize);
        // Retrieve the IDENTIFY data
        // Prepare the command
#define ID_CMD          0xEC            // Returns ID sector for ATA
        Command->irDriveRegs.bCommandReg = ID_CMD;
        DWORD BytesReturned = 0;
        if (!DeviceIoControl(m_hDisk,
            SMART_RCV_DRIVE_DATA, Command, sizeof(SENDCMDINPARAMS),
            Command, CommandSize,
            &BytesReturned, NULL))
        {
            break;
        }
       
        // Print the IDENTIFY data
        DWORD dwDiskData[256];
        USHORT* punIdSector = (USHORT*)
            (PIDENTIFY_DATA)((PSENDCMDOUTPARAMS)Command)->bBuffer;

        for (int iIjk = 0; iIjk < 256; iIjk++)
            dwDiskData[iIjk] = punIdSector[iIjk];

        PrintIdeInfo(dwDiskData, pwszSerialNum, pwszModelNum, pwszRevisionNum);
            
        // Done
          
        free(Command);
        bResult = true;
    } while (0);
	return bResult;
}
struct DISKSIGN
{
    PARTITION_STYLE partStyle;
    GUID    diskGuid;
};
bool CDisk::GetLetters(wchar_t wszLetters[MAX_PATH], int &letterCount)
{   
    letterCount = 0;
   // DWORD dwLetters = GetLogicalDrives();
    wchar_t wszLettersTmp[MAX_PATH] = { 0 };
    GetLogicalDriveStrings(MAX_PATH, wszLettersTmp);    
    DWORD dwOutBufferSize = sizeof(DRIVE_LAYOUT_INFORMATION_EX) + sizeof(PARTITION_INFORMATION_EX) * 100;
    DRIVE_LAYOUT_INFORMATION_EX* layoutEx = (DRIVE_LAYOUT_INFORMATION_EX*)new char[dwOutBufferSize];
    DWORD dwBytesReturned = 0;
    BOOL bRes = DeviceIoControl(m_hDisk, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, layoutEx, dwOutBufferSize, &dwBytesReturned, NULL);
    if (!bRes)
    {
        delete layoutEx;
        return false;       
    }

   
    DISKSIGN thisDiskSign;
    memset(&thisDiskSign, 0, sizeof(DISKSIGN));
    thisDiskSign.partStyle = (PARTITION_STYLE)layoutEx->PartitionStyle;
    if (thisDiskSign.partStyle == PARTITION_STYLE_GPT)
    {
        thisDiskSign.diskGuid = layoutEx->Gpt.DiskId;
    }
    else
    {
        thisDiskSign.diskGuid.Data1 = layoutEx->Mbr.Signature;
    }
   

    wchar_t* tmpWszLettersTmp = wszLettersTmp;
    while (int i = wcslen(tmpWszLettersTmp))
    {       
        wchar_t pwszPath[MAX_PATH] = { 0 };
        wsprintf(pwszPath, L"\\\\?\\%c:", tmpWszLettersTmp[0]);
        HANDLE hVol = CreateFile(pwszPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        
        if (hVol == INVALID_HANDLE_VALUE)
        {
            tmpWszLettersTmp += (i + 1);
            continue;
        }       

        bRes = DeviceIoControl(hVol, IOCTL_DISK_GET_DRIVE_LAYOUT_EX, NULL, 0, layoutEx, dwOutBufferSize, &dwBytesReturned, NULL);
        CloseHandle(hVol);
        if (!bRes)
        {   
            tmpWszLettersTmp += (i + 1);
            continue;
        }
       
        DISKSIGN thisDiskSignTmp;
        memset(&thisDiskSignTmp, 0, sizeof(DISKSIGN));
        thisDiskSignTmp.partStyle = (PARTITION_STYLE)layoutEx->PartitionStyle;
        if (thisDiskSignTmp.partStyle == PARTITION_STYLE_GPT)
        {
            thisDiskSignTmp.diskGuid = layoutEx->Gpt.DiskId;
        }
        else
        {
            thisDiskSignTmp.diskGuid.Data1 = layoutEx->Mbr.Signature;
        }

        if (!memcmp(&thisDiskSign, &thisDiskSignTmp, sizeof(DISKSIGN)))
        {
            wszLetters[letterCount++] = tmpWszLettersTmp[0];
        }

        tmpWszLettersTmp += (i + 1);
    }

    delete layoutEx;
    return true;
}

void CDisk::PrintIdeInfo(DWORD* dwDiskData, char pwszSerialNum[256], char pwszModelNum[256], char pwszRevisionNum[256])
{
    //  copy the hard iDrive serial number to the cszBuffer
    ConvertToString(dwDiskData, 10, 19, pwszSerialNum);
    ConvertToString(dwDiskData, 27, 46, pwszModelNum);
    ConvertToString(dwDiskData, 23, 26, pwszRevisionNum);
}
void CDisk::ConvertToString(DWORD dwDiskData[256],
    int iFirstIndex,
    int iLastIndex,
    char* pcszBuf)
{
    int iIndex = 0;
    int iPosition = 0;

    //  each integer has two characters stored in it backwards

    // Removes the spaces from the serial no
    for (iIndex = iFirstIndex; iIndex <= iLastIndex; iIndex++)
    {
        //  get high byte for 1st character	
        char ctemp = (char)(dwDiskData[iIndex] / 256);
        char cszmyspace[] = " ";
        //if (!(ctemp == *cszmyspace))
        {
            pcszBuf[iPosition++] = ctemp;
        }
        //  get low byte for 2nd character
        char ctemp1 = (char)(dwDiskData[iIndex] % 256);
       // if (!(ctemp1 == *cszmyspace))
        {
            pcszBuf[iPosition++] = ctemp1;
        }
    }

    //  end the string 
    pcszBuf[iPosition] = '\0';

    //  cut off the trailing blanks
    for (iIndex = iPosition - 1; iIndex > 0 && isspace(pcszBuf[iIndex]); iIndex--)
        pcszBuf[iIndex] = '\0';

    //return pcszBuf;
}

bool CDisk::init(int diskIndex)
{
    m_initBy = Init_By_Index;
    m_hDisk = DiskIO::OpenDisk(diskIndex);
    return m_hDisk == INVALID_HANDLE_VALUE ? false : true;
}
bool CDisk::init(wchar_t pwszDiskDevice[MAX_PATH])
{
    m_initBy = Init_By_Device;
    
    wsprintf(m_pwszSymbolicLinkDisk, L"\\\\?\\SymbolicLinkDisk%I64d", time(NULL));
    BOOL bRes = DefineDosDevice(DDD_RAW_TARGET_PATH, &m_pwszSymbolicLinkDisk[4], pwszDiskDevice);
    m_hDisk = DiskIO::OpenDisk(m_pwszSymbolicLinkDisk);
    return m_hDisk == INVALID_HANDLE_VALUE ? false : true;
}

bool CDisk::unInit()
{
    if (m_hDisk != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hDisk);
    }
    if (m_initBy == Init_By_Device)
    {
        BOOL bRes = DefineDosDevice(DDD_REMOVE_DEFINITION, &m_pwszSymbolicLinkDisk[4], m_pwszSymbolicLinkDisk);
    }
    return true;
}

