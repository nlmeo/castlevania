#include <windows.h>
#include "MyConsole.h"

#define  CLOSE_STREAM(s) if(s!=nullptr){fclose(s);s=nullptr;}

namespace HelpTool
{
	void MyConsole::ShowConsole()
	{
#ifdef _DEBUG
		FreeConsole();//
		AllocConsole();//分配控制台
		SetConsoleCtrlHandler(nullptr, true);
		freopen_s(&stream_in, "CONIN$", "r", stdin);
		freopen_s(&stream_out, "CONOUT$", "w", stdout);
#endif
	}

	void MyConsole::FreeConsole()
	{
#ifdef _DEBUG
		CLOSE_STREAM(stream_in);
		CLOSE_STREAM(stream_out);
#endif
	}
}