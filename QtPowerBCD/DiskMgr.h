#pragma once
#include <Windows.h>
#include <vector>
#include "disk.h"

class CDiskMgr
{
public:
	CDiskMgr();
	void InitDiskMgr();
private:
	std::vector<CDisk*> m_vecDisk;
};