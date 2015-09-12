#include <Windows.h>
#using <system.dll>
using namespace System;

public ref class Dispatcher
{
public:
	Dispatcher(String ^, Double);

	static Timers::Timer ^timer;

private:
	void createProc(String ^);
	void debugProc();
	void processAccessV(LPDEBUG_EVENT);
	static void terminateProc();
	static void timeoutEvent(Object ^, System::Timers::ElapsedEventArgs ^);

	static LPSTARTUPINFOA si;
	static LPPROCESS_INFORMATION pi;
};