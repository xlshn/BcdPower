#include "bcdDevicedata.h"

std::wstring BcdDeviceData::getDeviceDisplayString()
{
	return getBootDeviceString(DeviceType) + L", " + AdditionalOptions;
}

std::wstring BcdDeviceData::getBootDeviceString(BCD_DEVICE_TYPE DeviceType)
{
	switch (DeviceType)
	{
	case DEVICE_TYPE_BootDevice:
	{
		return L"BootDevice";
		break;
	}
	case DEVICE_TYPE_PartitionDevice:
	{
		return L"PartitionDevice";
		break;
	}
	case DEVICE_TYPE_FileDevice:
	{
		return L"FileDevice";
		break;
	}
	case DEVICE_TYPE_RamdiskDevice:
	{
		return L"RamdiskDevice";
		break;
	}
	case DEVICE_TYPE_UnknownDevice:
	{
		return L"UnknownDevice";
		break;
	}
	case DEVICE_TYPE_LocateDevice:
	{
		return L"LocateDevice";
		break;
	}
	case DEVICE_TYPE_LocateExDevice:
	{
		return L"LocateExDevice";
		break;
	}
	default:
		break;
	}
	return L"";
}

std::wstring BcdDeviceFileData::getDeviceDisplayString()
{
	std::wstring wstrBoot = getBootDeviceString(DeviceType) + L", " + Path;
	std::wstring wstrParent;
	if (Parent != NULL)
	{
		wstrParent = Parent->getDeviceDisplayString();
	}

	if (wstrParent.length() != 0)
	{
		wstrBoot = wstrBoot + L", " + wstrParent;
	}

	return wstrBoot;
}

std::wstring BcdDevicePartitionData::getDeviceDisplayString()
{
	return getBootDeviceString(DeviceType) + L", " + Path;
}

std::wstring BcdDeviceQualifiedPartitionData::getDeviceDisplayString()
{
	return getBootDeviceString(DeviceType) + L", " + ((PartitionStyle == GPTPart) ? L"GPT" : L"MBR") + L", " + DiskSignature + L", " + PartitionIdentifier;


}

 std::wstring BcdDeviceLocateStringData::getDeviceDisplayString()
{
	 return  getBootDeviceString(DeviceType) + L", " + Path;
}