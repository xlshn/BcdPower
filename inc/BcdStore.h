#pragma once
#include "BcdObject.h"
#include "wmibase.h"

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

