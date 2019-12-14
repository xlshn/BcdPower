#include "wmibase.h"

CWimBase::CWimBase()
{
	m_pSvc = NULL;
	m_pLoc = NULL;
}

HRESULT CWimBase::init()
{

	HRESULT hr;
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hr) && 0x80010106 != hr)
	{	
		return hr;
	}
	hr = CoInitializeSecurity(
		NULL,                        // Security descriptor    
		-1,                          // COM negotiates authentication service
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication level for proxies
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation level for proxies
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities of the client or server
		NULL);                       // Reserved

	if (FAILED(hr))
	{
		CoUninitialize();
		return hr;                  // Program has failed.
	}	
	hr = CoCreateInstance(CLSID_WbemLocator, 0,
		CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&m_pLoc);

	if (FAILED(hr))
	{
		CoUninitialize();
		return hr;     // Program has failed.
	}
	m_pSvc = 0;
	// Connect to the root\default namespace with the current user.
	hr = m_pLoc->ConnectServer(
		BSTR(L"ROOT\\WMI"),  //namespace
		NULL,       // User name 
		NULL,       // User password
		0,         // Locale 
		NULL,     // Security flags
		0,         // Authority 
		0,        // Context object 
		&m_pSvc);   // IWbemServices proxy

	if (FAILED(hr))
	{
		m_pLoc->Release();
		CoUninitialize();
		return hr;      // Program has failed.
	}
	return 0;
}

HRESULT CWimBase::unInit()
{
	
	if (m_pSvc != NULL)
	{
		m_pSvc->Release();
	}
	if (m_pLoc != NULL)
	{
		m_pLoc->Release();
	}
	CoUninitialize();
	return E_NOTIMPL;
}

IWbemServices * CWimBase::getSvc()
{

	return m_pSvc;
}

CWimBase * getWimBase()
{
	static CWimBase wb;
	return &wb;
}

IWbemServices * getSvc()
{
	return getWimBase()->getSvc();
}
