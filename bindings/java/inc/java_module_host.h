// Copyright (c) Microsoft. All rights reserved.
// Licensed under MIT license. See LICENSE file in the project root for full license information.

#ifndef JAVA_MODULE_HOST_H
#define JAVA_MODULE_HOST_H

#include <stdbool.h>
#include <cstdbool>
#include "module.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct JVM_OPTIONS_TAG
{
    int version;
    bool debug;
    int debug_port;
    bool verbose;
    void* additional_options;
} JVM_OPTIONS;

typedef struct JAVA_MODULE_HOST_CONFIG_TAG
{
    const char* class_path;
    const char* class_name;
    const char* configuration_json;
    JVM_OPTIONS* options;
} JAVA_MODULE_HOST_CONFIG;

MODULE_EXPORT const MODULE_APIS* MODULE_STATIC_GETAPIS(JAVA_MODULE_HOST)(void);

#ifdef __cplusplus
}
#endif

#endif /*JAVA_MODULE_HOST_H*/
