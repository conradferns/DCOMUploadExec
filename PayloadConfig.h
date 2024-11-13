#pragma once
#include "Windows.h"

// Values are set to the default AssemblyPayload payload.dll
// configure those according to your custom payloads

constexpr LPCWSTR PAYLOAD_ASSEMBLY_PATH = L"payload.dll";		// local path to your STRONG NAMED .NET assembly payload
constexpr LPCWSTR ASSEMBLY_NAME = L"payload";					// set to your assembly name
constexpr LPCWSTR ASSEMBLY_BITNESS = L"64";						// set to 64 or 32 for x64 or x86
constexpr LPCWSTR ASSEMBLY_VERSION = L"1.0.0.0";				// sigcheck.exe -n PAYLOAD_ASSEMBLY_PATH
constexpr LPCWSTR ASSEMBLY_PUBLIC_KEY = L"136e5fbf23bb401e";	// sn.exe -T PAYLOAD_ASSEMBLY_PATH

