#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include "element.h"


//Standard Application Objects
constexpr auto Windows_Boot_Manager_GUID = L"{9dea862c-5cdd-4e70-acc1-f32b344d4795}";
constexpr auto Firmware_Boot_Manager_GUID = L"{a5a30fa2-3d06-4e9f-b5f4-a01df9d1fcba}";
constexpr auto Windows_Memory_Tester_GUID = L"{b2721d73-1db4-4c62-bf78-c548a880142d}";
constexpr auto Windows_Resume_Application_GUID = L"{147aa509-0358-4473-b83b-d950dda00615}";
constexpr auto Legacy_Windows_Loader_GUID = L"{fa926493-6f1c-4193-a414-58f0b2456d1e}";
/////////////////////////////////////////////////////////////////////////////////////////////
//Standard Inheritable Objects
constexpr auto Standard_Inheritable_badmemory_GUID = L"{5189b25c-5558-4bf2-bca4-289b11bd29e2}";
constexpr auto Standard_Inheritable_bootloadersettings_GUID = L"{6efb52bf-1766-41db-a6b3-0ee5eff72bd7}";
constexpr auto Standard_Inheritable_dbgsettings_GUID = L"{4636856e-540f-4170-a130-a84776f4c654}";
constexpr auto Standard_Inheritable_emssetting_GUID = L"{0ce4991b-e6b3-4b16-b23c-5e0d9250e5d9}";
constexpr auto Standard_Inheritable_globalsettings_GUID = L"{7ea2e1ac-2e61-4728-aaa3-896d9d0a9f0e}";
constexpr auto Standard_Inheritable_resumeloadersettings_GUID = L"{1afa9c49-16ab-4a5c-901b-212802da9460}";
constexpr auto Standard_Inheritable_hypervisorsettings_GUID = L"{7ff607e0-4395-11db-b0de-0800200c9a66}";
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Standard ramdiskoptions
constexpr auto Standard_Ramdisk_Options_GUID = L"{AE5534E0-A924-466C-B836-758539A3EE3A}";


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
	ObjectCode GetObjectCode();
	bool GetBcdObjectId(std::wstring &wstrBcdId);
	void Release();
	BcdObject(IWbemClassObject* pwco);
	bool DeleteElement(UINT32 Type);
	bool EnumerateElements(std::vector<BcdElement*> &vecElement);
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
		std::vector<std::wstring > vecIds
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
	EleValueType GetElementValueType2(IWbemClassObject* pwboEleObject);
	bool GetBcdObjectDescription(std::wstring &wstrDescription);
	BCD_DEVICE_DATA_TYPE GetDeviceElementDataType(IWbemClassObject* pwboDeviceDataObject);
public:
	std::wstring m_StoreFilePath;
	std::wstring m_Id;
	unsigned int m_Type;

	IWbemClassObject* m_wbemBcdObject;
	IWbemClassObject* m_wbemBcdClass;
private:	
	ImageCode GetImageCode();	
	ApplicationCode GetApplicationCode();	
	bool GetBcdType(UINT &type);
	HRESULT getBcdObjectPath(VARIANT &varBcdObjectPath);	
	bool	getOutputReturnValue(IWbemClassObject* pWbcOutPut);
	
	IWbemClassObject* getElementClassObj(ULONG Type);
	/////////////////////////////////////////////////////////////////////////
	//
	BcdElement BuildBcdElementStruct(IWbemClassObject* pwboEle);

	BcdElement* BuildBcdElementStruct2(IWbemClassObject* pwboEle);


	BcdStringElement* Build_BcdString_ElementStruct(IWbemClassObject* pwboEle);
	BcdDeviceElement Build_BcdDevice_ElementStruct(IWbemClassObject* pwboEle);
	BcdBooleanElement* Build_BcdBoolean_ElementStruct(IWbemClassObject* pwboEle);
	BcdIntegerElement* Build_BcdInteger_ElementStruct(IWbemClassObject* pwboEle);
	BcdIntegerListElement* Build_BCDIntegerList_ElementStruct(IWbemClassObject* pwboEle);
	BcdObjectElement* Build_BcdObjec_ElementStruct(IWbemClassObject* pwboEle);
	BcdObjectListElement* Build_BcdObjectList_ElementStruct(IWbemClassObject* pwboEle);	
	BcdUnknownElement* Build_BcdUnknown_ElementStruct(IWbemClassObject* pwboEle);
	/////////////////////////////////////////////////////////////////////////
	//

	//////////////////////////////////////////////////////////////////////////
};

BcdObject* CreateBcdObjectObj(IWbemClassObject* pwboBcdObject);