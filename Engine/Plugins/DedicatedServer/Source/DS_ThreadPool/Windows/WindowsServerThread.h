#pragma once

#include "CoreTypes.h"


struct DS_THREADPOOL_API FWindowsServerThread
{
	static void Show();
	static void Hide();
	static bool IsShown();

	static FSimpleDelegate CompletedDelegate;
	static FSimpleDelegate RunDelegate;
};