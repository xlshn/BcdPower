#pragma once
#include <windows.h>
#include <string>
#include "bcddata.h"
#include "wmibase.h"
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
	BcdDeviceData Device;
};

class BcdBooleanElement : public BcdElement
{
public:
	bool Boolean;
};

class BcdIntegerElement : public BcdElement
{
public:
	ULONGLONG Integer;
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
};

class BcdObjectElement : public BcdElement
{
public:
	std::wstring Id;
	virtual ~BcdObjectElement()
	{
		Id.~basic_string();
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
};

class BcdStringElement : public BcdElement
{
public:
	std::wstring String;
	virtual ~BcdStringElement()
	{
		String.~basic_string();
	}
};

class BcdUnknownElement : public BcdElement
{
public:
	ULONGLONG ActualType;
};