
#include "BcdStore.h"
#include "BcdCmn.h"

std::vector<BcdObject*> BcdStore::getBcdObject()
{
	return m_vecBcdObject;
}
void BcdStore::Release()
{
	if (m_wboBcdStoreObject != NULL)
	{
		m_wboBcdStoreObject->Release();
	}
	if (m_wboBcdStoreClass != NULL)
	{
		m_wboBcdStoreClass->Release();
	}


}
BcdStore::BcdStore(IWbemClassObject * pwboBcdStoreObject, IWbemClassObject *pwboBcdStoreClass)
{
	m_wboBcdStoreObject = pwboBcdStoreObject;
	m_wboBcdStoreClass = pwboBcdStoreClass;
}

bool BcdStore::CopyObject(std::wstring SourceStoreFile, std::wstring SourceId, uint32 Flags, BcdObject* &Object)
{
	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = m_wboBcdStoreClass->GetMethod(L"CopyObject", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	VARIANT varSourceStoreFile;
	varSourceStoreFile.vt = VT_BSTR;
	varSourceStoreFile.bstrVal = (BSTR)SourceStoreFile.c_str();
	pwboInput->Put(L"SourceStoreFile", 0, &varSourceStoreFile, NULL);

	VARIANT varSourceId;
	varSourceId.vt = VT_BSTR;
	varSourceId.bstrVal = (BSTR)SourceId.c_str();
	pwboInput->Put(L"SourceId", 0, &varSourceId, NULL);

	VARIANT varFlags;
	varFlags.vt = VT_I4;
	varFlags.uintVal = Flags;
	pwboInput->Put(L"Flags", 0, &varFlags, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"CopyObject", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool bRetVaule = false;
	do
	{
		bRetVaule = BcdCmn::getOutputReturnValue(pwboOutput);
		if (!bRetVaule)
		{
			break;
		}
		VARIANT varBcdObject;
		hres = pwboOutput->Get(L"Object", 0, &varBcdObject, NULL, NULL);
		if (FAILED(hres))
		{
			break;
		}
		Object = CreateBcdObjectObj((IWbemClassObject*)varBcdObject.byref);
	} while (0);	
	pwboOutput->Release();

	if (!FAILED(hres) && bRetVaule)
	{
		return true;
	}
	return false;
}

bool BcdStore::CopyObjects(std::string SourceStoreFile, uint32 Type, uint32 Flags)
{
	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = m_wboBcdStoreClass->GetMethod(L"CopyObjects", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	VARIANT varSourceStoreFile;
	varSourceStoreFile.vt = VT_BSTR;
	varSourceStoreFile.bstrVal = (BSTR)SourceStoreFile.c_str();
	pwboInput->Put(L"SourceStoreFile", 0, &varSourceStoreFile, NULL);

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pwboInput->Put(L"Type", 0, &varType, NULL);

	VARIANT varFlags;
	varFlags.vt = VT_I4;
	varFlags.uintVal = Flags;
	pwboInput->Put(L"Flags", 0, &varFlags, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"CopyObjects", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		return false;
	}

	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return bRetValue;
}

bool BcdStore::CreateObject(std::wstring Id, uint32 Type, BcdObject* &Object)
{

	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = m_wboBcdStoreClass->GetMethod(L"CreateObject", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	VARIANT varId;
	varId.vt = VT_BSTR;
	varId.bstrVal = (BSTR)Id.c_str();
	pwboInput->Put(L"Id", 0, &varId, NULL);

	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pwboInput->Put(L"Type", 0, &varType, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"CreateObject", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool bRetVaule = false;
	do
	{
		bRetVaule = BcdCmn::getOutputReturnValue(pwboOutput);
		if (!bRetVaule)
		{
			break;
		}
		VARIANT varBcdObject;
		hres = pwboOutput->Get(L"Object", 0, &varBcdObject, NULL, NULL);
		if (FAILED(hres))
		{
			break;
		}
		Object = CreateBcdObjectObj((IWbemClassObject*)varBcdObject.byref);
	} while (0);
	pwboOutput->Release();

	if (!FAILED(hres) && bRetVaule)
	{
		return true;
	}
	return false;
}

bool BcdStore::CreateStore(std::wstring File, BcdStore* &Store)
{
	IWbemClassObject* pBcdStoreClass = NULL;
	HRESULT  hres = (WBEMSTATUS)getSvc()->GetObject(BSTR(L"BcdStore"), 0, NULL, &pBcdStoreClass, NULL);
	if (FAILED(hres))
	{
		return false;
	}

	VARIANT varPath;
	hres = BcdCmn::getBcdObjectPath(pBcdStoreClass, varPath);
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = pBcdStoreClass->GetMethod(L"CreateStore", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}
	VARIANT varFile;
	varFile.vt = VT_BSTR;
	varFile.bstrVal = (BSTR)File.c_str();
	pwboInput->Put(L"File", 0, &varFile, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"CreateStore", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}

	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	do
	{
		if (!bRetValue)
		{
			break;
		}
		VARIANT varBcdStore;
		hres = pwboOutput->Get(L"Store", 0, &varBcdStore, NULL, NULL);
		if (FAILED(hres))
		{
			break;
		}
		Store = CreateBcdStoreObj((IWbemClassObject*)varBcdStore.byref, pBcdStoreClass);

	} while (0);	
	pwboOutput->Release();
	if (!FAILED(hres) && bRetValue)
	{
		return true;
	}
	pBcdStoreClass->Release();
	return false;

}

bool BcdStore::DeleteObject(std::wstring Id)
{
	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = m_wboBcdStoreClass->GetMethod(L"DeleteObject", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	VARIANT varId;
	varId.vt = VT_BSTR;
	varId.bstrVal = (BSTR)Id.c_str();
	pwboInput->Put(L"Id", 0, &varId, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"DeleteObject", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		return false;
	}

	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return bRetValue;
}

bool BcdStore::DeleteSystemStore()
{
	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"DeleteSystemStore", 0, NULL, NULL, &pwboOutput, NULL);	
	if (FAILED(hres))
	{
		return false;
	}
	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return bRetValue;
}

bool BcdStore::EnumerateObjects(uint32 Type, std::vector<BcdObject*> &vecBcdObject)
{
	m_vecBcdObject.clear();
	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = m_wboBcdStoreClass->GetMethod(L"EnumerateObjects", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	VARIANT varType;
	varType.vt = VT_I4;
	varType.uintVal = Type;
	pwboInput->Put(L"Type", 0, &varType, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"EnumerateObjects", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	do
	{
		if (!bRetValue)
		{
			break;
		}

		VARIANT varObjects;
		hres = pwboOutput->Get(L"Objects", 0, &varObjects, NULL, NULL);
		if (FAILED(hres))
		{
			break;
		}
		if (!(varObjects.vt & VT_ARRAY)) //返回值应该是个数组
		{
			break;
		}

		for (ULONG i = 0; i < varObjects.parray->rgsabound->cElements; i++)
		{
			/*
			IWbemClassObject** pWcoArray = (IWbemClassObject**)(varElements.parray->pvData);
			IWbemClassObject* pWcoEle = pWcoArray[i];
			*/
			IWbemClassObject* pWcoBcdObjec = NULL;
			LONG iTmp = (LONG)i;
			SafeArrayGetElement(varObjects.parray, &iTmp, &pWcoBcdObjec);
			BcdObject* bo = CreateBcdObjectObj(pWcoBcdObjec);
			m_vecBcdObject.push_back(bo);
		}

	} while (0);
	pwboOutput->Release();
	if (!FAILED(hres) && bRetValue)
	{
		vecBcdObject = m_vecBcdObject;
		return true;
	}
	return false;
}

bool BcdStore::ExportStore(std::wstring File)
{
	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = m_wboBcdStoreClass->GetMethod(L"ExportStore", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	VARIANT varFile;
	varFile.vt = VT_BSTR;
	varFile.bstrVal = (BSTR)File.c_str();
	pwboInput->Put(L"File", 0, &varFile, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"ExportStore", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return bRetValue;
}

bool BcdStore::GetSystemDisk(std::wstring &wstrDisk)
{
	IWbemClassObject* pBcdStoreClass = NULL;
	HRESULT  hres = (WBEMSTATUS)getSvc()->GetObject(BSTR(L"BcdStore"), 0, NULL, &pBcdStoreClass, NULL);
	if (FAILED(hres))
	{
		return false;
	}

	VARIANT varPath;
	hres = BcdCmn::getBcdObjectPath(pBcdStoreClass, varPath);
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"GetSystemDisk", 0, NULL, NULL, &pwboOutput, NULL);
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}
	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	if (bRetValue)
	{
		VARIANT varDisk;
		pwboOutput->Get(L"Disk", 0, &varDisk, NULL, NULL);
		wstrDisk = (wchar_t*)varDisk.bstrVal;
	}
	pwboOutput->Release();
	pBcdStoreClass->Release();
	return bRetValue;
}

bool BcdStore::GetSystemPartition(std::wstring &wstrPartition)
{
	IWbemClassObject* pBcdStoreClass = NULL;
	HRESULT  hres = (WBEMSTATUS)getSvc()->GetObject(BSTR(L"BcdStore"), 0, NULL, &pBcdStoreClass, NULL);
	if (FAILED(hres))
	{
		return false;
	}

	VARIANT varPath;
	hres = BcdCmn::getBcdObjectPath(pBcdStoreClass, varPath);
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"GetSystemPartition", 0, NULL, NULL, &pwboOutput, NULL);
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}
	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	if (bRetValue)
	{
		VARIANT varPartition;
		pwboOutput->Get(L"Partition", 0, &varPartition, NULL, NULL);
		wstrPartition = (wchar_t*)varPartition.bstrVal;
	}
	pwboOutput->Release();
	pBcdStoreClass->Release();
	return bRetValue;
}

bool BcdStore::ImportStore(std::wstring File)
{
	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = m_wboBcdStoreClass->GetMethod(L"ImportStore", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	VARIANT varFile;
	varFile.vt = VT_BSTR;
	varFile.bstrVal = (BSTR)File.c_str();
	pwboInput->Put(L"File", 0, &varFile, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"ImportStore", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return bRetValue;
}

bool BcdStore::ImportStoreWithFlags(std::wstring File, uint32 Flags)
{
	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = m_wboBcdStoreClass->GetMethod(L"ImportStoreWithFlags", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	VARIANT varFile;
	varFile.vt = VT_BSTR;
	varFile.bstrVal = (BSTR)File.c_str();
	pwboInput->Put(L"File", 0, &varFile, NULL);

	VARIANT varFlags;
	varFlags.vt = VT_I4;
	varFlags.uintVal = Flags;
	pwboInput->Put(L"Flags", 0, &varFlags, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"ImportStoreWithFlags", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool bRetValue = BcdCmn::getOutputReturnValue(pwboOutput);
	pwboOutput->Release();
	return bRetValue;
}

bool BcdStore::OpenObject(std::wstring Id, BcdObject* &Object)
{
	VARIANT varPath;
	HRESULT  hres = BcdCmn::getBcdObjectPath(m_wboBcdStoreObject, varPath);
	if (FAILED(hres))
	{
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = m_wboBcdStoreClass->GetMethod(L"OpenObject", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		return false;
	}
	VARIANT varId;
	varId.vt = VT_BSTR;
	varId.bstrVal = (BSTR)Id.c_str();
	pwboInput->Put(L"Id", 0, &varId, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"OpenObject", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		return false;
	}
	bool bRetVaule = false;
	do
	{
		bRetVaule = BcdCmn::getOutputReturnValue(pwboOutput);
		if (!bRetVaule)
		{
			break;
		}
		VARIANT varBcdObject;
		hres = pwboOutput->Get(L"Object", 0, &varBcdObject, NULL, NULL);
		if (FAILED(hres))
		{
			break;
		}
		Object = CreateBcdObjectObj((IWbemClassObject*)varBcdObject.byref);
	} while (0);
	pwboOutput->Release();

	if (!FAILED(hres) && bRetVaule)
	{
		return true;
	}
	return false;
}

bool BcdStore::OpenStore(std::wstring File, BcdStore* &Store)
{
	IWbemClassObject* pBcdStoreClass = NULL;
	HRESULT  hres = (WBEMSTATUS)getSvc()->GetObject(BSTR(L"BcdStore"), 0, NULL, &pBcdStoreClass, NULL);
	if (FAILED(hres))
	{
		return false;
	}

	VARIANT varPath;
	hres = BcdCmn::getBcdObjectPath(pBcdStoreClass, varPath);
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}

	IWbemClassObject* pwboInput = NULL;
	hres = pBcdStoreClass->GetMethod(L"OpenStore", 0, &pwboInput, NULL);
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}
	VARIANT varFile;
	varFile.vt = VT_BSTR;
	varFile.bstrVal = (BSTR)File.c_str();
	pwboInput->Put(L"File", 0, &varFile, NULL);

	IWbemServices* psvc = getSvc();
	IWbemClassObject* pwboOutput = NULL;
	hres = psvc->ExecMethod(varPath.bstrVal, (BSTR)L"OpenStore", 0, NULL, pwboInput, &pwboOutput, NULL);
	pwboInput->Release();
	if (FAILED(hres))
	{
		pBcdStoreClass->Release();
		return false;
	}
	bool bRetVaule = false;
	do
	{
		bRetVaule = BcdCmn::getOutputReturnValue(pwboOutput);
		if (!bRetVaule)
		{
			break;
		}
		VARIANT varBcdObject;
		hres = pwboOutput->Get(L"Store", 0, &varBcdObject, NULL, NULL);
		if (FAILED(hres))
		{
			break;
		}
		Store = CreateBcdStoreObj((IWbemClassObject*)varBcdObject.byref, pBcdStoreClass);
	} while (0);
	pwboOutput->Release();
	//pBcdStoreClass->Release();
	if (!FAILED(hres) && bRetVaule)
	{
		return true;
	}
	pBcdStoreClass->Release();
	return false;
}

std::wstring BcdStore::getFilePath()
{
	VARIANT varFilePath;
	m_wboBcdStoreObject->Get(L"FilePath", 0, &varFilePath, NULL, NULL);

	return (wchar_t*)varFilePath.bstrVal;
}

BcdStore * CreateBcdStoreObj(IWbemClassObject * pwboBcdStoreObject, IWbemClassObject *pwboBcdStoreClass)
{
	return new BcdStore(pwboBcdStoreObject, pwboBcdStoreClass);
}
