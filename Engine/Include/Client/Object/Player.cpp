#include "Player.h"
#include "Component/Mesh2DComponent.h"
#include "Resource/Material.h"
#include "Bullet.h"
#include "Scene/Scene.h"
#include "Input.h"
#include "Component/Camera.h"
#include "Camera/CameraManager.h"
#include "InputObj.h"
#include "Component/SpriteComponent.h"
#include "Component/ColliderRect.h"
#include "Bow.h"
#include "Arrow.h"
#include "Component/ColliderOBB2D.h"
#include "Component/ColliderLine.h"
#include "Component/ColliderCircle.h"
#include "../UI/Missile.h"
#include "Scene/SceneManager.h"
#include "UI/UIFont.h"
#include "UI/Bar.h"
#include "UI/UIImage.h"
#include "Tile/TileMap.h"
#include "../GameMode/ColossusMode.h"
#include "Tile/Tile.h"
#include "Effect.h"
#include "Resource/ResourceManager.h"
#include "Component/Sound.h"
#include "../GameMode/TileMode.h"
#include "../GameMode/MainGameMode.h"
#include "Resource/ShaderManager.h"
#include "Types.h"
#include "Device.h"
#include "Colossus.h"

CPlayer::CPlayer() :
	m_pMesh(nullptr),
	m_fSpeed(100.f),
	m_bIdleEnable(true),
	m_fScaleX(0.f),
	m_fScaleY(0.f),
	m_eDir(DIR_8::D),
	m_bCharge(true),
	m_bRolling(false),
	m_pBow(nullptr),
	m_pArrow(nullptr),
	m_pBullet(nullptr),
	m_fBowAngle(0.f),
	m_pTileMap(nullptr),
	m_pCoord(nullptr),
	m_pShadow(nullptr),
	m_pRC(nullptr),
	m_eState(STATE::IDLE),
	m_fPushTime(0.f),
	m_fRollTime(0.f),
	m_fPrevSpeed(0.f),
	m_pSnd(nullptr),
	m_pMtrlMap(nullptr),
	m_fBackTime(0.f),
	m_bBack(false),
	m_fBackLimit(0.5f),
	m_pAirMap(nullptr),
	m_bWalkIn(false),
	m_fWalkTime(1.5f),
	m_pFade(nullptr),
	m_fDieDelay(2.5f)
{
	m_iObjClassType = (int)OBJ_CLASS_TYPE::OCT_PLAYER;
}

CPlayer::CPlayer(const CPlayer& player) :
	CObj(player)
{
	*this = player;
}

CPlayer::~CPlayer()
{
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pBullet);
	SAFE_RELEASE(m_pBow);
	SAFE_RELEASE(m_pArrow);
	SAFE_RELEASE(m_pTileMap);
	SAFE_RELEASE(m_pCoord);
	SAFE_RELEASE(m_pShadow);
	SAFE_RELEASE(m_pRC);
	SAFE_RELEASE(m_pSnd);
	SAFE_RELEASE(m_pMtrlMap);
	SAFE_RELEASE(m_pAirMap);
	SAFE_RELEASE(m_pFade);
}

void CPlayer::SetMeshComponent(CSpriteComponent* pMesh)
{
	SAFE_RELEASE(m_pMesh);

	m_pMesh = pMesh;

	if (m_pMesh)
		m_pMesh->AddRef();
}

void CPlayer::Notify(const std::string& strTag)
{
	if (strTag == "Shot")
	{
		Fire1(1.f, 0.f);
	}

	else if (strTag == "Water")
	{
	}
}

void CPlayer::Charge()
{
	m_pArrow->Enable(true);
	m_bIdleEnable = true;
	m_bCharge = true;
#ifdef _DEBUG
	OutputDebugStringA("Charge");
#endif
}

bool CPlayer::Init()
{
	m_pMesh = CreateComponent<CSpriteComponent>("Mesh", m_pLayer);

	m_pRC = CreateComponent<CColliderRect>("PlayerBody", m_pLayer);

	m_pRC->SetInheritScale(false);
	m_pRC->SetWorldScale(16.f, 16.f, 1.f);
	m_pRC->SetExtent(10.f, 14.f);
	m_pRC->SetProfile("Player");

	m_pRC->SetCallBack<CPlayer>(COLLISION_STATE::INIT, this, &CPlayer::ColStart);
	m_pRC->SetCallBack<CPlayer>(COLLISION_STATE::STAY, this, &CPlayer::ColStay);
	m_pRC->SetCallBack<CPlayer>(COLLISION_STATE::END, this, &CPlayer::ColEnd);

	m_pMesh->AddChild(m_pRC);

	m_pMesh->AddRenderState("Character");
	m_pMesh->AddRenderState("AlphaBlend");

	m_pShadow = CreateComponent<CSpriteComponent>("Shadow", m_pLayer);

	CMaterial* pMat = m_pMesh->GetMaterial();

	pMat->SetGray(false);

	SAFE_RELEASE(pMat);

	CMaterial* pMtrl = m_pShadow->GetMaterial();

	pMtrl->SetShader("Shadow");
	pMtrl->SetDiffuseColor(0.f, 0.f, 0.f, 0.5f);

	SAFE_RELEASE(pMtrl);

	m_pShadow->AddRenderState("Silhouette");
	m_pShadow->AddRenderState("AlphaBlend");
	m_pShadow->SetInheritScale(false);
	m_pShadow->SetRelativeScale(16.f, 16.f, 1.f);
	m_pShadow->SetPivot(0.5f, 0.5f, 0.f);

	m_pShadow->CreateSprite("IdleU", "ProIdleU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("IdleRU", "ProIdleRU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("IdleR", "ProIdleR", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("IdleRD", "ProIdleRD", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("IdleD", "ProIdleD", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("WalkU", "ProWalkU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("WalkRU", "ProWalkRU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("WalkR", "ProWalkR", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("WalkRD", "ProWalkRD", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("WalkD", "ProWalkD", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("AttackU", "ProAttackU", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("AttackRU", "ProAttackRU", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("AttackR", "ProAttackR", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("AttackRD", "ProAttackRD", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("AttackD", "ProAttackD", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("CallU", "ProCallU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("CallRU", "ProCallRU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("CallR", "ProCallR", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("CallRD", "ProCallRD", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("CallD", "ProCallD", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("RollU", "ProRollU", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("RollRU", "ProRollRU", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("RollR", "ProRollR", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("RollRD", "ProRollRD", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("RollD", "ProRollD", LOOP_OPTION::ONCE_RETURN);
	m_pShadow->CreateSprite("SwimU", "ProSwimU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("SwimRU", "ProSwimRU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("SwimR", "ProSwimR", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("SwimRD", "ProSwimRD", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("SwimD", "ProSwimD", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("DieU",	"ProDieU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("DieRU",	"ProDieRU", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("DieR",	"ProDieR", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("DieRD",	"ProDieRD", LOOP_OPTION::LOOP);
	m_pShadow->CreateSprite("DieD",	"ProDieD", LOOP_OPTION::LOOP);
	
	m_pMesh->AddChild(m_pShadow);

	m_pMesh->CreateSprite("IdleU", "ProIdleU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("IdleRU", "ProIdleRU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("IdleR", "ProIdleR", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("IdleRD", "ProIdleRD", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("IdleD", "ProIdleD", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("WalkU", "ProWalkU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("WalkRU", "ProWalkRU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("WalkR", "ProWalkR", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("WalkRD", "ProWalkRD", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("WalkD", "ProWalkD", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("AttackU", "ProAttackU", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("AttackRU", "ProAttackRU", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("AttackR", "ProAttackR", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("AttackRD", "ProAttackRD", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("AttackD", "ProAttackD", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("CallU", "ProCallU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("CallRU", "ProCallRU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("CallR", "ProCallR", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("CallRD", "ProCallRD", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("CallD", "ProCallD", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("RollU", "ProRollU", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("RollRU", "ProRollRU", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("RollR", "ProRollR", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("RollRD", "ProRollRD", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("RollD", "ProRollD", LOOP_OPTION::ONCE_RETURN);
	m_pMesh->CreateSprite("SwimU", "ProSwimU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("SwimRU", "ProSwimRU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("SwimR", "ProSwimR", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("SwimRD", "ProSwimRD", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("SwimD", "ProSwimD", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("DieU",	"ProDieU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("DieRU",	"ProDieRU", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("DieR",	"ProDieR", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("DieRD",	"ProDieRD", LOOP_OPTION::LOOP);
	m_pMesh->CreateSprite("DieD",	"ProDieD", LOOP_OPTION::LOOP);

	m_pMesh->AddNotify("SwimU", "Water", 2);
	m_pMesh->AddNotify("SwimD", "Water", 2);
	m_pMesh->AddNotify("SwimR", "Water", 2);
	m_pMesh->AddNotify("SwimRU", "Water", 2);
	m_pMesh->AddNotify("SwimRD", "Water", 2);

	m_pMesh->AddNotify("IdleU", "Water", 0.5f);
	m_pMesh->AddNotify("IdleD", "Water", 0.5f);
	m_pMesh->AddNotify("IdleR", "Water", 0.5f);
	m_pMesh->AddNotify("IdleRU", "Water", 0.5f);
	m_pMesh->AddNotify("IdleRD", "Water", 0.5f);

	m_pMesh->AddNotify("WalkU", "Water", 2);
	m_pMesh->AddNotify("WalkD", "Water", 2);
	m_pMesh->AddNotify("WalkR", "Water", 2);
	m_pMesh->AddNotify("WalkRU", "Water", 2);
	m_pMesh->AddNotify("WalkRD", "Water", 2);

	m_pMesh->AddNotify("DieU",	"Die", 0.5f);
	m_pMesh->AddNotify("DieD",	"Die", 0.5f);
	m_pMesh->AddNotify("DieR",	"Die", 0.5f);
	m_pMesh->AddNotify("DieRU", "Die", 0.5f);
	m_pMesh->AddNotify("DieRD", "Die", 0.5f);

	m_pMesh->AddCallBack("SwimU", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("SwimRU", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("SwimR", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("SwimRD", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("SwimD", "Water", this, &CPlayer::Water);

	m_pMesh->AddCallBack("WalkU", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("WalkRU", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("WalkR", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("WalkRD", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("WalkD", "Water", this, &CPlayer::Water);

	m_pMesh->AddCallBack("IdleU", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("IdleRU", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("IdleR", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("IdleRD", "Water", this, &CPlayer::Water);
	m_pMesh->AddCallBack("IdleD", "Water", this, &CPlayer::Water);

	m_pMesh->AddCallBack("DieU",	"Die", this, &CPlayer::Die);
	m_pMesh->AddCallBack("DieRU",	"Die", this, &CPlayer::Die);
	m_pMesh->AddCallBack("DieR",	"Die", this, &CPlayer::Die);
	m_pMesh->AddCallBack("DieRD",	"Die", this, &CPlayer::Die);
	m_pMesh->AddCallBack("DieD",	"Die", this, &CPlayer::Die);

	m_pMesh->SetEndFunc("RollU", this, &CPlayer::RollEnd);
	m_pMesh->SetEndFunc("RollRU", this, &CPlayer::RollEnd);
	m_pMesh->SetEndFunc("RollR", this, &CPlayer::RollEnd);
	m_pMesh->SetEndFunc("RollRD", this, &CPlayer::RollEnd);
	m_pMesh->SetEndFunc("RollD", this, &CPlayer::RollEnd);

	m_pMesh->SetRelativeScale(16.f, 16.f, 1.f);
	m_pMesh->SetPivot(0.5f, 0.5f, 0.f);

	SetRootComponent(m_pMesh);

	m_pInput->SetAxisFunc("MoveV", this, &CPlayer::MoveV);
	m_pInput->SetAxisFunc("MoveH", this, &CPlayer::MoveH);
	m_pInput->SetAxisFunc("Z", this, &CPlayer::Fire1);
	m_pInput->SetActionFunc("C", KT_PRESS, this, &CPlayer::ReturnArrow);
	m_pInput->SetActionFunc("Shift", KT_DOWN, this, &CPlayer::ShiftDown);
	m_pInput->SetActionFunc("X", KT_UP, this, &CPlayer::ShiftUp);
	m_pInput->SetActionFunc("O", KT_DOWN, this, &CPlayer::Option);
	m_pInput->SetActionFunc("TAB", KT_DOWN, this, &CPlayer::TileMatToggle);
	m_pInput->SetActionFunc("F1", KT_DOWN, this, &CPlayer::Stage1);
	m_pInput->SetActionFunc("C", KT_DOWN, this, &CPlayer::CloseUp);
	m_pInput->SetActionFunc("C", KT_UP, this, &CPlayer::CloseUpEnd);

	CLayer* pLayer = GET_SINGLE(CSceneManager)->GetScene()->FindLayer("Default");

	m_pBow = m_pScene->CreateObject<CBow>("Bow", pLayer);

	m_pArrow = m_pScene->CreateObject<CArrow>("Arrow", pLayer);

	m_pCoord = CreateComponent<CUIFont>("Coord", m_pLayer);

	m_pCoord->SetRect(0.f, 20.f, 500.f, 0.f);

	m_pMesh->AddChild(m_pCoord);

	m_pSnd = CreateComponent<CSound>("Sound");

	m_pSnd->SetSound("arrow_retrieval");

	m_pMesh->AddChild(m_pSnd);

	CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

	pCam->SetWorldPos(m_pMesh->GetWorldPos());

	pCam->SetTarget(this);

	SAFE_RELEASE(pCam);

	CLayer* pUI = m_pScene->FindLayer("UI");

	m_pFade = CreateComponent<CMesh2DComponent>("Fade", pUI);

	Resolution tRS = RESOLUTION;

	m_pFade->SetWorldScale(tRS.iWidth, tRS.iHeight, 0.f);
	m_pFade->SetPivot(0.5f, 0.5f, 0.f);

	m_pFade->SetInheritScale(false);	
	m_pFade->AddRenderState("AlphaBlend");

	m_pMesh->AddChild(m_pFade);

	return true;
}

void CPlayer::Start()
{

	CLayer* pMtrlLayer = m_pScene->FindLayer("1_MAT");

	if (pMtrlLayer)
	{
		SAFE_RELEASE(m_pMtrlMap);
		m_pMtrlMap = pMtrlLayer->GetTileMap();
	}

	CLayer* pColLayer = m_pScene->FindLayer("1_COL");

	if (pColLayer)
	{
		SAFE_RELEASE(m_pTileMap);
		m_pTileMap = pColLayer->GetTileMap();
	}

	CLayer* pAirLayer = m_pScene->FindLayer("1_COLA");

	if (pAirLayer)
	{
		SAFE_RELEASE(m_pAirMap);
		m_pAirMap = pAirLayer->GetTileMap();
	}
}

void CPlayer::Input(float fTime)
{
}

void CPlayer::Update(float fTime)
{
	CObj::Update(fTime);

#ifdef _DEBUG

	if (m_pBullet)
	{
		OutputDebugStringA("Bullet");
	}

	else
	{
		OutputDebugStringA("NoBullet");
	}

	if (m_bCharge)
	{
		OutputDebugStringA("Charge\n");
	}

	else
	{
		OutputDebugStringA("Not Charge\n");
	}

#endif

	TCHAR strPos[32] = {};

	Vector3 vPos = GetWorldPos();

	swprintf_s(strPos, TEXT("X: %.5f, Y: %.5f"), vPos.x, vPos.y);

	m_pCoord->SetText(strPos);
	m_pCoord->SetWorldPos(980.f, 5.f, 0.f);

	m_pBow->SetWorldPos(vPos);
	m_pArrow->SetWorldPos(vPos);

	if (m_eState == STATE::DIE)
	{
		Die(fTime);
		return;
	}

	if (m_bWalkIn)
	{
		if (m_bRolling)
		{
			m_bRolling = false;
		}

		m_fWalkTime += fTime;

		if (m_fWalkTime >= 1.5f)
		{
			CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

			pCam->SetZoom(false, -150.f);

			SAFE_RELEASE(pCam);
			ChangeStage(m_strMap.c_str());
		}

		SetState(STATE::WALK);

		m_bIdleEnable = false;

		CTileMode* pMode = (CTileMode*)m_pScene->GetGameMode();

		Vector3 vDir = pMode->GetDoorDir();

		m_pMesh->AddWorldPos(-vDir * 20.f * fTime);

		CMaterial* pMtrl = m_pFade->GetMaterial();

		pMtrl->SetDiffuseColor(1.f, 1.f, 1.f, m_fWalkTime / 1.5f);

		SAFE_RELEASE(pMtrl);
	}

	else if(m_fWalkTime > 0.f)
	{
		m_fWalkTime -= fTime;

		CMaterial* pMtrl = m_pFade->GetMaterial();

		pMtrl->SetDiffuseColor(1.f, 1.f, 1.f, m_fWalkTime / 1.5f);

		SAFE_RELEASE(pMtrl);

		if (m_fWalkTime < 0.f)
		{
			m_fWalkTime = 0.f;
		}
	}

	else if (!m_bRolling)
	{
		if (m_fScaleX == -1.f)	//	걷는 상태
		{
			if (m_fScaleY == 1.f)
			{
				m_eDir = DIR_8::LU;
				if (m_bIdleEnable)
				{
					if (m_eState != STATE::SWIM)
					{
						SetState(STATE::WALK);
					}

					m_fBowAngle = 45.f;
				}
				else
				{
					Attack(0.f, fTime);
				}
			}

			else if (m_fScaleY == -1.f)
			{
				m_eDir = DIR_8::LD;
				if (m_bIdleEnable)
				{
					if (m_eState != STATE::SWIM)
					{
						SetState(STATE::WALK);
					}

					m_fBowAngle = 135.f;
				}
				else
				{
					Attack(0.f, fTime);
				}
			}

			else
			{
				m_eDir = DIR_8::L;
				if (m_bIdleEnable)
				{
					if (m_eState != STATE::SWIM)
					{
						SetState(STATE::WALK);
					}

					m_fBowAngle = 90.f;
				}
				else
				{
					Attack(0.f, fTime);
				}
			}
		}

		else if (m_fScaleX == 1.f)
		{
			if (m_fScaleY == 1.f)
			{
				m_eDir = DIR_8::RU;
				if (m_bIdleEnable)
				{
					if (m_eState != STATE::SWIM)
					{
						SetState(STATE::WALK);
					}

					m_fBowAngle = -45.f;
				}
				else
				{
					Attack(0.f, fTime);
				}
			}

			else if (m_fScaleY == -1.f)
			{
				m_eDir = DIR_8::RD;
				if (m_bIdleEnable)
				{
					if (m_eState != STATE::SWIM)
					{
						SetState(STATE::WALK);
					}

					m_fBowAngle = -135.f;
				}
				else
				{
					Attack(0.f, fTime);
				}					
			}

			else
			{
				m_eDir = DIR_8::R;
				if (m_bIdleEnable)
				{
					if (m_eState != STATE::SWIM)
					{
						SetState(STATE::WALK);
					}

					m_fBowAngle = -90.f;
				}
				else
				{
					Attack(0.f, fTime);
				}
					
			}
		}

		else
		{
			if (m_fScaleY == 1.f)
			{
				m_eDir = DIR_8::U;
				if (m_bIdleEnable)
				{
					if (m_eState != STATE::SWIM)
					{
						SetState(STATE::WALK);
					}
						
					m_fBowAngle = 0.f;
				}
				else
				{
					Attack(0.f, fTime);
				}
			}

			else if (m_fScaleY == -1.f)
			{
				m_eDir = DIR_8::D;
				if (m_bIdleEnable)
				{
					if (m_eState != STATE::SWIM)
					{
						SetState(STATE::WALK);
					}

					m_fBowAngle = 180.f;
				}
				else
				{
					Attack(0.f, fTime);
				}
			}

			else
			{
				if (m_bIdleEnable)
				{
					m_pShadow->ReturnClip();
					m_pMesh->ReturnClip();
					m_pBow->ReturnClip();
					m_pArrow->ReturnClip();
				}
			}
		}
	}

	else if(!m_bBack)// 구르는 상태
	{
		m_fRollTime += fTime;
//
//		if (m_fRollTime >= 1.f)
//		{
//#ifdef _DEBUG
//			OutputDebugStringA("Stop Rolling");
//#endif
//
//			m_fRollTime = 0.f;
//			m_bRolling = false;
//			m_bIdleEnable = true;
//			m_fSpeed = m_fPrevSpeed;
//		}

		float fSpeed = 200.f;

		if (m_eState == STATE::SWIM)
		{
			fSpeed = 100.f;
		}

		Vector3 vPrevPos = m_pMesh->GetWorldPos();

			switch (m_eDir)
			{
			case DIR_8::U:
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime);
				break;
			case DIR_8::RU:
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime);
				break;
			case DIR_8::R:
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
				break;
			case DIR_8::RD:
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime * -1.f);
				break;
			case DIR_8::D:
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime * -1.f);
				break;
			case DIR_8::LD:
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime * -1.f);
				break;
			case DIR_8::L:
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
				break;
			case DIR_8::LU:
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime);
				break;
			}

			char cCol = TileMapCol(m_pTileMap);

			if (cCol != 0)
			{
				m_pMesh->SetWorldPos(vPrevPos);
				m_bBack = true;

				LookBack();

				CTileMode* pMode = (CTileMode*)m_pScene->GetGameMode();

				pMode->ShakeCam();

				SetState(m_eState);
			}

			else
			{
				cCol = TileMapCol(m_pAirMap);

				if (cCol != 0)
				{
					m_pMesh->SetWorldPos(vPrevPos);
					m_bBack = true;

					LookBack();

					CTileMode* pMode = (CTileMode*)m_pScene->GetGameMode();

					pMode->ShakeCam();

					SetState(m_eState);
				}

				else
				{
					cCol = TileMapCol(m_pMtrlMap);

					if ((cCol == '4' &&
						(m_eDir == DIR_8::LU ||
							m_eDir == DIR_8::RU ||
							m_eDir == DIR_8::U)) ||
						(cCol == '1' &&
							(m_eDir == DIR_8::R ||
								m_eDir == DIR_8::RU ||
								m_eDir == DIR_8::U)))
					{
						m_pMesh->SetWorldPos(vPrevPos);
						m_bBack = true;

						LookBack();

						CTileMode* pMode = (CTileMode*)m_pScene->GetGameMode();

						pMode->ShakeCam();

						SetState(m_eState);
					}
				}
			}
	}

	if (m_bBack)	// 넉백
	{
		m_fBackTime += fTime;

		if (m_fBackTime >= m_fBackLimit)
		{
			m_fBackTime = 0.f;

			m_bBack = false;

			m_bRolling = false;

#ifdef _DEBUG
			OutputDebugStringA("Stop Rolling");
#endif

			m_bIdleEnable = true;
		}

		float fSpeed = 40.f;

		Vector3 vPrevPos = m_pMesh->GetWorldPos();

		switch (m_eDir)
		{
		case DIR_8::U:
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime);
			break;
		case DIR_8::RU:
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime);
			break;
		case DIR_8::R:
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
			break;
		case DIR_8::RD:
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime * -1.f);
			break;
		case DIR_8::D:
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime * -1.f);
			break;
		case DIR_8::LD:
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime * -1.f);
			break;
		case DIR_8::L:
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
			break;
		case DIR_8::LU:
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * fSpeed * fTime);
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * fSpeed * fTime);
			break;
		}

		char cCol = TileMapCol(m_pTileMap);

		if (cCol != 0)
		{
			m_pMesh->SetWorldPos(vPrevPos);
			m_bBack = true;

			LookBack();

			SetState(m_eState);
		}
	}

	if (m_fRollTime >= 0.f)
	{
		m_fRollTime -= fTime;
	}
}

void CPlayer::PostUpdate(float fTime)
{
	CObj::PostUpdate(fTime);

	char cCol = TileMapCol(m_pTileMap);

	if (cCol != 0)
	{
		AddWorldPos(-m_fScaleX * m_fSpeed * fTime, -m_fScaleY * m_fSpeed * fTime, 0.f);
	}

	else if( m_pAirMap)
	{
		cCol = TileMapCol(m_pAirMap);

		if (cCol != 0)
		{
			AddWorldPos(-m_fScaleX * m_fSpeed * fTime, -m_fScaleY * m_fSpeed * fTime, 0.f);
		}
	}

	char cMtrl = TileMapCol(m_pMtrlMap);

	char cWater = atoi("4116");

	if (cMtrl == cWater)
	{
		if (m_eState != STATE::SWIM)
		{
			Water(0.f);
		}
		SetState(STATE::SWIM);
	}

	else if (m_eState == STATE::SWIM)
	{
		Water(0.f);

		SetState(STATE::IDLE);
	}
}

void CPlayer::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CPlayer::PreRender(float fTime)
{
	m_fScaleX = 0.f;
	m_fScaleY = 0.f;

	CObj::PreRender(fTime);
}

void CPlayer::Render(float fTime)
{
	CObj::Render(fTime);
}

void CPlayer::PostRender(float fTime)
{
	CObj::PostRender(fTime);

}

CPlayer* CPlayer::Clone()
{
	return new CPlayer(*this);
}

void CPlayer::Save(FILE* pFile)
{
	CObj::Save(pFile);

	int iLength = (int)m_pMesh->GetName().length();

	fwrite(&iLength, 4, 1, pFile);
	fwrite(m_pMesh->GetName().c_str(), 1, iLength, pFile);
}

void CPlayer::Load(FILE* pFile)
{
	CObj::Load(pFile);

	SAFE_RELEASE(m_pMesh);

	int iLength = 0;
	char strTag[256] = {};

	fread(&iLength, 4, 1, pFile);
	fread(strTag, 1, iLength, pFile);

	m_pMesh = (CSpriteComponent*)FindSceneComponent(strTag);

	m_pInput->SetAxisFunc("MoveV", this, &CPlayer::MoveV);
	m_pInput->SetAxisFunc("MoveH", this, &CPlayer::MoveH);
	m_pInput->SetAxisFunc("RotZ", this, &CPlayer::RotZ);
	m_pInput->SetActionFunc("Fire1", KT_DOWN, this, &CPlayer::Fire1);
}

void CPlayer::MoveV(float fScale, float fTime)
{
	if (m_eState == STATE::DIE)
	{
		return;
	}

	if (m_bIdleEnable && !m_bRolling)
	{
		char cCol = TileMapCol(m_pMtrlMap);

		if (cCol == '4')
		{
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * m_fSpeed * fTime * (fScale - abs(fScale) * 0.25f));
		}

		else
		{
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * m_fSpeed * fTime * fScale);
		}
	}

	m_fScaleY = fScale;
}

void CPlayer::MoveH(float fScale, float fTime)
{
	if (m_eState == STATE::DIE)
	{
		return;
	}

	m_fScaleX = fScale;

	if (m_fScaleX != 0.f)
	{
		SetWorldRotYAll(-m_fScaleX * 90.f + 90.f);
	}

	if (m_bIdleEnable && !m_bRolling)
	{
		char cCol = TileMapCol(m_pMtrlMap);

		if (cCol == '1')
		{
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * m_fSpeed * fTime * fScale * fScale);
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * m_fSpeed * fTime * fScale);
		}

		else
		{
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * m_fSpeed * fTime * fScale * fScale);
		}
	}
}

void CPlayer::RotZ(float fScale, float fTime)
{
	m_pMesh->AddRelativeRotZ(180.f * fTime * fScale);
}

void CPlayer::Attack(float fPushTime, float fTime)
{
	if (m_bRolling)
	{
		return;
	}

	m_bIdleEnable = false;

	if (m_bCharge)
	{
			CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

			pCam->SetZoom(true);


		if(!m_pBullet)
		{
			m_pBullet = (CBullet*)m_pScene->CreateCloneObj("Bullet", "Bullet",m_pLayer, m_pScene->GetSceneType());
			m_pBullet->SetSpeed(0.f);
		}

		float fDist = m_pBullet->GetWorldScale().x * m_pBullet->GetPivot().x + GetWorldScale().x * GetPivot().x + 1.f;

		pCam->SetTarget(m_pBullet);

		SAFE_RELEASE(pCam);

		m_pBullet->SetRelativePos(GetWorldPos() + Vector3(fDist * -sinf(DirectX::XMConvertToRadians(m_fBowAngle)), 
				fDist * cosf(DirectX::XMConvertToRadians(m_fBowAngle)), 0.f));
		m_pBullet->SetWorldRotZ(m_fBowAngle);

		if (m_fBowAngle > 180.f)
		{
			m_fBowAngle -= 360.f;
		}

		else if (m_fBowAngle < -180.f)
		{
			m_fBowAngle += 360.f;
		}

		switch (m_eDir)
		{
		case DIR_8::U:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("AttackU");
			SetDefaultClipAll("IdleU");

			m_fBowAngle = 0.f;

			break;
		case DIR_8::RU:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("AttackRU");
			SetDefaultClipAll("IdleRU");

			m_fBowAngle = -45.f;

			break;
		case DIR_8::R:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("AttackR");
			SetDefaultClipAll("IdleR");

			m_fBowAngle = -90.f;

			break;
		case DIR_8::RD:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("AttackRD");
			SetDefaultClipAll("IdleRD");

			m_fBowAngle = -135.f;

			break;
		case DIR_8::D:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("AttackD");
			SetDefaultClipAll("IdleD");

			m_fBowAngle = 180.f;

			break;
		case DIR_8::LD:
			SetWorldRotYAll(180.f);
			ChangeSequenceAll("AttackRD");
			SetDefaultClipAll("IdleRD");

			m_fBowAngle = 135.f;

			break;
		case DIR_8::L:
			SetWorldRotYAll(180.f);
			ChangeSequenceAll("AttackR");
			SetDefaultClipAll("IdleR");

			m_fBowAngle = 90.f;
			break;
		case DIR_8::LU:
			SetWorldRotYAll(180.f);
			ChangeSequenceAll("AttackRU");
			SetDefaultClipAll("IdleRU");

			m_fBowAngle = 45.f;
			break;
		}
	}

	else
	{
		switch (m_eDir)
		{
		case DIR_8::U:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("CallU");
			SetDefaultClipAll("IdleU");
			break;
		case DIR_8::RU:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("CallRU");
			SetDefaultClipAll("IdleRU");
			break;
		case DIR_8::R:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("CallR");
			SetDefaultClipAll("IdleR");
			break;
		case DIR_8::RD:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("CallRD");
			SetDefaultClipAll("IdleRD");
			break;
		case DIR_8::D:
			SetWorldRotYAll(0.f);
			ChangeSequenceAll("CallD");
			SetDefaultClipAll("IdleD");
			break;
		case DIR_8::LD:
			SetWorldRotYAll(180.f);
			ChangeSequenceAll("CallRD");
			SetDefaultClipAll("IdleRD");
			break;
		case DIR_8::L:
			SetWorldRotYAll(180.f);
			ChangeSequenceAll("CallR");
			SetDefaultClipAll("IdleR");
			break;
		case DIR_8::LU:
			SetWorldRotYAll(180.f);
			ChangeSequenceAll("CallRU");
			SetDefaultClipAll("IdleRU");
			break;
		}
	}
}

void CPlayer::AttackFire(float fPushTime, float fTime)
{
	if (m_bRolling)
	{
		return;
	}

	m_bIdleEnable = true;

	CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

	pCam->SetZoom(false);
	pCam->SetTarget(this);

	SAFE_RELEASE(pCam);

	if (!m_pBullet)
	{
		return;
	}

	m_bCharge = false;

	m_pArrow->Enable(false);

#ifdef _DEBUG
	char strTime[32] = {};
	sprintf_s(strTime, "PT: %.5f\n", fPushTime);
	OutputDebugStringA(strTime);
#endif

	if (m_pBullet)
	{
		m_pBullet->SetLimitDist(m_fPushTime * 500.f);
		m_pBullet->SetDist(0.f);
		m_pBullet->SetSpeed(1000.f);
	}

	SAFE_RELEASE(m_pBullet);
}

void CPlayer::AttackEnd()
{
}

void CPlayer::Fire1(float fScale, float fTime)
{
	if (fScale != 0.f)
	{
		CObj* pMissile = m_pScene->CreateCloneObj("Missile", "Missile", m_pLayer, m_pScene->GetSceneType());

		m_pSnd->SetSound("arrow_shoot");

		m_pSnd->Play(fTime);

		if (pMissile)
		{
			pMissile->SetWorldPos(GetWorldPos());

			pMissile->SetWorldRot(GetWorldRot());

			pMissile->AddWorldPos(pMissile->GetWorldAxis(WORLD_AXIS::AXIS_Y) * 32.f);
		}

		SAFE_RELEASE(pMissile);
	}
}

void CPlayer::ShiftDown(float fPushTime, float fTime)
{
	if (m_bRolling ||
		m_eState == STATE::DIE ||
		m_fRollTime > 0.f ||
		!m_bIdleEnable)
	{
		return;
	}

	m_fRollTime = 0.5f;

	SetState(STATE::ROLL);
}

void CPlayer::ShiftUp(float fPushTIme, float fTime)
{
	m_fSpeed = 100.f;
}

float CPlayer::GetAngle()
{
	Vector3 vCurAxis = GetWorldAxis(WORLD_AXIS::AXIS_Y);
	Vector3 vAxis = Vector3::Axis[(int)WORLD_AXIS::AXIS_Y];

	float fCos = vAxis.Dot(vCurAxis);
	float fTheta = acosf(fCos);

	if (vCurAxis.x)
		fTheta *= -1.f;

	return fTheta;
}

void CPlayer::ReturnArrow(float fPushTime, float fTime)
{
	m_fPushTime = fPushTime;

	if (m_bCharge)
	{
		if (!m_pBullet)
		{
			Attack(fPushTime, fTime);
		}

		Vector3 vPos = GetWorldPos();

		CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

		switch (m_eDir)
		{
		case DIR_8::U:
			pCam->SetMovePos(vPos + Vector3(0.f, 50.f, 0.f));
			break;
		case DIR_8::RU:
			pCam->SetMovePos(vPos + Vector3(50.f / sqrtf(2), 50.f / sqrtf(2), 0.f));
			break;
		case DIR_8::R:
			pCam->SetMovePos(vPos + Vector3(50.f, 0.f, 0.f));
			break;
		case DIR_8::RD:
			pCam->SetMovePos(vPos + Vector3(50.f / sqrtf(2), -50.f / sqrtf(2), 0.f));
			break;
		case DIR_8::D:
			pCam->SetMovePos(vPos + Vector3(0.f, -50.f, 0.f));
			break;
		case DIR_8::LD:
			pCam->SetMovePos(vPos + Vector3(-50.f / sqrtf(2), -50.f / sqrtf(2), 0.f));
			break;
		case DIR_8::L:
			pCam->SetMovePos(vPos + Vector3(-50.f, 0.f, 0.f));
			break;
		case DIR_8::LU:
			pCam->SetMovePos(vPos + Vector3(-50.f / sqrtf(2), 50.f / sqrtf(2), 0.f));
			break;
		}

		SAFE_RELEASE(pCam);

		return;
	}

	CLayer* pLayer = m_pScene->FindLayer("Default");
	CObj* pArw = pLayer->FindObj("Bullet");

	if (pArw)
	{
		float fSpeed = ((CBullet*)pArw)->GetSpeed();

		if (fSpeed == 0.f)
		{
			Vector3 vPos = GetWorldPos();

			Vector3 vArwPos = pArw->GetWorldPos();

			Vector3 vDir = vPos - vArwPos;

			Vector3 vAxisY = Vector3(0.f, 1.f, 0.f);

			vDir.Normalize();

			vArwPos += vDir * 100.f * fTime * fPushTime * fPushTime;

			char cCol = m_pTileMap->GetTileCol(Vector2(vArwPos.x, vArwPos.y));

			if (cCol == 0.f)
			{
				int iCol = ((CBullet*)pArw)->GetCol();

				if (iCol == 0)
				{
					pArw->AddWorldPos(vDir * 100.f * fTime * fPushTime * fPushTime);
				}
			}

			float fDot = vAxisY.Dot(vDir);

			float fAngle = DirectX::XMConvertToDegrees(acosf(fDot));

			if (vDir.x > 0.f)
				fAngle *= -1.f;

			pArw->SetWorldRotZ(fAngle);

			m_pSnd->SetSound("arrow_retrieval");

			m_pSnd->Play(0.f);
		}
	}

	SAFE_RELEASE(pArw);
}

void CPlayer::RollEnd()
{
	m_bIdleEnable = true;
	m_bRolling = false;

	if (m_fSpeed == 150.f)
		m_fSpeed = 100.f;

	else if (m_fSpeed == 100.f && m_eState == STATE::SWIM)
		m_fSpeed = 50.f;
}

void CPlayer::RollEnd(float fTime)
{
	RollEnd();
}

void CPlayer::ColStart(CCollider* pSrc, CCollider* pDst, float fTime)
{
	std::string strSrc = pSrc->GetName();

	std::string strDst = pDst->GetName();

	if (strDst == "JAM/colossus")
	{
		if (((CTileMode*)m_pScene->GetGameMode())->GetLock())
			return;

		m_strMap = strDst + ".tmx";

		m_bWalkIn = true;

		CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

		pCam->SetZoom(true, -150.f);
		pCam->SetMovePos(GetWorldPos());

		SAFE_RELEASE(pCam);
	}

	else if (strDst == "JAM/JAM")
	{
		if (((CTileMode*)m_pScene->GetGameMode())->GetLock())
			return;

		m_strMap = strDst + ".tmx";

		m_bWalkIn = true;

		CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

		pCam->SetZoom(true, -150.f);
		pCam->SetMovePos(GetWorldPos());

		SAFE_RELEASE(pCam);
	}

	else if (strDst == "RightHandBody" ||
		strDst == "LeftHandBody")
	{
		CColossus* pCol = (CColossus*)pDst->GetObj();

		ATTACK_STAGE eStage = pCol->GetStage();

		if (eStage == ATTACK_STAGE::DOWN)
		{
			SetState(STATE::DIE);
		}

		else
		{
			if (m_fScaleX != 0.f)
			{
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * -m_fSpeed * fTime * m_fScaleX * m_fScaleX);
			}

			if (m_fScaleY != 0.f)
			{
				m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * -m_fSpeed * fTime * m_fScaleY);
			}
		}
	}
}

void CPlayer::ColStay(CCollider* pSrc, CCollider* pDst, float fTime)
{
	if (pDst->GetName() == "LeftHandBody" ||
		pDst->GetName() == "RightHandBody")
	{
		RectInfo tInfo = ((CColliderRect*)pSrc)->GetInfo();

		Vector3 vCross = pSrc->GetCross();

		Vector3 vPos = GetWorldPos();

		if (m_fScaleX != 0.f)
		{
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_X) * -m_fSpeed * fTime * m_fScaleX * m_fScaleX);
		}

		if (m_fScaleY != 0.f)
		{
			m_pMesh->AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * -m_fSpeed * fTime * m_fScaleY);
		}
	}
}

void CPlayer::ColEnd(CCollider* pSrc, CCollider* pDst, float fTime)
{
}

void CPlayer::Option(float fPushTime, float fTime)
{
	CLayer * pUI = m_pScene->FindLayer("UI");

	CObj* pObj = pUI->FindObj("Button");

	pObj->Enable(!pObj->IsEnable());

	SAFE_RELEASE(pObj);
}

void CPlayer::Fire2(float fScale, float fTime)
{
}

void CPlayer::TileMatToggle(float fScale, float fTime)
{
	CLayer* pLayer = m_pScene->FindLayer("1_COL");

	if (pLayer)
	{
		CObj* pObj = pLayer->FindObj("1_COL");

		CSceneComponent* pCom = pObj->FindSceneComponent("1_COL");

		pCom->Enable(!pCom->IsEnable());

		SAFE_RELEASE(pCom);

		SAFE_RELEASE(pObj);
	}

	pLayer = m_pScene->FindLayer("1_COLA");

	if (pLayer)
	{
		CObj* pObj = pLayer->FindObj("1_COLA");

		CSceneComponent* pCom = pObj->FindSceneComponent("1_COLA");

		pCom->Enable(!pCom->IsEnable());

		SAFE_RELEASE(pCom);

		SAFE_RELEASE(pObj);
	}

	pLayer = m_pScene->FindLayer("2_COL");

	if (pLayer)
	{
		CObj* pObj = pLayer->FindObj("2_COL");

		CSceneComponent* pCom = pObj->FindSceneComponent("2_COL");

		pCom->Enable(!pCom->IsEnable());

		SAFE_RELEASE(pCom);

		SAFE_RELEASE(pObj);
	}

	pLayer = m_pScene->FindLayer("1_MAT");

	if (pLayer)
	{
		CObj* pObj = pLayer->FindObj("1_MAT");

		CSceneComponent* pCom = pObj->FindSceneComponent("1_MAT");

		pCom->Enable(!pCom->IsEnable());

		SAFE_RELEASE(pCom);

		SAFE_RELEASE(pObj);
	}
}

void CPlayer::Stage1(float fScale, float fTime)
{
	ChangeStage("JAM\\colossus.tmx");
}

void CPlayer::CloseUp(float fScale, float fTime)
{
	CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

	pCam->SetZoom(true);

	Vector3 vPos = GetWorldPos();

	switch (m_eDir)
	{
	case DIR_8::U:
		pCam->SetMovePos(vPos + Vector3(0.f, 100.f, 0.f));
		break;
	case DIR_8::RU:
		pCam->SetMovePos(vPos + Vector3(70.7f, 70.7f, 0.f));
		break;
	case DIR_8::R:
		pCam->SetMovePos(vPos + Vector3(100.f, 0.f, 0.f));
		break;
	case DIR_8::RD:
		pCam->SetMovePos(vPos + Vector3(70.7f, -70.7f, 0.f));
		break;
	case DIR_8::D:
		pCam->SetMovePos(vPos + Vector3(0.f, -100.f, 0.f));
		break;
	case DIR_8::LD:
		pCam->SetMovePos(vPos + Vector3(-70.7f, -70.7f, 0.f));
		break;
	case DIR_8::L:
		pCam->SetMovePos(vPos + Vector3(-100.f, 0.f, 0.f));
		break;
	case DIR_8::LU:
		pCam->SetMovePos(vPos + Vector3(-70.7f, 70.7f, 0.f));
		break;
	}

	SAFE_RELEASE(pCam);

	Attack(fScale, fTime);
}

void CPlayer::CloseUpEnd(float fScale, float fTime)
{
	CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

	pCam->SetZoom(false);

	SAFE_RELEASE(pCam);

	AttackFire(fScale, fTime);
}

char CPlayer::TileMapCol(CTileMap* pMap)
{
	if (!pMap)
		return 0;

	Vector2 vExtent = m_pRC->GetExtent();

	Vector3 vScale = Vector3(vExtent.x, vExtent.y, 0.f);
	Vector3 vPos = GetWorldPos() - GetPivot() * vScale;

	char cCol = pMap->GetTileCol(Vector2(vPos.x, vPos.y));

	if (cCol == 0)
	{
		cCol = pMap->GetTileCol(Vector2(vPos.x, vPos.y) + Vector2(0.f, vScale.y));

		if (cCol == 0)
		{
			cCol = pMap->GetTileCol(Vector2(vPos.x, vPos.y) + Vector2(vScale.x, 0.f));

			if (cCol == 0)
			{
				cCol = pMap->GetTileCol(Vector2(vPos.x, vPos.y) + Vector2(vScale.x, vScale.y));
			}
		}
	}

	return cCol;
}

void CPlayer::ChangeSequenceAll(const char* pSeq)
{
	m_pShadow->ChangeSequence(pSeq);
	m_pMesh->ChangeSequence(pSeq);
	m_pBow->ChangeClip(pSeq);
	m_pArrow->ChangeClip(pSeq);
}

void CPlayer::SetDefaultClipAll(const char* pSeq)
{
	m_pShadow->SetDefaultSeq(pSeq);
	m_pMesh->SetDefaultSeq(pSeq);
	m_pBow->SetDefaultClip(pSeq);
	m_pArrow->SetDefaultClip(pSeq);
}

void CPlayer::SetWorldRotYAll(float fRot)
{
	m_pShadow->SetWorldRotY(fRot);
	m_pMesh->SetWorldRotY(fRot);
	m_pBow->SetWorldRotY(fRot);
	m_pArrow->SetWorldRotY(fRot);
}

void CPlayer::SetState(STATE eState)
{
	if (m_bRolling && m_eState != eState && !m_bBack)
	{
		m_bRolling = false;
		m_bIdleEnable = true;
		m_fSpeed = m_fPrevSpeed;
	}

	STATE ePrevState = m_eState;

	m_eState = eState;

	switch (m_eState)
	{
	case STATE::IDLE:
	{
		switch (m_eDir)
		{
		case DIR_8::U:
		{
			SetDefaultClipAll("IdleU");
		}
		break;
		case DIR_8::RU:
		{
			SetDefaultClipAll("IdleRU");
		}
		break;
		case DIR_8::R:
		{
			SetDefaultClipAll("IdleR");
		}
		break;
		case DIR_8::RD:
		{
			SetDefaultClipAll("IdleRD");
		}
		break;
		case DIR_8::D:
		{
			SetDefaultClipAll("IdleD");
		}
		break;
		case DIR_8::LD:
		{
			SetDefaultClipAll("IdleRD");
		}
		break;
		case DIR_8::L:
		{
			SetDefaultClipAll("IdleR");
		}
		break;
		case DIR_8::LU:
		{
			SetDefaultClipAll("IdleRU");
		}
		break;
		}
	}
		break;
	case STATE::WALK:
	{
		m_fSpeed = 100.f;
		switch (m_eDir)
		{
		case DIR_8::U:
		{
			ChangeSequenceAll("WalkU");
			SetDefaultClipAll("IdleU");
		}
		break;
		case DIR_8::RU:
		{
			ChangeSequenceAll("WalkRU");
			SetDefaultClipAll("IdleRU");
		}
		break;
		case DIR_8::R:
		{
			ChangeSequenceAll("WalkR");
			SetDefaultClipAll("IdleR");
		}
		break;
		case DIR_8::RD:
		{
			ChangeSequenceAll("WalkRD");
			SetDefaultClipAll("IdleRD");
		}
		break;
		case DIR_8::D:
		{
			ChangeSequenceAll("WalkD");
			SetDefaultClipAll("IdleD");
		}
		break;
		case DIR_8::LD:
		{

			ChangeSequenceAll("WalkRD");
			SetDefaultClipAll("IdleRD");
		}
		break;
		case DIR_8::L:
		{
			ChangeSequenceAll("WalkR");
			SetDefaultClipAll("IdleR");
		}
		break;
		case DIR_8::LU:
		{
			ChangeSequenceAll("WalkRU");
			SetDefaultClipAll("IdleRU");
		}
		break;
		}
	}
		break;
	case STATE::ROLL:
	{
		m_fPrevSpeed = m_fSpeed;

		if (ePrevState == STATE::SWIM)
		{
			m_bRolling = true;
			m_bIdleEnable = false;
			Water(0.f);
		}

		else
		{
			m_pSnd->SetSound("Roll");

			m_pSnd->Play(0.f);

			m_bRolling = true;
			m_bIdleEnable = false;

			switch (m_eDir)
			{
			case DIR_8::U:
				SetWorldRotYAll(0.f);
				ChangeSequenceAll("RollU");
				break;
			case DIR_8::RU:
				SetWorldRotYAll(0.f);
				ChangeSequenceAll("RollRU");
				break;
			case DIR_8::R:
				SetWorldRotYAll(0.f);
				ChangeSequenceAll("RollR");
				break;
			case DIR_8::RD:
				SetWorldRotYAll(0.f);
				ChangeSequenceAll("RollRD");
				break;
			case DIR_8::D:
				SetWorldRotYAll(0.f);
				ChangeSequenceAll("RollD");
				break;
			case DIR_8::LD:
				SetWorldRotYAll(180.f);
				ChangeSequenceAll("RollRD");
				break;
			case DIR_8::L:
				SetWorldRotYAll(180.f);
				ChangeSequenceAll("RollR");
				break;
			case DIR_8::LU:
				SetWorldRotYAll(180.f);
				ChangeSequenceAll("RollRU");
				break;
			}
		}

		m_eState = ePrevState;
	}
		break;
	case STATE::SWIM:
	{
		m_fSpeed = 50.f;
		switch (m_eDir)
		{
		case DIR_8::U:
		{
			ChangeSequenceAll("SwimU");
			SetDefaultClipAll("SwimU");
		}
		break;
		case DIR_8::RU:
		{
			ChangeSequenceAll("SwimRU");
			SetDefaultClipAll("SwimRU");
		}
		break;
		case DIR_8::R:
		{
			ChangeSequenceAll("SwimR");
			SetDefaultClipAll("SwimR");
		}
		break;
		case DIR_8::RD:
		{
			ChangeSequenceAll("SwimRD");
			SetDefaultClipAll("SwimRD");
		}
		break;
		case DIR_8::D:
		{
			ChangeSequenceAll("SwimD");
			SetDefaultClipAll("SwimD");
		}
		break;
		case DIR_8::LD:
		{

			ChangeSequenceAll("SwimRD");
			SetDefaultClipAll("SwimRD");
		}
		break;
		case DIR_8::L:
		{
			ChangeSequenceAll("SwimR");
			SetDefaultClipAll("SwimR");
		}
		break;
		case DIR_8::LU:
		{
			ChangeSequenceAll("SwimRU");
			SetDefaultClipAll("SwimRU");
		}
		break;
		}
	}
		break;
	case STATE::ATTACK:
		break;
	case STATE::CALL:
		break;
	case STATE::DIE:
	{
		if (ePrevState != STATE::DIE)
		{
			m_fDieDelay = 1.f;
		}

		switch (m_eDir)
		{
		case DIR_8::U:
		{
			ChangeSequenceAll("DieU");
			SetDefaultClipAll("DieU");
		}
		break;
		case DIR_8::RU:
		{
			ChangeSequenceAll("DieRU");
			SetDefaultClipAll("DieRU");
		}
		break;
		case DIR_8::R:
		{
			ChangeSequenceAll("DieR");
			SetDefaultClipAll("DieR");
		}
		break;
		case DIR_8::RD:
		{
			ChangeSequenceAll("DieRD");
			SetDefaultClipAll("DieRD");
		}
		break;
		case DIR_8::D:
		{
			ChangeSequenceAll("DieD");
			SetDefaultClipAll("DieD");
		}
		break;
		case DIR_8::LD:
		{

			ChangeSequenceAll("DieRD");
			SetDefaultClipAll("DieRD");
		}
		break;
		case DIR_8::L:
		{
			ChangeSequenceAll("DieR");
			SetDefaultClipAll("DieR");
		}
		break;
		case DIR_8::LU:
		{
			ChangeSequenceAll("DieRU");
			SetDefaultClipAll("DieRU");
		}
		break;
		}
	}
		break;
	}
}

void CPlayer::Water(float fTime)
{
	char cMtrl = TileMapCol(m_pMtrlMap);

	char cWater = atoi("4116");

	char cHalfWater = atoi("4100");

	if (cWater == cMtrl ||
		cHalfWater == cMtrl)
	{
		CLayer* pGround = m_pScene->FindLayer("Ground");

		CObj* pWater = m_pScene->CreateCloneObj("Water", "Water", pGround, m_pScene->GetSceneType());

		pWater->SetWorldPos(GetWorldPos());

		if (cHalfWater == cMtrl)
		{
			pWater->AddWorldPos(Vector3(0.f, -GetWorldScale().y * GetPivot().y, 0.f));
		}

		CSpriteComponent* pCom = pWater->FindComByType<CSpriteComponent>();

		pCom->AddCallBack<CEffect>("Water", "Water", (CEffect*)pWater, &CEffect::Destroy);

		SAFE_RELEASE(pCom);

		SAFE_RELEASE(pWater);
	}
}

void CPlayer::LookBack()
{
	if (m_eDir != DIR_8::U && m_eDir != DIR_8::D)
	{
		m_pMesh->AddWorldRotY(180.f);
	}

	m_eDir = (DIR_8)(((int)m_eDir + 4)%(int)DIR_8::END);
}

void CPlayer::ChangeStage(const char* pStage)
{
	if (strcmp(pStage, "JAM/JAM.tmx") == 0)
	{
		GET_SINGLE(CSceneManager)->CreateNextScene();

		GET_SINGLE(CSceneManager)->SetGameMode<CMainGameMode>(false);
	}

	else
	{
		GET_SINGLE(CSceneManager)->CreateNextScene();

		GET_SINGLE(CSceneManager)->SetGameMode<CTileMode>(false);

		CScene* pScene = GET_SINGLE(CSceneManager)->GetNextScene();

		CTileMode* pMode = (CTileMode*)pScene->GetGameMode();

		pMode->LoadXml(pStage, MAP_PATH);
	}
}

void CPlayer::Die(float fTime)
{
	if (m_fDieDelay > 0.f)
	{
		m_fDieDelay -= fTime;
		return;
	}

	CMaterial* pMtrl = m_pFade->GetMaterial();

	Vector4 vDif = pMtrl->GetDif();

	vDif.w += fTime;

	pMtrl->SetDiffuseColor(vDif);

	SAFE_RELEASE(pMtrl);

	if (vDif.w >= 1.f)
	{
		GET_SINGLE(CSceneManager)->CreateNextScene();

		GET_SINGLE(CSceneManager)->SetGameMode<CMainGameMode>(false);
	}
}
