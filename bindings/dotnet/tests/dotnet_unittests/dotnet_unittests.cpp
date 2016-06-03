// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <cstdlib>
#ifdef _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "testrunnerswitcher.h"
#include "micromock.h"
#include "micromockcharstararenullterminatedstrings.h"
#include "azure_c_shared_utility/lock.h"
#include "azure_c_shared_utility/base64.h"
#include "dotnet.h"
#include "message.h"
#include "azure_c_shared_utility/constmap.h"
#include "azure_c_shared_utility/map.h"


static MICROMOCK_MUTEX_HANDLE g_testByTest;
static MICROMOCK_GLOBAL_SEMAPHORE_HANDLE g_dllByDll;

static size_t gMessageSize;
static const unsigned char * gMessageSource;

#define GBALLOC_H

extern "C" int   gballoc_init(void);
extern "C" void  gballoc_deinit(void);
extern "C" void* gballoc_malloc(size_t size);
extern "C" void* gballoc_calloc(size_t nmemb, size_t size);
extern "C" void* gballoc_realloc(void* ptr, size_t size);
extern "C" void  gballoc_free(void* ptr);

namespace BASEIMPLEMENTATION
{
    /*if malloc is defined as gballoc_malloc at this moment, there'd be serious trouble*/
#define Lock(x) (LOCK_OK + gballocState - gballocState) /*compiler warning about constant in if condition*/
#define Unlock(x) (LOCK_OK + gballocState - gballocState)
#define Lock_Init() (LOCK_HANDLE)0x42
#define Lock_Deinit(x) (LOCK_OK + gballocState - gballocState)
#include "gballoc.c"
#undef Lock
#undef Unlock
#undef Lock_Init
#undef Lock_Deinit

#include <stddef.h>   /* size_t */    

};

TYPED_MOCK_CLASS(CDOTNETMocks, CGlobalMock)
{
public:
    // memory
    MOCK_STATIC_METHOD_1(, void*, gballoc_malloc, size_t, size)
        void* result2 = BASEIMPLEMENTATION::gballoc_malloc(size);
    MOCK_METHOD_END(void*, result2);

    MOCK_STATIC_METHOD_1(, void, gballoc_free, void*, ptr)
        BASEIMPLEMENTATION::gballoc_free(ptr);
    MOCK_VOID_METHOD_END()
};

DECLARE_GLOBAL_MOCK_METHOD_1(CDOTNETMocks, , void*, gballoc_malloc, size_t, size);
DECLARE_GLOBAL_MOCK_METHOD_1(CDOTNETMocks, , void, gballoc_free, void*, ptr);



BEGIN_TEST_SUITE(dotnet_unittests)
    TEST_SUITE_INITIALIZE(TestClassInitialize)
    {
        TEST_INITIALIZE_MEMORY_DEBUG(g_dllByDll);
        g_testByTest = MicroMockCreateMutex();
        ASSERT_IS_NOT_NULL(g_testByTest);
    }

    TEST_SUITE_CLEANUP(TestClassCleanup)
    {
        MicroMockDestroyMutex(g_testByTest);
        TEST_DEINITIALIZE_MEMORY_DEBUG(g_dllByDll);
    }

    TEST_FUNCTION_INITIALIZE(TestMethodInitialize)
    {
        if (!MicroMockAcquireMutex(g_testByTest))
        {
            ASSERT_FAIL("our mutex is ABANDONED. Failure in test framework");
        }
    }

    TEST_FUNCTION_CLEANUP(TestMethodCleanup)
    {
        if (!MicroMockReleaseMutex(g_testByTest))
        {
            ASSERT_FAIL("failure in test framework at ReleaseMutex");
        }
    }

	/* Tests_SRS_DOTNET_04_001: [ DotNET_Create shall return NULL if bus is NULL. ] */
	TEST_FUNCTION(DotNET_Create_returns_NULL_when_bus_is_Null)
	{
		///arrage
		CDOTNETMocks mocks;
		const MODULE_APIS* theAPIS = Module_GetAPIS();

		mocks.ResetAllCalls();

		///act
		auto  result = theAPIS->Module_Create(NULL, "Anything");

		///assert
		mocks.AssertActualAndExpectedCalls();
		ASSERT_IS_NULL(result);

		///cleanup
	}

	/* Tests_SRS_DOTNET_04_002: [ DotNET_Create shall return NULL if configuration is NULL. ] */
	TEST_FUNCTION(DotNET_Create_returns_NULL_when_configuration_is_Null)
	{
		///arrage
		CDOTNETMocks mocks;
		const MODULE_APIS* theAPIS = Module_GetAPIS();

		mocks.ResetAllCalls();

		///act
		auto  result = theAPIS->Module_Create((MESSAGE_BUS_HANDLE)0x42, NULL);

		///assert
		mocks.AssertActualAndExpectedCalls();
		ASSERT_IS_NULL(result);

		///cleanup
	}
	
	/* Test_SRS_DOTNET_04_003: [ DotNET_Create shall return NULL if configuration->dotnet_module_path is NULL. ] */
	TEST_FUNCTION(DotNET_Create_returns_NULL_when_dotnet_module_path_is_Null)
	{
		///arrage
		CDOTNETMocks mocks;
		const MODULE_APIS* theAPIS = Module_GetAPIS();
		DOTNET_HOST_CONFIG dotNetConfig;
		dotNetConfig.dotnet_module_path = NULL;
		dotNetConfig.dotnet_module_entry_class = "mycsharpmodule.classname";
		dotNetConfig.dotnet_module_args = "module configuration";

		mocks.ResetAllCalls();

		///act
		auto  result = theAPIS->Module_Create((MESSAGE_BUS_HANDLE)0x42, &dotNetConfig);

		///assert
		mocks.AssertActualAndExpectedCalls();
		ASSERT_IS_NULL(result);

		///cleanup
	}

	/* Tests_SRS_DOTNET_04_004: [ DotNET_Create shall return NULL if configuration->dotnet_module_entry_class is NULL. ] */
	TEST_FUNCTION(DotNET_Create_returns_NULL_when_dotnet_module_entry_class_is_Null)
	{
		///arrage
		CDOTNETMocks mocks;
		const MODULE_APIS* theAPIS = Module_GetAPIS();
		DOTNET_HOST_CONFIG dotNetConfig;
		dotNetConfig.dotnet_module_path = "/path/to/csharp_module.dll";
		dotNetConfig.dotnet_module_entry_class = NULL;
		dotNetConfig.dotnet_module_args = "module configuration";

		mocks.ResetAllCalls();

		///act
		auto  result = theAPIS->Module_Create((MESSAGE_BUS_HANDLE)0x42, &dotNetConfig);

		///assert
		mocks.AssertActualAndExpectedCalls();
		ASSERT_IS_NULL(result);

		///cleanup
	}

	/* Tests_SRS_DOTNET_04_005: [ DotNET_Create shall return NULL if configuration->dotnet_module_args is NULL. ] */
	TEST_FUNCTION(DotNET_Create_returns_NULL_when_dotnet_module_args_is_Null)
	{
		///arrage
		CDOTNETMocks mocks;
		const MODULE_APIS* theAPIS = Module_GetAPIS();
		DOTNET_HOST_CONFIG dotNetConfig;
		dotNetConfig.dotnet_module_path = "/path/to/csharp_module.dll";
		dotNetConfig.dotnet_module_entry_class = "mycsharpmodule.classname";
		dotNetConfig.dotnet_module_args = NULL;

		mocks.ResetAllCalls();

		///act
		auto  result = theAPIS->Module_Create((MESSAGE_BUS_HANDLE)0x42, &dotNetConfig);

		///assert
		mocks.AssertActualAndExpectedCalls();
		ASSERT_IS_NULL(result);

		///cleanup
	}

	/* Tests_SRS_DOTNET_04_006: [ DotNET_Create shall return NULL if an underlying API call fails. ] */
	TEST_FUNCTION(DotNET_Create_returns_NULL_when_malloc_fails)
	{
		///arrage
		CDOTNETMocks mocks;
		const MODULE_APIS* theAPIS = Module_GetAPIS();
		DOTNET_HOST_CONFIG dotNetConfig;
		dotNetConfig.dotnet_module_path = "/path/to/csharp_module.dll";
		dotNetConfig.dotnet_module_entry_class = "mycsharpmodule.classname";
		dotNetConfig.dotnet_module_args = "module configuration";

		mocks.ResetAllCalls();

		STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
			.IgnoreArgument(1)
			.SetFailReturn((void*)NULL);

		///act
		auto  result = theAPIS->Module_Create((MESSAGE_BUS_HANDLE)0x42, &dotNetConfig);

		///assert
		mocks.AssertActualAndExpectedCalls();
		ASSERT_IS_NULL(result);

		///cleanup
	}

	/* Tests_SRS_DOTNET_04_007: [ DotNET_Create shall return a non-NULL MODULE_HANDLE when successful. ] */
	/* Tests_SRS_DOTNET_04_008: [ DotNET_Create shall allocate memory for an instance of the DOTNET_HOST_HANDLE_DATA structure and use that as the backing structure for the module handle. ] */
	/* Tests_SRS_DOTNET_04_012: [ DotNET_Create shall get the 3 CLR Host Interfaces (CLRMetaHost, CLRRuntimeInfo and CorRuntimeHost) and save it on DOTNET_HOST_HANDLE_DATA. ] */
	TEST_FUNCTION(DotNET_Create_succeed)
	{
		///arrage
		CDOTNETMocks mocks;
		const MODULE_APIS* theAPIS = Module_GetAPIS();
		DOTNET_HOST_CONFIG dotNetConfig;
		dotNetConfig.dotnet_module_path = "/path/to/csharp_module.dll";
		dotNetConfig.dotnet_module_entry_class = "mycsharpmodule.classname";
		dotNetConfig.dotnet_module_args = "module configuration";

		mocks.ResetAllCalls();

		STRICT_EXPECTED_CALL(mocks, gballoc_malloc(IGNORED_NUM_ARG))
			.IgnoreArgument(1);

		///act
		auto  result = theAPIS->Module_Create((MESSAGE_BUS_HANDLE)0x42, &dotNetConfig);

		///assert
		mocks.AssertActualAndExpectedCalls();
		ASSERT_IS_NOT_NULL(result);

		///cleanup
		theAPIS->Module_Destroy((MODULE_HANDLE)result);
	}




	/* Tests_SRS_DOTNET_04_021: [ Module_GetAPIS shall return a non-NULL pointer to a structure of type MODULE_APIS that has all fields initialized to non-NULL values. ] */
	TEST_FUNCTION(Module_GetAPIS_returns_non_NULL)
	{
		///arrage
		CDOTNETMocks mocks;

		///act
		const MODULE_APIS* apis = Module_GetAPIS();

		///assert
		ASSERT_IS_NOT_NULL(apis);
		ASSERT_IS_NOT_NULL(apis->Module_Destroy);
		ASSERT_IS_NOT_NULL(apis->Module_Create);
		ASSERT_IS_NOT_NULL(apis->Module_Receive);

		///cleanup
	}

END_TEST_SUITE(dotnet_unittests)