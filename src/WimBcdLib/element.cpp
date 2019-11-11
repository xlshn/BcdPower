#include "element.h"


std::wstring BcdDeviceElement::getValueString()
{
	return Device->getDeviceDisplayString();
}