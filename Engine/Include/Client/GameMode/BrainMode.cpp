#include "pch.h"
#include "BrainMode.h"
#include "../Object/Fire.h"
#include "../Object/BrainFreeze.h"
#include "../Object/FireButton.h"
#include "Scene/Scene.h"
#include "../BossManager.h"

bool CBrainMode::m_bClear = false;

CBrainMode::CBrainMode()	:
	m_pFire(nullptr)
{
}

CBrainMode::~CBrainMode()
{
	SAFE_RELEASE(m_pFire);
}

CFire* CBrainMode::GetFire() const
{
	if (m_pFire)
		m_pFire->AddRef();

	return m_pFire;
}

bool CBrainMode::Init()
{
	CFireButton* pButton = CScene::CreateProtoObj<CFireButton>("4292", m_pScene, m_pScene->GetSceneType());

	SAFE_RELEASE(pButton);

	m_pFire = m_pScene->CreateObject<CFire>("Fire", m_pScene->FindLayer("Default"), m_pScene->GetSceneType());

	m_pFire->Enable(false);

	if (!CTileMode::Init())
		return false;

	if (m_bClear)
	{
		CObj* pObj = m_pScene->FindLayer("Default")->FindObj("boss_brainfreeze");

		if (pObj)
		{
			pObj->Destroy();

			pObj->Release();
		}

		pObj = GET_SINGLE(CBossManager)->FindMonster("boss_brainfreeze");

		m_pScene->FindLayer("Default")->AddObj(pObj);

		SAFE_RELEASE(pObj);
	}

	m_pPlayer->SetWorldPos(464.f, 384.f, 0.f);

	return true;
}
