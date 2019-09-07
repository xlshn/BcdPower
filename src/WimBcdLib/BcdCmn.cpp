#include "BcdCmn.h"
#include <cassert>

HRESULT BcdCmn::getBcdObjectPath(IWbemClassObject * pwco, VARIANT & varPath)
{
	VARIANT varPathTmp;
	HRESULT hres = pwco->Get(L"__RELPATH", 0, &varPathTmp, NULL, NULL);
	if (!FAILED(hres))
	{
		varPath = varPathTmp;
	}
	return hres;
}

bool BcdCmn::getOutputReturnValue(IWbemClassObject * pWbcOutPut)
{
	VARIANT varOutput;
	HRESULT hres = pWbcOutPut->Get(L"ReturnValue", 0, &varOutput, NULL, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	assert(varOutput.vt == VT_BOOL);
	return varOutput.boolVal;
}
