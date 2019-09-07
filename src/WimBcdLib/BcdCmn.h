#pragma once
#include <windows.h>
#include <wbemidl.h>
namespace BcdCmn
{
	HRESULT getBcdObjectPath(IWbemClassObject * pwco, VARIANT & varPath);
	bool	getOutputReturnValue(IWbemClassObject* pWbcOutPut);
}