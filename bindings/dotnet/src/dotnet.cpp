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

#include "dotnet.h"

#include <metahost.h>
#pragma comment(lib, "mscoree.lib")

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
		else
		{
			/* Codes_SRS_DOTNET_04_008: [ DotNET_Create shall allocate memory for an instance of the DOTNET_HOST_HANDLE_DATA structure and use that as the backing structure for the module handle. ] */
			result = new DOTNET_HOST_HANDLE_DATA();
			if (result == NULL)
			{
				/* Codes_SRS_DOTNET_04_006: [ DotNET_Create shall return NULL if an underlying API call fails. ] */
				LogError("unable to malloc");
			}
			else
			{
				HRESULT hr;
				/* Codes_SRS_DOTNET_04_007: [ DotNET_Create shall return a non-NULL MODULE_HANDLE when successful. ] */
				result->bus = busHandle;
			
				hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&result->pMetaHost));
				if (FAILED(hr))
				{
					wprintf(L"CLRCreateInstance failed w/hr 0x%08lx\n", hr);
					delete(result);
					result = NULL;					
				}
				else
				{
					hr = (result->pMetaHost)->GetRuntime(DefaultCLRVersion, IID_PPV_ARGS(&result->pRuntimeInfo));
					if (FAILED(hr))
					{
						wprintf(L"ICLRMetaHost::GetRuntime failed w/hr 0x%08lx\n", hr);
						(result->pMetaHost)->Release();
						result->pMetaHost = NULL;
						delete(result);
						result = NULL;
					}
					else
					{
						BOOL fLoadable;
						hr = (result->pRuntimeInfo)->IsLoadable(&fLoadable);
						if (FAILED(hr))
						{
							wprintf(L"ICLRRuntimeInfo::IsLoadable failed w/hr 0x%08lx\n", hr);
							(result->pRuntimeInfo)->Release();
							result->pRuntimeInfo = NULL;
							(result->pMetaHost)->Release();
							result->pMetaHost = NULL;
							delete(result);
							result = NULL;
						}
						else
						{
							if (!fLoadable)
							{
								wprintf(L".NET runtime %s cannot be loaded\n", DefaultCLRVersion);
								(result->pRuntimeInfo)->Release();
								result->pRuntimeInfo = NULL;
								(result->pMetaHost)->Release();
								result->pMetaHost = NULL;
								delete(result);
								result = NULL;
							}
							else
							{
								hr = (result->pRuntimeInfo)->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(&(result->pCorRuntimeHost)));
								if (FAILED(hr))
								{
									wprintf(L"ICLRRuntimeInfo::GetInterface failed w/hr 0x%08lx\n", hr);
									(result->pRuntimeInfo)->Release();
									result->pRuntimeInfo = NULL;
									(result->pMetaHost)->Release();
									result->pMetaHost = NULL;
									delete(result);
									result = NULL;
								}
								else
								{
									hr = (result->pCorRuntimeHost)->Start();
									if (FAILED(hr))
									{
										wprintf(L"CLR failed to start w/hr 0x%08lx\n", hr);
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
										IUnknownPtr spAppDomainThunk = NULL;
										hr = (result->pCorRuntimeHost)->GetDefaultDomain(&spAppDomainThunk);
										if (FAILED(hr))
										{
											wprintf(L"ICorRuntimeHost::GetDefaultDomain failed w/hr 0x%08lx\n", hr);
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
											_AppDomainPtr spDefaultAppDomain = NULL;
											hr = spAppDomainThunk->QueryInterface(IID_PPV_ARGS(&spDefaultAppDomain));
											if (FAILED(hr))
											{
												wprintf(L"Failed to get default AppDomain w/hr 0x%08lx\n", hr);
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
												// The .NET assembly to load.
												bstr_t bstrClientModuleAssemblyName(dotNetConfig->dotnet_module_path);
												_AssemblyPtr spClientModuleAssembly = NULL;

												hr = spDefaultAppDomain->Load_2(bstrClientModuleAssemblyName, &spClientModuleAssembly);
												if (FAILED(hr))
												{
													wprintf(L"Failed to load the assembly w/hr 0x%08lx\n", hr);
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
													// The .NET class to instantiate.
													bstr_t bstrClientModuleClassName(dotNetConfig->dotnet_module_entry_class);
													

													hr = spClientModuleAssembly->GetType_2(bstrClientModuleClassName, &result->spClientModuleType);
													if (FAILED(hr))
													{
														wprintf(L"Failed to get the Type interface w/hr 0x%08lx\n", hr);
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
														bstr_t bstrAzureIoTGatewayAssemblyName(AZUREIOTGATEWAYASSEMBLYNAME);

														hr = spDefaultAppDomain->Load_2(bstrAzureIoTGatewayAssemblyName, &(result->spAzureIoTGatewayAssembly));
														if (FAILED(hr))
														{
															wprintf(L"Failed to load the assembly w/hr 0x%08lx\n", hr);
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
															bstr_t bstrAzureIoTGatewayMessageBusClassName(AZUREIOTGATEWAY_MESSAGEBUS_CLASSNAME);
															_TypePtr spAzureIoTGatewayMessageBusClassType = NULL;

															hr = result->spAzureIoTGatewayAssembly->GetType_2(bstrAzureIoTGatewayMessageBusClassName, &spAzureIoTGatewayMessageBusClassType);
															if (FAILED(hr))
															{
																wprintf(L"Failed to get the Type interface w/hr 0x%08lx\n", hr);
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
																bstr_t bstrAzureIoTGatewayMessageClassName(AZUREIOTGATEWAY_MESSAGE_CLASSNAME);

																hr = result->spAzureIoTGatewayAssembly->GetType_2(bstrAzureIoTGatewayMessageClassName, &(result->spAzureIoTGatewayMessageClassType));
																if (FAILED(hr))
																{
																	wprintf(L"Failed to get the Type interface w/hr 0x%08lx\n", hr);
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
																	else
																	{
																		LONG index = 0;
																		variant_t msgBus((long long)result->bus);
																		hr = SafeArrayPutElement(psaAzureIoTGatewayMessageBusConstructorArgs, &index, &msgBus);
																		if (FAILED(hr))
																		{
																			wprintf(L"Adding Element on the safe array failed. w/hr 0x%08lx\n", hr);
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
																			variant_t vtAzureIoTGatewayMessageBusObject;

																			hr = result->spAzureIoTGatewayAssembly->CreateInstance_3(bstrAzureIoTGatewayMessageBusClassName, true, static_cast<BindingFlags>(BindingFlags_Instance | BindingFlags_Public), NULL, psaAzureIoTGatewayMessageBusConstructorArgs, NULL, NULL, &vtAzureIoTGatewayMessageBusObject);
																			if (FAILED(hr))
																			{
																				wprintf(L"Creating an instance of Message Bus failed with hr 0x%08lx\n", hr);
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
																				//Create an instance of the Client Module (Default Blank Constructor will be called. 
																				hr = spClientModuleAssembly->CreateInstance(bstrClientModuleClassName, &result->vtClientModuleObject);
																				if (FAILED(hr))
																				{
																					wprintf(L"Creating an instance of Client Class failed with hr 0x%08lx\n", hr);
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
																					else
																					{
																						index = 0;
																						hr = SafeArrayPutElement(psaClientModuleCreateArgs, &index, &vtAzureIoTGatewayMessageBusObject);
																						if (FAILED(hr))
																						{
																							wprintf(L"Adding Element on the safe array failed. w/hr 0x%08lx\n", hr);
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
																							index = 1;

																							variant_t vtdotNetArgsArg(dotNetConfig->dotnet_module_args);
																							hr = SafeArrayPutElement(psaClientModuleCreateArgs, &index, &vtdotNetArgsArg);
																							if (FAILED(hr))
																							{
																								wprintf(L"Adding Element on the safe array failed. w/hr 0x%08lx\n", hr);
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
																								bstr_t bstrCreateClientMethodName(L"Create");
																								variant_t vt_Empty;

																								hr = result->spClientModuleType->InvokeMember_3(bstrCreateClientMethodName, static_cast<BindingFlags>(BindingFlags_Instance | BindingFlags_Public | BindingFlags_InvokeMethod), NULL, result->vtClientModuleObject, psaClientModuleCreateArgs, &vt_Empty);
																								if (FAILED(hr))
																								{
																									wprintf(L"Failed to invoke Create Method with hr 0x%08lx\n", hr);
																									(result->pCorRuntimeHost)->Release();
																									result->pCorRuntimeHost = NULL;
																									(result->pRuntimeInfo)->Release();
																									result->pRuntimeInfo = NULL;
																									(result->pMetaHost)->Release();
																									result->pMetaHost = NULL;
																									delete(result);
																									result = NULL;
																								}
																							}
																						}
																						SafeArrayDestroy(psaClientModuleCreateArgs);
																						psaClientModuleCreateArgs = NULL;
																					}
																				}
																			}
																		}
																		SafeArrayDestroy(psaAzureIoTGatewayMessageBusConstructorArgs);
																		psaAzureIoTGatewayMessageBusConstructorArgs = NULL;
																	}
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
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
		DOTNET_HOST_HANDLE_DATA* result = (DOTNET_HOST_HANDLE_DATA*)moduleHandle;
		int32_t msg_size;
		const unsigned char*msgByteArray = Message_ToByteArray(messageHandle, &msg_size);
		
		variant_t msgContentInByteArray;

		V_VT(&msgContentInByteArray) = VT_ARRAY | VT_UI1;

		SAFEARRAYBOUND rgsabound[1];
		rgsabound[0].cElements = msg_size;
		rgsabound[0].lLbound = 0;

		V_ARRAY(&msgContentInByteArray) = SafeArrayCreate(VT_UI1, 1, rgsabound);

		void * pArrayData = NULL;
		SafeArrayAccessData(msgContentInByteArray.parray, &pArrayData);
		memcpy(pArrayData, msgByteArray, msg_size);
		SafeArrayUnaccessData(msgContentInByteArray.parray);
	
		SAFEARRAY *psaAzureIoTGatewayMessageConstructorArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		LONG index = 0;
		HRESULT hr = SafeArrayPutElement(psaAzureIoTGatewayMessageConstructorArgs, &index, &msgContentInByteArray);
		
		bstr_t bstrAzureIoTGatewayMessageClassName(AZUREIOTGATEWAY_MESSAGE_CLASSNAME);

		variant_t vtAzureIoTGatewayMessageObject;

	    hr = result->spAzureIoTGatewayAssembly->CreateInstance_3
		(
			bstrAzureIoTGatewayMessageClassName,
			true,
			static_cast<BindingFlags>(BindingFlags_Instance | BindingFlags_Public),
			NULL,
			psaAzureIoTGatewayMessageConstructorArgs,
			NULL, NULL,
			&vtAzureIoTGatewayMessageObject
		);
				
		bstr_t bstrCreateClientMethodName(L"Receive");
		SAFEARRAY *psaAzureIoTGatewayClientReceiveArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);
		variant_t vt_Empty;

		index = 0;
		hr = SafeArrayPutElement(psaAzureIoTGatewayClientReceiveArgs, &index, &vtAzureIoTGatewayMessageObject);

		hr = result->spClientModuleType->InvokeMember_3
		(
			bstrCreateClientMethodName, 
			static_cast<BindingFlags>(BindingFlags_Instance | BindingFlags_Public | BindingFlags_InvokeMethod), 
			NULL, 
			result->vtClientModuleObject, 
			psaAzureIoTGatewayClientReceiveArgs,
			&vt_Empty
		);


		//AZUREIOTGATEWAY_MESSAGE_CLASSNAME
	}
}


static void DotNET_Destroy(MODULE_HANDLE module)
{
    /*first stop the thread*/
	DOTNET_HOST_HANDLE_DATA* handleData = (DOTNET_HOST_HANDLE_DATA*)module;

	handleData->pCorRuntimeHost->Release();
	handleData->pCorRuntimeHost = NULL;
	(handleData->pRuntimeInfo)->Release();
	handleData->pRuntimeInfo = NULL;
	(handleData->pMetaHost)->Release();
	handleData->pMetaHost = NULL;

	delete(handleData);
}


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
