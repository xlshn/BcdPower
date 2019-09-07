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
	std::wstring StoreFilePath;
	std::wstring ObjectId;
	unsigned int Type;
};

class BcdDeviceElement : public BcdElement
{
public:
	BcdDeviceData Device;
};

class BcdBooleanElement : BcdElement
{
public:
	boolean Boolean;
};

class BcdIntegerElement : BcdElement
{
public:
	__int64 Integer;
};

class BcdIntegerListElement : BcdElement
{
public:
	//std::wstring Integers[];
	std::wstring *Integers;
};

class BcdObjectElement : BcdElement
{
public:
	std::wstring Id;
};

class BcdObjectListElement : BcdElement
{
public:
	//std::wstring Ids[];
	std::wstring *Ids;
};



