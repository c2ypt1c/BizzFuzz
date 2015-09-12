#include <Windows.h>
#include <msclr\marshal_cppstd.h>

#include "Dispatcher.h"

Dispatcher::Dispatcher(String ^cmdline, Double timeout)
{
	si = new STARTUPINFOA();
	pi = new PROCESS_INFORMATION();

	timer = gcnew Timers::Timer(timeout * 1000);
	timer->Elapsed += gcnew System::Timers::ElapsedEventHandler(timeoutEvent);
	timer->Start();

	createProc(cmdline);
	debugProc();
}

void Dispatcher::createProc(String ^cmdline)
{
	// convert cmdline to unmanaged string
	std::string strCmdLine = msclr::interop::marshal_as<std::string>(cmdline);
	const char *chCmdLine = strCmdLine.c_str();

	BOOL bSuccess = CreateProcessA(NULL, LPSTR(chCmdLine), NULL, NULL, FALSE, DEBUG_PROCESS, NULL, NULL, si, pi);
	if (!bSuccess)
	{
		System::Console::WriteLine("Process could not be created...");
		return;
	}

	System::Console::WriteLine("Process created with PID: {0}", pi->dwProcessId);
}

void Dispatcher::debugProc()
{
	DebugActiveProcess(pi->dwProcessId);
	System::Console::WriteLine("Debugging Process {0}", pi->dwProcessId);

	LPDEBUG_EVENT dbgEvent = new DEBUG_EVENT();

	for (;;)
	{
		WaitForDebugEvent(dbgEvent, INFINITE);

		// exit process
		if (dbgEvent->dwDebugEventCode == EXIT_PROCESS_DEBUG_EVENT)
			return;

		// access violation
		if (dbgEvent->u.Exception.ExceptionRecord.ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
			processAccessV(dbgEvent);

		// all other events
		ContinueDebugEvent(dbgEvent->dwProcessId, dbgEvent->dwThreadId, DBG_CONTINUE);
	}
}

void Dispatcher::processAccessV(LPDEBUG_EVENT dbgEvent)
{
	System::Console::WriteLine("ACCESS VIOLATION!!");
	terminateProc();
}

void Dispatcher::timeoutEvent(Object ^source, System::Timers::ElapsedEventArgs ^e)
{
	terminateProc();
}

void Dispatcher::terminateProc()
{
	System::Console::WriteLine("Terminating process...");
	timer->Stop();
	TerminateProcess(pi->hProcess, 0);
}