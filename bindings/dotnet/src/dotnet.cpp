// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "azure_c_shared_utility/gballoc.h"

#include "module.h"
#include "message.h"
#include "azure_c_shared_utility/iot_logging.h"
#include "azure_c_shared_utility/base64.h"

#ifdef UNDER_TEST
#define SafeArrayCreateVector myTest_SafeArrayCreateVector
#define SafeArrayPutElement   myTest_SafeArrayPutElement
#define SafeArrayDestroy      myTest_SafeArrayDestroy
#endif

#include "dotnet.h"

#include <new>

#include <metahost.h>

#define DefaultCLRVersion L"v4.0.30319"
#define AZUREIOTGATEWAYASSEMBLYNAME L"Microsoft.Azure.IoT.Gateway"
#define AZUREIOTGATEWAY_MESSAGEBUS_CLASSNAME L"Microsoft.Azure.IoT.Gateway.MessageBus"
#define AZUREIOTGATEWAY_MESSAGE_CLASSNAME L"Microsoft.Azure.IoT.Gateway.Message"




// Import mscorlib.tlb (Microsoft Common Language Runtime Class Library).
#import "mscorlib.tlb" raw_interfaces_only				\
    high_property_prefixes("_get","_put","_putref")		\
    rename("ReportEvent", "InteropServices_ReportEvent")
using namespace mscorlib;

typedef struct DOTNET_HOST_HANDLE_DATA_TAG
{
    MESSAGE_BUS_HANDLE          bus;
    ICLRMetaHost                *pMetaHost;
    ICLRRuntimeInfo             *pRuntimeInfo;
    ICorRuntimeHost             *pCorRuntimeHost;
	variant_t                   vtClientModuleObject;
	_TypePtr                    spClientModuleType;
	_TypePtr                    spAzureIoTGatewayMessageClassType;
	_AssemblyPtr                spAzureIoTGatewayAssembly;
}DOTNET_HOST_HANDLE_DATA;

static MODULE_HANDLE DotNET_Create(MESSAGE_BUS_HANDLE busHandle, const void* configuration)
{
	DOTNET_HOST_HANDLE_DATA* result;
    if (
        (busHandle == NULL) ||
		(configuration == NULL)
        )
    {
		/* Codes_SRS_DOTNET_04_001: [ DotNET_Create shall return NULL if bus is NULL. ] */
		/* Codes_SRS_DOTNET_04_002: [ DotNET_Create shall return NULL if configuration is NULL. ] */
        LogError("invalid arg busHandle=%p", busHandle);
        result = NULL;
    }
    else
    {
		DOTNET_HOST_CONFIG* dotNetConfig = (DOTNET_HOST_CONFIG*)configuration;
		if (dotNetConfig->dotnet_module_path == NULL)
		{
			/* Codes_SRS_DOTNET_04_003: [ DotNET_Create shall return NULL if configuration->dotnet_module_path is NULL. ] */
			LogError("invalid configuration. dotnet_module_path=%p", dotNetConfig->dotnet_module_path);
			result = NULL;
		}
		else if (dotNetConfig->dotnet_module_entry_class == NULL)
		{
			/* Codes_SRS_DOTNET_04_004: [ DotNET_Create shall return NULL if configuration->dotnet_module_entry_class is NULL. ] */
			LogError("invalid configuration. dotnet_module_entry_class=%p", dotNetConfig->dotnet_module_entry_class);
			result = NULL;
		}
		else if (dotNetConfig->dotnet_module_args == NULL)
		{
			/* Codes_SRS_DOTNET_04_005: [ DotNET_Create shall return NULL if configuration->dotnet_module_args is NULL. ] */
			LogError("invalid configuration. dotnet_module_args=%p", dotNetConfig->dotnet_module_args);
			result = NULL;
		}		
		/* Codes_SRS_DOTNET_04_008: [ DotNET_Create shall allocate memory for an instance of the DOTNET_HOST_HANDLE_DATA structure and use that as the backing structure for the module handle. ] */
		else
		{
			try
			{
				result = new DOTNET_HOST_HANDLE_DATA();
			}
			catch (const std::bad_alloc& badAllocExpection)
			{
				/* Codes_SRS_DOTNET_04_006: [ DotNET_Create shall return NULL if an underlying API call fails. ] */
				LogError("unable to allocateMemory. Message: %s", badAllocExpection.what());
				result = NULL;
			}

			if (result == NULL)
			{
				/* Codes_SRS_DOTNET_04_006: [ DotNET_Create shall return NULL if an underlying API call fails. ] */
				LogError("unable to allocate memory.");
			}
			else
			{
				HRESULT hr;
				BOOL fLoadable;
				IUnknownPtr spAppDomainThunk = NULL;
				_AppDomainPtr spDefaultAppDomain = NULL;
				bstr_t bstrClientModuleAssemblyName(dotNetConfig->dotnet_module_path);
				_AssemblyPtr spClientModuleAssembly = NULL;
				bstr_t bstrClientModuleClassName(dotNetConfig->dotnet_module_entry_class);
				bstr_t bstrAzureIoTGatewayAssemblyName(AZUREIOTGATEWAYASSEMBLYNAME);
				bstr_t bstrAzureIoTGatewayMessageBusClassName(AZUREIOTGATEWAY_MESSAGEBUS_CLASSNAME);
				_TypePtr spAzureIoTGatewayMessageBusClassType = NULL;
				bstr_t bstrAzureIoTGatewayMessageClassName(AZUREIOTGATEWAY_MESSAGE_CLASSNAME);


				/* Codes_SRS_DOTNET_04_007: [ DotNET_Create shall return a non-NULL MODULE_HANDLE when successful. ] */
				result->bus = busHandle;

				/* Codes_SRS_DOTNET_04_012: [ DotNET_Create shall get the 3 CLR Host Interfaces (CLRMetaHost, CLRRuntimeInfo and CorRuntimeHost) and save it on DOTNET_HOST_HANDLE_DATA. ] */
				hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&result->pMetaHost));
				if (FAILED(hr))
				{
					/* Codes_SRS_DOTNET_04_006: [ DotNET_Create shall return NULL if an underlying API call fails. ] */
					LogError("CLRCreateInstance failed w/hr 0x%08lx\n", hr);
					delete(result);
					result = NULL;
				}
				/* Codes_SRS_DOTNET_04_012: [ DotNET_Create shall get the 3 CLR Host Interfaces (CLRMetaHost, CLRRuntimeInfo and CorRuntimeHost) and save it on DOTNET_HOST_HANDLE_DATA. ] */
				else if (FAILED(hr = (result->pMetaHost)->GetRuntime(DefaultCLRVersion, IID_PPV_ARGS(&result->pRuntimeInfo))))
				{
					/* Codes_SRS_DOTNET_04_006: [ DotNET_Create shall return NULL if an underlying API call fails. ] */
					LogError("ICLRMetaHost::GetRuntime failed w/hr 0x%08lx", hr);
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if(FAILED(hr = (result->pRuntimeInfo)->IsLoadable(&fLoadable)))
				{
					LogError("ICLRRuntimeInfo::IsLoadable failed w/hr 0x%08lx\n", hr);
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if(!fLoadable)
				{
					LogError(".NET runtime %ls cannot be loaded\n", DefaultCLRVersion);
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if(FAILED(hr = (result->pRuntimeInfo)->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(&(result->pCorRuntimeHost)))))
				{
					LogError("ICLRRuntimeInfo::GetInterface failed w/hr 0x%08lx\n", hr);
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if(FAILED(hr = (result->pCorRuntimeHost)->Start()))
				{
					LogError("CLR failed to start w/hr 0x%08lx\n", hr);
					(result->pCorRuntimeHost)->Release();
					result->pCorRuntimeHost = NULL;
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if (FAILED(hr = (result->pCorRuntimeHost)->GetDefaultDomain(&spAppDomainThunk)))
				{
					LogError("ICorRuntimeHost::GetDefaultDomain failed w/hr 0x%08lx\n", hr);
					(result->pCorRuntimeHost)->Release();
					result->pCorRuntimeHost = NULL;
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if (FAILED(hr = spAppDomainThunk->QueryInterface(IID_PPV_ARGS(&spDefaultAppDomain))))
				{
					LogError("Failed to get default AppDomain w/hr 0x%08lx\n", hr);
					(result->pCorRuntimeHost)->Release();
					result->pCorRuntimeHost = NULL;
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if (FAILED(hr = spDefaultAppDomain->Load_2(bstrClientModuleAssemblyName, &spClientModuleAssembly)))
				{
					LogError("Failed to load the assembly w/hr 0x%08lx\n", hr);
					(result->pCorRuntimeHost)->Release();
					result->pCorRuntimeHost = NULL;
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL; 
				}
				else if (FAILED(hr = spClientModuleAssembly->GetType_2(bstrClientModuleClassName, &result->spClientModuleType)))
				{
					LogError("Failed to get the Type interface w/hr 0x%08lx\n", hr);
					(result->pCorRuntimeHost)->Release();
					result->pCorRuntimeHost = NULL;
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if (FAILED(hr = spDefaultAppDomain->Load_2(bstrAzureIoTGatewayAssemblyName, &(result->spAzureIoTGatewayAssembly))))
				{
					LogError("Failed to load the assembly w/hr 0x%08lx\n", hr);
					(result->pCorRuntimeHost)->Release();
					result->pCorRuntimeHost = NULL;
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if (FAILED(hr = result->spAzureIoTGatewayAssembly->GetType_2(bstrAzureIoTGatewayMessageBusClassName, &spAzureIoTGatewayMessageBusClassType)))
				{
					LogError("Failed to get the Type interface w/hr 0x%08lx\n", hr);
					(result->pCorRuntimeHost)->Release();
					result->pCorRuntimeHost = NULL;
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else if (FAILED(hr = result->spAzureIoTGatewayAssembly->GetType_2(bstrAzureIoTGatewayMessageClassName, &(result->spAzureIoTGatewayMessageClassType))))
				{
					LogError("Failed to get the Type interface w/hr 0x%08lx\n", hr);
					(result->pCorRuntimeHost)->Release();
					result->pCorRuntimeHost = NULL;
					(result->pRuntimeInfo)->Release();
					result->pRuntimeInfo = NULL;
					(result->pMetaHost)->Release();
					result->pMetaHost = NULL;
					delete(result);
					result = NULL;
				}
				else
				{
					SAFEARRAY *psaAzureIoTGatewayMessageBusConstructorArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);
					LONG index = 0;
					variant_t msgBus((long long)result->bus);
					variant_t vtAzureIoTGatewayMessageBusObject;

					if (psaAzureIoTGatewayMessageBusConstructorArgs == NULL)
					{
						LogError("Failed to create Safe Array. ");
						(result->pCorRuntimeHost)->Release();
						result->pCorRuntimeHost = NULL;
						(result->pRuntimeInfo)->Release();
						result->pRuntimeInfo = NULL;
						(result->pMetaHost)->Release();
						result->pMetaHost = NULL;
						delete(result);
						result = NULL;
					}
					else if (FAILED(hr = SafeArrayPutElement(psaAzureIoTGatewayMessageBusConstructorArgs, &index, &msgBus)))
					{
						LogError("Adding Element on the safe array failed. w/hr 0x%08lx\n", hr);
						(result->pCorRuntimeHost)->Release();
						result->pCorRuntimeHost = NULL;
						(result->pRuntimeInfo)->Release();
						result->pRuntimeInfo = NULL;
						(result->pMetaHost)->Release();
						result->pMetaHost = NULL;
						delete(result);
						result = NULL;
					}
					else if (FAILED(hr = result->spAzureIoTGatewayAssembly->CreateInstance_3(bstrAzureIoTGatewayMessageBusClassName, true, static_cast<BindingFlags>(BindingFlags_Instance | BindingFlags_Public), NULL, psaAzureIoTGatewayMessageBusConstructorArgs, NULL, NULL, &vtAzureIoTGatewayMessageBusObject)))
					{
						LogError("Creating an instance of Message Bus failed with hr 0x%08lx\n", hr);
						(result->pCorRuntimeHost)->Release();
						result->pCorRuntimeHost = NULL;
						(result->pRuntimeInfo)->Release();
						result->pRuntimeInfo = NULL;
						(result->pMetaHost)->Release();
						result->pMetaHost = NULL;
						delete(result);
						result = NULL;
					}
					//Create an instance of the Client Module (Default Blank Constructor will be called. 
					else if (FAILED(hr = spClientModuleAssembly->CreateInstance(bstrClientModuleClassName, &result->vtClientModuleObject)))
					{
						LogError("Creating an instance of Client Class failed with hr 0x%08lx\n", hr);
						(result->pCorRuntimeHost)->Release();
						result->pCorRuntimeHost = NULL;
						(result->pRuntimeInfo)->Release();
						result->pRuntimeInfo = NULL;
						(result->pMetaHost)->Release();
						result->pMetaHost = NULL;
						delete(result);
						result = NULL;
					}
					else
					{
						SAFEARRAY *psaClientModuleCreateArgs = SafeArrayCreateVector(VT_VARIANT, 0, 2);
						index = 0;
						if (psaClientModuleCreateArgs == NULL)
						{
							LogError("Failed to create Safe Array. ");
							(result->pCorRuntimeHost)->Release();
							result->pCorRuntimeHost = NULL;
							(result->pRuntimeInfo)->Release();
							result->pRuntimeInfo = NULL;
							(result->pMetaHost)->Release();
							result->pMetaHost = NULL;
							delete(result);
							result = NULL;
						}
						else if (FAILED(hr = SafeArrayPutElement(psaClientModuleCreateArgs, &index, &vtAzureIoTGatewayMessageBusObject)))
						{
							LogError("Adding Element on the safe array failed. w/hr 0x%08lx\n", hr);
							(result->pCorRuntimeHost)->Release();
							result->pCorRuntimeHost = NULL;
							(result->pRuntimeInfo)->Release();
							result->pRuntimeInfo = NULL;
							(result->pMetaHost)->Release();
							result->pMetaHost = NULL;
							delete(result);
							result = NULL;
						}
						else
						{
							variant_t vtdotNetArgsArg(dotNetConfig->dotnet_module_args);
							bstr_t bstrCreateClientMethodName(L"Create");
							variant_t vt_Empty;
							index = 1;

							if (FAILED(hr = SafeArrayPutElement(psaClientModuleCreateArgs, &index, &vtdotNetArgsArg)))
							{
								LogError("Adding Element on the safe array failed. w/hr 0x%08lx\n", hr);
								(result->pCorRuntimeHost)->Release();
								result->pCorRuntimeHost = NULL;
								(result->pRuntimeInfo)->Release();
								result->pRuntimeInfo = NULL;
								(result->pMetaHost)->Release();
								result->pMetaHost = NULL;
								delete(result);
								result = NULL;
							}
							else if (FAILED(hr = result->spClientModuleType->InvokeMember_3(bstrCreateClientMethodName, static_cast<BindingFlags>(BindingFlags_Instance | BindingFlags_Public | BindingFlags_InvokeMethod), NULL, result->vtClientModuleObject, psaClientModuleCreateArgs, &vt_Empty)))
							{
								LogError("Failed to invoke Create Method with hr 0x%08lx\n", hr);
								(result->pCorRuntimeHost)->Release();
								result->pCorRuntimeHost = NULL;
								(result->pRuntimeInfo)->Release();
								result->pRuntimeInfo = NULL;
								(result->pMetaHost)->Release();
								result->pMetaHost = NULL;
								delete(result);
								result = NULL;
							}
							SafeArrayDestroy(psaClientModuleCreateArgs);
							psaClientModuleCreateArgs = NULL;
						}
						SafeArrayDestroy(psaAzureIoTGatewayMessageBusConstructorArgs);
						psaAzureIoTGatewayMessageBusConstructorArgs = NULL;
					}
				}
			}
		}
	}
    return result;
}

static void DotNET_Receive(MODULE_HANDLE moduleHandle, MESSAGE_HANDLE messageHandle)
{
	if (
		(moduleHandle == NULL) ||
		(messageHandle == NULL)
		)
	{
		LogError("invalid arg moduleHandle=%p, messageHandle=%p", moduleHandle, messageHandle);
		/*do nothing*/
	}
	else
	{
		//DOTNET_HOST_HANDLE_DATA* result = (DOTNET_HOST_HANDLE_DATA*)moduleHandle;
		//int32_t msg_size;
		//const unsigned char*msgByteArray = Message_ToByteArray(messageHandle, &msg_size);
		//
		//variant_t msgContentInByteArray;

		//V_VT(&msgContentInByteArray) = VT_ARRAY | VT_UI1;

		//SAFEARRAYBOUND rgsabound[1];
		//rgsabound[0].cElements = msg_size;
		//rgsabound[0].lLbound = 0;

		//V_ARRAY(&msgContentInByteArray) = SafeArrayCreate(VT_UI1, 1, rgsabound);

		//void * pArrayData = NULL;
		//SafeArrayAccessData(msgContentInByteArray.parray, &pArrayData);
		//memcpy(pArrayData, msgByteArray, msg_size);
		//SafeArrayUnaccessData(msgContentInByteArray.parray);
	
		//SAFEARRAY *psaAzureIoTGatewayMessageConstructorArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		//LONG index = 0;
		//HRESULT hr = SafeArrayPutElement(psaAzureIoTGatewayMessageConstructorArgs, &index, &msgContentInByteArray);
		//
		//bstr_t bstrAzureIoTGatewayMessageClassName(AZUREIOTGATEWAY_MESSAGE_CLASSNAME);

		//variant_t vtAzureIoTGatewayMessageObject;

	 //   hr = result->spAzureIoTGatewayAssembly->CreateInstance_3
		//(
		//	bstrAzureIoTGatewayMessageClassName,
		//	true,
		//	static_cast<BindingFlags>(BindingFlags_Instance | BindingFlags_Public),
		//	NULL,
		//	psaAzureIoTGatewayMessageConstructorArgs,
		//	NULL, NULL,
		//	&vtAzureIoTGatewayMessageObject
		//);
		//		
		//bstr_t bstrCreateClientMethodName(L"Receive");
		//SAFEARRAY *psaAzureIoTGatewayClientReceiveArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		//variant_t vt_Empty;

		//index = 0;
		//hr = SafeArrayPutElement(psaAzureIoTGatewayClientReceiveArgs, &index, &vtAzureIoTGatewayMessageObject);

		//hr = result->spClientModuleType->InvokeMember_3
		//(
		//	bstrCreateClientMethodName, 
		//	static_cast<BindingFlags>(BindingFlags_Instance | BindingFlags_Public | BindingFlags_InvokeMethod), 
		//	NULL, 
		//	result->vtClientModuleObject, 
		//	psaAzureIoTGatewayClientReceiveArgs,
		//	&vt_Empty
		//);
	}
}


static void DotNET_Destroy(MODULE_HANDLE module)
{
    /*first stop the thread*/
	DOTNET_HOST_HANDLE_DATA* handleData = (DOTNET_HOST_HANDLE_DATA*)module;

	//handleData->pCorRuntimeHost->Release();
	//handleData->pCorRuntimeHost = NULL;
	//(handleData->pRuntimeInfo)->Release();
	//handleData->pRuntimeInfo = NULL;
	//(handleData->pMetaHost)->Release();
	//handleData->pMetaHost = NULL;

	delete(handleData);
}

//bool dotnetHost_PublishMessage(MESSAGE_BUS_HANDLE bus, MODULE_HANDLE sourceModule, const unsigned char* source, int32_t size)
//{
//	//Create a MEssage from Byte Array. 
//	//Publish this message on the bus. 
//	MESSAGE_HANDLE msgHandle = Message_CreateFromByteArray(source, size);
//
//	MESSAGE_BUS_RESULT msgBusResult = MessageBus_Publish(bus, sourceModule, msgHandle);
//
//	if (msgBusResult != MESSAGE_BUS_OK)
//	{
//		return false;
//	}
//	else
//	{
//		return true;
//	}
//}


static const MODULE_APIS DOTNET_APIS_all =
{
	DotNET_Create,
	DotNET_Destroy,
	DotNET_Receive
};



#ifdef BUILD_MODULE_TYPE_STATIC
MODULE_EXPORT const MODULE_APIS* MODULE_STATIC_GETAPIS(DOTNET_HOST)(void)
#else
MODULE_EXPORT const MODULE_APIS* Module_GetAPIS(void)
#endif
{
	/* Codes_SRS_DOTNET_04_021: [ Module_GetAPIS shall return a non-NULL pointer to a structure of type MODULE_APIS that has all fields initialized to non-NULL values. ] */
	return &DOTNET_APIS_all;
}
