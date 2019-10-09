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
		return false;
	}
	IWbemClassObject* pWboInParam = NULL;
	hres = m_wbemBcdClass->GetMethod(L"GetElementWithFlags", 0, &pWboInParam, NULL);
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
		break;
	case EleValueType_Boolean:
		return Build_BcdBoolean_ElementStruct(pwboEle);
		break;
	case EleValueType_Integer:
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
/*
BcdDeviceElement BcdObject::Build_BcdDevice_ElementStruct(IWbemClassObject* pwboEle)
{
	VARIANT varTmp;
	memset(&varTmp, 0, sizeof(VARIANT));
	BcdStringElement stringEle;
	HRESULT hres = pwboEle->Get(L"String", 0, &varTmp, NULL, NULL);

	stringEle.String = varTmp.bstrVal;

	BcdElement ele = BuildBcdElementStruct(pwboEle);
	stringEle.ObjectId = ele.ObjectId;
	stringEle.StoreFilePath = ele.StoreFilePath;
	stringEle.Type = ele.Type;

	memset(&varTmp, 0, sizeof(VARIANT));
	hres = pwboEle->Get(L"Device", 0, &varTmp, NULL, NULL);
	
	IWbemClassObject* wcoDevice = (IWbemClassObject*)varTmp.byref;
	BcdDeviceData deviceData;
	memset(&varTmp, 0, sizeof(VARIANT));
	hres = wcoDevice->Get(L"DeviceType", 0, &varTmp, NULL, NULL);
	deviceData.DeviceType = (BCD_DEVICE_TYPE)varTmp.uintVal;

	memset(&varTmp, 0, sizeof(VARIANT));
	hres = wcoDevice->Get(L"AdditionalOptions", 0, &varTmp, NULL, NULL);
	deviceData.AdditionalOptions = (wchar_t)varTmp.bstrVal;
	
	BCD_DEVICE_DATA_TYPE deviceDataType = GetDeviceElementDataType(wcoDevice);
	if (deviceDataType == DEVICE_DATA_TYPE_DeviceData)
	{

	}
	//return stringEle;
}
*/

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
	HRESULT hres = pwboEle->Get(L"Integer", 0, &varTmp, NULL, NULL);

	integerEle->Integer = varTmp.ullVal;

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