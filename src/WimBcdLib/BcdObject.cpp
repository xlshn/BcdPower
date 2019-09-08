#include "BcdObject.h"
#include <cassert>
CWimBase wimBase;

void BcdObject::Release()
{
	m_wbemBcdObject->Release();
}
ImageCode BcdObject::GetImageCode()
{
	return (ImageCode)((m_Type & 0x00f00000) >> 20);
}
ObjectCode BcdObject::GetObjectCode()
{
	return (ObjectCode)((m_Type & 0xf0000000) >> 28);
}
ApplicationCode BcdObject::GetApplicationCode()
{
	return (ApplicationCode)(m_Type & 0x000fffff);
}
bool BcdObject::GetBcdObjectId(std::wstring &wstrBcdId)
{
	VARIANT varBcdObjectId;
	HRESULT  hres = m_wbemBcdObject->Get(L"ID", 0, &varBcdObjectId, NULL, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	wstrBcdId = (wchar_t*)varBcdObjectId.bstrVal;
	return true;
	
}
bool BcdObject::GetBcdType(UINT &type)
{
	VARIANT varBcdObjectType;
	HRESULT  hres = m_wbemBcdObject->Get(L"Type", 0, &varBcdObjectType, NULL, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	type = varBcdObjectType.uintVal;
	return true;

}
BcdObject::BcdObject(IWbemClassObject * pwco)
{
	m_wbemBcdObject = pwco;
	GetBcdType(m_Type);
}

bool BcdObject::DeleteElement(UINT32 Type)
{
	bool bReturnValue = false; //默认为false
	IWbemClassObject* pWcoDeleteElementInput = NULL;
	HRESULT hWmiRes = m_wbemBcdObject->GetMethod(L"DeleteElement", 0, &pWcoDeleteElementInput, NULL);
	if (FAILED(hWmiRes))
	{
		return false;
	}

	do
	{ 
		VARIANT varType;
		varType.vt = VT_I4;
		varType.uintVal = Type;
		hWmiRes = pWcoDeleteElementInput->Put(L"Type", 0, &varType, 0);
		if (FAILED(hWmiRes))
		{
			break;
		}

		VARIANT varBcdObjectPath;			
		hWmiRes = m_wbemBcdObject->Get(L"__RELPATH", 0, &varBcdObjectPath, NULL, NULL);

		IWbemServices* pWbemServ = wimBase.getSvc();
		IWbemClassObject* pOutputParam = NULL;
		hWmiRes = pWbemServ->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"DeleteElement"), 0, NULL, pWcoDeleteElementInput, &pOutputParam, NULL);
		if (FAILED(hWmiRes))
		{
			break;
		}
		VARIANT varReturnValue;			
		hWmiRes = pOutputParam->Get(L"ReturnValue", 0, &varReturnValue, NULL, NULL);
		if (FAILED(hWmiRes))
		{
			pOutputParam->Release();
			break;
		}

		if (varReturnValue.vt == VT_BOOL && varReturnValue.boolVal)
		{
			bReturnValue = true;
		}
		pOutputParam->Release();
	} while (0);

	pWcoDeleteElementInput->Release();
	if (FAILED(hWmiRes))
	{
		return false;
	}
	else
	{
		return bReturnValue;
	}
}

bool BcdObject::EnumerateElements(std::vector<BcdElement>& vecElement)
{
	bool bReturnValue = false; //默认为false
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemServices* pWbemServ = wimBase.getSvc();
	IWbemClassObject* pWbcEnumEleOutPut = NULL;
	hres = pWbemServ->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"EnumerateElements"), 0, NULL, NULL, &pWbcEnumEleOutPut, NULL);
	if (FAILED(hres))
	{
		return false;
	}

	do
	{
		if (!getOutputReturnValue(pWbcEnumEleOutPut))
		{			
			break;
		}
		VARIANT varElements;
		/*需要注意的是当元素为空的时候，返回值是什么*/
		hres = pWbcEnumEleOutPut->Get(L"Elements", 0, &varElements, NULL, NULL);
		if (FAILED(hres))
		{
			break;
		}
		if (!(varElements.vt & VT_ARRAY)) //返回值应该是个数组
		{
			break;
		}
		for (ULONG i = 0; i < varElements.parray->rgsabound->cElements; i++)
		{
			/*
			IWbemClassObject** pWcoArray = (IWbemClassObject**)(varElements.parray->pvData);
			IWbemClassObject* pWcoEle = pWcoArray[i];
			*/
			IWbemClassObject* pWcoEle = NULL;
			LONG iTmp = (LONG)i;
			SafeArrayGetElement(varElements.parray, &iTmp, &pWcoEle);
			BcdElement bcdEle;

			VARIANT varTmp;
			pWcoEle->Get(L"StoreFilePath", 0, &varTmp, NULL, NULL);
			bcdEle.StoreFilePath = (wchar_t*)varTmp.bstrVal;

			pWcoEle->Get(L"ObjectId", 0, &varTmp, NULL, NULL);
			bcdEle.ObjectId = (wchar_t*)varTmp.bstrVal;

			pWcoEle->Get(L"Type", 0, &varTmp, NULL, NULL);
			bcdEle.Type = varTmp.uintVal;
			vecElement.push_back(bcdEle);
		}
		bReturnValue = true;
		
	} while (0);

	pWbcEnumEleOutPut->Release();
	if (FAILED(hres))
	{
		return false;
	}
	else
	{
		return bReturnValue;
	}
	return false;
}

bool BcdObject::EnumerateElementTypes(std::vector<UINT32>& vecType)
{
	std::vector<BcdElement> vecElement;
	bool bRet = EnumerateElements(vecElement);
	if (!bRet)
	{
		return false;
	}
	for (size_t i = 0; i < vecElement.size(); i++)
	{
		vecType.push_back(vecElement[i].Type);
	}
	return true;
}

bool BcdObject::GetElementWithFlags(ULONG Type, ULONG Flags, BcdElement & Element)
{

	bool bReturnValue = false; //默认为false
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboGetElementInput = NULL;
	hres = m_wbemBcdObject->GetMethod(L"GetElement", 0, &pWboGetElementInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWbcGetelementOutput = NULL;
	do {
		VARIANT varTypeIn;
		varTypeIn.vt = VT_I4;
		varTypeIn.uintVal = Type;
		pWboGetElementInput->Put(L"Type", 0, &varTypeIn, 0);

		VARIANT varFlagsIn;
		varFlagsIn.vt = VT_I4;
		varFlagsIn.uintVal = Flags;
		pWboGetElementInput->Put(L"Flags", 0, &varFlagsIn, 0);

		IWbemServices* pWbemServ = wimBase.getSvc();
		
		hres = pWbemServ->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"GetElementWithFlags"), 0, NULL, pWboGetElementInput, &pWbcGetelementOutput, NULL);
		if (FAILED(hres))
		{			
			break;
		}
		bReturnValue = getOutputReturnValue(pWbcGetelementOutput);
		if (!bReturnValue)
		{			
			break;
		}
		VARIANT varOutEle;
		pWbcGetelementOutput->Get(L"Element", 0, &varOutEle, NULL, NULL);
		IWbemClassObject* pWcoElement = (IWbemClassObject* )varOutEle.byref;
		BcdElement eleTmp;
		VARIANT varStoreFilePath;
		pWcoElement->Get(L"StoreFilePath", 0, &varStoreFilePath, NULL, NULL);		
		eleTmp.StoreFilePath = varStoreFilePath.bstrVal;

		VARIANT varObjectId;
		pWcoElement->Get(L"ObjectId", 0, &varObjectId, NULL, NULL);
		eleTmp.ObjectId = varObjectId.bstrVal;

		VARIANT varType;
		pWcoElement->Get(L"Type", 0, &varType, NULL, NULL);
		eleTmp.Type = varType.uintVal;
	} while (0);
	pWboGetElementInput->Release();
	if (pWbcGetelementOutput != NULL)
	{
		pWbcGetelementOutput->Release();
	}
	
	if (FAILED(hres))
	{
		return false;
	}
	else
	{
		return bReturnValue;
	}
	return false;

}

bool BcdObject::SetBooleanElement(UINT32 Type, BOOL Boolean)
{	
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetBooleanElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}
	VARIANT varTypeIn;
	varTypeIn.vt = VT_I4;
	varTypeIn.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varTypeIn, 0);

	VARIANT varBoolIn;
	varBoolIn.vt = VT_BOOL;
	varBoolIn.boolVal = Boolean;
	pWboInParam->Put(L"Boolean", 0, &varTypeIn, 0);

	IWbemClassObject* pWboOutParam = NULL;
	IWbemServices* pWboSvc = getSvc();
	hres = pWboSvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetBooleanElement"), 0, NULL, pWboInParam, &pWboOutParam, NULL);
	pWboInParam->Release(); //不需要了
	if (FAILED(hres))
	{		
		return false;
	}	
	bool b = getOutputReturnValue(pWboOutParam);
	pWboOutParam->Release();
	return b;
}

bool BcdObject::SetDeviceElement(UINT32 Type, UINT32 DeviceType, std::wstring AdditionalOptions)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetDeviceElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}

	VARIANT varTypeIn;
	varTypeIn.vt = VT_I4;
	varTypeIn.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varTypeIn, 0);

	VARIANT varDeviceTypeIn;
	varDeviceTypeIn.vt = VT_I4;
	varDeviceTypeIn.uintVal = DeviceType;
	pWboInParam->Put(L"DeviceType", 0, &varDeviceTypeIn, 0);

	VARIANT varAdditionalOptionsIn;
	varAdditionalOptionsIn.vt = VT_BSTR;
	varAdditionalOptionsIn.bstrVal = (BSTR)AdditionalOptions.c_str();
	pWboInParam->Put(L"AdditionalOptions", 0, &varAdditionalOptionsIn, 0);
	IWbemServices *pSvc = getSvc();
	IWbemClassObject* pWboOutParam = NULL;
	hres = pSvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetDeviceElement"), 0, NULL, pWboInParam, &pWboOutParam, NULL);
	pWboInParam->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pWboOutParam);
	pWboOutParam->Release();
	return b;
}

bool BcdObject::SetFileDeviceElement(uint32 Type, 
									uint32 DeviceType, 
									std::wstring AdditionalOptions, 
									std::wstring Path, 
									uint32 ParentDeviceType, 
									std::wstring ParentAdditionalOptions, 
									std::wstring ParentPath)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;	
	hres = m_wbemBcdObject->GetMethod(L"SetFileDeviceElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, NULL);

	VARIANT varDeviceType;
	varDeviceType.vt = VT_I4;
	varDeviceType.uintVal = DeviceType;
	pWboInParam->Put(L"DeviceType", 0, &varDeviceType, NULL);

	VARIANT varAdditionalOptions;
	varAdditionalOptions.vt = VT_BSTR;
	varAdditionalOptions.bstrVal = (BSTR)AdditionalOptions.c_str();
	pWboInParam->Put(L"AdditionalOptions", 0, &varAdditionalOptions, NULL);

	VARIANT varPath;
	varPath.vt = VT_BSTR;
	varPath.bstrVal = (BSTR)Path.c_str();
	pWboInParam->Put(L"Path", 0, &varPath, NULL);

	VARIANT varParentDeviceType;
	varParentDeviceType.vt = VT_I4;
	varParentDeviceType.uintVal = ParentDeviceType;
	pWboInParam->Put(L"ParentDeviceType", 0, &varParentDeviceType, NULL);

	VARIANT varParentAdditionalOptions;
	varParentAdditionalOptions.vt = VT_BSTR;
	varParentAdditionalOptions.bstrVal = (BSTR)ParentAdditionalOptions.c_str();
	pWboInParam->Put(L"ParentAdditionalOptions", 0, &varParentAdditionalOptions, NULL);

	VARIANT varParentPath;
	varParentPath.vt = VT_BSTR;
	varParentPath.bstrVal = (BSTR)ParentPath.c_str();
	pWboInParam->Put(L"ParentPath", 0, &varParentPath, NULL);

	IWbemServices* pSvc = getSvc();
	IWbemClassObject* pwcoOutput = NULL;
	hres = pSvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetFileDeviceElement"), 0, NULL, pWboInParam, &pwcoOutput, NULL);
	pWboInParam->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pwcoOutput);
	pwcoOutput->Release();
	return b;
}

bool BcdObject::SetIntegerElement(uint32 Type, uint64 Integer)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetIntegerElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}
	
	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, 0);

	VARIANT varInteger;
	varInteger.vt = VT_I8;
	varInteger.ullVal = Integer;
	pWboInParam->Put(L"Integer", 0, &varInteger, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pWcoOutput = NULL;
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetIntegerElement"), 0, NULL, pWboInParam, &pWcoOutput, NULL);
	pWboInParam->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pWcoOutput);
	pWcoOutput->Release();
	return b;

}

bool BcdObject::SetIntegerListElement(uint32 Type, std::vector<uint64> vecIntegers)
{
	if (vecIntegers.size() == 0)
	{
		return true;
	}
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetIntegerListElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}
	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, 0);

	SAFEARRAY *psa = NULL;
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].cElements = (ULONG)vecIntegers.size();
	rgsabound[0].lLbound = 0;
	psa = SafeArrayCreate(VT_I8, 1, rgsabound);
	for (size_t i = 0; i < vecIntegers.size(); i++)
	{
		uint64  integer64 = vecIntegers[i];
		LONG iTmp = (LONG)i;
		SafeArrayPutElement(psa, &iTmp, &integer64);
	}

	VARIANT varIntegers;
	varIntegers.vt = VT_ARRAY | VT_UI8;
	varIntegers.parray = psa;
	hres = pWboInParam->Put(L"Integers", 0, &varIntegers, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pWcoOutput = NULL;
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetIntegerListElement"), 0, NULL, pWboInParam, &pWcoOutput, NULL);
	pWboInParam->Release();
	SafeArrayDestroyDescriptor(psa);
	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pWcoOutput);
	pWcoOutput->Release();
	return b;
}

bool BcdObject::SetObjectElement(uint32 Type, std::wstring Id)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetObjectElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, NULL);

	VARIANT varId;
	varId.vt = VT_BSTR;
	varId.bstrVal = (BSTR)Id.c_str();
	pWboInParam->Put(L"Id", 0, &varId, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pWboOutput = NULL;
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetObjectElement"), 0, NULL, pWboInParam, &pWboOutput, NULL);
	pWboInParam->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pWboOutput);
	pWboOutput->Release();
	return b;
}

bool BcdObject::SetObjectListElement(uint32 Type, std::vector<std::wstring> vecIds)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetObjectListElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, NULL);

	VARIANT varIds;
	varIds.vt = VT_ARRAY | VT_BSTR;
	SAFEARRAY *psa = NULL;
	SAFEARRAYBOUND bound[1];
	bound[0].cElements = (ULONG)vecIds.size();
	bound[0].lLbound = 0;
	psa = SafeArrayCreate(VT_BSTR, 1, bound);
	for (int i = 0; i < vecIds.size(); i++)
	{
		long iTmp = (long)i;
		std::wstring wstrId = vecIds[i];
		BSTR bstrTmp = (BSTR)wstrId.c_str();
		SafeArrayPutElement(psa, &iTmp, bstrTmp); //最后一个参数不知道是否正确
	}
	varIds.parray = psa;
	pWboInParam->Put(L"Ids", 0, &varIds, NULL);
	IWbemClassObject* pwboOutput = NULL;
	IWbemServices* psvc = getSvc();
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetObjectListElement"), 0, NULL, pWboInParam, &pwboOutput, NULL);
	SafeArrayDestroyDescriptor(psa);
	pWboInParam->Release();

	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return b;
}

bool BcdObject::SetPartitionDeviceElement(uint32 Type, uint32 DeviceType, std::wstring AdditionalOptions, std::wstring Path)
{
	return SetPartitionDeviceElementWithFlags(Type, DeviceType, AdditionalOptions, Path, 0);
}

bool BcdObject::SetPartitionDeviceElementWithFlags(uint32 Type, 
													uint32 DeviceType, 
													std::wstring AdditionalOptions, 
													std::wstring Path, 
													uint32 Flags)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetPartitionDeviceElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, NULL);

	VARIANT varDeviceType;
	varDeviceType.vt = VT_I4;
	varDeviceType.uintVal = DeviceType;
	pWboInParam->Put(L"DeviceType", 0, &varDeviceType, NULL);

	VARIANT varAdditionalOptionse;
	varAdditionalOptionse.vt = VT_BSTR;
	varAdditionalOptionse.bstrVal = (BSTR)AdditionalOptions.c_str();
	pWboInParam->Put(L"varAdditionalOptionse", 0, &varAdditionalOptionse, NULL);

	VARIANT varPath;
	varPath.vt = VT_BSTR;
	varPath.bstrVal = (BSTR)Path.c_str();
	pWboInParam->Put(L"DeviceType", 0, &varPath, NULL);

	VARIANT varFlags;
	varFlags.vt = VT_I4;
	varFlags.uintVal = Flags;
	pWboInParam->Put(L"Flags", 0, &varFlags, NULL);

	IWbemClassObject* pwboOutput = NULL;
	IWbemServices* psvc = getSvc();
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetPartitionDeviceElement"), 0, NULL, pWboInParam, &pwboOutput, NULL);
	pWboInParam->Release();

	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return b;
}

bool BcdObject::SetQualifiedPartitionDeviceElement(ULONG Type, ULONG PartitionStyle, PCWSTR DiskSignature, PCWSTR PartitionIdentifier)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetQualifiedPartitionDeviceElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, NULL);

	VARIANT varPartitionStyle;
	varPartitionStyle.vt = VT_I4;
	varPartitionStyle.uintVal = PartitionStyle;
	pWboInParam->Put(L"PartitionStyle", 0, &varPartitionStyle, NULL);

	VARIANT varDiskSignature;
	varDiskSignature.vt = VT_LPWSTR;
	varDiskSignature.bstrVal = (BSTR)DiskSignature;
	pWboInParam->Put(L"DiskSignature", 0, &varDiskSignature, NULL);

	VARIANT varPartitionIdentifier;
	varPartitionIdentifier.vt = VT_LPWSTR;
	varPartitionIdentifier.bstrVal = (BSTR)PartitionIdentifier;
	pWboInParam->Put(L"PartitionIdentifier", 0, &varPartitionIdentifier, NULL);

	IWbemClassObject* pwboOutput = NULL;
	IWbemServices* psvc = getSvc();
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetQualifiedPartitionDeviceElement"), 0, NULL, pWboInParam, &pwboOutput, NULL);
	pWboInParam->Release();

	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return b;
}

bool BcdObject::SetStringElement(uint32 Type, std::wstring String)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetStringElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, NULL);

	VARIANT varString;
	varString.vt = VT_BSTR;
	varString.bstrVal = (BSTR)String.c_str();
	pWboInParam->Put(L"String", 0, &varString, NULL);

	IWbemClassObject* pwboOutput = NULL;
	IWbemServices* psvc = getSvc();
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetStringElement"), 0, NULL, pWboInParam, &pwboOutput, NULL);
	pWboInParam->Release();

	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return b;
}

bool BcdObject::SetVhdDeviceElement(ULONG Type, 
	PCWSTR Path, 
	ULONG ParentDeviceType, 
	PCWSTR ParentAdditionalOptions, 
	PCWSTR ParentPath, 
	ULONG CustomLocate)
{

	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"SetVhdDeviceElement", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, NULL);

	VARIANT varPath;
	varPath.vt = VT_LPWSTR;
	varPath.bstrVal = (BSTR)Path;
	pWboInParam->Put(L"Path", 0, &varPath, NULL);

	VARIANT varParentDeviceType;
	varParentDeviceType.vt = VT_I4;
	varParentDeviceType.uintVal = ParentDeviceType;
	pWboInParam->Put(L"ParentDeviceType", 0, &varParentDeviceType, NULL);

	VARIANT varParentAdditionalOptions;
	varParentAdditionalOptions.vt = VT_LPWSTR;
	varParentAdditionalOptions.bstrVal = (BSTR)ParentAdditionalOptions;
	pWboInParam->Put(L"ParentAdditionalOptions", 0, &varParentAdditionalOptions, NULL);

	VARIANT varParentPath;
	varParentPath.vt = VT_LPWSTR;
	varParentPath.bstrVal = (BSTR)ParentPath;
	pWboInParam->Put(L"ParentPath", 0, &varParentPath, NULL);

	VARIANT varCustomLocate;
	varCustomLocate.vt = VT_I4;
	varCustomLocate.uintVal = CustomLocate;
	pWboInParam->Put(L"CustomLocate", 0, &varCustomLocate, NULL);

	IWbemClassObject* pwboOutput = NULL;
	IWbemServices* psvc = getSvc();
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, BSTR(L"SetVhdDeviceElement"), 0, NULL, pWboInParam, &pwboOutput, NULL);
	pWboInParam->Release();

	if (FAILED(hres))
	{
		return false;
	}
	bool b = getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return b;
}

EleValueType BcdObject::GetElementValueType(ULONG Type)
{
	IWbemClassObject* pwboGetElementWithFlagsOutput = getElementClassObj(Type);
	if (pwboGetElementWithFlagsOutput == NULL)
	{
		return EleValueType_error;
	}

	VARIANT varElement;
	HRESULT hres;
 	hres =	pwboGetElementWithFlagsOutput->Get(L"Element", 0, &varElement, NULL, NULL);
	if (FAILED(hres))
	{
		pwboGetElementWithFlagsOutput->Release();
		return EleValueType_error;
	}
	EleValueType valueType = EleValueType_error;
	IWbemClassObject* pWboEle = (IWbemClassObject*)varElement.byref;
	VARIANT varClass;
	pWboEle->Get(L"__Class", 0, &varClass, NULL, NULL);
	if (wcscmp(varClass.bstrVal, L"BcdBooleanElement"))
	{
		valueType = EleValueType_Boolean;
	}
	else if (wcscmp(varClass.bstrVal, L"BcdDeviceElement"))
	{
		valueType = EleValueType_Device;
	}
	else if (wcscmp(varClass.bstrVal, L"BcdElement"))
	{
		valueType = EleValueType_Base;
	}
	else if (wcscmp(varClass.bstrVal, L"BcdIntegerElement"))
	{
		valueType = EleValueType_Integer;
	}
	else if (wcscmp(varClass.bstrVal, L"BcdIntegerListElement"))
	{
		valueType = EleValueType_IntegersArray;
	}
	else if (wcscmp(varClass.bstrVal, L"BcdObjectElement"))
	{
		valueType = EleValueType_BcdObjectId;
	}
	else if (wcscmp(varClass.bstrVal, L"BcdObjectListElement"))
	{
		valueType = EleValueType_BcdObjectIdArray;
	}
	else if (wcscmp(varClass.bstrVal, L"BcdStringElement"))
	{
		valueType = EleValueType_BcdString;
	}
	else if (wcscmp(varClass.bstrVal, L"BcdUnknownElement"))
	{
		valueType = EleValueType_BcdUnknown;
	}
	pwboGetElementWithFlagsOutput->Release();
	return valueType;
}

bool BcdObject::GetElement(UINT32 Type, BcdElement &Element)
{	
	return GetElementWithFlags(Type, 0, Element);
}

HRESULT BcdObject::getBcdObjectPath(VARIANT &varBcdObjectPath)
{
	VARIANT varBcdObjectPathTmp;
	HRESULT hres = m_wbemBcdObject->Get(L"__RELPATH", 0, &varBcdObjectPathTmp, NULL, NULL);
	if(!FAILED(hres))
	{
		varBcdObjectPath = varBcdObjectPathTmp;
	}
	return hres;
}

bool BcdObject::getOutputReturnValue(IWbemClassObject * pWbcOutPut)
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

IWbemServices* BcdObject::getSvc()
{
	return wimBase.getSvc();	
}

IWbemClassObject * BcdObject::getElementClassObj(ULONG Type)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdObject->GetMethod(L"GetElementWithFlags", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  false;
	}

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, NULL);

	VARIANT varFlags;
	varFlags.vt = VT_I4;
	varFlags.uintVal = -1;
	pWboInParam->Put(L"Flags", 0, &varFlags, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwcoOutput = NULL;
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, (BSTR)L"GetElementWithFlags", 0, NULL, pWboInParam, &pwcoOutput, NULL);
	pWboInParam->Release();
	if (FAILED(hres))
	{
		pWboInParam->Release();
		return NULL;
	}
	do
	{
		bool bReturnValue = getOutputReturnValue(pwcoOutput);
		if (!bReturnValue)
		{
			break;
		}
		return pwcoOutput;		
	} while (0);
	pwcoOutput->Release();
	return NULL;
}

BcdObject* CreateBcdObjectObj(IWbemClassObject* pwboBcdObject)
{
	return new BcdObject(pwboBcdObject);
}
