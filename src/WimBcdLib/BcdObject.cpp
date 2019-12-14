#include "BcdObject.h"
#include <cassert>


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
std::wstring BcdObject::GetBcdObjectId()
{
	return m_Id;
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
	///////////////////////////////////////////////////////////////////////
	//
	IWbemClassObject* wbemBcdClass = NULL;
	HRESULT  hres = (WBEMSTATUS)getSvc()->GetObject(BSTR(L"BcdObject"), 0, NULL, &wbemBcdClass, NULL);
	if (FAILED(hres))
	{
		//
	}
	m_wbemBcdClass = wbemBcdClass;
	///////////////////////////////////////////////////////////////////////
	m_wbemBcdObject = pwco;
	GetBcdType(m_Type);
	GetBcdObjectId(m_Id);
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

		IWbemServices* pWbemServ = getSvc();
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

bool BcdObject::EnumerateElements(std::vector<BcdElement*>& vecElement)
{
	bool bReturnValue = false; //默认为false
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return false;
	}
	IWbemServices* pWbemServ = getSvc();
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
			/*
			EleValueType valueType = GetElementValueType2(pWcoEle);
			BcdElement bcdEle;
			VARIANT varTmp;
			pWcoEle->Get(L"StoreFilePath", 0, &varTmp, NULL, NULL);
			bcdEle.StoreFilePath = (wchar_t*)varTmp.bstrVal;

			pWcoEle->Get(L"ObjectId", 0, &varTmp, NULL, NULL);
			bcdEle.ObjectId = (wchar_t*)varTmp.bstrVal;

			pWcoEle->Get(L"Type", 0, &varTmp, NULL, NULL);
			bcdEle.Type = varTmp.uintVal;
			vecElement.push_back(bcdEle);
			*/
			BcdElement* pEle = BuildBcdElementStruct2(pWcoEle);
			if (pEle == NULL)
			{
				continue;
			}
			vecElement.push_back(pEle);
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
	std::vector<BcdElement*> vecElement;
	bool bRet = EnumerateElements(vecElement);
	if (!bRet)
	{
		return false;
	}
	for (size_t i = 0; i < vecElement.size(); i++)
	{
		vecType.push_back(vecElement[i]->Type);
		delete vecElement[i];
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

		IWbemServices* pWbemServ = getSvc();
		
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
	EleValueType valueType = EleValueType_error;
	IWbemClassObject* pWboEle = getElementClassObj(Type);
	if (pWboEle == NULL)
	{
		return valueType;
	}		
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
	return valueType;
}
EleValueType BcdObject::GetElementValueType2(IWbemClassObject* pwboEleObject)
{
	EleValueType valueType = EleValueType_error;
	if (pwboEleObject == NULL)
	{
		return valueType;
	}
	VARIANT varClass;
	pwboEleObject->Get(L"__Class", 0, &varClass, NULL, NULL);
	if (!wcscmp(varClass.bstrVal, L"BcdBooleanElement"))
	{
		valueType = EleValueType_Boolean;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdDeviceElement"))
	{
		valueType = EleValueType_Device;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdElement"))
	{
		valueType = EleValueType_Base;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdIntegerElement"))
	{
		valueType = EleValueType_Integer;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdIntegerListElement"))
	{
		valueType = EleValueType_IntegersArray;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdObjectElement"))
	{
		valueType = EleValueType_BcdObjectId;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdObjectListElement"))
	{
		valueType = EleValueType_BcdObjectIdArray;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdStringElement"))
	{
		valueType = EleValueType_BcdString;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdUnknownElement"))
	{
		valueType = EleValueType_BcdUnknown;
	}
	return valueType;
}
bool BcdObject::GetBcdObjectDescription(std::wstring &wstrDescription)
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//
	std::wstring wstrBcdObjectDescription;
	if (!wcscmp(m_Id.c_str(), Windows_Boot_Manager_GUID))
	{
		wstrBcdObjectDescription = L"Windows Boot Manager";
	}
	else if (!wcscmp(m_Id.c_str(), Firmware_Boot_Manager_GUID))
	{
		wstrBcdObjectDescription = L"Firmware Boot Manager";
	}
	else if (!_wcsicmp(m_Id.c_str(), Standard_Inheritable_badmemory_GUID))
	{
		wstrBcdObjectDescription = L"{badmemory}";
	}
	else if (!_wcsicmp(m_Id.c_str(), Standard_Inheritable_bootloadersettings_GUID))
	{
		wstrBcdObjectDescription = L"{bootloadersettings}";
	}
	else if (!_wcsicmp(m_Id.c_str(), Standard_Inheritable_dbgsettings_GUID))
	{
		wstrBcdObjectDescription = L"{dbgsettings}";
	}
	else if (!_wcsicmp(m_Id.c_str(), Standard_Inheritable_emssetting_GUID))
	{
		wstrBcdObjectDescription = L"{emssetting}";
	}
	else if (!_wcsicmp(m_Id.c_str(), Standard_Inheritable_globalsettings_GUID))
	{
		wstrBcdObjectDescription = L"{globalsettings}";
	}
	else if (!_wcsicmp(m_Id.c_str(), Standard_Inheritable_resumeloadersettings_GUID))
	{
		wstrBcdObjectDescription = L"{resumeloadersettings}";
	}
	else if (!_wcsicmp(m_Id.c_str(), Standard_Inheritable_hypervisorsettings_GUID))
	{
		wstrBcdObjectDescription = L"{hypervisorsettings}";
	}
	else if (!_wcsicmp(m_Id.c_str(), Standard_Ramdisk_Options_GUID))
	{
		wstrBcdObjectDescription = L"{ramdiskoptions}";
	}
	if (wstrBcdObjectDescription.length())
	{
		wstrDescription = wstrBcdObjectDescription;
		return true;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	IWbemClassObject* pWboEle = getElementClassObj(0x12000004);
	if (pWboEle != NULL)
	{
		EleValueType eleValueType = GetElementValueType2(pWboEle);
		if (eleValueType != EleValueType_BcdString)
		{
			pWboEle->Release();
			return false;
		}
		BcdStringElement* stringEle = Build_BcdString_ElementStruct(pWboEle);
		wstrDescription = stringEle->String;
		return true;	
	}

	///////////////////////////////////////////////////////////
	ObjectCode oc = GetObjectCode();
	switch (oc)
	{
		case Application:
		{
			wstrDescription = L"Application Object";
			break;
		}

		case Inheritable:
		{
			wstrDescription = L"Inheritable Object";
			break;
		}
		case Device:
		{
			wstrDescription = L"Device Object";
			break;
		}
		default:
		{
			return false;			
		}
		
	}
	////////////////////////////////////////////////////////////
	return true;
}

BCD_DEVICE_DATA_TYPE BcdObject::GetDeviceElementDataType(IWbemClassObject* pwboDeviceDataObject)
{
	BCD_DEVICE_DATA_TYPE deviceDataValueType = DEVICE_DATA_TYPE_DeviceUnknownData;
	if (pwboDeviceDataObject == NULL)
	{
		return deviceDataValueType;
	}
	VARIANT varClass;
	pwboDeviceDataObject->Get(L"__Class", 0, &varClass, NULL, NULL);
	if (!wcscmp(varClass.bstrVal, L"BcdDeviceFileData"))
	{
		deviceDataValueType = DEVICE_DATA_TYPE_DeviceFileData;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdDeviceLocateData"))
	{
		deviceDataValueType = DEVICE_DATA_TYPE_DeviceLocateData;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdDeviceLocateElementChildData"))
	{
		deviceDataValueType = DEVICE_DATA_TYPE_DeviceLocateElementChildData;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdDeviceLocateElementData"))
	{
		deviceDataValueType = DEVICE_DATA_TYPE_DeviceLocateElementData;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdDeviceLocateStringData"))
	{
		deviceDataValueType = DEVICE_DATA_TYPE_DeviceLocateStringData;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdDevicePartitionData"))
	{
		deviceDataValueType = DEVICE_DATA_TYPE_DevicePartitionData;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdDeviceQualifiedPartitionData"))
	{
		deviceDataValueType = DEVICE_DATA_TYPE_DeviceQualifiedPartitionData;
	}
	else if (!wcscmp(varClass.bstrVal, L"BcdDeviceUnknownData"))
	{
		deviceDataValueType = DEVICE_DATA_TYPE_DeviceUnknownData;
	}
	else
	{
		deviceDataValueType = DEVICE_DATA_TYPE_DeviceData;
	}
	return deviceDataValueType;
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

IWbemClassObject * BcdObject::getElementClassObj(ULONG Type)
{
	VARIANT varBcdObjectPath;
	HRESULT hres = getBcdObjectPath(varBcdObjectPath);
	if (FAILED(hres))
	{
		return NULL;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdClass->GetMethod(L"GetElementWithFlags", 0, &pWboInParam, NULL);
	if (FAILED(hres))
	{
		return  NULL;
	}

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pWboInParam->Put(L"Type", 0, &varType, NULL);


	VARIANT varFlags;
	varFlags.vt = VT_I4;
	varFlags.uintVal = 1;
	pWboInParam->Put(L"Flags", 0, &varFlags, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwcoOutput = NULL;
	hres = psvc->ExecMethod(varBcdObjectPath.bstrVal, (BSTR)L"GetElementWithFlags", 0, NULL, pWboInParam, &pwcoOutput, NULL);
	pWboInParam->Release();
	if (FAILED(hres))
	{		
		return NULL;
	}
	IWbemClassObject* pWboEle = NULL;
	do
	{
		bool bReturnValue = getOutputReturnValue(pwcoOutput);
		if (!bReturnValue)
		{
			break;
		}
		VARIANT varElement;
		HRESULT hres;
		hres = pwcoOutput->Get(L"Element", 0, &varElement, NULL, NULL);		
		if (FAILED(hres))
		{
			break;
		}
		pWboEle = (IWbemClassObject*)varElement.byref;	
	} while (0);
	pwcoOutput->Release();
	return pWboEle;
}

BcdObject* CreateBcdObjectObj(IWbemClassObject* pwboBcdObject)
{
	return new BcdObject(pwboBcdObject);
}

BcdElement BcdObject::BuildBcdElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdElement ele;

	HRESULT hres = pwboEle->Get(L"StoreFilePath", 0, &varTmp, NULL, NULL);
	ele.StoreFilePath = varTmp.bstrVal;

	memset(&varTmp, 0, sizeof(VARIANT));
	hres = pwboEle->Get(L"ObjectId", 0, &varTmp, NULL, NULL);
	ele.ObjectId = varTmp.bstrVal;

	memset(&varTmp, 0, sizeof(VARIANT));
	hres = pwboEle->Get(L"Type", 0, &varTmp, NULL, NULL);
	ele.Type = varTmp.uintVal;

	//EleValueType eleValueType = GetElementValueType2(pwboEle);

	return ele;
}
BcdElement* BcdObject::BuildBcdElementStruct2(IWbemClassObject* pwboEle)
{
	EleValueType eleValueType = GetElementValueType2(pwboEle);
	
	switch (eleValueType)
	{
	case EleValueType_error:
	{
		return NULL;
	}
	break;
	case EleValueType_Base:
	{
		BcdElement bcdEle = BuildBcdElementStruct(pwboEle);		
		BcdElement* pBcdEle = new BcdElement;
		pBcdEle->valueType = EleValueType_Base;
		*pBcdEle = bcdEle;		
		return pBcdEle;
		break;
	}
		
	case EleValueType_Device:
		return Build_BcdDevice_ElementStruct(pwboEle);
		break;
	case EleValueType_Boolean: 
		return Build_BcdBoolean_ElementStruct(pwboEle);
		break;
	case EleValueType_Integer://用字符串保存的64位数值
		return Build_BcdInteger_ElementStruct(pwboEle);
		break;
	case EleValueType_IntegersArray:
		return Build_BCDIntegerList_ElementStruct(pwboEle);
		break;
	case EleValueType_BcdObjectId:
		return Build_BcdObjec_ElementStruct(pwboEle);
		break;
	case EleValueType_BcdObjectIdArray:
		return Build_BcdObjectList_ElementStruct(pwboEle);
		break;
	case EleValueType_BcdString:
		return Build_BcdString_ElementStruct(pwboEle);
		break;
	case EleValueType_BcdUnknown:
		return Build_BcdUnknown_ElementStruct(pwboEle);
		break;
	default:
		break;
	}
	return NULL;
}
BcdStringElement* BcdObject::Build_BcdString_ElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdStringElement *stringEle = new BcdStringElement;
	HRESULT hres = pwboEle->Get(L"String", 0, &varTmp, NULL, NULL);

	stringEle->String = varTmp.bstrVal;

	BcdElement ele = BuildBcdElementStruct(pwboEle);
	stringEle->ObjectId = ele.ObjectId;
	stringEle->StoreFilePath = ele.StoreFilePath;
	stringEle->Type = ele.Type;
	stringEle->valueType = EleValueType_BcdString;
	return stringEle;
}

BcdDeviceElement* BcdObject::Build_BcdDevice_ElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdDeviceElement *deviceEle = new BcdDeviceElement;

	BcdElement ele = BuildBcdElementStruct(pwboEle);
	deviceEle->ObjectId = ele.ObjectId;
	deviceEle->StoreFilePath = ele.StoreFilePath;
	deviceEle->Type = ele.Type;
	deviceEle->valueType = EleValueType_Device;

	memset(&varTmp, 0, sizeof(VARIANT));
	HRESULT hres = pwboEle->Get(L"Device", 0, &varTmp, NULL, NULL);	
	IWbemClassObject* wcoDevice = (IWbemClassObject*)varTmp.byref;
	deviceEle->Device = BuildBcdDevice(wcoDevice);
	return deviceEle;
}


BcdBooleanElement* BcdObject::Build_BcdBoolean_ElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdBooleanElement *boolEle = new BcdBooleanElement;
	HRESULT hres = pwboEle->Get(L"Boolean", 0, &varTmp, NULL, NULL);

	boolEle->Boolean = varTmp.boolVal;

	BcdElement ele = BuildBcdElementStruct(pwboEle);
	boolEle->ObjectId = ele.ObjectId;
	boolEle->StoreFilePath = ele.StoreFilePath;
	boolEle->Type = ele.Type;
	boolEle->valueType = EleValueType_Boolean;
	return boolEle;
}

BcdIntegerElement* BcdObject::Build_BcdInteger_ElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdIntegerElement *integerEle = new BcdIntegerElement;
	//用字符串保存的64位数值
	HRESULT hres = pwboEle->Get(L"Integer", 0, &varTmp, NULL, NULL);	
	integerEle->Integer = _wtoi64(varTmp.bstrVal);

	BcdElement ele = BuildBcdElementStruct(pwboEle);
	integerEle->ObjectId = ele.ObjectId;
	integerEle->StoreFilePath = ele.StoreFilePath;
	integerEle->Type = ele.Type;
	integerEle->valueType = EleValueType_Integer;
	return integerEle;
}

BcdIntegerListElement* BcdObject::Build_BCDIntegerList_ElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdIntegerListElement *integerListEle = new BcdIntegerListElement;
	HRESULT hres = pwboEle->Get(L"Integers", 0, &varTmp, NULL, NULL);

	if (!(varTmp.vt & VT_ARRAY)) //返回值应该是个数组
	{
		return NULL;
	}
	for (ULONG i = 0; i < varTmp.parray->rgsabound->cElements; i++)
	{
		BSTR bStr;
		LONG iTmp = (LONG)i;
		SafeArrayGetElement(varTmp.parray, &iTmp, &bStr);
		integerListEle->vecInteger.push_back((wchar_t*)bStr);
	}
	BcdElement ele = BuildBcdElementStruct(pwboEle);
	integerListEle->ObjectId = ele.ObjectId;
	integerListEle->StoreFilePath = ele.StoreFilePath;
	integerListEle->Type = ele.Type;
	integerListEle->valueType = EleValueType_IntegersArray;
	return integerListEle;
}

BcdObjectElement* BcdObject::Build_BcdObjec_ElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdObjectElement *objectEle = new BcdObjectElement;
	HRESULT hres = pwboEle->Get(L"Id", 0, &varTmp, NULL, NULL);
	objectEle->Id = varTmp.bstrVal;
	BcdElement ele = BuildBcdElementStruct(pwboEle);
	objectEle->ObjectId = ele.ObjectId;
	objectEle->StoreFilePath = ele.StoreFilePath;
	objectEle->Type = ele.Type;
	objectEle->valueType = EleValueType_BcdObjectId;
	return objectEle;
}

BcdObjectListElement* BcdObject::Build_BcdObjectList_ElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdObjectListElement *objectListEle = new BcdObjectListElement;
	HRESULT hres = pwboEle->Get(L"Ids", 0, &varTmp, NULL, NULL);

	if (!(varTmp.vt & VT_ARRAY)) //返回值应该是个数组
	{
		return NULL;
	}
	for (ULONG i = 0; i < varTmp.parray->rgsabound->cElements; i++)
	{
		BSTR bStr;
		LONG iTmp = (LONG)i;
		SafeArrayGetElement(varTmp.parray, &iTmp, &bStr);
		objectListEle->vecId.push_back((wchar_t*)bStr);
	}
	BcdElement ele = BuildBcdElementStruct(pwboEle);
	objectListEle->ObjectId = ele.ObjectId;
	objectListEle->StoreFilePath = ele.StoreFilePath;
	objectListEle->Type = ele.Type;
	objectListEle->valueType = EleValueType_BcdObjectIdArray;
	return objectListEle;
}

BcdUnknownElement* BcdObject::Build_BcdUnknown_ElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdUnknownElement *unknownEle = new BcdUnknownElement;
	HRESULT hres = pwboEle->Get(L"ActualType", 0, &varTmp, NULL, NULL);
	unknownEle->ActualType = varTmp.ullVal;
	BcdElement ele = BuildBcdElementStruct(pwboEle);
	unknownEle->ObjectId = ele.ObjectId;
	unknownEle->StoreFilePath = ele.StoreFilePath;
	unknownEle->Type = ele.Type;
	unknownEle->valueType = EleValueType_BcdUnknown;
	return unknownEle;
}

BcdDeviceData* BcdObject::BuildBcdDevice_Data(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdDeviceData *deviceData = new BcdDeviceData;
	HRESULT hres = pwboEle->Get(L"DeviceType", 0, &varTmp, NULL, NULL);
	deviceData->DeviceType = (BCD_DEVICE_TYPE)varTmp.uintVal;

	memset(&varTmp, 0, sizeof(VARIANT));
	hres = pwboEle->Get(L"AdditionalOptions", 0, &varTmp, NULL, NULL);
	deviceData->AdditionalOptions = varTmp.bstrVal;
	return deviceData;
}
BcdDeviceFileData* BcdObject::BuildBcdDevice_FileData(IWbemClassObject* pwbo)
{
	BcdDeviceFileData *fileData = new BcdDeviceFileData;
	BcdDeviceData* deviceData = BuildBcdDevice_Data(pwbo);
	fileData->AdditionalOptions = deviceData->AdditionalOptions;
	fileData->DeviceType = deviceData->DeviceType;
	fileData->DataType = DEVICE_DATA_TYPE_DeviceFileData;

	delete deviceData;
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	HRESULT hres = pwbo->Get(L"Path", 0, &varTmp, NULL, NULL);
	fileData->Path = varTmp.bstrVal;

	memset(&varTmp, 0, sizeof(VARIANT));
	hres = pwbo->Get(L"Parent", 0, &varTmp, NULL, NULL);
	if (!FAILED(hres) && varTmp.byref != NULL)
	{
		IWbemClassObject* parentWbo = (IWbemClassObject*)varTmp.byref;
		fileData->Parent = BuildBcdDevice(parentWbo);		
	}

	return fileData;
}

BcdDeviceLocateData* BcdObject::BuildBcdDevice_LocateData(IWbemClassObject* pwbo)
{
	BcdDeviceData* deviceData =  BuildBcdDevice_Data(pwbo);
	BcdDeviceLocateData* locateData = new BcdDeviceLocateData;
	locateData->AdditionalOptions = deviceData->AdditionalOptions;
	locateData->DeviceType = deviceData->DeviceType;
	locateData->DataType = DEVICE_DATA_TYPE_DeviceLocateData;
	delete deviceData;

	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	HRESULT hres = pwbo->Get(L"Type", 0, &varTmp, NULL, NULL);
	locateData->Type = (BcdDeviceLocateDataType)varTmp.uintVal;
	return locateData;
}

BcdDevicePartitionData* BcdObject::BuildBcdDevice_PartitionData(IWbemClassObject* pwbo)
{
	BcdDeviceData* deviceData = BuildBcdDevice_Data(pwbo);
	BcdDevicePartitionData* partitionData = new BcdDevicePartitionData;
	partitionData->AdditionalOptions = deviceData->AdditionalOptions;
	partitionData->DeviceType = deviceData->DeviceType;
	partitionData->DataType = DEVICE_DATA_TYPE_DevicePartitionData;
	delete deviceData;

	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	HRESULT hres = pwbo->Get(L"Path", 0, &varTmp, NULL, NULL);
	partitionData->Path = varTmp.bstrVal;
	return partitionData;
}

BcdDeviceQualifiedPartitionData* BcdObject::BuildBcdDevice_QualiPartitionData(IWbemClassObject* pwbo)
{
	BcdDeviceData* deviceData = BuildBcdDevice_Data(pwbo);
	BcdDeviceQualifiedPartitionData* qualifiedPartitionData = new BcdDeviceQualifiedPartitionData;
	qualifiedPartitionData->AdditionalOptions = deviceData->AdditionalOptions;
	qualifiedPartitionData->DeviceType = deviceData->DeviceType;
	qualifiedPartitionData->DataType = DEVICE_DATA_TYPE_DeviceQualifiedPartitionData;
	delete deviceData;

	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	HRESULT hres = pwbo->Get(L"PartitionStyle", 0, &varTmp, NULL, NULL);
	qualifiedPartitionData->PartitionStyle = (DevicePartitionStyle)varTmp.uintVal;

	memset(&varTmp, 0, sizeof(VARIANT));
	hres = pwbo->Get(L"DiskSignature", 0, &varTmp, NULL, NULL);
	qualifiedPartitionData->DiskSignature = varTmp.bstrVal;

	memset(&varTmp, 0, sizeof(VARIANT));
	hres = pwbo->Get(L"PartitionIdentifier", 0, &varTmp, NULL, NULL);
	qualifiedPartitionData->PartitionIdentifier = varTmp.bstrVal;

	return qualifiedPartitionData;

}

BcdDeviceUnknownData* BcdObject::BuildBcdDevice_UnknownData(IWbemClassObject* pwbo)
{
	BcdDeviceData* deviceData = BuildBcdDevice_Data(pwbo);
	BcdDeviceUnknownData* unknownData = new BcdDeviceUnknownData;
	unknownData->AdditionalOptions = deviceData->AdditionalOptions;
	unknownData->DeviceType = deviceData->DeviceType;
	unknownData->DataType = DEVICE_DATA_TYPE_DeviceUnknownData;
	delete deviceData;

	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	HRESULT hres = pwbo->Get(L"Data", 0, &varTmp, NULL, NULL);
	unsigned int bufferSize = varTmp.parray->rgsabound->cElements;
	unsigned char *pBuffer  = new unsigned char[bufferSize];
	memcpy(pBuffer, varTmp.parray->pvData, bufferSize);
	unknownData->Data = pBuffer;
	unknownData->DataSize = bufferSize;
	return unknownData;
}

BcdDeviceLocateStringData* BcdObject::BuildBcdDevice_LocateStringData(IWbemClassObject* pwbo)
{
	BcdDeviceLocateStringData* locateStringData = new BcdDeviceLocateStringData;
	BcdDeviceLocateData* dldTmp = (BcdDeviceLocateData*)locateStringData;
	BcdDeviceLocateData* dld = BuildBcdDevice_LocateData(pwbo);
	*dldTmp = *dld;
	delete dld;
	locateStringData->DataType = DEVICE_DATA_TYPE_DeviceLocateStringData;
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	HRESULT hres = pwbo->Get(L"Path", 0, &varTmp, NULL, NULL);
	locateStringData->Path = varTmp.bstrVal;
	return locateStringData;
}

BcdDeviceLocateElementData* BcdObject::BuildBcdDevice_LocateElementData(IWbemClassObject* pwbo)
{
	BcdDeviceLocateElementData* locateElementData = new BcdDeviceLocateElementData;
	BcdDeviceLocateData* dldTmp = (BcdDeviceLocateData*)locateElementData;
	BcdDeviceLocateData* dld = BuildBcdDevice_LocateData(pwbo);
	*dldTmp = *dld;
	delete dld;
	locateElementData->DataType = DEVICE_DATA_TYPE_DeviceLocateElementData;
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	HRESULT hres = pwbo->Get(L"Element", 0, &varTmp, NULL, NULL);
	locateElementData->Element = varTmp.uintVal;
	return locateElementData;
}

BcdDeviceLocateElementChildData* BcdObject::BuildBcdDevice_LocateElementChildData(IWbemClassObject* pwbo)
{
	BcdDeviceLocateElementChildData* locateElementChildData = new BcdDeviceLocateElementChildData;
	BcdDeviceLocateData* dldTmp = (BcdDeviceLocateData*)locateElementChildData;
	BcdDeviceLocateData* dld = BuildBcdDevice_LocateData(pwbo);
	*dldTmp = *dld;
	delete dld;
	locateElementChildData->DataType = DEVICE_DATA_TYPE_DeviceLocateElementChildData;
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	HRESULT hres = pwbo->Get(L"Element", 0, &varTmp, NULL, NULL);
	locateElementChildData->Element = varTmp.uintVal;

	memset(&varTmp, 0, sizeof(VARIANT));
	hres = pwbo->Get(L"Parent", 0, &varTmp, NULL, NULL);
	if (varTmp.byref != NULL)
	{
		IWbemClassObject* pParentWbo = (IWbemClassObject*)varTmp.byref;
		locateElementChildData->Parent = BuildBcdDevice(pParentWbo);
	}
	else
	{
		locateElementChildData->Parent = NULL;
	}	
	return locateElementChildData;
}

BcdDeviceData* BcdObject::BuildBcdDevice(IWbemClassObject* pwboEle)
{
	BCD_DEVICE_DATA_TYPE deviceDataType = GetDeviceElementDataType(pwboEle);
	BcdDeviceData* bcdDeviceData = NULL;
	switch (deviceDataType)
	{
		case DEVICE_DATA_TYPE_DeviceData:
		{
			bcdDeviceData = BuildBcdDevice_Data(pwboEle);
		}
		break;
		case DEVICE_DATA_TYPE_DeviceFileData:
		{
			bcdDeviceData = BuildBcdDevice_FileData(pwboEle);
		}
		break;
		case DEVICE_DATA_TYPE_DeviceLocateData:
		{
			bcdDeviceData = BuildBcdDevice_LocateData(pwboEle);
		}
		break;
		case DEVICE_DATA_TYPE_DeviceLocateElementChildData:
		{
			bcdDeviceData = BuildBcdDevice_LocateElementChildData(pwboEle);
		}
		break;
		case DEVICE_DATA_TYPE_DeviceLocateElementData:
		{
			bcdDeviceData = BuildBcdDevice_LocateElementData(pwboEle);
		}
		break;
		case DEVICE_DATA_TYPE_DeviceLocateStringData:
		{
			bcdDeviceData = BuildBcdDevice_LocateStringData(pwboEle);
		}
		break;
		case DEVICE_DATA_TYPE_DevicePartitionData:
		{
			bcdDeviceData = BuildBcdDevice_PartitionData(pwboEle);
		}
		break;
		case DEVICE_DATA_TYPE_DeviceQualifiedPartitionData:
		{
			bcdDeviceData = BuildBcdDevice_QualiPartitionData(pwboEle);
		}
		break;
		case DEVICE_DATA_TYPE_DeviceUnknownData:
		{
			bcdDeviceData = BuildBcdDevice_UnknownData(pwboEle);
		}
		break;
		default:
		{
			return NULL;
		}	
	}

	return bcdDeviceData;
}

std::wstring BcdObject::getEleDisplay(unsigned int ele)
{
	switch (ele)
	{
	case BcdBootMgrObjectList_DisplayOrder:
	{
		return L"DisplayOrder";
		break;
	}
	case BcdBootMgrObjectList_BootSequence:
	{
		return L"BootSequence";
		break;
	}
	case BcdBootMgrObject_DefaultObject:
	{
		return L"DefaultObject";
		break;
	}
	case BcdBootMgrInteger_Timeout:
	{
		return L"Timeout";
		break;
	}
	case BcdBootMgrBoolean_AttemptResume:
	{
		return L"AttemptResume";
		break;
	}
	case BcdBootMgrObject_ResumeObject:
	{
		return L"ResumeObject";
		break;
	}
	case BcdBootMgrObjectList_ToolsDisplayOrder:
	{
		return L"ToolsDisplayOrder";
		break;
	}
	case BcdBootMgrBoolean_DisplayBootMenu:
	{
		return L"DisplayBootMenu";
		break;
	}
	case BcdBootMgrBoolean_NoErrorDisplay:
	{
		return L"NoErrorDisplay";
		break;
	}
	case BcdBootMgrDevice_BcdDevice:
	{
		return L"BcdDevice";
		break;
	}
	case BcdBootMgrString_BcdFilePath:
	{
		return L"BcdFilePath";
		break;
	}
	case BcdBootMgrBoolean_ProcessCustomActionsFirst:
	{
		return L"ProcessCustomActionsFirst";
		break;
	}
	case BcdBootMgrIntegerList_CustomActionsList:
	{
		return L"CustomActionsList";
		break;
	}
	case BcdBootMgrBoolean_PersistBootSequence:
	{
		return L"PersistBootSequence";
		break;
	}
	case BcdDeviceInteger_RamdiskImageOffset:
	{
		return L"RamdiskImageOffse";
		break;
	}
	case BcdDeviceInteger_TftpClientPort:
	{
		return L"TftpClientPort";
		break;
	}
	case BcdDeviceInteger_SdiDevice:
	{
		return L"SdiDevice";
		break;
	}
	case BcdDeviceInteger_SdiPath:
	{
		return L"SdiPath";
		break;
	}
	case BcdDeviceInteger_RamdiskImageLength:
	{
		return L"RamdiskImageLength";
		break;
	}
	case BcdDeviceBoolean_RamdiskExportAsCd:
	{
		return L"RamdiskExportAsCd";
		break;
	}
	case BcdDeviceInteger_RamdiskTftpBlockSize:
	{
		return L"RamdiskTftpBlockSize";
		break;
	}
	case BcdDeviceInteger_RamdiskTftpWindowSize:
	{
		return L"RamdiskTftpWindowSize";
		break;
	}
	case BcdDeviceBoolean_RamdiskMulticastEnabled:
	{
		return L"RamdiskMulticastEnabled";
		break;
	}
	case BcdDeviceBoolean_RamdiskMulticastTftpFallback:
	{
		return L"RamdiskMulticastTftpFallback";
		break;
	}
	case BcdDeviceBoolean_RamdiskTftpVarWindow:
	{
		return L"RamdiskTftpVarWindow";
		break;
	}
	case BcdLibraryDevice_ApplicationDevice:
	{
		return L"ApplicationDevice";
		break;
	}
	case BcdLibraryString_ApplicationPath:
	{
		return L"ApplicationPath";
		break;
	}
	case BcdLibraryString_Description:
	{
		return L"Description";
		break;
	}
	case BcdLibraryString_PreferredLocale:
	{
		return L"PreferredLocale";
		break;
	}
	case BcdLibraryObjectList_InheritedObjects:
	{
		return L"InheritedObjects";
		break;
	}
	case BcdLibraryInteger_TruncatePhysicalMemory:
	{
		return L"TruncatePhysicalMemory";
		break;
	}
	case BcdLibraryObjectList_RecoverySequence:
	{
		return L"RecoverySequence";
		break;
	}
	case BcdLibraryBoolean_AutoRecoveryEnabled:
	{
		return L"AutoRecoveryEnabled";
		break;
	}
	case BcdLibraryIntegerList_BadMemoryList:
	{
		return L"BadMemoryList";
		break;
	}
	case BcdLibraryBoolean_AllowBadMemoryAccess:
	{
		return L"AllowBadMemoryAccess";
		break;
	}
	case BcdLibraryInteger_FirstMegabytePolicy:
	{
		return L"FirstMegabytePolicy";
		break;
	}
	case BcdLibraryInteger_RelocatePhysicalMemory:
	{
		return L"RelocatePhysicalMemory";
		break;
	}
	case BcdLibraryInteger_AvoidLowPhysicalMemory:
	{
		return L"AvoidLowPhysicalMemory";
		break;
	}
	case BcdLibraryBoolean_DebuggerEnabled:
	{
		return L"DebuggerEnabled";
		break;
	}
	case BcdLibraryInteger_DebuggerType:
	{
		return L"DebuggerType";
		break;
	}
	case BcdLibraryInteger_SerialDebuggerPortAddress:
	{
		return L"SerialDebuggerPortAddress";
		break;
	}
	case BcdLibraryInteger_SerialDebuggerPort:
	{
		return L"SerialDebuggerPort";
		break;
	}
	case BcdLibraryInteger_SerialDebuggerBaudRate:
	{
		return L"SerialDebuggerBaudRate";
		break;
	}
	case BcdLibraryInteger_1394DebuggerChannel:
	{
		return L"1394DebuggerChannel";
		break;
	}
	case BcdLibraryString_UsbDebuggerTargetName:
	{
		return L"UsbDebuggerTargetName";
		break;
	}
	case BcdLibraryBoolean_DebuggerIgnoreUsermodeExceptions:
	{
		return L"DebuggerIgnoreUsermodeExceptions";
		break;
	}
	case BcdLibraryInteger_DebuggerStartPolicy:
	{
		return L"DebuggerStartPolicy";
		break;
	}
	case BcdLibraryString_DebuggerBusParameters:
	{
		return L"DebuggerBusParameters";
		break;
	}
	case BcdLibraryInteger_DebuggerNetHostIP:
	{
		return L"DebuggerNetHostIP";
		break;
	}
	case BcdLibraryInteger_DebuggerNetPort:
	{
		return L"DebuggerNetPort";
		break;
	}
	case BcdLibraryBoolean_DebuggerNetDhcp:
	{
		return L"DebuggerNetDhcp";
		break;
	}
	case BcdLibraryString_DebuggerNetKey:
	{
		return L"DebuggerNetKey";
		break;
	}
	case BcdLibraryBoolean_EmsEnabled:
	{
		return L"EmsEnabled";
		break;
	}
	case BcdLibraryInteger_EmsPort:
	{
		return L"EmsPort";
		break;
	}
	case BcdLibraryInteger_EmsBaudRate:
	{
		return L"EmsBaudRate";
		break;
	}
	case BcdLibraryString_LoadOptionsString:
	{
		return L"LoadOptionsString";
		break;
	}
	case BcdLibraryBoolean_DisplayAdvancedOptions:
	{
		return L"DisplayAdvancedOptions";
		break;
	}
	case BcdLibraryBoolean_DisplayOptionsEdit:
	{
		return L"DisplayOptionsEdit";
		break;
	}
	case BcdLibraryDevice_BsdLogDevice:
	{
		return L"BsdLogDevice";
		break;
	}
	case BcdLibraryString_BsdLogPath:
	{
		return L"BsdLogPath";
		break;
	}
	case BcdLibraryBoolean_GraphicsModeDisabled:
	{
		return L"GraphicsModeDisabled";
		break;
	}
	case BcdLibraryInteger_ConfigAccessPolicy:
	{
		return L"ConfigAccessPolicy";
		break;
	}
	case BcdLibraryBoolean_DisableIntegrityChecks:
	{
		return L"DisableIntegrityChecks";
		break;
	}
	case BcdLibraryBoolean_AllowPrereleaseSignatures:
	{
		return L"AllowPrereleaseSignatures";
		break;
	}
	case BcdLibraryString_FontPath:
	{
		return L"FontPath";
		break;
	}
	case BcdLibraryInteger_SiPolicy:
	{
		return L"SiPolicy";
		break;
	}
	case BcdLibraryInteger_FveBandId:
	{
		return L"FveBandId";
		break;
	}
	case BcdLibraryBoolean_ConsoleExtendedInput:
	{
		return L"ConsoleExtendedInput";
		break;
	}
	case BcdLibraryInteger_GraphicsResolution:
	{
		return L"GraphicsResolution";
		break;
	}
	case BcdLibraryBoolean_RestartOnFailure:
	{
		return L"RestartOnFailure";
		break;
	}
	case BcdLibraryBoolean_GraphicsForceHighestMode:
	{
		return L"GraphicsForceHighestMode";
		break;
	}
	case BcdLibraryBoolean_IsolatedExecutionContext:
	{
		return L"IsolatedExecutionContext";
		break;
	}
	case BcdLibraryBoolean_BootUxDisable:
	{
		return L"BootUxDisable";
		break;
	}
	case BcdLibraryBoolean_BootShutdownDisabled:
	{
		return L"BootShutdownDisabled";
		break;
	}
	case BcdLibraryIntegerList_AllowedInMemorySettings:
	{
		return L"AllowedInMemorySettings";
		break;
	}
	case BcdLibraryBoolean_ForceFipsCrypto:
	{
		return L"ForceFipsCrypto";
		break;
	}
	case BcdMemDiagInteger_PassCount:
	{
		return L"PassCount";
		break;
	}
	case BcdMemDiagInteger_FailureCount:
	{
		return L"FailureCount";
		break;
	}
	case BcdOSLoaderDevice_OSDevice:
	{
		return L"OSDevice";
		break;
	}
	case BcdOSLoaderString_SystemRoot:
	{
		return L"SystemRoot";
		break;
	}
/*	case BcdOSLoaderObject_AssociatedResumeObject:
	{
		return L"AssociatedResumeObject";
		break;
	}
	*/
	case BcdOSLoaderBoolean_DetectKernelAndHal:
	{
		return L"DetectKernelAndHal";
		break;
	}
	case BcdOSLoaderString_KernelPath:
	{
		return L"KernelPath";
		break;
	}
	case BcdOSLoaderString_HalPath:
	{
		return L"HalPath";
		break;
	}
	case BcdOSLoaderString_DbgTransportPath:
	{
		return L"DbgTransportPath";
		break;
	}
	case BcdOSLoaderInteger_NxPolicy:
	{
		return L"NxPolicy";
		break;
	}
	case BcdOSLoaderInteger_PAEPolicy:
	{
		return L"PAEPolicy";
		break;
	}
	case BcdOSLoaderBoolean_WinPEMode:
	{
		return L"WinPEMode";
		break;
	}
	case BcdOSLoaderBoolean_DisableCrashAutoReboot:
	{
		return L"DisableCrashAutoReboot";
		break;
	}
	case BcdOSLoaderBoolean_UseLastGoodSettings:
	{
		return L"UseLastGoodSettings";
		break;
	}
	case BcdOSLoaderBoolean_AllowPrereleaseSignatures:
	{
		return L"AllowPrereleaseSignatures";
		break;
	}
	case BcdOSLoaderBoolean_NoLowMemory:
	{
		return L"NoLowMemory";
		break;
	}
	case BcdOSLoaderInteger_RemoveMemory:
	{
		return L"RemoveMemory";
		break;
	}
	case BcdOSLoaderInteger_IncreaseUserVa:
	{
		return L"IncreaseUserVa";
		break;
	}
	case BcdOSLoaderBoolean_UseVgaDriver:
	{
		return L"UseVgaDriver";
		break;
	}
	case BcdOSLoaderBoolean_DisableBootDisplay:
	{
		return L"DisableBootDisplay";
		break;
	}
	case BcdOSLoaderBoolean_DisableVesaBios:
	{
		return L"DisableVesaBios";
		break;
	}
	case BcdOSLoaderBoolean_DisableVgaMode:
	{
		return L"DisableVgaMode";
		break;
	}
	case BcdOSLoaderInteger_ClusterModeAddressing:
	{
		return L"ClusterModeAddressing";
		break;
	}
	case BcdOSLoaderBoolean_UsePhysicalDestination:
	{
		return L"UsePhysicalDestination";
		break;
	}
	case BcdOSLoaderInteger_RestrictApicCluster:
	{
		return L"RestrictApicCluster";
		break;
	}
	case BcdOSLoaderBoolean_UseLegacyApicMode:
	{
		return L"UseLegacyApicMode";
		break;
	}
	case BcdOSLoaderInteger_X2ApicPolicy:
	{
		return L"X2ApicPolicy";
		break;
	}
	case BcdOSLoaderBoolean_UseBootProcessorOnly:
	{
		return L"UseBootProcessorOnly";
		break;
	}
	case BcdOSLoaderInteger_NumberOfProcessors:
	{
		return L"NumberOfProcessors";
		break;
	}
	case BcdOSLoaderBoolean_ForceMaximumProcessors:
	{
		return L"ForceMaximumProcessors";
		break;
	}
	case BcdOSLoaderBoolean_ProcessorConfigurationFlags:
	{
		return L"ProcessorConfigurationFlags";
		break;
	}
	case BcdOSLoaderBoolean_MaximizeGroupsCreated:
	{
		return L"MaximizeGroupsCreated";
		break;
	}
	case BcdOSLoaderBoolean_ForceGroupAwareness:
	{
		return L"ForceGroupAwareness";
		break;
	}
	case BcdOSLoaderInteger_GroupSize:
	{
		return L"GroupSize";
		break;
	}
	case BcdOSLoaderInteger_UseFirmwarePciSettings:
	{
		return L"UseFirmwarePciSettings";
		break;
	}
	case BcdOSLoaderInteger_MsiPolicy:
	{
		return L"MsiPolicy";
		break;
	}
	case BcdOSLoaderInteger_SafeBoot:
	{
		return L"SafeBoot";
		break;
	}
	case BcdOSLoaderBoolean_SafeBootAlternateShell:
	{
		return L"SafeBootAlternateShell";
		break;
	}
	case BcdOSLoaderBoolean_BootLogInitialization:
	{
		return L"BootLogInitialization";
		break;
	}
	case BcdOSLoaderBoolean_VerboseObjectLoadMode:
	{
		return L"VerboseObjectLoadMode";
		break;
	}
	case BcdOSLoaderBoolean_KernelDebuggerEnabled:
	{
		return L"KernelDebuggerEnabled";
		break;
	}
	case BcdOSLoaderBoolean_DebuggerHalBreakpoint:
	{
		return L"DebuggerHalBreakpoint";
		break;
	}
	case BcdOSLoaderBoolean_UsePlatformClock:
	{
		return L"UsePlatformClock";
		break;
	}
	case BcdOSLoaderBoolean_ForceLegacyPlatform:
	{
		return L"ForceLegacyPlatform";
		break;
	}
	case BcdOSLoaderInteger_TscSyncPolicy:
	{
		return L"TscSyncPolicy";
		break;
	}
	case BcdOSLoaderBoolean_EmsEnabled:
	{
		return L"EmsEnabled";
		break;
	}
	case BcdOSLoaderInteger_DriverLoadFailurePolicy:
	{
		return L"DriverLoadFailurePolicy";
		break;
	}
	case BcdOSLoaderInteger_BootMenuPolicy:
	{
		return L"BootMenuPolicy";
		break;
	}
	case BcdOSLoaderBoolean_AdvancedOptionsOneTime:
	{
		return L"AdvancedOptionsOneTime";
		break;
	}
	case BcdOSLoaderInteger_BootStatusPolicy:
	{
		return L"BootStatusPolicy";
		break;
	}
	case BcdOSLoaderBoolean_DisableElamDrivers:
	{
		return L"DisableElamDrivers";
		break;
	}
	case BcdOSLoaderInteger_HypervisorLaunchType:
	{
		return L"HypervisorLaunchType";
		break;
	}
	case BcdOSLoaderBoolean_HypervisorDebuggerEnabled:
	{
		return L"HypervisorDebuggerEnabled";
		break;
	}
	case BcdOSLoaderInteger_HypervisorDebuggerType:
	{
		return L"HypervisorDebuggerType";
		break;
	}
	case BcdOSLoaderInteger_HypervisorDebuggerPortNumber:
	{
		return L"HypervisorDebuggerPortNumber";
		break;
	}
	case BcdOSLoaderInteger_HypervisorDebuggerBaudrate:
	{
		return L"HypervisorDebuggerBaudrate";
		break;
	}
	case BcdOSLoaderInteger_HypervisorDebugger1394Channel:
	{
		return L"HypervisorDebugger1394Channel";
		break;
	}
	case BcdOSLoaderInteger_BootUxPolicy:
	{
		return L"BootUxPolicy";
		break;
	}
	case BcdOSLoaderString_HypervisorDebuggerBusParams:
	{
		return L"HypervisorDebuggerBusParams";
		break;
	}
	case BcdOSLoaderInteger_HypervisorNumProc:
	{
		return L"HypervisorNumProc";
		break;
	}
	case BcdOSLoaderInteger_HypervisorRootProcPerNode:
	{
		return L"HypervisorRootProcPerNode";
		break;
	}
	case BcdOSLoaderBoolean_HypervisorUseLargeVTlb:
	{
		return L"HypervisorUseLargeVTlb";
		break;
	}
	case BcdOSLoaderInteger_HypervisorDebuggerNetHostIp:
	{
		return L"HypervisorDebuggerNetHostIp";
		break;
	}
	case BcdOSLoaderInteger_HypervisorDebuggerNetHostPort:
	{
		return L"HypervisorDebuggerNetHostPort";
		break;
	}
	case BcdOSLoaderInteger_TpmBootEntropyPolicy:
	{
		return L"TpmBootEntropyPolicy";
		break;
	}
	case BcdOSLoaderString_HypervisorDebuggerNetKey:
	{
		return L"HypervisorDebuggerNetKey";
		break;
	}
	case BcdOSLoaderBoolean_HypervisorDebuggerNetDhcp:
	{
		return L"HypervisorDebuggerNetDhcp";
		break;
	}
	case BcdOSLoaderInteger_HypervisorIommuPolicy:
	{
		return L"HypervisorIommuPolicy";
		break;
	}
	case BcdOSLoaderInteger_XSaveDisable:
	{
		return L"XSaveDisable";
		break;
	}
	/*case Reserved1:
	{
		return L"Reserved1";
		break;
	}
	case Reserved2:
	{
		return L"Reserved2";
		break;
	}
	*/
	case BcdResumeBoolean_UseCustomSettings:
	{
		return L"UseCustomSettings";
		break;
	}
	case BcdResumeDevice_AssociatedOsDevice:
	{
		return L"AssociatedOsDevice";
		break;
	}
	case BcdResumeBoolean_DebugOptionEnabled:
	{
		return L"DebugOptionEnabled";
		break;
	}
	case BcdResumeInteger_BootMenuPolicy:
	{
		return L"BootMenuPolicy";
		break;
	}
	default:
		
		break;
	}
	return L"";
}