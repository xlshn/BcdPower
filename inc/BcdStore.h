#pragma once
#include "BcdObject.h"
#include "wmibase.h"

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



class BcdStore
{
public:
	void Release();
	BcdStore(IWbemClassObject* pwboBcdStore, IWbemClassObject *pwboBcdStoreClass);
	bool CopyObject(
		std::wstring    SourceStoreFile,
		std::wstring    SourceId,
		uint32    Flags,
		BcdObject* &Object
	);

	bool CopyObjects(
		std::string SourceStoreFile,
		uint32 Type,
		uint32 Flags
	);

	bool CreateObject(
		std::wstring    Id,
		uint32    Type,
		BcdObject* &Object
	);
	static bool CreateStore(
		std::wstring   File,
		BcdStore* &Store
	);

	bool DeleteObject(
		std::wstring Id
	);

	bool DeleteSystemStore();
	bool EnumerateObjects(
		uint32    Type,
		std::vector<BcdObject*> &vecBcdObject
	);
	bool ExportStore(
		std::wstring File
	);

	static bool GetSystemDisk(
		std::wstring &Disk
	);
	static bool GetSystemPartition(
		std::wstring &wstrPartition
	);
	bool ImportStore(
		std::wstring File
	);

	bool ImportStoreWithFlags(
		std::wstring File,
		uint32 Flags
	);

	bool OpenObject(
		std::wstring    Id,
		BcdObject* &Object
	);

	static bool OpenStore(
		std::wstring   File,
		BcdStore* &Store
	);

	std::wstring getFilePath();
private:
	//std::string FilePath;
	IWbemClassObject* m_wboBcdStoreObject;
	IWbemClassObject* m_wboBcdStoreClass;
};

BcdStore* CreateBcdStoreObj(IWbemClassObject* pwboBcdStoreObject, IWbemClassObject* pwboBcdStoreClass);

