// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "module.h"
#include "azure_c_shared_utility/iot_logging.h"

#include "azure_c_shared_utility/threadapi.h"
#include "dotnet.h"
#include "azure_c_shared_utility/iot_logging.h"
#include "azure_c_shared_utility/lock.h"

typedef struct DOTNET_HOST_HANDLE_DATA_TAG
{
    MESSAGE_BUS_HANDLE          bus;
    //ICLRMetaHost                *pMetaHost;
    //ICLRRuntimeInfo             *pRuntimeInfo;
    //ICorRuntimeHost             *pCorRuntimeHost;
}DOTNET_HOST_HANDLE_DATA;

static MODULE_HANDLE DotNET_Create(MESSAGE_BUS_HANDLE busHandle, const void* configuration)
{
	DOTNET_HOST_HANDLE_DATA* result;
    if (
        (busHandle == NULL) /*configuration is not used*/
        )
    {
        LogError("invalid arg busHandle=%p", busHandle);
        result = NULL;
    }
    else
    {
        result = malloc(sizeof(DOTNET_HOST_HANDLE_DATA));
        if(result == NULL)
        {
            LogError("unable to malloc");
        }
        else
        {
            result->bus = busHandle;
            //TODO: put here code to get the CLR Stuff. 

        }
	}
    return result;
}

static void DotNET_Receive(MODULE_HANDLE moduleHandle, MESSAGE_HANDLE messageHandle)
{
    /*TODO */
}


static void DotNET_Destroy(MODULE_HANDLE module)
{
    /*first stop the thread*/
	DOTNET_HOST_HANDLE_DATA* handleData = module;
  
    free(handleData);
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
	return &DOTNET_APIS_all;
}
