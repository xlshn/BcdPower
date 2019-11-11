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
	BCD_DEVICE_DATA_TYPE DataType;
	BcdDeviceData()
	{
		DataType = DEVICE_DATA_TYPE_DeviceData;
		DeviceType = DEVICE_TYPE_BootDevice;
	}
	virtual ~BcdDeviceData()
	{
		AdditionalOptions.~basic_string();
	}

	virtual std::wstring getDeviceDisplayString();
protected:
	std::wstring getBootDeviceString(BCD_DEVICE_TYPE DeviceType);
};

class BcdDeviceFileData : public BcdDeviceData
{
public:
	BcdDeviceData* Parent;
	std::wstring  Path;
	BcdDeviceFileData()
	{
		Parent = NULL;
	}
	~BcdDeviceFileData()
	{
		if (Parent != NULL)
		{
			delete Parent;
			Parent = NULL;
		}
		Path.~basic_string();
	}
	virtual std::wstring getDeviceDisplayString();
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
	~BcdDevicePartitionData()
	{
		Path.~basic_string();
	}

	virtual std::wstring getDeviceDisplayString();
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
	~BcdDeviceQualifiedPartitionData()
	{
		DiskSignature.~basic_string();
		PartitionIdentifier.~basic_string();
	}

	virtual std::wstring getDeviceDisplayString();
};

class BcdDeviceUnknownData : public BcdDeviceData
{
public:
	//unsigned char Data[];
	unsigned char *Data;
	unsigned int DataSize;
	BcdDeviceUnknownData()
	{
		Data = NULL;
		DataSize = 0;
	}
	~BcdDeviceUnknownData()
	{
		if (Data != NULL)
		{
			delete Data;
			Data = NULL;
			DataSize = 0;
		}
	}
};

class BcdDeviceLocateStringData : public BcdDeviceLocateData
{
public:
	std::wstring Path;
	~BcdDeviceLocateStringData()
	{
		Path.~basic_string();
	}

	virtual std::wstring getDeviceDisplayString();
};

class BcdDeviceLocateElementData : public BcdDeviceLocateData
{
public:
	unsigned int Element;
};

class BcdDeviceLocateElementChildData : public BcdDeviceLocateData
{
public:
	unsigned int        Element;
	BcdDeviceData* Parent;
	BcdDeviceLocateElementChildData()
	{
		Parent = NULL;
	}
	~BcdDeviceLocateElementChildData()
	{
		if (Parent)
		{
			delete Parent;
			Parent = NULL;
		}
	}
};