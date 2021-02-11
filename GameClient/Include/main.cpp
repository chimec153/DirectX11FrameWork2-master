#include "Engine.h"
#include "Client.h"
#include "Client/Client.h"
#include "Client/GameMode/StartGameMode.h"
#include "Scene/SceneManager.h"
#include "Client/BossManager.h"
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	if (!GET_SINGLE(CEngine)->Init(TEXT("TITAN SOULS"), TEXT("TITAN SOULS"), hInstance, 1280, 720))
	{
		DESTROY_SINGLE(CEngine);
		return 0;
	}

	FILE* fp;

	AllocConsole();
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);

	if (!GET_SINGLE(CClient)->GlobalSettings())
	{
		DESTROY_SINGLE(CEngine);
		return 0;
	}

	GET_SINGLE(CSceneManager)->SetGameMode<CStartGameMode>();

	int iRetVal = GET_SINGLE(CEngine)->Run();

	DESTROY_SINGLE(CBossManager);

	DESTROY_SINGLE(CEngine);

	DESTROY_SINGLE(CClient);

	return iRetVal;
}