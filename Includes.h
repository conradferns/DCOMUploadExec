#pragma once
#define _WIN32_DCOM

#include <iostream>
#include <string>
#include <Windows.h>
#include <intsafe.h>
#include <comdef.h>
#include <combaseapi.h>
#include <comdef.h>
#include <propsys.h>
#include <shlwapi.h>
#include "ReversedMsi.h"
#include "ComUtils.h"
#include "msiquery.h"
#include "Utils.h"
#pragma comment (lib, "Msi.lib")
#include <format>
#include <winsxs.h>