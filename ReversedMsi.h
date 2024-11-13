#pragma once
#include <Msi.h>

#include "Includes.h"
#include "MsiEnums.h"


extern GUID CLSID_MsiServer;
extern GUID IID_IMsiSever;
extern GUID IID_IMsiCustomAction;
extern GUID CLSID_MSIRemoteApi;

typedef wchar_t ICHAR;
typedef ICHAR ichar;
typedef USHORT ushort;
typedef ULONG ulong;
typedef UCHAR uchar;
typedef BOOL Bool;


typedef PVOID IMsiRemoteAPI;
typedef PVOID IMsiRecord;
typedef PVOID IMsiMessage;


struct IMsiCustomAction : IUnknown
{
	virtual HRESULT PrepareDLLCustomAction(const ICHAR* szActionName,
		const ICHAR* szPath,
		const ICHAR* szEntryPoint, 
		unsigned long hmsi,
		boolean fDebugBreak,
		boolean fAppCompat,  
		const GUID* pguidAppCompatDB,
		const GUID* pguidAppCompatID,
		DWORD* dwThreadId)=0;
	virtual HRESULT RunDLLCustomAction(DWORD dwThreadId,  unsigned long* pulRet) = 0;
	virtual HRESULT FinishDLLCustomAction( DWORD dwThreadId) = 0;
	virtual HRESULT RunScriptAction( int icaType, IDispatch* piEngine,  const ICHAR* szSource, // described in tongzx/nt5src/Source/XPSP1/NT/admin/darwin/src/engine/camgr.cpp line 352
		const ICHAR* szTarget,  LANGID iLangId,  int* iScriptResult,  int* pcb,
		char** pchRecord) = 0;
	virtual HRESULT QueryPathOfRegTypeLib( REFGUID guid, unsigned short wVerMajor,  unsigned short wVerMinor,
		 LCID lcid,  OLECHAR* lpszPathName,  int cchPath) = 0;
	virtual HRESULT ProcessTypeLibrary(const OLECHAR* szLibID, LCID lcidLocale,
		const OLECHAR* szTypeLib, const OLECHAR* szHelpPath,
		 int fRemove,  int* fInfoMismatch) = 0;
	virtual HRESULT SQLInstallDriverEx(int cDrvLen,  const ichar* szDriver,
		const ichar* szPathIn,
		ichar* szPathOut,  WORD cbPathOutMax,
		 WORD* pcbPathOut, WORD fRequest,  DWORD* pdwUsageCount) = 0;
	virtual HRESULT SQLConfigDriver( WORD fRequest,
		const ichar* szDriver,
		const ichar* szArgs,
		ichar* szMsg,  WORD cbMsgMax,
		 WORD* pcbMsgOut) = 0;
	virtual HRESULT SQLRemoveDriver( const ichar* szDriver, int fRemoveDSN,
		 DWORD* pdwUsageCount) = 0;
	virtual HRESULT SQLInstallTranslatorEx(int cTransLen, const ichar* szTranslator,
		 const ichar* szPathIn,
		 ichar* szPathOut,  WORD cbPathOutMax,
		 WORD* pcbPathOut,  WORD fRequest,  DWORD* pdwUsageCount) = 0;
	virtual HRESULT SQLRemoveTranslator( const ichar* szTranslator,  DWORD* pdwUsageCount) = 0;
	virtual HRESULT SQLConfigDataSource( WORD fRequest,
		 const ichar* szDriver,
		 const ichar* szAttributes,
		 DWORD cbAttrSize) = 0;
	virtual HRESULT SQLInstallDriverManager( ichar* szPath,
		 WORD cbPathMax,  WORD* pcbPathOut) = 0;
	virtual HRESULT SQLRemoveDriverManager( DWORD* pdwUsageCount) = 0;
	virtual HRESULT SQLInstallerError( WORD iError,  DWORD* pfErrorCode,
		 ichar* szErrorMsg,  WORD cbErrorMsgMax,
		 WORD* pcbErrorMsg) = 0;

	virtual HRESULT URTMakeFusionFullPath(ushort const*, ushort*, ulong, int*) = 0;
	virtual HRESULT URTCarryingNDP(int) = 0;
	virtual HRESULT URTUnloadFusionBinaries() = 0;
	virtual HRESULT URTAddAssemblyInstallComponent(wchar_t const* UserDefinedGuid1, wchar_t const* UserDefinedGuid2, wchar_t const* UserDefinedName) = 0;
	virtual HRESULT URTIsAssemblyInstalled(ushort const*, ushort const*, int*, int*, char**) = 0;
	virtual HRESULT URTProvideGlobalAssembly(wchar_t const* AsmName, DWORD InstallMode, wchar_t* AsmPath) = 0;
	virtual HRESULT URTCommitAssemblies(wchar_t const* UserDefinedGuid1, int* pInt, char** pStr) = 0;
	virtual HRESULT URTUninstallAssembly(ushort const*, ushort const*, int*, char**) = 0;
	virtual HRESULT URTGetAssemblyCacheItem(wchar_t const* UserDefinedGuid1, wchar_t const* UserDefinedGuid2, ulong zeroed, int* pInt, char** pStr) = 0;
	virtual HRESULT URTCreateAssemblyFileStream(wchar_t const* FileName, int Format) = 0;
	virtual HRESULT URTWriteAssemblyBits(const char* pv, ulong cb, ulong* pcbWritten) = 0;
	virtual HRESULT URTCommitAssemblyStream() = 0;
	virtual HRESULT URTGetFusionPath(ushort const*, int, ushort*, ulong, ulong*, ushort*, ulong, int*) = 0;
	virtual HRESULT URTAreAssembliesEqual(ushort const*, ushort const*, int*, int*, char**) = 0;
	virtual HRESULT URTQueryAssembly(ICHAR const*, ushort const*, ulong, int*, char**) = 0;
	virtual HRESULT LoadEmbeddedDLL(ichar const* path, bool debug) = 0; // changed uchar to bool
	virtual HRESULT CallInitDLL(ulong intVar, PVOID pVar, ulong* pInt, ulong* pExportReturnCode) = 0;
	virtual HRESULT CallMessageDLL(UINT, ulong, ulong*) = 0;
	virtual HRESULT CallShutdownDLL(ulong*) = 0;
	virtual HRESULT UnloadEmbeddedDLL() = 0;
	virtual HRESULT SetNewClientProcess(ulong, IMsiRemoteAPI*) = 0;
	// more methods exist

};

struct IMsiConfigurationManager : IUnknown
{
	virtual iesEnum InstallFinalize( iesEnum iesState,  void* riMessage,  boolean fUserChangedDuringInstall) = 0;
	virtual IMsiRecord* SetLastUsedSource( const ICHAR* szProductCode,  const wchar_t* szPath, boolean fAddToList,  boolean fPatch) = 0;
	virtual boolean Reboot() = 0;
	virtual int DoInstall( ireEnum ireProductCode,  const ICHAR* szProduct,  const ICHAR* szAction,
		 const ICHAR* szCommandLine,  const ICHAR* szLogFile,
		 int iLogMode,  boolean fFlushEachLine,  IMsiMessage* riMessage,  iioEnum iioOptions , ULONG, HWND__*, IMsiRecord* ) = 0;

	virtual HRESULT IsServiceInstalling() = 0;
	virtual IMsiRecord* RegisterUser( const ICHAR* szProductCode,  const ICHAR* szUserName,const ICHAR* szCompany,  const ICHAR* szProductID) = 0;
	virtual IMsiRecord* RemoveRunOnceEntry( const ICHAR* szEntry) = 0;
	virtual boolean CleanupTempPackages( IMsiMessage& riMessage, bool flag) = 0;
	virtual HRESULT SourceListClearByType(const ICHAR* szProductCode, const ICHAR*, isrcEnum isrcType) = 0;
	virtual HRESULT SourceListAddSource( const ICHAR* szProductCode,  const ICHAR* szUserName,  isrcEnum isrcType,const ICHAR* szSource) = 0 ;
	virtual HRESULT SourceListClearLastUsed( const ICHAR* szProductCode,  const ICHAR* szUserName) = 0;
	virtual HRESULT RegisterCustomActionServer( icacCustomActionContext* picacContext,  const unsigned char* rgchCookie,  const int cbCookie, IMsiCustomAction* piCustomAction,  unsigned long* dwProcessId,  IMsiRemoteAPI** piRemoteAPI,  DWORD* dwPrivileges) = 0;

	virtual HRESULT CreateCustomActionServer( const icacCustomActionContext icacContext,  const unsigned long dwProcessId,  IMsiRemoteAPI* piRemoteAPI,
	const WCHAR* pvEnvironment,  DWORD cchEnvironment,  DWORD dwPrivileges,  char* rgchCookie,  int* cbCookie,  IMsiCustomAction** piCustomAction,  unsigned long* dwServerProcessId, bool unkFalse) = 0;
};