#pragma once

#include <Windows.h>
#define _WIN32_DCOM
#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

class CWimBase
{
public:
	CWimBase();
	HRESULT					init();
	HRESULT					unInit();
	IWbemServices*			getSvc();
private:
	IWbemLocator			*m_pLoc;
	IWbemServices			*m_pSvc;
};
CWimBase* getWimBase();
IWbemServices* getSvc();