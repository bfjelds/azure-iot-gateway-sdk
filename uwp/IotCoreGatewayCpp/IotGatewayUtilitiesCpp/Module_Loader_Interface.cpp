#include "pch.h"

#define UWP
#define WIN32

#include "module_loader.h"

MODULE_LIBRARY_HANDLE ModuleLoader_Load(const char* moduleLibraryFileName)
{
	return NULL;
}

const MODULE_APIS* ModuleLoader_GetModuleAPIs(MODULE_LIBRARY_HANDLE moduleLibraryHandle)
{
	return NULL;
}

void ModuleLoader_Unload(MODULE_LIBRARY_HANDLE moduleLibraryHandle)
{

}
