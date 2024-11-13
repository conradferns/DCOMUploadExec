#include "ComUtils.h"

void ComUtils::InitAuthStructs(COAUTHIDENTITY* pAuthId, COAUTHINFO* pAuthInfo,  std::wstring& w_domain,  std::wstring& w_username, std::wstring& w_password)
{

    if (!w_domain.empty()) {
        pAuthId->Domain = (unsigned short*)w_domain.c_str();
        pAuthId->DomainLength = wcslen(w_domain.c_str());
    }
    if (!w_password.empty()) {
        pAuthId->Password = (unsigned short*)w_password.c_str();
        pAuthId->PasswordLength = wcslen(w_password.c_str());
    }
    if (!w_username.empty()) {
        pAuthId->User = (unsigned short*)w_username.c_str();
        pAuthId->UserLength = wcslen(w_username.c_str());
    }

    pAuthInfo->dwAuthnLevel = RPC_C_AUTHN_LEVEL_PKT_INTEGRITY ;
    pAuthInfo->dwAuthnSvc = RPC_C_AUTHN_WINNT;
    pAuthInfo->dwAuthzSvc = RPC_C_AUTHZ_NONE;
    pAuthInfo->dwCapabilities = EOAC_NONE;
    pAuthInfo->dwImpersonationLevel = RPC_C_IMP_LEVEL_IMPERSONATE; 

    // setup identity details if existing
    if (!w_username.empty() || !w_password.empty() || !w_domain.empty())
    {
        pAuthId->Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
        pAuthInfo->pAuthIdentityData = pAuthId;
    }
    else
    {
        pAuthInfo->pAuthIdentityData = nullptr;
    }
}

IUnknown* ComUtils::CreateObject(CLSID clsid, IID iid, DWORD ctx, std::wstring& w_address, COAUTHINFO* pAuthInfo)
{
    HRESULT hr = NULL;

    // Initialize COM
    if(!gComInited)
    {
		hr = CoInitialize(NULL);
        if(FAILED(hr))
        {
            std::cout << "[-] Failed CoInitialize: " << std::hex << hr << '\n';
            return nullptr;
        }
    }

    COSERVERINFO coserverinfo = {};
    // setup network info if we are directed to a remote address
    if (!w_address.empty())
        coserverinfo.pwszName = w_address.data();
    coserverinfo.pAuthInfo = pAuthInfo;
    
    MULTI_QI qi ={};
    qi.pIID = &iid; // the interface we aim to get
    qi.pItf = NULL;
    qi.hr = 0;

    SOLE_AUTHENTICATION_INFO sni = {};
    sni.dwAuthnSvc = pAuthInfo->dwAuthnSvc;
    sni.dwAuthzSvc = pAuthInfo->dwAuthzSvc;
    sni.pAuthInfo = pAuthInfo->pAuthIdentityData;

    SOLE_AUTHENTICATION_LIST sal = {};
    sal.cAuthInfo = 1;
    sal.aAuthInfo = &sni;

    // initialize security 
    hr = CoInitializeSecurity(nullptr, -1, NULL, NULL, pAuthInfo->dwAuthnLevel, pAuthInfo->dwImpersonationLevel, &sal, EOAC_NONE, NULL);
    if (FAILED(hr)) {
        std::cout << "[-] Failed CoInitializeSecurity: " << std::hex << hr << '\n';
        return nullptr;
    }

	hr = CoCreateInstanceEx(clsid, NULL, ctx, &coserverinfo, 1, &qi);

    if (FAILED(hr)) {
        std::cout << "[-] Failed CoCreateInstanceEx: " << std::hex << hr << '\n';
    }
    return qi.pItf;
}

IUnknown* ComUtils::CreateObjectFromDllFactory(HMODULE hdll, CLSID clsidFactory)
{
    IClassFactory* fact;
    IUnknown* requestedObject = nullptr;
    DllGetClassObjectFunc DllGetClassObject = (DllGetClassObjectFunc)GetProcAddress(hdll, "DllGetClassObject");
    if(!DllGetClassObject)
    {
        std::cout << "[-] Failed locating DllGetClassObject on hdll: " << std::hex << GetLastError() << '\n';
        return nullptr;
    }

	HRESULT hr = DllGetClassObject(clsidFactory, IID_IClassFactory, (PVOID*)&fact);
    if(FAILED(hr))
    {
        std::cout << "[-] Failed DllGetClassObject on hdll: " << std::hex << GetLastError() << '\n';
        return nullptr;
    }
    hr = fact->CreateInstance(NULL, clsidFactory, (PVOID*)&requestedObject);
    if (FAILED(hr))
    {
        std::cout << "[-] Failed CreateInstance on hdll: " << std::hex << GetLastError() << '\n';
    }
    return requestedObject;
}

HRESULT ComUtils::SetupInterfaceAuth(IUnknown* iface, COAUTHINFO* pAuthInfo)
{
    OLECHAR* pServerPrincName = nullptr;
    std::wstring principleName;
	if (pAuthInfo->pAuthIdentityData)
    {
         principleName = std::format(L"{}\\{}", (WCHAR*)pAuthInfo->pAuthIdentityData->Domain, (WCHAR*)pAuthInfo->pAuthIdentityData->User);
         pServerPrincName = principleName.data();
    }

    HRESULT hr = CoSetProxyBlanket(iface, pAuthInfo->dwAuthnSvc, pAuthInfo->dwAuthzSvc, pServerPrincName, pAuthInfo->dwAuthnLevel,
        pAuthInfo->dwImpersonationLevel, pAuthInfo->pAuthIdentityData, EOAC_DEFAULT);
    if (FAILED(hr)) {
        std::cout << "[-] Failed CoSetProxyBlanket: " << std::hex << hr << '\n';
    }
    return hr;
}

/*
 * Setup auth on interfaces the correct way. We must apply auth to the underlying IID_IUnknown
 * else QueryInterface will fail with access denied when attempting to query the derived interface.
 * read more: "https://learn.microsoft.com/en-us/windows/win32/wmisdk/setting-authentication-using-c-"
 */
HRESULT ComUtils::SetupAuthOnParentIUnknownCastToIID(IUnknown* pAnyIface, COAUTHINFO* pAuthInfo, IUnknown** ppCastedOutput, const IID& riid)
{
    IUnknown* pUnknown = nullptr;
    SetupInterfaceAuth(pAnyIface, pAuthInfo);

    HRESULT hr = pAnyIface->QueryInterface(IID_IUnknown, reinterpret_cast<void**>(&pUnknown));
    if (FAILED(hr)) {
        pAnyIface->Release();
        std::cout << "[-] Failed QueryInterface to underlying IUnknown: " << std::hex << hr << '\n';
    }
   SetupInterfaceAuth(pUnknown, pAuthInfo);

   hr = pUnknown->QueryInterface(riid, reinterpret_cast<void**>(ppCastedOutput));
   if (FAILED(hr)) {
       pUnknown->Release();
       pAnyIface->Release();
       std::cout << "[-] Failed QueryInterface to cast back from IUnknown: " << std::hex << hr << '\n';
   }
   return SetupInterfaceAuth(*ppCastedOutput, pAuthInfo);
}
