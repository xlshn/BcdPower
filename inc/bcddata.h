#pragma once
#include <windows.h>
#include <string>

enum BCD_DEVICE_TYPE
{	
	DEVICE_TYPE_BootDevice = 0x1,
	DEVICE_TYPE_PartitionDevice = 0x2,
	DEVICE_TYPE_FileDevice = 0x3,
	DEVICE_TYPE_RamdiskDevice = 0x4,
	DEVICE_TYPE_UnknownDevice = 0x5,
	DEVICE_TYPE_QualifiedPartition = 0x6,
	DEVICE_TYPE_LocateDevice = 0x7,
	DEVICE_TYPE_LocateExDevice = 0x8
};

enum BCD_DEVICE_DATA_TYPE
{
	DEVICE_DATA_TYPE_DeviceData = 0x0,
	DEVICE_DATA_TYPE_DeviceFileData = 0x1,
	DEVICE_DATA_TYPE_DeviceLocateData = 0x2,
	DEVICE_DATA_TYPE_DeviceLocateElementChildData = 0x3,
	DEVICE_DATA_TYPE_DeviceLocateElementData = 0x4,
	DEVICE_DATA_TYPE_DeviceLocateStringData = 0x5,
	DEVICE_DATA_TYPE_DevicePartitionData = 0x6,
	DEVICE_DATA_TYPE_DeviceQualifiedPartitionData = 0x7,
	DEVICE_DATA_TYPE_DeviceUnknownData = 0x8
};


class BcdDeviceData
{
public:
	BCD_DEVICE_TYPE DeviceType;
	std::wstring AdditionalOptions;
};

class BcdDeviceFileData : public BcdDeviceData
{
public:
	BcdDeviceData* Parent;
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
public:
	BcdDeviceLocateDataType Type;
};

class BcdDevicePartitionData : public BcdDeviceData
{
public:
	std::wstring Path;
};

enum DevicePartitionStyle
{
	GPTPart = 0x1,
	MBRPart = 0x0
};
class BcdDeviceQualifiedPartitionData : public BcdDeviceData
{
public:
	DevicePartitionStyle PartitionStyle;
	std::wstring DiskSignature;
	std::wstring PartitionIdentifier;
};

class BcdDeviceUnknownData : BcdDeviceData
{
public:
	//unsigned char Data[];
	unsigned char *Data;
};

class BcdDeviceLocateStringData : BcdDeviceLocateData
{
public:
	std::wstring Path;
};

class BcdDeviceLocateElementData : BcdDeviceLocateData
{
public:
	unsigned int Element;
};