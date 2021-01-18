#include "MainGameMode.h"
#include "../Object/Player.h"
#include "Scene/Scene.h"
#include "Scene/SceneResource.h"
#include "Resource/Texture.h"
#include "../Object/Minion.h"
#include "../Object/Bullet.h"
#include "../Object/IceBall.h"
#include "../UI/Button.h"
#include "Resource/ResourceManager.h"
#include "../Object/Effect.h"
#include "../UI/UIBar.h"
#include "SoundManager.h"
#include "../UI/CheckObj.h"
#include "../UI/RadioObj.h"
#include "UI/Slider.h"
#include "SoundManager.h"
#include "Scene/SceneManager.h"
#include "StartGameMode.h"
#include "UI/UIButton.h"
#include "Tile/TileMap.h"
#include "../UI/Missile.h"
#include "../../tinyxml.h"
#include "PathManager.h"
#include "Component/Sound.h"
#include "Camera/CameraManager.h"
#include "Component/Camera.h"
#include "Component/ColliderRect.h"
#include "ColossusMode.h"
#include "Component/SpriteComponent.h"
#include "Resource/Material.h"
#include "SoundManager.h"
#include "PathManager.h"
#include "Resource/ShaderManager.h"
#include "../../Custom.h"
#include "Resource/GraphicShader.h"

CMainGameMode::CMainGameMode()
{
	m_iGameModeClassType = (int)GAMEMODE_CLASS_TYPE::GCT_MAIN;
}

CMainGameMode::~CMainGameMode()
{
}

bool CMainGameMode::Init()
{
	static bool bInit = false;

	if (!bInit)
	{
		bInit = true;

		LoadResource();

		LoadSequance("data\\Sequances.csv");

		LoadFrame("data\\Frames.csv");
	}

	LoadXml("MAPS\\JAM\\JAM.tmx");

	CLayer* pLayer = m_pScene->FindLayer("Default");

	pLayer->Set2D();

	CPlayer* pPlayer = m_pScene->CreateObject<CPlayer>("player", pLayer);

	SetPlayer(pPlayer);

	pPlayer->SetWorldPos(1376.f + 8.f, 160.f*16.f - 1359.f + 8.f, 0.f);

	SAFE_RELEASE(pPlayer);

	CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

	pCam->SetWorldPos(1376.f + 8.f, 160.f * 16.f - 1359.f + 8.f, 0.f);

	SAFE_RELEASE(pCam);

	CLayer* pBack = m_pScene->FindLayer("Back");

	CBullet* pBullet = CScene::CreateProtoObj<CBullet>("Bullet", m_pScene, m_pScene->GetSceneType());

	SAFE_RELEASE(pBullet);

	CEffect* pEft = CScene::CreateProtoObj<CEffect>("Fire", m_pScene, m_pScene->GetSceneType());

	CSpriteComponent* pFire = pEft->CreateComponent<CSpriteComponent>("Fire");

	pFire->CreateSprite("Fire", "Fire", LOOP_OPTION::LOOP);
	pFire->SetPivot(0.5f, 0.5f, 0.f);
	pFire->SetWorldScale(16.f, 16.f, 1.f);

	pEft->SetRootComponent(pFire);

	SAFE_RELEASE(pFire);

	SAFE_RELEASE(pEft);

	CEffect* pChargeEffect = CScene::CreateProtoObj<CEffect>("Charge", m_pScene, m_pScene->GetSceneType());
	
	CSpriteComponent* pCharge = pChargeEffect->CreateComponent<CSpriteComponent>("Charge", m_pScene->FindLayer("Default"));

	pCharge->CreateSprite("Charge", "Charge", LOOP_OPTION::ONCE_DESTROY);

	pCharge->SetPivot(0.5f, 0.5f, 0.f);
	pCharge->SetRelativeScale(16.f, 16.f, 1.f);
	pCharge->AddNotify("Charge", "FireEnd", 1.f);

	pChargeEffect->SetRootComponent(pCharge);
	pChargeEffect->SetSpeed(0.f);

	SAFE_RELEASE(pCharge);

	SAFE_RELEASE(pChargeEffect);

	CEffect* pWaterEffect = CScene::CreateProtoObj<CEffect>("Water", m_pScene, m_pScene->GetSceneType());

	CSpriteComponent* pWater = pWaterEffect->CreateComponent<CSpriteComponent>("Water");

	pWater->CreateSprite("Water", "Water", LOOP_OPTION::LOOP);

	pWater->SetPivot(0.5f, 0.5f, 0.f);
	pWater->SetRelativeScale(32.f, 16.f, 1.f);
	pWater->AddNotify("Water", "Water", 1.f);
	pWater->AddCallBack<CEffect>("Water", "Water", pWaterEffect, &CEffect::Destroy);
	pWater->AddRenderState("AlphaBlend");

	CMaterial* pMtrl = pWater->GetMaterial();

	pMtrl->SetDiffuseColor(1.f, 1.f, 1.f, 0.3f);

	SAFE_RELEASE(pMtrl);

	pWaterEffect->SetRootComponent(pWater);
	pWaterEffect->SetSpeed(0.f);
	pWaterEffect->SetScaleSpeed(Vector3(16.f, 8.f, 0.f));

	SAFE_RELEASE(pWater);

	SAFE_RELEASE(pWaterEffect);

	CMissile* pMissile = CScene::CreateProtoObj<CMissile>("Missile", m_pScene, m_pScene->GetSceneType());

	SAFE_RELEASE(pMissile);

	CreateUI();

	CEffect* pBossEffect = m_pScene->CreateObject<CEffect>("BossEffect", 
		m_pScene->FindLayer("Light"), m_pScene->GetSceneType());

	CSpriteComponent* pBossCom = pBossEffect->CreateComponent<CSpriteComponent>("BossEffect", 
		m_pScene->FindLayer("Light"));

	pBossCom->SetWorldScale(48.f, 48.f, 0.f);
	pBossCom->SetPivot(0.5f, 0.5f, 0.f);

	pBossCom->CreateSprite("Effect2", "BossEffect2", LOOP_OPTION::LOOP);

	pBossCom->AddNotify("Effect2", "End", 3.f);

	pBossCom->AddCallBack<CEffect>("Effect2", "End", pBossEffect, &CEffect::Destroy);

	pBossCom->AddRenderState("AlphaBlend");

	pBossEffect->SetRootComponent(pBossCom);

	SAFE_RELEASE(pBossCom);

	pBossEffect->SetWorldPos(1384.f, 2560.f - 1351.f, 0.f);
	pBossEffect->SetSpeed(10.f);
	pBossEffect->SetAlphaSpeed(0.3333f);

	SAFE_RELEASE(pBossEffect);

	return true;
}

void CMainGameMode::LoadResource()
{
	if (!GET_SINGLE(CResourceManager)->LoadTexture("Pro", TEXT("player.png")))
		return;

	GET_SINGLE(CSoundManager)->LoadSound("BtnPress", SOUND_TYPE::EFFECT, "UI\\Select.mp3");

	GET_SINGLE(CResourceManager)->LoadTexture("Bow", TEXT("Bow.png"));

	GET_SINGLE(CResourceManager)->LoadTexture("Arrow", TEXT("Arrow.png"));

	GET_SINGLE(CResourceManager)->LoadTexture("Effect", TEXT("Particles.png"));

	GET_SINGLE(CResourceManager)->LoadTexture("UnderWorld", TEXT("Underworld.png"));
	GET_SINGLE(CResourceManager)->LoadTexture("Overworld", TEXT("Overworld.png"));

	GET_SINGLE(CResourceManager)->LoadTexture("colossus", TEXT("BOSS\\COLOSSUS\\colossus.png"));

	GET_SINGLE(CPathManager)->AddPath("Player_Sound_Path", L"Data\\SOUND\\PLAYER\\");
	GET_SINGLE(CPathManager)->AddPath("Sound_Path2", L"Data\\SOUND\\");

	GET_SINGLE(CSoundManager)->LoadSound("arrow_retrieval", SOUND_TYPE::EFFECT,
		"arrow_retrieval.ogg", "Player_Sound_Path");

	GET_SINGLE(CSoundManager)->LoadSound("arrow_shoot", SOUND_TYPE::EFFECT,
		"arrow_shoot.ogg", "Player_Sound_Path");

	GET_SINGLE(CSoundManager)->LoadSound("ArrowImpact1", SOUND_TYPE::EFFECT,
		"ArrowImpact1.ogg", "Player_Sound_Path");

	GET_SINGLE(CSoundManager)->LoadSound("Pickuparrow", SOUND_TYPE::EFFECT,
		"Pickuparrow.ogg", "Player_Sound_Path");

	GET_SINGLE(CSoundManager)->LoadSound("Roll", SOUND_TYPE::EFFECT,
		"Roll.ogg", "Player_Sound_Path");

	GET_SINGLE(CSoundManager)->LoadSound("Roll_bash", SOUND_TYPE::EFFECT,
		"Roll_bash.ogg", "Player_Sound_Path");

	GET_SINGLE(CSoundManager)->LoadSound("Roar", SOUND_TYPE::EFFECT,
		"SFX\\BOSS\\COLOSSUS\\Roar.ogg", "Sound_Path2");

	CTexture* pTex = GET_SINGLE(CResourceManager)->FindTexture("Pro");
	CTexture* pBowTex = GET_SINGLE(CResourceManager)->FindTexture("Bow");
	CTexture* pArrowTex = GET_SINGLE(CResourceManager)->FindTexture("Arrow");
	CTexture* pEftTex = GET_SINGLE(CResourceManager)->FindTexture("Effect");

	CResourceManager* pRes = GET_SINGLE(CResourceManager);

	pRes->CreateAni2DSeq("Arrow", pTex);

	for (int i = 0; i < 1; ++i)
		pRes->AddFrame("Arrow", Vector2(16.f * 31.f, 0.f), Vector2(16.f * 32.f, 16.f));

	GET_SINGLE(CResourceManager)->CreateAni2DSeq("Fire", pEftTex);

	for (int i = 0; i < 9; ++i)
		GET_SINGLE(CResourceManager)->AddFrame("Fire", Vector2(i * 16.f, 176.f), Vector2((i+1) * 16.f, 191.f));

	GET_SINGLE(CResourceManager)->AddNotify("Fire", "FireEnd", 8);

	SAFE_RELEASE(pTex);
	SAFE_RELEASE(pBowTex);
	SAFE_RELEASE(pArrowTex);
	SAFE_RELEASE(pEftTex);

	pRes->LoadTexture("Yeti", TEXT("Yeti.png"));

	CTexture* pYetiTex = pRes->FindTexture("Yeti");

	pRes->CreateAni2DSeq("YetiIdleR", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiIdleR", Vector2(96.f * i, 0.f), Vector2(96.f * (1 + i), 96.f));

	pRes->CreateAni2DSeq("YetiAttackR", pYetiTex, 0.2f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiAttackR", Vector2(96.f * (i + 3), 0.f), Vector2(96.f * (4 + i), 96.f));

	pRes->CreateAni2DSeq("YetiRollR", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiRollR", Vector2(96.f * (6.f + i), 0.f), Vector2(96.f * (7.f + i), 96.f));


	pRes->CreateAni2DSeq("YetiIdleU", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiIdleU", Vector2(96.f * i, 96.f), Vector2(96.f * (1 + i), 96.f * 2.f));

	pRes->CreateAni2DSeq("YetiAttackU", pYetiTex, 0.2f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiAttackU", Vector2(96.f * (i + 3), 96.f), Vector2(96.f * (4 + i), 96.f * 2.f));

	pRes->CreateAni2DSeq("YetiRollU", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiRollU", Vector2(96.f * (6.f + i), 96.f), Vector2(96.f * (7.f + i), 96.f * 2.f));


	pRes->CreateAni2DSeq("YetiIdleD", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiIdleD", Vector2(96.f * i, 96.f * 2.f), Vector2(96.f * (1 + i), 96.f * 3.f));

	pRes->CreateAni2DSeq("YetiAttackD", pYetiTex, 0.2f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiAttackD", Vector2(96.f * (i + 3), 96.f * 2.f), Vector2(96.f * (4 + i), 96.f * 3.f));

	pRes->CreateAni2DSeq("YetiRollD", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiRollD", Vector2(96.f * (6.f + i), 96.f * 2.f), Vector2(96.f * (7.f + i), 96.f * 3.f));


	pRes->CreateAni2DSeq("YetiIdleRU", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiIdleRU", Vector2(96.f * i, 96.f * 3.f), Vector2(96.f * (1 + i), 96.f * 4.f));

	pRes->CreateAni2DSeq("YetiAttackRU", pYetiTex, 0.2f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiAttackRU", Vector2(96.f * (i + 3), 96.f * 3.f), Vector2(96.f * (4 + i), 96.f * 4.f));

	pRes->CreateAni2DSeq("YetiRollRU", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiRollRU", Vector2(96.f * (6.f + i), 96.f * 3.f), Vector2(96.f * (7.f + i), 96.f * 4.f));


	pRes->CreateAni2DSeq("YetiIdleRD", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiIdleRD", Vector2(96.f * i, 96.f * 4.f), Vector2(96.f * (1 + i), 96.f * 5.f));

	pRes->CreateAni2DSeq("YetiAttackRD", pYetiTex, 0.2f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiAttackRD", Vector2(96.f * (i + 3), 96.f * 4.f), Vector2(96.f * (4 + i), 96.f * 5.f));

	pRes->CreateAni2DSeq("YetiRollRD", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiRollRD", Vector2(96.f * (6.f + i), 96.f * 4.f), Vector2(96.f * (7.f + i), 96.f * 5.f));


	pRes->CreateAni2DSeq("YetiDieStandR", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiDieStandR", Vector2(96.f * i, 96.f * 5.f), Vector2(96.f * (1 + i), 96.f * 6.f));

	pRes->CreateAni2DSeq("YetiDieStandU", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiDieStandU", Vector2(96.f * i, 96.f * 6.f), Vector2(96.f * (1 + i), 96.f * 7.f));

	pRes->CreateAni2DSeq("YetiDieStandD", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiDieStandD", Vector2(96.f * i, 96.f * 7.f), Vector2(96.f * (1 + i), 96.f * 8.f));

	pRes->CreateAni2DSeq("YetiDieStandRU", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiDieStandRU", Vector2(96.f * i, 96.f * 8.f), Vector2(96.f * (1 + i), 96.f * 9.f));

	pRes->CreateAni2DSeq("YetiDieStandRD", pYetiTex, 0.3f);
	for (int i = 0; i < 3; ++i)
		pRes->AddFrame("YetiDieStandRD", Vector2(96.f * i, 96.f * 9.f), Vector2(96.f * (1 + i), 96.f * 10.f));


	pRes->CreateAni2DSeq("YetiDieR", pYetiTex, 0.3f);
	for (int i = 4; i < 7; ++i)
		pRes->AddFrame("YetiDieR", Vector2(96.f * i, 96.f * 5.f), Vector2(96.f * (1 + i), 96.f * 6.f));

	pRes->CreateAni2DSeq("YetiDieU", pYetiTex, 0.3f);
	for (int i = 4; i < 7; ++i)
		pRes->AddFrame("YetiDieU", Vector2(96.f * i, 96.f * 6.f), Vector2(96.f * (1 + i), 96.f * 7.f));

	pRes->CreateAni2DSeq("YetiDieD", pYetiTex, 0.3f);
	for (int i = 3; i < 7; ++i)
		pRes->AddFrame("YetiDieD", Vector2(96.f * i, 96.f * 7.f), Vector2(96.f * (1 + i), 96.f * 8.f));

	pRes->CreateAni2DSeq("YetiDieRU", pYetiTex, 0.3f);
	for (int i = 4; i < 7; ++i)
		pRes->AddFrame("YetiDieRU", Vector2(96.f * i, 96.f * 8.f), Vector2(96.f * (1 + i), 96.f * 9.f));

	pRes->CreateAni2DSeq("YetiDieRD", pYetiTex, 0.3f);
	for (int i = 4; i < 7; ++i)
		pRes->AddFrame("YetiDieRD", Vector2(96.f * i, 96.f * 9.f), Vector2(96.f * (1 + i), 96.f * 10.f));


	pRes->CreateAni2DSeq("IceBall", pYetiTex, 2.f);

	pRes->AddFrame("IceBall", Vector2(32.f, 1024.f - 32.f), Vector2(64.f, 1024.f));

	pRes->CreateAni2DSeq("IceBallShadow", pYetiTex, 2.f);

	pRes->AddFrame("IceBallShadow", Vector2(0.f, 1024.f - 32.f), Vector2(32.f, 1024.f));

	pRes->CreateAni2DSeq("YetiShadow", pYetiTex, 2.f);

	pRes->AddFrame("YetiShadow", Vector2(0.f, 960.f), Vector2(80.f, 992.f));

	SAFE_RELEASE(pYetiTex);

	GET_SINGLE(CSoundManager)->LoadSound("Slam", SOUND_TYPE::EFFECT, "Eye Cube\\Slam1.mp3");
}

void CMainGameMode::StartScene(float)
{
	GET_SINGLE(CSceneManager)->CreateNextScene();

	GET_SINGLE(CSceneManager)->SetGameMode<CStartGameMode>(false);
}

void CMainGameMode::CreateUI()
{
	CLayer* pUI = m_pScene->FindLayer("UI");

	pUI->SetUI();

	CClientButton* pBtn = m_pScene->CreateObject<CClientButton>("Button", pUI);

	CUIButton* pButton = (CUIButton*)pBtn->FindSceneComponent("Button");

	pButton->SetCallBack<CMainGameMode>(BUTTON_STATE::PUSH, this, &CMainGameMode::StartScene);

	pBtn->Enable(false);

	SAFE_RELEASE(pButton);

	SAFE_RELEASE(pBtn);
	
	/*

	CUIBar* pBar = m_pScene->CreateObject<CUIBar>("Bar", pUI);

	pBar->SetWorldPos(200.f, 200.f, 0.f);

	SAFE_RELEASE(pBar);

	CCheckObj* pChk = m_pScene->CreateObject<CCheckObj>("CheckBox", pUI);

	SAFE_RELEASE(pChk);

	CRadioObj* pRadio = m_pScene->CreateObject<CRadioObj>("Radio", pUI);

	SAFE_RELEASE(pRadio);*/
}

bool CMainGameMode::LoadXml(const char* pFileName, const std::string& strPathKey)
{
	return CTileMode::LoadXml(pFileName, strPathKey);
}

void CMainGameMode::searchXMLData(TiXmlElement* pElem)
{
	CTileMode::searchXMLData(pElem);
}

bool CMainGameMode::LoadSequance(const char* pFileName, const std::string& strPathKey)
{
	char strFullPath[MAX_PATH] = {};

	const char* pPath = GET_SINGLE(CPathManager)->FindMultibytePath(strPathKey);

	if (pPath)
		strcpy_s(strFullPath, pPath);

	strcat_s(strFullPath, pFileName);

	FILE* pFile = nullptr;

	fopen_s(&pFile, strFullPath, "rt");

	if (pFile)
	{
		char cLine[1024] = {};

		fgets(cLine, 1024, pFile);

		while (true)
		{
			char cLine[1024] = {};

			fgets(cLine, 1024, pFile);

			if (strcmp(cLine, "") == 0)
				break;

			char* pContext = nullptr;

			char* pResult = strtok_s(cLine, ", ", &pContext);

			char* pTex = strtok_s(nullptr, ", ", &pContext);

			char* pTime = strtok_s(nullptr, ", ", &pContext);

			float fMaxTime = (float)atof(pTime);

			float fRate = (float)atof(pContext);

			GET_SINGLE(CResourceManager)->CreateAni2DSeq(pResult, pTex, fMaxTime, fRate);
		}

		fclose(pFile);
	}

	return true;
}

bool CMainGameMode::LoadFrame(const char* pFileName, const std::string& strPathKey)
{
	char strFullPath[MAX_PATH] = {};

	const char* pPath = GET_SINGLE(CPathManager)->FindMultibytePath(strPathKey);

	if (pPath)
		strcpy_s(strFullPath, pPath);

	strcat_s(strFullPath, pFileName);

	FILE* pFile = nullptr;

	fopen_s(&pFile, strFullPath, "rt");

	if (pFile)
	{
		char cLine[1024] = {};

		fgets(cLine, 1024, pFile);

		while (true)
		{
			char cLine[1024] = {};

			fgets(cLine, 1024, pFile);

			if (strcmp(cLine, "") == 0)
				break;

			char* pContext = nullptr;

			char* pResult = strtok_s(cLine, ", ", &pContext);

			char* pStartX = strtok_s(nullptr, ", ", &pContext);

			char* pStartY = strtok_s(nullptr, ", ", &pContext);

			char* pEndX = strtok_s(nullptr, ", ", &pContext);

			char* pEndY = strtok_s(nullptr, ", ", &pContext);

			Vector2 vStart = Vector2((float)atof(pStartX), (float)atof(pStartY));
			Vector2 vEnd = Vector2((float)atof(pEndX), (float)atof(pEndY));

			GET_SINGLE(CResourceManager)->AddFrame(pResult, vStart, vEnd);
		}

		fclose(pFile);
	}

	return true;
}

