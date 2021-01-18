#include "ColossusMode.h"
#include "PathManager.h"
#include "../../tinyxml.h"
#include "Scene/Scene.h"
#include "Tile/TileMap.h"
#include "Resource/ResourceManager.h"
#include "../Object/Player.h"
#include "../Object/Colossus.h"
#include "SoundManager.h"
#include "Component/Sound.h"
#include "Camera/CameraManager.h"
#include "Component/Camera.h"
#include "../Object/Bullet.h"
#include "../Object/Effect.h"
#include "Component/SpriteComponent.h"

CColossusMode::CColossusMode()
{
}

CColossusMode::~CColossusMode()
{
}

bool CColossusMode::Init()
{
	LoadResource();

	LoadXml("MAPS\\JAM\\colossus.tmx");

	m_pScene->SortInstText();

	CLayer* pLayer = m_pScene->FindLayer("Default");

	CPlayer* pPlayer = m_pScene->CreateObject<CPlayer>("player", pLayer);

	pPlayer->SetWorldPos(488.f, 500.f, 0.f);

	SetPlayer(pPlayer);

	SAFE_RELEASE(pPlayer);

	CColossus* pColossus = m_pScene->CreateObject<CColossus>("Colossus", pLayer);

	pColossus->SetWorldPos(488.f, 640.f, 0.f);

	SAFE_RELEASE(pColossus);

	CBullet* pBullet = CScene::CreateProtoObj<CBullet>("Bullet", m_pScene, m_pScene->GetSceneType());

	SAFE_RELEASE(pBullet);

	CEffect* pChargeEffect = CScene::CreateProtoObj<CEffect>("Charge", m_pScene, m_pScene->GetSceneType());

	CSpriteComponent* pCharge = pChargeEffect->CreateComponent<CSpriteComponent>("Charge");

	pCharge->CreateSprite("Charge", "Charge", LOOP_OPTION::ONCE_DESTROY);

	pCharge->SetInheritScale(false);
	pCharge->SetPivot(0.5f, 0.5f, 0.f);
	pCharge->SetWorldScale(16.f, 16.f, 1.f);
	pCharge->AddNotify("Charge", "FireEnd", 1.f);

	pChargeEffect->SetRootComponent(pCharge);
	pChargeEffect->SetSpeed(0.f);

	SAFE_RELEASE(pCharge);

	SAFE_RELEASE(pChargeEffect);

	return true;
}

bool CColossusMode::LoadXml(const char* pFileName, const std::string& strPathKey)
{
	return CTileMode::LoadXml(pFileName, strPathKey);
}

void CColossusMode::searchXMLData(TiXmlElement* pElem)
{
	CTileMode::searchXMLData(pElem);
}

void CColossusMode::LoadResource()
{

	GET_SINGLE(CResourceManager)->CreateAni2DSeq("ColossusHeadIdle", "colossus");

	GET_SINGLE(CResourceManager)->AddFrame("ColossusHeadIdle", Vector2(0.f, 0.f), Vector2(64.f, 64.f));

	GET_SINGLE(CResourceManager)->CreateAni2DSeq("ColossusBodyIdle", "colossus");

	GET_SINGLE(CResourceManager)->AddFrame("ColossusBodyIdle", Vector2(0.f, 112.f), Vector2(96.f, 240.f));

	GET_SINGLE(CResourceManager)->CreateAni2DSeq("ColossusLeftHandIdle", "colossus");

	GET_SINGLE(CResourceManager)->AddFrame("ColossusLeftHandIdle", Vector2(407.f, 214.f), Vector2(407.f, 214.f)+ Vector2(48.f, 48.f));
}
