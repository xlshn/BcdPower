#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "element.h"

typedef enum ObjectType
{
	Application = 0x1,
	Inheritable = 0x2,
	Device = 0x3
}ObjectCode;

typedef enum ImageType
{
	FirmwareApplicatio = 0x1,
	BootApplication = 0x2,
	NtldrBasedLoaderObject = 0x3,
	RealModeApplication = 0x4
}ImageCode;

//参考资料1：https://docs.microsoft.com/en-us/previous-versions/windows/desktop/bcd/bcdobject
//参考文档2：
typedef enum AppType
{
	FirmwareBootManager = 0x1,
	WindowsBootManager = 0x2,
	WindowsBootLoader = 0x3,
	WindowsResumeApplication = 0x4,
	WindowsMemoryTester = 0x5,
	LegacyNtldr = 0x6,
	LegacySetupLdr = 0x7,
	BootSector = 0x8,
	StartupModule = 0x9,
	GenericApplication = 0xa
}ApplicationCode;

typedef struct
{
	union
	{
		ULONG PackedValue;
		struct 
		{
			ULONG ObjectCode : 4;
			ULONG Reserved : 4;
			ULONG ImageCode : 4;
			ULONG AppliactionCode : 20;
		};
	};
}BcdApplicationType;

typedef unsigned int uint32;
typedef unsigned __int64  uint64;
class BcdObject
{
public:
	void Release();
	ImageCode GetImageCode();
	ObjectCode GetObjectCode();
	ApplicationCode GetApplicationCode();
	bool GetBcdObjectId(std::wstring &wstrBcdId);
	bool GetBcdType(UINT &type);
	BcdObject(IWbemClassObject* pwco);
	bool DeleteElement(UINT32 Type);
	bool EnumerateElements(std::vector<BcdElement> &vecElement);
	bool EnumerateElementTypes(std::vector<UINT32> &Types);
	bool GetElement(UINT32 Type,  BcdElement &Element);
	bool GetElementWithFlags(ULONG Type, ULONG Flags, BcdElement &Element);
	bool SetBooleanElement( UINT32  Type, BOOL Boolean);
	bool SetDeviceElement(UINT32 Type, UINT32 DeviceType, std::wstring AdditionalOptions);
	bool SetFileDeviceElement(
		uint32 Type,
		uint32 DeviceType,
		std::wstring AdditionalOptions,
		std::wstring Path,
		uint32 ParentDeviceType,
		std::wstring ParentAdditionalOptions,
		std::wstring ParentPath
	);
	bool SetIntegerElement(
		uint32 Type,
		uint64 Integer
	);

	bool SetIntegerListElement(
		uint32 Type,
		std::vector<uint64> vecIntegers
	);

	bool SetObjectElement(
		uint32 Type,
		std::wstring Id
	);
	bool SetObjectListElement(
		uint32 Type,
		std::vector<std::wstring> vecIds
	);
	bool SetPartitionDeviceElement(
		uint32 Type,
		uint32 DeviceType,
		std::wstring AdditionalOptions,
		std::wstring Path
	);

	bool SetPartitionDeviceElementWithFlags(
		uint32 Type,
		uint32 DeviceType,
		std::wstring AdditionalOptions,
		std::wstring Path,
		uint32 Flags
	);

	bool SetQualifiedPartitionDeviceElement(
		ULONG  Type,
		ULONG  PartitionStyle,
		PCWSTR DiskSignature,
		PCWSTR PartitionIdentifier
	);

	bool SetStringElement(
		uint32 Type,
		std::wstring String
	);
	bool SetVhdDeviceElement(
		ULONG  Type,
		PCWSTR Path,
		ULONG  ParentDeviceType,
		PCWSTR ParentAdditionalOptions,
		PCWSTR ParentPath,
		ULONG  CustomLocate
	);

	EleValueType GetElementValueType(ULONG Type);
public:
	std::wstring m_StoreFilePath;
	std::wstring m_Id;
	unsigned int m_Type;

	IWbemClassObject* m_wbemBcdObject;
private:
	HRESULT getBcdObjectPath(VARIANT &varBcdObjectPath);	
	bool	getOutputReturnValue(IWbemClassObject* pWbcOutPut);
	IWbemServices* getSvc();
	IWbemClassObject* getElementClassObj(ULONG Type);
};

BcdObject* CreateBcdObjectObj(IWbemClassObject* pwboBcdObject);