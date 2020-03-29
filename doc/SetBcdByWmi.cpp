// WMITestProject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include <stdio.h>
#include <tchar.h>

#define _WIN32_DCOM

#include <iostream>
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
#include <windows.h>
#include <objbase.h>
#include <atlbase.h>
#include <iostream>
#include <wbemidl.h>
#include <comutil.h>


/// <summary>
/// flags for BCDCopy
/// </summary>
enum BcdFlags
{
	CreateNewId = 0x1,
	DeleteExistingObject = 0x02
};

enum BcdBootMgrElementTypes
{
	BcdBootMgrDevice_BcdDevice = 553648162,
	BcdBootMgrString_BcdFilePath = 570425379,
	BcdBootMgrObject_DefaultObject = 587202563,
	BcdBootMgrObject_ResumeObject = 587202566,
	BcdBootMgrObjectList_DisplayOrder = 603979777,
	BcdBootMgrObjectList_BootSequence = 603979778,
	BcdBootMgrObjectList_ToolsDisplayOrder = 603979792,
	BcdBootMgrInteger_Timeout = 620756996,
	BcdBootMgrBoolean_AttemptResume = 637534213,
	BcdBootMgrBoolean_NoErrorDisplay = 637534241,
};


enum BcdOSLoaderElementTypes
{
	BcdOSLoaderDevice_OSDevice = 553648129,
	BcdOSLoaderString_SystemRoot = 570425346,
	BcdOSLoaderString_KernelPath = 570425361,
	BcdOSLoaderString_HalPath = 570425362,
	BcdOSLoaderString_DbgTransportPath = 570425363,
	BcdOSLoaderString_OSLoaderTypeEVStore = 570425427,
	BcdOSLoaderString_HypervisorPath = 570425585,
	BcdOSLoaderObject_AssociatedResumeObject = 587202563,
	BcdOSLoaderInteger_NxPolicy = 620757024,
	BcdOSLoaderInteger_PAEPolicy = 620757025,
	BcdOSLoaderInteger_RemoveMemory = 620757041,
	BcdOSLoaderInteger_IncreaseUserVa = 620757042,
	BcdOSLoaderInteger_PerformaceDataMemory = 620757043,
	BcdOSLoaderInteger_ClusterModeAddressing = 620757072,
	BcdOSLoaderInteger_RestrictApicCluster = 620757074,
	BcdOSLoaderInteger_X2ApicPolicy = 620757077,
	BcdOSLoaderInteger_NumberOfProcessors = 620757089,
	BcdOSLoaderBoolean_ProcessorConfigurationFlags = 620757091,
	BcdOSLoaderBoolean_GroupSize = 620757094,
	BcdOSLoaderInteger_UseFirmwarePciSettings = 620757104,
	BcdOSLoaderInteger_SafeBoot = 620757120,
	BcdOSLoaderInteger_DriverLoadFailurePolicy = 620757185,
	BcdOSLoaderInteger_BootStatusPolicy = 620757216,
	BcdOSLoaderInteger_HypervisorLaunchType = 620757232,
	BcdOSLoaderInteger_HypervisorDebuggerType = 620757235,
	BcdOSLoaderInteger_HypervisorDebuggerPortNumber = 620757236,
	BcdOSLoaderInteger_HypervisorDebuggerBaudrate = 620757237,
	BcdOSLoaderInteger_HypervisorDebugger1394Channel = 620757238,
	BcdOSLoaderInteger_BootUxPolicy = 620757239,
	BcdOSLoaderInteger_TpmBootEntropyPolicy = 620757248,
	BcdOSLoaderBoolean_StampDisks = 637534212,
	BcdOSLoaderBoolean_DetectKernelAndHal = 637534224,
	BcdOSLoaderBoolean_WinPEMode = 637534242,
	BcdOSLoaderBoolean_DisableCrashAutoReboot = 637534244,
	BcdOSLoaderBoolean_UseLastGoodSettings = 637534245,
	BcdOSLoaderBoolean_DisableCodeIntegrityChecks = 637534246,
	BcdOSLoaderBoolean_AllowPrereleaseSignatures = 637534247,
	BcdOSLoaderBoolean_NoLowMemory = 637534256,
	BcdOSLoaderBoolean_UseVgaDriver = 637534272,
	BcdOSLoaderBoolean_DisableBootDisplay = 637534273,
	BcdOSLoaderBoolean_DisableVesaBios = 637534274,
	BcdOSLoaderBoolean_UsePhysicalDestination = 637534289,
	BcdOSLoaderBoolean_UseLegacyApicMode = 637534292,
	BcdOSLoaderBoolean_UseBootProcessorOnly = 637534304,
	BcdOSLoaderBoolean_ForceMaximumProcessors = 637534306,
	BcdOSLoaderBoolean_MaximizeGroupsCreated = 637534308,
	BcdOSLoaderBoolean_ForceGroupAwareness = 637534309,
	BcdOSLoaderBoolean_SafeBootAlternateShell = 637534337,
	BcdOSLoaderBoolean_BootLogInitialization = 637534352,
	BcdOSLoaderBoolean_VerboseObjectLoadMode = 637534353,
	BcdOSLoaderBoolean_KernelDebuggerEnabled = 637534368,
	BcdOSLoaderBoolean_DebuggerHalBreakpoint = 637534369,
	BcdOSLoaderBoolean_UsePlatformClock = 637534370,
	BcdOSLoaderBoolean_EmsEnabled = 637534384,
	BcdOSLoaderBoolean_HypervisorDebuggerEnabled = 637534450,
	BcdOSLoaderInteger_HypervisorSlatDisabled = 637534456,
	BcdOSLoaderInteger_HypervisorUseLargeVTlb = 637534460,
};

enum BcdLibraryElementTypes
{
	BcdLibraryDevice_ApplicationDevice = 285212673,
	BcdLibraryString_ApplicationPath = 301989890,
	BcdLibraryString_Description = 301989892,
	BcdLibraryString_PreferredLocale = 301989893,
	BcdLibraryString_UsbDebuggerTargetName = 301989910,
	BcdLibraryString_LoadOptionsString = 301989936,
	BcdLibraryObjectList_InheritedObjects = 335544326,
	BcdLibraryObjectList_RecoverySequence = 335544328,
	BcdLibraryInteger_TruncatePhysicalMemory = 352321543,
	BcdLibraryInteger_FirstMegabytePolicy = 352321548,
	BcdLibraryInteger_RelocatePhysicalMemory = 352321549,
	BcdLibraryInteger_DebuggerType = 352321553,
	BcdLibraryInteger_SerialDebuggerPortAddress = 352321554,
	BcdLibraryInteger_SerialDebuggerPort = 352321555,
	BcdLibraryInteger_SerialDebuggerBaudRate = 352321556,
	BcdLibraryInteger_1394DebuggerChannel = 352321557,
	BcdLibraryInteger_DebuggerStartPolicy = 352321560,
	BcdLibraryInteger_EmsPort = 352321570,
	BcdLibraryInteger_EmsBaudRate = 352321571,
	BcdLibraryInteger_FVEKeyRingAddress = 352321602,
	BcdLibraryInteger_ConfigAccessPolicy = 352321607,
	BcdLibraryInteger_SiPolicy = 352321611,
	BcdLibraryBoolean_AutoRecoveryEnabled = 369098761,
	BcdLibraryBoolean_AllowBadMemoryAccess = 369098763,
	BcdLibraryBoolean_DebuggerEnabled = 369098768,
	BcdLibraryBoolean_DebuggerIgnoreUsermodeExceptions = 369098775,
	BcdLibraryBoolean_EmsEnabled = 369098784,
	BcdLibraryBoolean_AttemptNonBcdStart = 369098801,
	BcdLibraryBoolean_DisplayAdvancedOptions = 369098816,
	BcdLibraryBoolean_DisplayOptionsEdit = 369098817,
	BcdLibraryBoolean_GraphicsModeDisabled = 369098822,
	BcdLibraryBoolean_DisableIntegrityChecks = 369098824,
	BcdLibraryBoolean_AllowPrereleaseSignatures = 369098825,
	BcdLibraryIntegerList_BadMemoryList = 385875978,

};

#define BCD_bootmgr					L"{9dea862c-5cdd-4e70-acc1-f32b344d4795}"
#define BCD_fbootmgr				L"{a5a30fa2-3d06-4e9f-b5f4-a01df9d1fcba}"
#define BCD_memdiag					L"{b2721d73-1db4-4c62-bf78-c548a880142d}"
#define BCD_resumeapplication		L"{147aa509-0358-4473-b83b-d950dda00615}"
#define BCD_ntldr					L"{466f5a88-0af2-4f76-9038-095b170dc21c}"
#define BCD_current					L"{fa926493-6f1c-4193-a414-58f0b2456d1e}"
#define BCD_badmemory				L"{5189b25c-5558-4bf2-bca4-289b11bd29e2}"
#define BCD_bootloadersettings		L"{6efb52bf-1766-41db-a6b3-0ee5eff72bd7}"
#define BCD_dbgsettings				L"{4636856e-540f-4170-a130-a84776f4c654}"
#define BCD_emssettings				L"{0ce4991b-e6b3-4b16-b23c-5e0d9250e5d9}"
#define BCD_globalsettings			L"{7ea2e1ac-2e61-4728-aaa3-896d9d0a9f0e}"
#define BCD_resumeloadersettings	L"{1afa9c49-16ab-4a5c-901b-212802da9460}"
#define BCD_hypervisordbgsettings	L"{7ff607e0-4395-11db-b0de-0800200c9a66}"




#if defined(UNICODE) || defined(_UNICODE)
#define tcout std::wcout
#else
#define tcout std::cout
#endif

# pragma comment(lib, "wbemuuid.lib")


int Test(void);

int _tmain(int argc, _TCHAR* argv[])
{
	return  Test();
}

///////////////////////////////////////////////////////////////////////////////
//
// Test BCDObjects
//
///////////////////////////////////////////////////////////////////////////////
int Test(void)
{
	IWbemServices* pSvc = NULL;
	IWbemLocator* pBemLocation = NULL;
	CIMTYPE cimType;
	long flavor;
	HRESULT hres = S_OK;

	///////////////////////////////////////////////////////////////////////////
	//
	//	init com and connect to root\\WMI
	// 
	///////////////////////////////////////////////////////////////////////////

	// Initialize COM. ------------------------------------------
	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		tcout << _T("Failed to initialize COM library. Error code = 0x") << hex << hres << endl;
		return hres;					// Program has failed.
	}

	// Set general COM security levels --------------------------
	hres = CoInitializeSecurity(
		NULL,
		-1,								// COM negotiates service
		NULL,							// Authentication services
		NULL,							// Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,		// Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE,	// Default Impersonation
		NULL,							// Authentication info
		EOAC_NONE,						// Additional capabilities 
		NULL							// Reserved
		);

	if (FAILED(hres))
	{
		tcout << _T("Failed to initialize security. Error code = 0x") << hex << hres << endl;
		CoUninitialize();
		return hres;					// Program has failed.
	}

	// Obtain the initial locator to WMI -------------------------
	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pBemLocation);

	if (FAILED(hres))
	{
		tcout << _T("Failed to create IWbemLocator object. Err code = 0x") << hex << hres << endl;
		CoUninitialize();
		return hres;				// Program has failed.
	}

	// Connect to the local root\cimv2 namespace
	// and obtain pointer pSvc to make IWbemServices calls.
	hres = pBemLocation->ConnectServer(
		_bstr_t(L"root\\wmi"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc
		);

	if (FAILED(hres))
	{
		tcout << _T("Could not connect. Error code = 0x") << hex << hres << endl;

		return hres;					// Program has failed.
	}

	hres = CoSetProxyBlanket(
		pSvc,							// Indicates the proxy to set
		RPC_C_AUTHN_WINNT,				// RPC_C_AUTHN_xxx 
		RPC_C_AUTHZ_NONE,				// RPC_C_AUTHZ_xxx 
		NULL,							// Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,			// RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE,	// RPC_C_IMP_LEVEL_xxx
		NULL,							// client identity
		EOAC_NONE						// proxy capabilities 
		);

	if (FAILED(hres))
	{
		tcout << _T("Could not set proxy blanket. Error code = 0x") << hex << hres << endl;
		return hres;					// Program has failed.
	}

	///////////////////////////////////////////////////////////////////////////
	//
	//  call BCDStore.OpenStore("")
	// 
	///////////////////////////////////////////////////////////////////////////

	// Create BCDStore class  Instance
	IWbemClassObject* pBCDStoreClass = NULL;
	BSTR bcdStoreClassName = SysAllocString(L"BCDStore");
	hres = pSvc->GetObject(bcdStoreClassName, 0, NULL, &pBCDStoreClass, NULL);
	if (FAILED(hres))
	{
		tcout << _T("Could not get BCDStore class. Error code = 0x") << hex << hres << endl;
		return hres;					// Program has failed.
	}

	// get the path of the BCDStore
	VARIANT bcdStorePath;
	hres = pBCDStoreClass->Get(L"__RELPATH", 0, &bcdStorePath, &cimType, &flavor);
	assert(hres == S_OK);

	// Get the parameters for OpenStore
	IWbemClassObject* pBCDStoreInParamsDefinition = NULL;
	hres = pBCDStoreClass->GetMethod(L"OpenStore", 0, &pBCDStoreInParamsDefinition, NULL);
	if (FAILED(hres))
	{
		tcout << _T("Could not get OpenStore method. Error code = 0x") << hex << hres << endl;
		return hres;					// Program has failed.
	}

	// spawn instance from pBCDStoreInParamsDefinition
	IWbemClassObject* pBCDStoreClassInstance = NULL;
	hres = pBCDStoreInParamsDefinition->SpawnInstance(0, &pBCDStoreClassInstance);
	if (FAILED(hres))
	{
		tcout << _T("Could not spawn BCDStore class instance. Error code = 0x")
			<< hex << hres << endl;
		return hres;					// Program has failed.
	}

	// Create the values for the in parameters
	VARIANT paramFile;
	paramFile.vt = VT_BSTR;
	paramFile.bstrVal = _bstr_t(L"");
	hres = pBCDStoreClassInstance->Put(L"File", 0, &paramFile, 0);
	assert(hres == S_OK);

	// execute the OpenStore method
	IWbemClassObject* pBCDStoreOutParamsDefinition = NULL;
	hres = pSvc->ExecMethod(bcdStorePath.bstrVal, bstr_t(L"OpenStore"), 0, NULL, 
		pBCDStoreClassInstance, &pBCDStoreOutParamsDefinition, NULL);
	assert(hres == S_OK);

	// extract the OpenStore Store result
	VARIANT bcdStoreVariant;
	hres = pBCDStoreOutParamsDefinition->Get(L"Store", 0, &bcdStoreVariant, &cimType, &flavor);
	assert(hres == S_OK);

	// get the Path OpenStore Store result
	VARIANT bcdOpenStorePath;
	IWbemClassObject* pBcdStore = (IWbemClassObject*)bcdStoreVariant.byref;
	hres = pBcdStore->Get(L"__RELPATH", 0, &bcdOpenStorePath, &cimType, &flavor);
	assert(hres == S_OK);

	///////////////////////////////////////////////////////////////////////////
	//
	//	call BCDStore.OpenObject(BCD_current) using  the BCDStore we just opened
	//	passing GUID for current configuration
	// 
	///////////////////////////////////////////////////////////////////////////

	// get the base input parameters for BCDStore.OpenObject
	IWbemClassObject* pInOpenObjectParams = NULL;
	hres = pBCDStoreClass->GetMethod(L"OpenObject", 0, &pInOpenObjectParams, NULL);
	assert(hres == S_OK);

	// set the Id parameter for OpenObject
	VARIANT paramId;
	paramId.vt = VT_BSTR;
	paramId.bstrVal = _bstr_t(BCD_current);
	hres = pInOpenObjectParams->Put(L"Id", 0, &paramId, 0);
	assert(hres == S_OK);

	// Execute OpenObject
	IWbemClassObject* pOutOpenObjectParamsDefinition = NULL;
	hres = pSvc->ExecMethod(bcdOpenStorePath.bstrVal, bstr_t(L"OpenObject"), 0, NULL, 
		pInOpenObjectParams, &pOutOpenObjectParamsDefinition, NULL);
	assert(hres == S_OK);
	
	// extract the BCDObject Object from the result
	VARIANT bcdObjectVariant;
	hres = pOutOpenObjectParamsDefinition->Get(L"Object", 0, &bcdObjectVariant, &cimType, &flavor);
	assert(hres == S_OK);
	IWbemClassObject* pBcdObject = (IWbemClassObject*)bcdObjectVariant.byref;

	// get the Path of the BCDObject that we opened
	VARIANT bcdObjectPath;
	hres = pBcdObject->Get(L"__RELPATH", 0, &bcdObjectPath, &cimType, &flavor);
	assert(hres == S_OK);

	///////////////////////////////////////////////////////////////////////////
	//
	//	call BCDObject.OpenObject using the BCDObject we just opened
	//	passing BcdLibraryString_Description to retrieve the name
	// 
	///////////////////////////////////////////////////////////////////////////

	// create BCDObject class
	IWbemClassObject* pBCDObjClass = NULL;
	BSTR bcdObjectClassName = SysAllocString(L"BCDObject");
	hres = pSvc->GetObject(bcdObjectClassName, 0, NULL, &pBCDObjClass, NULL);
	if (FAILED(hres))
	{
		tcout << _T("Could not get BCDObject class. Error code = 0x") << hex << hres << endl;
		return hres;					// Program has failed.
	}

	// get the parameters for pBCDObjClass.GetElement
	IWbemClassObject* pBCDObjInParamsDefinition = NULL;
	hres = pBCDObjClass->GetMethod(L"GetElement", 0, &pBCDObjInParamsDefinition, NULL);
	if (FAILED(hres))
	{
		tcout << _T("Could not get BCDObject.GetElement method. Error code = 0x")
			<< hex << hres << endl;
		return hres;					// Program has failed.
	}

	// spawn instance
	IWbemClassObject* pBCDObjClassInstance = NULL;
	hres = pBCDObjInParamsDefinition->SpawnInstance(0, &pBCDObjClassInstance);
	if (FAILED(hres))
	{
		tcout << _T("Could not spawn BCDStore class instance. Error code = 0x") << hex << hres << endl;
		return hres;					// Program has failed.
	}

	// Get the parameters for BCDObject.GetElement Method
	IWbemClassObject* pInGetElementParams = NULL;
	hres = pBCDObjClass->GetMethod(L"GetElement", 0, &pInGetElementParams, NULL);
	assert(hres == S_OK);

	// add the Type parameter
	VARIANT paramType;
	paramType.vt = VT_I4;
	paramType.lVal = BcdLibraryString_Description;
	hres = pInGetElementParams->Put(L"Type", 0, &paramType, 0);
	assert(hres == S_OK);

	// excute BCDObject.GetElement
	IWbemClassObject* pOutGetElementDefinition = NULL;
	hres = pSvc->ExecMethod(bcdObjectPath.bstrVal, bstr_t(L"GetElement"), 0, NULL, pInGetElementParams, &pOutGetElementDefinition, NULL);
	assert(hres == S_OK);
	
	// extract the GetElement element 
	VARIANT bcdElementDefinitionvariant;
	hres = pOutGetElementDefinition->Get(L"Element", 0, &bcdElementDefinitionvariant, &cimType, &flavor);
	assert(hres == S_OK);

	///////////////////////////////////////////////////////////////////////////
	//
	// Get the string from the element
	//
	///////////////////////////////////////////////////////////////////////////
	IWbemClassObject* pBCDElement = (IWbemClassObject*)bcdElementDefinitionvariant.byref;
	VARIANT bcdElementvariant;
	hres = pBCDElement->Get(L"String", 0, &bcdElementvariant, &cimType, &flavor);
	assert(hres == S_OK);

	wcout << L"Current boot configuration name \"" << bcdElementvariant.bstrVal << "\"" << endl;

	// Clean Up

	VariantClear(&bcdStorePath);
	VariantClear(&paramFile);
	VariantClear(&bcdStoreVariant);
	VariantClear(&bcdOpenStorePath);
	VariantClear(&paramId);
	VariantClear(&bcdObjectVariant);
	VariantClear(&bcdObjectPath);
	VariantClear(&paramType);

	return 0;
}