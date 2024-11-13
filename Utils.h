#pragma once
#include <regex>
#include <shlwapi.h>
#include <Windows.h>

constexpr std::wstring_view LOCAL_ATTACK_KEYWORD = L"LOCALHOST";

size_t GetEnvironmentSizeW(WCHAR* pchEnvironment);
bool parseArgument(const std::wstring& arg, std::wstring& domain, std::wstring& user, std::wstring& password, std::wstring& address);