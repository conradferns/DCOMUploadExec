#pragma once
#include "Includes.h"

typedef HRESULT(*DllGetClassObjectFunc)(
	REFCLSID rclsid,
	REFIID   riid,
	LPVOID* ppv
	);

class ComUtils
{
public:
	static void InitAuthStructs(COAUTHIDENTITY* pAuthId, COAUTHINFO* pAuthInfo,  std::wstring& w_domain,  std::wstring& w_username, std::wstring& w_password);
	static IUnknown* CreateObject(CLSID clsid, IID iid, DWORD ctx, std::wstring& w_address, COAUTHINFO* pAuthInfo = nullptr);
	static IUnknown* CreateObjectFromDllFactory(HMODULE hdll,CLSID clsidFactory);
	static HRESULT SetupInterfaceAuth(IUnknown* pIface, COAUTHINFO* pAuthInfo);
	static HRESULT SetupAuthOnParentIUnknownCastToIID(IUnknown* pAnyIface, COAUTHINFO* pAuthInfo, IUnknown** ppCastedOutput, const IID& riid);

private:
	static inline bool gComInited = false;
};


