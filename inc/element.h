#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "bcddevicedata.h"
#include "wmibase.h"

///////////////////////////////////////////////////////
typedef enum BcdBootMgrElementTypes {
	BcdBootMgrObjectList_DisplayOrder = 0x24000001,
	BcdBootMgrObjectList_BootSequence = 0x24000002,
	BcdBootMgrObject_DefaultObject = 0x23000003,
	BcdBootMgrInteger_Timeout = 0x25000004,
	BcdBootMgrBoolean_AttemptResume = 0x26000005,
	BcdBootMgrObject_ResumeObject = 0x23000006,
	BcdBootMgrObjectList_ToolsDisplayOrder = 0x24000010,
	BcdBootMgrBoolean_DisplayBootMenu = 0x26000020,
	BcdBootMgrBoolean_NoErrorDisplay = 0x26000021,
	BcdBootMgrDevice_BcdDevice = 0x21000022,
	BcdBootMgrString_BcdFilePath = 0x22000023,
	BcdBootMgrBoolean_ProcessCustomActionsFirst = 0x26000028,
	BcdBootMgrIntegerList_CustomActionsList = 0x27000030,
	BcdBootMgrBoolean_PersistBootSequence = 0x26000031
} BcdBootMgrElementTypes;

typedef enum BcdDeviceObjectElementTypes {
	BcdDeviceInteger_RamdiskImageOffset = 0x35000001,
	BcdDeviceInteger_TftpClientPort = 0x35000002,
	BcdDeviceInteger_SdiDevice = 0x31000003,
	BcdDeviceInteger_SdiPath = 0x32000004,
	BcdDeviceInteger_RamdiskImageLength = 0x35000005,
	BcdDeviceBoolean_RamdiskExportAsCd = 0x36000006,
	BcdDeviceInteger_RamdiskTftpBlockSize = 0x36000007,
	BcdDeviceInteger_RamdiskTftpWindowSize = 0x36000008,
	BcdDeviceBoolean_RamdiskMulticastEnabled = 0x36000009,
	BcdDeviceBoolean_RamdiskMulticastTftpFallback = 0x3600000A,
	BcdDeviceBoolean_RamdiskTftpVarWindow = 0x3600000B
} BcdDeviceObjectElementTypes;

typedef enum BcdLibrary_DebuggerType {
	DebuggerSerial = 0,
	Debugger1394 = 1,
	DebuggerUsb = 2,
	DebuggerNet = 3
} BcdLibrary_DebuggerType;

typedef enum BcdLibrary_SafeBoot {
	SafemodeMinimal = 0,
	SafemodeNetwork = 1,
	SafemodeDsRepair = 2
} BcdLibrary_SafeBoot;

typedef enum BcdLibraryElementTypes {
	BcdLibraryDevice_ApplicationDevice = 0x11000001,
	BcdLibraryString_ApplicationPath = 0x12000002,
	BcdLibraryString_Description = 0x12000004,
	BcdLibraryString_PreferredLocale = 0x12000005,
	BcdLibraryObjectList_InheritedObjects = 0x14000006,
	BcdLibraryInteger_TruncatePhysicalMemory = 0x15000007,
	BcdLibraryObjectList_RecoverySequence = 0x14000008,
	BcdLibraryBoolean_AutoRecoveryEnabled = 0x16000009,
	BcdLibraryIntegerList_BadMemoryList = 0x1700000a,
	BcdLibraryBoolean_AllowBadMemoryAccess = 0x1600000b,
	BcdLibraryInteger_FirstMegabytePolicy = 0x1500000c,
	BcdLibraryInteger_RelocatePhysicalMemory = 0x1500000D,
	BcdLibraryInteger_AvoidLowPhysicalMemory = 0x1500000E,
	BcdLibraryBoolean_DebuggerEnabled = 0x16000010,
	BcdLibraryInteger_DebuggerType = 0x15000011,
	BcdLibraryInteger_SerialDebuggerPortAddress = 0x15000012,
	BcdLibraryInteger_SerialDebuggerPort = 0x15000013,
	BcdLibraryInteger_SerialDebuggerBaudRate = 0x15000014,
	BcdLibraryInteger_1394DebuggerChannel = 0x15000015,
	BcdLibraryString_UsbDebuggerTargetName = 0x12000016,
	BcdLibraryBoolean_DebuggerIgnoreUsermodeExceptions = 0x16000017,
	BcdLibraryInteger_DebuggerStartPolicy = 0x15000018,
	BcdLibraryString_DebuggerBusParameters = 0x12000019,
	BcdLibraryInteger_DebuggerNetHostIP = 0x1500001A,
	BcdLibraryInteger_DebuggerNetPort = 0x1500001B,
	BcdLibraryBoolean_DebuggerNetDhcp = 0x1600001C,
	BcdLibraryString_DebuggerNetKey = 0x1200001D,
	BcdLibraryBoolean_EmsEnabled = 0x16000020,
	BcdLibraryInteger_EmsPort = 0x15000022,
	BcdLibraryInteger_EmsBaudRate = 0x15000023,
	BcdLibraryString_LoadOptionsString = 0x12000030,
	BcdLibraryBoolean_DisplayAdvancedOptions = 0x16000040,
	BcdLibraryBoolean_DisplayOptionsEdit = 0x16000041,
	BcdLibraryDevice_BsdLogDevice = 0x11000043,
	BcdLibraryString_BsdLogPath = 0x12000044,
	BcdLibraryBoolean_GraphicsModeDisabled = 0x16000046,
	BcdLibraryInteger_ConfigAccessPolicy = 0x15000047,
	BcdLibraryBoolean_DisableIntegrityChecks = 0x16000048,
	BcdLibraryBoolean_AllowPrereleaseSignatures = 0x16000049,
	BcdLibraryString_FontPath = 0x1200004A,
	BcdLibraryInteger_SiPolicy = 0x1500004B,
	BcdLibraryInteger_FveBandId = 0x1500004C,
	BcdLibraryBoolean_ConsoleExtendedInput = 0x16000050,
	BcdLibraryInteger_GraphicsResolution = 0x15000052,
	BcdLibraryBoolean_RestartOnFailure = 0x16000053,
	BcdLibraryBoolean_GraphicsForceHighestMode = 0x16000054,
	BcdLibraryBoolean_IsolatedExecutionContext = 0x16000060,
	BcdLibraryBoolean_BootUxDisable = 0x1600006C,
	BcdLibraryBoolean_BootShutdownDisabled = 0x16000074,
	BcdLibraryIntegerList_AllowedInMemorySettings = 0x17000077,
	BcdLibraryBoolean_ForceFipsCrypto = 0x16000079
} BcdLibraryElementTypes;

typedef enum BcdMemDiagElementTypes {
	BcdMemDiagInteger_PassCount = 0x25000001,
	BcdMemDiagInteger_FailureCount = 0x25000003
} BcdMemDiagElementTypes;

typedef enum BcdOSLoader_NxPolicy {
	NxPolicyOptIn = 0,
	NxPolicyOptOut = 1,
	NxPolicyAlwaysOff = 2,
	NxPolicyAlwaysOn = 3
} BcdOSLoader_NxPolicy;

typedef enum BcdOSLoader_PAEPolicy {
	PaePolicyDefault = 0,
	PaePolicyForceEnable = 1,
	PaePolicyForceDisable = 2
} BcdOSLoader_PAEPolicy;

typedef enum BcdOSLoaderElementTypes {
	BcdOSLoaderDevice_OSDevice = 0x21000001,
	BcdOSLoaderString_SystemRoot = 0x22000002,
	BcdOSLoaderObject_AssociatedResumeObject = 0x23000003,
	BcdOSLoaderBoolean_DetectKernelAndHal = 0x26000010,
	BcdOSLoaderString_KernelPath = 0x22000011,
	BcdOSLoaderString_HalPath = 0x22000012,
	BcdOSLoaderString_DbgTransportPath = 0x22000013,
	BcdOSLoaderInteger_NxPolicy = 0x25000020,
	BcdOSLoaderInteger_PAEPolicy = 0x25000021,
	BcdOSLoaderBoolean_WinPEMode = 0x26000022,
	BcdOSLoaderBoolean_DisableCrashAutoReboot = 0x26000024,
	BcdOSLoaderBoolean_UseLastGoodSettings = 0x26000025,
	BcdOSLoaderBoolean_AllowPrereleaseSignatures = 0x26000027,
	BcdOSLoaderBoolean_NoLowMemory = 0x26000030,
	BcdOSLoaderInteger_RemoveMemory = 0x25000031,
	BcdOSLoaderInteger_IncreaseUserVa = 0x25000032,
	BcdOSLoaderBoolean_UseVgaDriver = 0x26000040,
	BcdOSLoaderBoolean_DisableBootDisplay = 0x26000041,
	BcdOSLoaderBoolean_DisableVesaBios = 0x26000042,
	BcdOSLoaderBoolean_DisableVgaMode = 0x26000043,
	BcdOSLoaderInteger_ClusterModeAddressing = 0x25000050,
	BcdOSLoaderBoolean_UsePhysicalDestination = 0x26000051,
	BcdOSLoaderInteger_RestrictApicCluster = 0x25000052,
	BcdOSLoaderBoolean_UseLegacyApicMode = 0x26000054,
	BcdOSLoaderInteger_X2ApicPolicy = 0x25000055,
	BcdOSLoaderBoolean_UseBootProcessorOnly = 0x26000060,
	BcdOSLoaderInteger_NumberOfProcessors = 0x25000061,
	BcdOSLoaderBoolean_ForceMaximumProcessors = 0x26000062,
	BcdOSLoaderBoolean_ProcessorConfigurationFlags = 0x25000063,
	BcdOSLoaderBoolean_MaximizeGroupsCreated = 0x26000064,
	BcdOSLoaderBoolean_ForceGroupAwareness = 0x26000065,
	BcdOSLoaderInteger_GroupSize = 0x25000066,
	BcdOSLoaderInteger_UseFirmwarePciSettings = 0x26000070,
	BcdOSLoaderInteger_MsiPolicy = 0x25000071,
	BcdOSLoaderInteger_SafeBoot = 0x25000080,
	BcdOSLoaderBoolean_SafeBootAlternateShell = 0x26000081,
	BcdOSLoaderBoolean_BootLogInitialization = 0x26000090,
	BcdOSLoaderBoolean_VerboseObjectLoadMode = 0x26000091,
	BcdOSLoaderBoolean_KernelDebuggerEnabled = 0x260000a0,
	BcdOSLoaderBoolean_DebuggerHalBreakpoint = 0x260000a1,
	BcdOSLoaderBoolean_UsePlatformClock = 0x260000A2,
	BcdOSLoaderBoolean_ForceLegacyPlatform = 0x260000A3,
	BcdOSLoaderInteger_TscSyncPolicy = 0x250000A6,
	BcdOSLoaderBoolean_EmsEnabled = 0x260000b0,
	BcdOSLoaderInteger_DriverLoadFailurePolicy = 0x250000c1,
	BcdOSLoaderInteger_BootMenuPolicy = 0x250000C2,
	BcdOSLoaderBoolean_AdvancedOptionsOneTime = 0x260000C3,
	BcdOSLoaderInteger_BootStatusPolicy = 0x250000E0,
	BcdOSLoaderBoolean_DisableElamDrivers = 0x260000E1,
	BcdOSLoaderInteger_HypervisorLaunchType = 0x250000F0,
	BcdOSLoaderBoolean_HypervisorDebuggerEnabled = 0x260000F2,
	BcdOSLoaderInteger_HypervisorDebuggerType = 0x250000F3,
	BcdOSLoaderInteger_HypervisorDebuggerPortNumber = 0x250000F4,
	BcdOSLoaderInteger_HypervisorDebuggerBaudrate = 0x250000F5,
	BcdOSLoaderInteger_HypervisorDebugger1394Channel = 0x250000F6,
	BcdOSLoaderInteger_BootUxPolicy = 0x250000F7,
	BcdOSLoaderString_HypervisorDebuggerBusParams = 0x220000F9,
	BcdOSLoaderInteger_HypervisorNumProc = 0x250000FA,
	BcdOSLoaderInteger_HypervisorRootProcPerNode = 0x250000FB,
	BcdOSLoaderBoolean_HypervisorUseLargeVTlb = 0x260000FC,
	BcdOSLoaderInteger_HypervisorDebuggerNetHostIp = 0x250000FD,
	BcdOSLoaderInteger_HypervisorDebuggerNetHostPort = 0x250000FE,
	BcdOSLoaderInteger_TpmBootEntropyPolicy = 0x25000100,
	BcdOSLoaderString_HypervisorDebuggerNetKey = 0x22000110,
	BcdOSLoaderBoolean_HypervisorDebuggerNetDhcp = 0x26000114,
	BcdOSLoaderInteger_HypervisorIommuPolicy = 0x25000115,
	BcdOSLoaderInteger_XSaveDisable = 0x2500012b
} BcdOSLoaderElementTypes;

typedef enum _BcdResumeElementTypes {
	Reserved1 = 0x21000001,
	Reserved2 = 0x22000002,
	BcdResumeBoolean_UseCustomSettings = 0x26000003,
	BcdResumeDevice_AssociatedOsDevice = 0x21000005,
	BcdResumeBoolean_DebugOptionEnabled = 0x26000006,
	BcdResumeInteger_BootMenuPolicy = 0x25000008
} BcdResumeElementTypes;

///////////////////////////////////////////////////////
enum BcdElementClass
{
	BcdEleClassLibrary,
	BcdEleClassApplication,
	BcdEleClassDevice,
	BcdEleClassOEM
};

enum BcdElementFormat
{
	BcdEleFormatDevice = 0x1, 
	BcdEleFormatString = 0x2,
	BcdEleFormatObject = 0x3,
	BcdEleFormatObjectList = 0x4,
	BcdEleFormatInteger = 0x5,
	BcdEleFormatBoolean = 0x6,
	BcdEleFormatIntegerList = 0x7
};
typedef struct {
	union {
		ULONG  PackedValue;
		struct {
			ULONG SubType : 24;
			ULONG Format : 4;
			ULONG Class : 4;
		};
	};
} BcdElementType;

enum EleValueType
{
	EleValueType_error,
	EleValueType_Base,
	EleValueType_Device,
	EleValueType_Boolean,
	EleValueType_Integer,
	EleValueType_IntegersArray,
	EleValueType_BcdObjectId,
	EleValueType_BcdObjectIdArray,
	EleValueType_BcdString, //BcdStringElement
	EleValueType_BcdUnknown  //BcdUnknownElement
};
class BcdElement
{
public:
	virtual std::wstring getValueString()
	{
		return L"";
	}
	BcdElement()
	{
		valueType = EleValueType_BcdUnknown;
		UserData = NULL;
	}
	std::wstring StoreFilePath;
	std::wstring ObjectId;
	unsigned int Type;
	EleValueType valueType;
	void* UserData;
	virtual ~BcdElement()
	{
		StoreFilePath.~basic_string();
		ObjectId.~basic_string();
	}	
};

class BcdDeviceElement : public BcdElement
{
public:
	BcdDeviceData* Device;
	BcdDeviceElement()
	{
		Device = NULL;
	}
	virtual ~BcdDeviceElement()
	{
		if (Device != NULL)
		{
			delete Device;
		}
		Device = NULL;
	}
	std::wstring getValueString();
};

class BcdBooleanElement : public BcdElement
{
public:
	bool Boolean;
	virtual std::wstring getValueString()
	{
		if (Boolean)
		{
			return L"True";
		}

		return L"False";
	}
};

class BcdIntegerElement : public BcdElement
{
public:
	ULONGLONG Integer;
	virtual std::wstring getValueString()
	{
		wchar_t pwsz[MAX_PATH] = { 0 };
		wsprintf(pwsz, L"%I64d", Integer);
		return pwsz;
	}
};

class BcdIntegerListElement : public BcdElement
{
public:
	//std::wstring Integers[];
	//std::wstring *Integers;
	std::vector<std::wstring> vecInteger;	
	virtual ~BcdIntegerListElement()
	{
		vecInteger.clear();
		vecInteger.~vector();
	}
	virtual std::wstring getValueString()
	{
		std::wstring wstrTmp;
		for (size_t i = 0; i < vecInteger.size(); i++)
		{
			wstrTmp += vecInteger[i];
			wstrTmp += L" ";
		}
		return wstrTmp;
	}
};

class BcdObjectElement : public BcdElement
{
public:
	std::wstring Id;
	virtual ~BcdObjectElement()
	{
		Id.~basic_string();
	}
	virtual std::wstring getValueString()
	{
		return Id;
	}
};

class BcdObjectListElement : public BcdElement
{
public:
	//std::wstring Ids[];
	//std::wstring *Ids;
	std::vector<std::wstring> vecId;
	virtual ~BcdObjectListElement()
	{
		vecId.~vector();
	}
	virtual std::wstring getValueString()
	{
		std::wstring wstrTmp;
		for (size_t i = 0; i < vecId.size(); i++)
		{
			wstrTmp += vecId[i];
			wstrTmp += L" ";
		}
		return wstrTmp;	
	}
};

class BcdStringElement : public BcdElement
{
public:
	std::wstring String;
	virtual ~BcdStringElement()
	{
		String.~basic_string();
	}
	virtual std::wstring getValueString()
	{
		return String;
	}
};

class BcdUnknownElement : public BcdElement
{
public:
	ULONGLONG ActualType;

	virtual std::wstring getValueString()
	{
		wchar_t pwsz[MAX_PATH] = { 0 };
		wsprintf(pwsz, L"%I64d", ActualType);
		return pwsz;
	}
};