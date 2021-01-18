#pragma once

#include "GameEngine.h"

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "GameEngine_Debug.lib")
#else
#pragma comment(lib, "GameEngine.lib")
#endif

#else

#ifdef _DEBUG
#pragma comment(lib, "GameEngine32_Debug.lib")
#else
#pragma comment(lib, "GameEngine32.lib")
#endif

#endif

typedef struct _tagTreeItem
{
	HTREEITEM	hItem;
	CString		tTag;
}TreeItem, *PTreeItem;