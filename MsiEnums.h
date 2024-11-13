#pragma once
typedef enum iesEnum  // return status from IMsiEngine::Sequence and DoAction
{
	iesNoAction = 0,  // action not invoked
	iesSuccess = 1,  // IDOK,     completed actions successfully
	iesUserExit = 2,  // IDCANCEL, user terminated prematurely,$resume with next action
	iesFailure = 3,  // IDABORT,  unrecoverable error occurred
	iesSuspend = 4,  // IDRETRY,  sequence suspended, resume with same action
	iesFinished = 5,  // IDIGNORE, skip remaining actions
	iesWrongState = 6,  // calling sequence error, not in executable state
	iesBadActionData = 7,  // invalid Action table record data
	iesInstallRunning = 8, // another install is in progress
	iesNextEnum
} iesEnum;

enum iseEnum
{
	iseInclude = 1,
	iseChar = 2,
	iseEnd = 4,
	iseTrim = 8,
	iseFirst = 0,
	iseLast = iseEnd,
	iseUpto = iseChar,
	iseIncluding = iseChar + iseInclude,
	iseFrom = iseChar + iseInclude + iseEnd,
	iseAfter = iseChar + iseEnd,
	iseUptoTrim = iseUpto + iseTrim,
	iseAfterTrim = iseAfter + iseTrim,
};

enum iscEnum
{
	iscExact = 0,
	iscExactI = 1,
	iscStart = 2,
	iscStartI = 3,
	iscEnd = 4,
	iscEndI = 5,
	iscWithin = iscStart + iscEnd,
	iscWithinI = iscStart + iscEnd + 1,
};

// IMsiMessage::Message return status codes
typedef enum imsEnum
{
	imsError = 0x3fff, // unsuccessful
	imsNone = 0, // no action taken
	imsOk = 1, // IDOK
	imsCancel = 2, // IDCANCEL
	imsAbort = 3, // IDABORT
	imsRetry = 4, // IDRETRY
	imsIgnore = 5, // IDIGNORE
	imsYes = 6, // IDYES
	imsNo = 7, // IDNO
} imsEnum;

const int imtShiftCount = 24;  // message type in high-order bits
const int imtTypeMask = 255 << imtShiftCount;  // mask for type code and flags

enum imtEnum
{
	// messages processed by modal dialog and/or log
	imtFatalExit = 0 << imtShiftCount, // thread terminated prematurely
	imtError = 1 << imtShiftCount, // error message,   [1] is imsg/idbg code
	imtWarning = 2 << imtShiftCount, // warning message, [1] is imsg/idbg code, not fatal
	imtUser = 3 << imtShiftCount, // user request,    [1] is imsg/idbg code
	imtInfo = 4 << imtShiftCount, // informative message for log, not to be displayed
	imtFilesInUse = 5 << imtShiftCount, // list of files in use than need to be processed
	imtResolveSource = 6 << imtShiftCount,
	imtOutOfDiskSpace = 7 << imtShiftCount, // out of disk space for one or more volumes
	// messages processed by modeless progress dialog
	imtActionStart = 8 << imtShiftCount, // start of action, [1] action name, [2] description
	imtActionData = 9 << imtShiftCount, // data associated with individual action item
	imtProgress = 10 << imtShiftCount, // progress gauge info, [1] units so far, [2] total
	imtCommonData = 11 << imtShiftCount, // control message to handler [1] is control message type followed by params
	// messages processed by message dispatcher, not sent to display or log
	imtLoadHandler = 12 << imtShiftCount, // load external UI handler
	imtFreeHandler = 13 << imtShiftCount, // free external UI handler
	imtShowDialog = 14 << imtShiftCount, // use handler to show dialog or run wizard
	imtInternalExit = 15 << imtShiftCount, // private use by MsiUIMessageContext

	// not sent to Message(), strings used for log and UI fields, MUST TRACK imsg values
	imtLogHeader = 12 << imtShiftCount, // log header format string, not sent to Message
	imtLogTrailer = 13 << imtShiftCount, // log trailer format string, not sent to Message
	imtActionStarted = 14 << imtShiftCount, // action started log message
	imtActionEnded = 15 << imtShiftCount, // action ended log message
	// all preceeding messsages cached by the engine, following ones cached by message handler
	imtTimeRemaining = 16 << imtShiftCount, // Time remaining string for basic UI progress dlg
	imtOutOfMemory = 17 << imtShiftCount, // out of memory format string, CANNOT CONTAIN PARAMETERS
	imtTimedOut = 18 << imtShiftCount, // engine timeout format string, CANNOT CONTAIN PARAMETERS
	imtException = 19 << imtShiftCount, // premature termination format string, CANNOT CONTAIN PARAMETERS
	imtBannerText = 20 << imtShiftCount, // string displayed in basic UI in the ActionStart field.
	imtScriptInProgress = 21 << imtShiftCount, // Info string displayed while script is being built
	imtUpgradeRemoveTimeRemaining = 22 << imtShiftCount, // Time remaining string for uninstall during upgrade
	imtUpgradeRemoveScriptInProgress = 23 << imtShiftCount, // Info string displayed during script generation for uninstal during upgrade
	// message box button styles - identical to Win32 definitions, default is imtOK
	imtOk = 0,    // MB_OK
	imtOkCancel = 1,    // MB_OKCANCEL
	imtAbortRetryIgnore = 2,    // MB_ABORTRETRYIGNORE
	imtYesNoCancel = 3,    // MB_YESNOCANCEL
	imtYesNo = 4,    // MB_YESNO
	imtRetryCancel = 5,    // MB_RETRYCANCEL
	// message box icon styles - identical to Win32 definitions, default is none
	imtIconError = 1 << 4, // MB_ICONERROR
	imtIconQuestion = 2 << 4, // MB_ICONQUESTION
	imtIconWarning = 3 << 4, // MB_ICONWARNING
	imtIconInfo = 4 << 4, // MB_ICONINFORMATION
	// message box default button - identical to Win32 definitions, default is ficat
	imtDefault1 = 0 << 8, // MB_DEFBUTTON1
	imtDefault2 = 1 << 8, // MB_DEFBUTTON2
	imtDefault3 = 2 << 8, // MB_DEFBUTTON3

	// internal flags, not sent to UI handlers
	imtSendToEventLog = 1 << 29,
	imtForceQuietMessage = 1 << 30, // force message in quiet or basic UI
	imtSuppressLog = 1 << 31, // suppress message from log (LOGACTIONS property)
};

typedef enum ireEnum  // enumeration for 1st arg of CreateAndRunEngine
{
	ireProductCode,
	irePackagePath,
	ireSubStorage,
	ireDatabaseHandle,
	ireInstallFinalize,
} ireEnum;

typedef enum iioEnum  // various options flags passed to IMsiEngine::Initialize
{
	iioUpgrade = 0x00000001,
	iioChild = 0x00000002,
	iioDisablePlatformValidation = 0x00000004,
	iioEndDialog = 0x00000008,
	iioCreatingAdvertiseScript = 0x00000010,
	iioDisableRollback = 0x00000020,
	iioMustAccessInstallerKey = 0x00000040,    // Insist that we can access the installer key.
	iioReinstallModePackage = 0x00000080,
	iioClientEngine = 0x00000100,    // Used to indicate that we have a client engine.
	iioSimulateX86 = 0x00000200,    // ignore current execution platform and use X86, only valid w/ iioCreatingAdvertiseScript
	iioSimulateIA64 = 0x00000400,    // ignore current execution platform and use IA64, only valid w/ iioCreatingAdvertiseScript
	iioRestrictedEngine = 0x00000800,    // create a "safe" engine that is not subject to machine state and cannot modify machine state
	iioSimulateAMD64 = 0x00001000,     // ignore current execution platform and use AMD64, only valid w/iioCreateAdvertiseScript
} iioEnum;

typedef enum isrcEnum  // enumeration for sourcelist type to work with
{
	isrcNet = 0,
	isrcMedia = 1,
	isrcURL = 2,
} isrcEnum;

typedef enum icacCustomActionContext  // enumeration for types of custom action servers
{
	icacFirst = 0,
	icac32Impersonated = 0,
	icac64Impersonated = 1,
	icac32Elevated = 2,
	icac64Elevated = 3,
	icacNext = 4,
} icacCustomActionContext;

enum idtEnum  // using same values as Win32 API
{
	idtUnknown = 0,
	idtAllDrives = 1, // Input only!
	idtRemovable = 2, // DRIVE_REMOVABLE,
	idtFloppy = 2, // temporary until floppies and removables are distinguished
	idtFixed = 3, // DRIVE_FIXED,
	idtRemote = 4, // DRIVE_REMOTE,
	idtCDROM = 5, // DRIVE_CDROM,
	idtRAMDisk = 6, // DRIVE_RAMDISK,
	idtNextEnum,
};

enum ifaEnum
{
	ifaArchive,
	ifaDirectory,
	ifaHidden,
	ifaNormal,
	ifaReadOnly,
	ifaSystem,
	ifaTemp,
	ifaCompressed,
	ifaNextEnum,
};

enum ipcEnum
{
	ipcEqual,
	ipcChild,
	ipcParent,
	ipcNoRelation,
};

enum iclEnum
{
	iclExistNoFile,
	iclExistNoLang,
	iclExistSubset,
	iclExistEqual,
	iclExistIntersect,
	iclExistSuperset,
	iclExistNullSet,
	iclExistLangNeutral,
	iclNewLangNeutral,
	iclExistLangSetError,
	iclNewLangSetError,
	iclLangStringError,
};

enum icfhEnum
{
	icfhNoExistingFile,
	icfhMatch,
	icfhMismatch,
	icfhFileInUseError,
	icfhAccessDenied,
	icfhHashError,
	icfhNextEnum
};

enum icfvEnum
{
	icfvNoExistingFile,
	icfvExistingLower,
	icfvExistingEqual,
	icfvExistingHigher,
	icfvVersStringError,
	icfvFileInUseError,
	icfvAccessToFileDenied,
	icfvNextEnum
};

enum ifsEnum
{
	ifsAbsent = 0, // There is no currently installed file.
	ifsExistingLowerVersion = 1, // The currently installed file has a lower version.
	ifsExistingEqualVersion = 3, // The currently installed file has an equal version.
	ifsExistingNewerVersion = 5, // The currently installed file has a higher version.
	ifsExistingCorrupt = 7, // A checksum test on the currently installed file failed.
	ifsExistingAlwaysOverwrite = 9, // An InstallMode flag specified that the currently installed
	//  file should always be overwritten.
	ifsCompanionSyntax = 10, // This file is a companion file - the install state needs to
	//  determined by the state of it's companion parent.
	ifsCompanionExistsSyntax = 11, // This file is a companion file, and an installed version exists.
	ifsExistingFileInUse = 13, // Sharing violation prevent determination of version
	ifsAccessToFileDenied = 15, // Installer has insufficient privileges to access file
	ifsNextEnum
};

enum MsiDate {};  // an int containing DosTime and DosDate
enum scEnum // server context g_scServerContext
{
	scClient,
	scServer,
	scService,
	scCustomActionServer,
};