/**
 ******************************************************************************
 * Azure : Open Source Multi-Target Memory Editor                             *
 * File  : targets.h                                                          *
 ******************************************************************************
 * Copyright 2018 Satori. All rights reserved.                                *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

// TODO: this needs a lot of work

#pragma once

#if defined(__APPLE__)

#include <TargetConditionals.h>

#if defined(TARGET_OS_IPHONE)
#define AZURE_TARGET_IOS 1

#elif defined(TARGET_OS_OSX)
#define AZURE_TARGET_MACOS 1

#endif  // TARGET_OS_IPHONE

#elif defined(ANDROID) || defined(__ANDROID__)

#define AZURE_TARGET_ANDROID 1

#elif defined(__linux__)

#define AZURE_TARGET_LINUX

#elif defined(_WIN32)

#define AZURE_TARGET_WINDOWS

#else

#warning "Unknown target platform. Compilation may fail."

#endif