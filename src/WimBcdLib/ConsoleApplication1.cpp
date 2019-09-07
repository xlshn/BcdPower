

#include "pch.h"
#include <iostream>
#define _WIN32_DCOM

#include <wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")
#include "BcdStore.h"
int main()
{
	CWimBase* pbb = getWimBase();
	pbb->init();
	BcdStore* bss = NULL;
	bool bx =  BcdStore::OpenStore(L"C:\\Users\\wsmh0\\Desktop\\bcd_bios", bss);
	std::wstring p = bss->getFilePath();
	
	bss->GetSystemDisk(p);
	HRESULT hr;
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		std::cout << "Failed to initialize COM library. Error code = 0x"
			<< hr << hr << std::endl;
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
		std::cout << "Failed to initialize security. Error code = 0x"
			<< hr << hr << std::endl;
		CoUninitialize();
		return hr;                  // Program has failed.
	}

	IWbemLocator *pLoc = 0;
	hr = CoCreateInstance(CLSID_WbemLocator, 0,
		CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hr))
	{
		std::cout << "Failed to create IWbemLocator object. Err code = 0x"
			<< hr << hr << std::endl;
		CoUninitialize();
		return hr;     // Program has failed.
	}

	IWbemServices *pSvc = 0;
	// Connect to the root\default namespace with the current user.
	hr = pLoc->ConnectServer(
		BSTR(L"ROOT\\WMI"),  //namespace
		NULL,       // User name 
		NULL,       // User password
		0,         // Locale 
		NULL,     // Security flags
		0,         // Authority 
		0,        // Context object 
		&pSvc);   // IWbemServices proxy


	if (FAILED(hr))
	{
		std::cout << "Could not connect. Error code = 0x"
			<< hr << hr << std::endl;
		pLoc->Release();
		CoUninitialize();
		return hr;      // Program has failed.
	}

	IWbemClassObject* pBcdStoreClass = NULL;
	hr = (WBEMSTATUS)pSvc->GetObject(BSTR(L"BcdStore"), 0, NULL, &pBcdStoreClass, NULL);
	VARIANT var;
	CIMTYPE type;
	LONG l;
	hr = pBcdStoreClass->Get(BSTR(L"__RELPATH"), 0, &var, &type, &l);
	IWbemClassObject* pOpenStoreMethod = NULL;
	hr = pBcdStoreClass->GetMethod(BSTR(L"OpenStore"), 0, &pOpenStoreMethod, NULL);
	VARIANT varTmp;
	varTmp.vt = VT_BSTR;
	varTmp.bstrVal = BSTR(L"C:\\Users\\wsmh0\\Desktop\\BCD_tmp");
	hr = pOpenStoreMethod->Put(BSTR(L"File"), 0, &varTmp, 0);
	IWbemClassObject* pBcdStoreClassInstanceOutPut = NULL;
	hr = pSvc->ExecMethod(BSTR(L"BcdStore"), BSTR(L"OpenStore"), 0, NULL, pOpenStoreMethod, &pBcdStoreClassInstanceOutPut, NULL);
	VARIANT varBcdStoreClassInsOutPut;
	hr = pBcdStoreClassInstanceOutPut->Get(BSTR(L"ReturnValue"), 0, &varBcdStoreClassInsOutPut, &type, &l);
	hr = pBcdStoreClassInstanceOutPut->Get(BSTR(L"Store"), 0, &varBcdStoreClassInsOutPut, &type, &l);
	IWbemClassObject* pBcdStoreClassInstance = (IWbemClassObject*)varBcdStoreClassInsOutPut.byref;
	VARIANT varBcdStorePath;
	pBcdStoreClassInstance->Get(L"__RELPATH", 0, &varBcdStorePath, &type, &l);
	IWbemClassObject* pOpenObjectMethod = NULL;
	hr = pBcdStoreClass->GetMethod(BSTR(L"OpenObject"), 0, &pOpenObjectMethod, NULL);
	VARIANT varObjectId;
	varObjectId.vt = VT_BSTR;
	varObjectId.bstrVal = BSTR(L"{b2721d73-1db4-4c62-bf78-c548a880142d}");
	hr = pOpenObjectMethod->Put(BSTR(L"Id"), 0, &varObjectId, 0);	
	IWbemClassObject* pOpenObjectOut = NULL; 
	hr = pSvc->ExecMethod(varBcdStorePath.bstrVal, BSTR(L"OpenObject"), 0, NULL, pOpenObjectMethod, &pOpenObjectOut,NULL);
	VARIANT varBcdObject;
	hr = pOpenObjectOut->Get(L"Object", 0, &varBcdObject, &type, &l);
	IWbemClassObject* pBcdObjectClass = NULL;
	pSvc->GetObject(BSTR(L"BcdObject"), 0, NULL, &pBcdObjectClass, NULL);
	//IWbemClassObject* pNew = NULL;
	//pBcdObjectClass->SpawnInstance(0, &pNew);
	IWbemClassObject* pBcdObjectInstance = (IWbemClassObject*)varBcdObject.byref;
	VARIANT varBcdObjectInstancePath;
	pBcdObjectInstance->Get(L"__RELPATH", 0, &varBcdObjectInstancePath, &type, &l);
	////////////////////////////////////////////////////////////////////////////////
	//
	IWbemClassObject* EnumerateElementsOutPut = NULL;
	hr = pSvc->ExecMethod(varBcdObjectInstancePath.bstrVal, BSTR(L"EnumerateElements"), 0, NULL, NULL, &EnumerateElementsOutPut, NULL);
	VARIANT varEnumerate;
	hr = EnumerateElementsOutPut->Get(L"Elements", 0, &varEnumerate, NULL, NULL);
	/////////////////////////////////////////////////////////////////////////////////
	IWbemClassObject** pTmp = (IWbemClassObject**)(varEnumerate.parray->pvData);
	
	VARIANT varTmp001;
	pTmp[2]->Get(L"StoreFilePath", 0, &varTmp001, NULL, NULL);
	
	IWbemClassObject* pGetElementIn = NULL;
	//hr = pBcdObjectClass->GetMethod(L"GetElement", 0, &pGetElementIn, NULL);
	hr = pBcdObjectClass->GetMethod(L"GetElementWithFlags", 0, &pGetElementIn, NULL);
	///////////////////////////////////////////////////////////////////////////////
	VARIANT varGetElementIn;
	varGetElementIn.uintVal = 0x11000001;
	varGetElementIn.vt = VT_I4;
	pGetElementIn->Put(BSTR(L"Type"), 0,&varGetElementIn, 0);
	
	VARIANT varGetElementFlags;
	varGetElementFlags.vt = VT_I4;
	varGetElementFlags.uintVal = 1;
	pGetElementIn->Put(BSTR(L"Flags"), 0, &varGetElementFlags, 0);
	IWbemClassObject* pGetElementOutput = NULL;
	hr = pSvc->ExecMethod(varBcdObjectInstancePath.bstrVal, BSTR(L"GetElementWithFlags"), 0, NULL, pGetElementIn, &pGetElementOutput, NULL);
	VARIANT varElement;
	hr = pGetElementOutput->Get(L"Element", 0, &varElement, &type, &l);
	IWbemClassObject* pElement11000001 = (IWbemClassObject*)varElement.byref;

	VARIANT varTopClassName;
	hr = pElement11000001->Get(BSTR(L"__Class"), 0, &varTopClassName, &type, &l);
	VARIANT varStoreFilePath;
	hr = pElement11000001->Get(L"StoreFilePath", 0, &varStoreFilePath, &type, &l);

	VARIANT varTypeTmp;
	hr = pElement11000001->Get(L"Type", 0, &varTypeTmp, &type, &l);

	VARIANT varObjectIdTmp;
	hr = pElement11000001->Get(L"ObjectId", 0, &varObjectIdTmp, &type, &l);

	VARIANT varDevice;
	hr = pElement11000001->Get(L"Device", 0, &varDevice, &type, &l);

	IWbemClassObject* pDeviceObject = (IWbemClassObject*)varDevice.byref;

	VARIANT verDeviceType;
	hr = pDeviceObject->Get(L"DeviceType", 0, &verDeviceType, &type, &l);

	hr = pDeviceObject->Get(BSTR(L"__Class"), 0, &varTopClassName, &type, &l);
	VARIANT verAdditionalOptions;
	hr = pDeviceObject->Get(L"AdditionalOptions", 0, &verAdditionalOptions, &type, &l);
	VARIANT verParent;
	hr = pDeviceObject->Get(L"Parent", 0, &verParent, &type, &l);

	IWbemClassObject* pDeviceObjectParent = (IWbemClassObject*)verParent.byref;
	VARIANT verParentDeviceType;
	hr = pDeviceObjectParent->Get(L"DeviceType", 0, &verParentDeviceType, &type, &l);

	VARIANT verPartPath;
	hr = pDeviceObjectParent->Get(L"Path", 0, &verPartPath, &type, &l);
	system("pause");
	
}
