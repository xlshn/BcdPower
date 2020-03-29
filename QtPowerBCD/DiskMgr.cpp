#include "Disk.h"
#include "DiskMgr.h"


CDiskMgr::CDiskMgr()
{

}
void CDiskMgr::InitDiskMgr()
{
    for (int i = 0; i < 255; i++)
    {
        CDisk* pDisk = new CDisk;
        bool res = pDisk->OpenDisk(i);
    }
   
    
}