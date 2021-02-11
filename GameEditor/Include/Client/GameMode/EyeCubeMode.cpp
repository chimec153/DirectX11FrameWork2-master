#include "pch.h"
#include "EyeCubeMode.h"
#include "../Object/Eyecube.h"
#include "Scene/Scene.h"
#include "../BossManager.h"

bool CEyeCubeMode::m_bClear = false;

CEyeCubeMode::CEyeCubeMode()
{
}

CEyeCubeMode::~CEyeCubeMode()
{
}

bool CEyeCubeMode::Init()
{
	if (!CTileMode::Init())
		return false;

	if (m_bClear)
	{
		CObj* pObj = m_pScene->FindLayer("Default")->FindObj("boss_eyecube");

		if (pObj)
		{
			pObj->Destroy();

			pObj->Release();
		}

		pObj = GET_SINGLE(CBossManager)->FindMonster("boss_eyecube");

		m_pScene->FindLayer("Default")->AddObj(pObj);

		SAFE_RELEASE(pObj);
	}

	m_pPlayer->SetWorldPos(432.f, 384.f, 0.f);

	return true;
}
