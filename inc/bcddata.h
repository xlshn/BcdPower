#pragma once
#include <windows.h>
#include <string>

enum DeviceType
{
	BootDevice = 0x1,
	PartitionDevice = 0x2,
	FileDevice = 0x3,
	RamdiskDevice = 0x4,
	UnknownDevice = 0x5,
	QualifiedPartition = 0x6,
	LocateDevice = 0x7,
	LocateExDevice = 0x8
};
class BcdDeviceData
{
	unsigned int DeviceType;
	std::wstring AdditionalOptions;
};

class BcdDeviceFileData : public BcdDeviceData
{
	BcdDeviceData Parent;
	std::wstring  Path;
};

enum BcdDeviceLocateDataType
{
	BcdDeviceLocateDataElement,
	BcdDeviceLocateDataString,
	BcdDeviceLocateDataElementChild
};
class BcdDeviceLocateData : public BcdDeviceData
{
private:
	BcdDeviceLocateDataType Type;
};

class BcdDevicePartitionData : public BcdDeviceData
{
	std::wstring Path;
};

enum DevicePartitionStyle
{
	GPTPart = 0x1,
	MBRPart = 0x0
};
class BcdDeviceQualifiedPartitionData : public BcdDeviceData
{
	DevicePartitionStyle PartitionStyle;
	std::wstring DiskSignature;
	std::wstring PartitionIdentifier;
};

class BcdDeviceUnknownData : BcdDeviceData
{
	//unsigned char Data[];
	unsigned char *Data;
};

class BcdDeviceLocateStringData : BcdDeviceLocateData
{
	std::wstring Path;
};

class BcdDeviceLocateElementData : BcdDeviceLocateData
{
	unsigned int Element;
};