#include "Colossus.h"
#include "Component/SpriteComponent.h"
#include "Component/ColliderRect.h"
#include "Scene/Scene.h"
#include "Camera/CameraManager.h"
#include "Component/Camera.h"
#include "../GameMode/TileMode.h"
#include "Scene/SceneManager.h"
#include "Component/Sound.h"
#include "Resource/Material.h"
#include "Resource/ShaderManager.h"

CColossus::CColossus()	:
	m_pHead(nullptr),
	m_pBody(nullptr),
	m_pLeftHand(nullptr),
	m_pRightHand(nullptr),
	m_pLeftShoulder(nullptr),
	m_pRightShoulder(nullptr),
	m_pLight(nullptr),
	m_pLeftHandShadow(nullptr),
	m_pRightHandShadow(nullptr),
	m_eState(TITAN_STATE::SLEEP),
	m_bRising(false),
	m_bLeft(false),
	m_eAttackStage(ATTACK_STAGE::IDLE),
	m_fReadyTime(0.f),
	m_fDownDist(64.f),
	m_fLimitDist(64.f),
	m_fHandSpeed(420.f),
	m_fInertiaTime(0.f),
	m_fInertiaLimit(0.5f),
	m_fMoveSpeed(120.f)
{
}

CColossus::CColossus(const CColossus& obj)	:
	CObj(obj)
	, m_pHead(static_cast<CSpriteComponent*>(FindSceneComponent("Head")))
	, m_pBody(static_cast<CSpriteComponent*>(FindSceneComponent("Body")))
	, m_pLeftHand(static_cast<CSpriteComponent*>(FindSceneComponent("LeftHand")))
	, m_pRightHand(static_cast<CSpriteComponent*>(FindSceneComponent("RightHand")))
	, m_pLeftShoulder(static_cast<CSpriteComponent*>(FindSceneComponent("LeftShoulder")))
	, m_pRightShoulder(static_cast<CSpriteComponent*>(FindSceneComponent("RightShoulder")))
	, m_pLight(static_cast<CSpriteComponent*>(FindSceneComponent("Light")))
	, m_pLeftHandShadow(static_cast<CSpriteComponent*>(FindSceneComponent("LeftHandShadow")))
	, m_pRightHandShadow(static_cast<CSpriteComponent*>(FindSceneComponent("RightHandShadow")))
	, m_eState(TITAN_STATE::SLEEP)
	, m_bRising(false)
	, m_eAttackStage(ATTACK_STAGE::IDLE)
	, m_bLeft(false)
	, m_fReadyTime(0.f)
	, m_fDownDist(obj.m_fDownDist)
	, m_fLimitDist(obj.m_fLimitDist)
	, m_fHandSpeed(obj.m_fHandSpeed)
	, m_fInertiaTime(0.f)
	, m_fInertiaLimit(obj.m_fInertiaLimit)
	, m_fMoveSpeed(obj.m_fMoveSpeed)
{
	CColliderRect* pRC = m_pBody->FindComByType<CColliderRect>();

	pRC->SetCallBack<CColossus>(COLLISION_STATE::INIT, this, &CColossus::ColStart);
	pRC->SetCallBack<CColossus>(COLLISION_STATE::INIT, this, &CColossus::ColStay);
	pRC->SetCallBack<CColossus>(COLLISION_STATE::INIT, this, &CColossus::ColLast);

	SAFE_RELEASE(pRC);

	m_pHead->AddCallBack("Cry", "Cry", this, &CColossus::Cry);
	m_pHead->AddCallBack("Cry", "CryEnd", this, &CColossus::ReturnCam);
	m_pHead->AddCallBack("Cry", "ShakeEnd", this, &CColossus::StopShaking);
}

CColossus::~CColossus()
{
	SAFE_RELEASE(m_pHead);
	SAFE_RELEASE(m_pBody);
	SAFE_RELEASE(m_pLeftHand);
	SAFE_RELEASE(m_pRightHand);
	SAFE_RELEASE(m_pLeftShoulder);
	SAFE_RELEASE(m_pRightShoulder);
	SAFE_RELEASE(m_pLight);
	SAFE_RELEASE(m_pLeftHandShadow);
	SAFE_RELEASE(m_pRightHandShadow);
}

bool CColossus::Init()
{
	if(!CObj::Init())
		return false;

	CLayer* pFore = m_pScene->FindLayer("Fore");

	m_pLight = CreateComponent<CSpriteComponent>("Light", m_pScene->FindLayer("Temp"));
	m_pLight->CreateSprite("Idle", "ColossusLight", LOOP_OPTION::LOOP);

	m_pLight->SetPivot(0.5f, 0.5f, 0.f);
	m_pLight->SetRelativePos(0.f, -56.f, 0.f);
	m_pLight->SetWorldScale(96.f, 48.f, 0.f);
	m_pLight->SetInheritScale(false);

	m_pLight->AddRenderState("AlphaBlend");

	CLayer* pGround = m_pScene->FindLayer("Ground");

	m_pBody = CreateComponent<CSpriteComponent>("Body", m_pScene->FindLayer("Temp"));
	m_pBody->CreateSprite("Idle", "ColossusBodyIdle", LOOP_OPTION::LOOP);

	m_pBody->SetPivot(0.5f, 0.5f, 0.f);
	m_pBody->SetWorldScale(96.f, 128.f, 0.f);
	m_pBody->SetRelativePos(0.f, -64.f, 0.f);
	m_pBody->SetInheritScale(false);

	m_pBody->AddRenderState("Character");

	CColliderRect* pRC = CreateComponent<CColliderRect>("ColossusBody",m_pLayer);

	pRC->SetExtent(28.f, 12.f);
	pRC->SetRelativePos(0.f, 8.f, 0.f);
	pRC->SetCallBack<CColossus>(COLLISION_STATE::INIT, this, &CColossus::ColStart);
	pRC->SetCallBack<CColossus>(COLLISION_STATE::STAY, this, &CColossus::ColStay);
	pRC->SetCallBack<CColossus>(COLLISION_STATE::END, this, &CColossus::ColLast);

	m_pBody->AddChild(pRC);

	SAFE_RELEASE(pRC);

	m_pHead = CreateComponent<CSpriteComponent>("Head", m_pLayer);
	m_pHead->CreateSprite("Idle", "ColossusHeadIdle", LOOP_OPTION::LOOP);
	m_pHead->CreateSprite("Cry", "ColossusHeadCry", LOOP_OPTION::ONCE_RETURN);
	m_pHead->CreateSprite("Stop", "ColossusHeadStop", LOOP_OPTION::LOOP);
	m_pHead->AddNotify("Cry", "Cry", 1);
	m_pHead->AddNotify("Cry", "ShakeEnd", 5);
	m_pHead->AddNotify("Cry", "CryEnd", 7);

	m_pHead->SetPivot(0.5f, 0.5f, 0.f);
	m_pHead->SetWorldScale(64.f, 64.f, 0.f);
	m_pHead->SetInheritScale(false);

	m_pHead->AddChild(m_pBody);

	SetRootComponent(m_pHead);

	CLayer* pZero = m_pScene->FindLayer("BackDefault");

	m_pLeftHand = CreateComponent<CSpriteComponent>("LeftHand", pZero);

	m_pLeftHand->CreateSprite("Idle", "ColossusHandIdle", LOOP_OPTION::LOOP);
	m_pLeftHand->CreateSprite("Down", "ColossusHandDown", LOOP_OPTION::LOOP);
	m_pLeftHand->CreateSprite("Up", "ColossusHandUp", LOOP_OPTION::LOOP);
	m_pLeftHand->CreateSprite("Deffence", "ColossusHandDeffence", LOOP_OPTION::LOOP);
	m_pLeftHand->CreateSprite("Rock", "ColossusHandRock", LOOP_OPTION::LOOP);

	m_pLeftHand->SetPivot(0.5f, 0.25f, 0.f);
	m_pLeftHand->SetRelativePos(-32.f + 480.f, -80.f + 672.f, 0.f);
	m_pLeftHand->SetWorldScale(48.f, 48.f, 0.f);
	m_pLeftHand->SetInheritScale(false);
	m_pLeftHand->SetInheritPos(false);

	m_pLeftHand->AddRenderState("ForeGround");
	
	CColliderRect* pLHRC = CreateComponent<CColliderRect>("LeftHandBody", m_pLayer);

	pLHRC->SetExtent(40.f, 20.f);

	m_pLeftHand->AddChild(pLHRC);

	SAFE_RELEASE(pLHRC);

	m_pLeftHandShadow = CreateComponent<CSpriteComponent>("LeftHandShadow", pGround);

	m_pLeftHandShadow->SetWorldScale(48.f, 32.f, 0.f);
	m_pLeftHandShadow->SetPivot(0.5f, 0.5f, 0.f);
	m_pLeftHandShadow->SetRelativePos(0.f, 16.f, 0.f);

	m_pLeftHandShadow->CreateSprite("Down", "ColossusHandDownShadow", LOOP_OPTION::LOOP);
	m_pLeftHandShadow->CreateSprite("Up", "ColossusHandUpShadow", LOOP_OPTION::LOOP);
	m_pLeftHandShadow->CreateSprite("Deffence", "ColossusHandDeffenceShadow", LOOP_OPTION::LOOP);
	m_pLeftHandShadow->CreateSprite("Rock", "ColossusHandRockShadow", LOOP_OPTION::LOOP);

	m_pLeftHandShadow->SetInheritScale(false);
	m_pLeftHandShadow->AddRenderState("AlphaBlend");

	CMaterial* pLHSMtrl = m_pLeftHandShadow->GetMaterial();

	pLHSMtrl->SetDiffuseColor(1.f, 1.f, 1.f, 0.75f);

	SAFE_RELEASE(pLHSMtrl);

	m_pLeftHand->AddChild(m_pLeftHandShadow);

	m_pHead->AddChild(m_pLeftHand);

	m_pRightHand = CreateComponent<CSpriteComponent>("RightHand", pZero);

	m_pRightHand->CreateSprite("Idle", "ColossusHandIdle", LOOP_OPTION::LOOP);
	m_pRightHand->CreateSprite("Down", "ColossusHandDown", LOOP_OPTION::LOOP);
	m_pRightHand->CreateSprite("Up", "ColossusHandUp", LOOP_OPTION::LOOP);
	m_pRightHand->CreateSprite("Deffence", "ColossusHandDeffence", LOOP_OPTION::LOOP);
	m_pRightHand->CreateSprite("Rock", "ColossusHandRock", LOOP_OPTION::LOOP);

	m_pRightHand->SetPivot(0.5f, 0.25f, 0.f);
	m_pRightHand->SetRelativePos(32.f + 480.f, -80.f + 672.f, 0.f);
	m_pRightHand->SetWorldScale(48.f, 48.f, 0.f);
	m_pRightHand->SetInheritScale(false);
	m_pRightHand->SetWorldRotY(180.f);
	m_pRightHand->SetInheritPos(false);

	m_pRightHand->AddRenderState("ForeGround");

	m_pHead->AddChild(m_pRightHand);

	CColliderRect* pRHRC = CreateComponent<CColliderRect>("RightHandBody", m_pLayer);

	pRHRC->SetExtent(40.f, 20.f);

	m_pRightHand->AddChild(pRHRC);

	SAFE_RELEASE(pRHRC);

	m_pRightHandShadow = CreateComponent<CSpriteComponent>("RightHandShadow", pGround);

	m_pRightHandShadow->SetWorldScale(48.f, 32.f, 0.f);
	m_pRightHandShadow->SetPivot(0.5f, 0.5f, 0.f);
	m_pRightHandShadow->SetRelativePos(0.f, 16.f, 0.f);

	m_pRightHandShadow->CreateSprite("Down", "ColossusHandDownShadow", LOOP_OPTION::LOOP);
	m_pRightHandShadow->CreateSprite("Up", "ColossusHandUpShadow", LOOP_OPTION::LOOP);
	m_pRightHandShadow->CreateSprite("Deffence", "ColossusHandDeffenceShadow", LOOP_OPTION::LOOP);
	m_pRightHandShadow->CreateSprite("Rock", "ColossusHandRockShadow", LOOP_OPTION::LOOP);

	m_pRightHandShadow->SetInheritScale(false);
	m_pRightHandShadow->AddRenderState("AlphaBlend");

	CMaterial* pRHSMtrl = m_pRightHandShadow->GetMaterial();

	pRHSMtrl->SetDiffuseColor(1.f, 1.f, 1.f, 0.75f);

	SAFE_RELEASE(pRHSMtrl);

	m_pRightHand->AddChild(m_pRightHandShadow);

	m_pLeftShoulder = CreateComponent<CSpriteComponent>("LeftShoulder", m_pLayer);
	m_pLeftShoulder->CreateSprite("Idle", "ColossusShoulderLeft", LOOP_OPTION::LOOP);

	m_pLeftShoulder->SetPivot(0.5f, 0.5f, 0.f);
	m_pLeftShoulder->SetRelativePos(-40.f, -24.f, 0.f);
	m_pLeftShoulder->SetWorldScale(48.f, 48.f, 0.f);
	m_pLeftShoulder->SetInheritScale(false);

	m_pHead->AddChild(m_pLeftShoulder);

	m_pRightShoulder = CreateComponent<CSpriteComponent>("RightShoulder", m_pLayer);
	m_pRightShoulder->CreateSprite("Idle", "ColossusShoulderRight", LOOP_OPTION::LOOP);

	m_pRightShoulder->SetPivot(0.5f, 0.5f, 0.f);
	m_pRightShoulder->SetRelativePos(40.f, -24.f, 0.f);
	m_pRightShoulder->SetWorldScale(48.f, 48.f, 0.f);
	m_pRightShoulder->SetInheritScale(false);

	m_pHead->AddChild(m_pRightShoulder);

	m_pHead->AddChild(m_pLight);

	return true;
}

void CColossus::Start()
{
	CObj::Start();
}

void CColossus::Update(float fTime)
{
	CObj::Update(fTime);

	if (m_bRising)
	{
		Vector3 vPos = m_pHead->GetWorldPos();

		if (vPos.y >= 718.f)
		{
			m_bRising = false;
			m_pHead->SetWorldPos(vPos.x, 718.f, vPos.z);
			m_pHead->ChangeSequence("Cry");
		}

		else
		{
			m_pHead->AddWorldPos(0.f, 100.f * fTime, 0.f);
		}
	}

	else
	{
		switch (m_eState)
		{
		case TITAN_STATE::SLEEP:
			break;
		case TITAN_STATE::IDLE:
		{
				SetState(TITAN_STATE::ATTACK);
		}
			break;
		case TITAN_STATE::ATTACK:
		{
			switch (m_eAttackStage)
			{
			case ATTACK_STAGE::IDLE:
			{
				m_pLeftHand->SetWorldScale(48.f, 64.f, 0.f);
				m_pRightHand->SetWorldScale(48.f, 64.f, 0.f);

				CObj* pObj = m_pScene->GetGameMode()->GetPlayer();

				Vector3 vPos = pObj->GetWorldPos();

				if (vPos.x > GetWorldPos().x)
				{
					m_bLeft = false;

					m_pRightHand->ChangeSequence("Down");

					m_pRightHandShadow->ChangeSequence("Rock");

					m_pLeftHand->ChangeSequence("Deffence");

					m_pLeftHandShadow->ChangeSequence("Deffence");
				}

				else
				{
					m_bLeft = true;

					m_pLeftHand->ChangeSequence("Down");

					m_pLeftHandShadow->ChangeSequence("Rock");

					m_pRightHand->ChangeSequence("Deffence");

					m_pRightHandShadow->ChangeSequence("Deffence");
				}

				SAFE_RELEASE(pObj);

				m_eAttackStage = ATTACK_STAGE::FIND;
			}
				break;
			case ATTACK_STAGE::FIND:
			{
				m_fReadyTime += fTime;

				CObj* pObj = m_pScene->GetGameMode()->GetPlayer();

				Vector3 vObjPos = pObj->GetWorldPos() + Vector3(0.f, m_fLimitDist, 0.f);

				Vector3 vPos = {};

				if (m_bLeft)
				{
					vPos = m_pLeftHand->GetWorldPos();

					Vector3 vDir = vObjPos - vPos;

					float fLength = vDir.Length();

					if (fLength < 10.f)
					{
						m_eAttackStage = ATTACK_STAGE::READY;

					}

					else if (fLength < m_fMoveSpeed)
					{
						m_pLeftHand->AddWorldPos(vDir / fLength * m_fMoveSpeed * fTime);
					}

					else
					{
						m_pLeftHand->AddWorldPos(vDir * fTime);
					}

					m_fDownDist -= fTime * m_fMoveSpeed;

					if (m_fDownDist <= 0.f)
					{
						m_fDownDist = 0.f;
					}

					else
					{
						m_pLeftHandShadow->AddRelativePos(0.f, -m_fMoveSpeed * fTime, 0.f);

						m_pLeftHandShadow->AddRelativeScale(-60.f * fTime, -40.f * fTime, 0.f);

						if (m_fDownDist >= 16.f)
						{
							CColliderRect* pRC = m_pLeftHand->FindComByType<CColliderRect>();

							pRC->Enable(false);

							SAFE_RELEASE(pRC);
						}
					}

					//Vector3 vBodyDir = m_pBody->GetWorldPos() - m_pRightHand->GetWorldPos();

					//float fBodyDist = vBodyDir.Length();

					//vBodyDir /= fBodyDist;

					//if (fBodyDist > 5.f)
					//{
					//	m_pRightHand->AddWorldPos(vBodyDir* m_fMoveSpeed* fTime);
					//}

					//else
					//{
					//	m_pRightHand->AddWorldPos(vBodyDir* 5.f * fTime);
					//}
				}

				else
				{
					vPos = m_pRightHand->GetWorldPos();

					Vector3 vDir = vObjPos - vPos;

					float fLength = vDir.Length();

					if (fLength < 10.f)
					{
						m_eAttackStage = ATTACK_STAGE::READY;
					}

					else if (fLength < m_fMoveSpeed)
					{
						m_pRightHand->AddWorldPos(vDir / fLength * m_fMoveSpeed * fTime);
					}

					else
					{
						m_pRightHand->AddWorldPos(vDir* fTime);
					}

					m_fDownDist -= fTime * m_fMoveSpeed;

					if (m_fDownDist <= 0.f)
					{
						m_fDownDist = 0.f;
					}

					else
					{
						m_pRightHandShadow->AddRelativePos(0.f, -m_fMoveSpeed * fTime, 0.f);

						m_pRightHandShadow->AddRelativeScale(-60.f * fTime, -40.f * fTime, 0.f);

						if (m_fDownDist >= 16.f)
						{
							CColliderRect* pRC = m_pRightHand->FindComByType<CColliderRect>();

							pRC->Enable(false);

							SAFE_RELEASE(pRC);
						}
					}

					//Vector3 vBodyDir = m_pBody->GetWorldPos() - m_pLeftHand->GetWorldPos();

					//float fBodyDist = vBodyDir.Length();

					//vBodyDir /= fBodyDist;

					//if (fBodyDist > 5.f)
					//{
					//	m_pLeftHand->AddWorldPos(vBodyDir* m_fMoveSpeed* fTime);
					//}

					//else
					//{
					//	m_pLeftHand->AddWorldPos(vBodyDir* 5.f * fTime);
					//}
				}

				SAFE_RELEASE(pObj);
			}
				break;
			case ATTACK_STAGE::READY:
			{
				m_fReadyTime += fTime;

				if (m_fReadyTime >= 1.f)
				{
					m_fReadyTime = 0.f;

					m_fDownDist = 0.f;

					m_eAttackStage = ATTACK_STAGE::DOWN;					

					if (m_bLeft)
					{
						m_pLeftHand->ChangeSequence("Down");

						m_pLeftHandShadow->ChangeSequence("Down");

						CColliderRect* pRC = m_pLeftHand->FindComByType<CColliderRect>();

						pRC->Enable(false);

						SAFE_RELEASE(pRC);
					}

					else
					{
						m_pRightHand->ChangeSequence("Down");

						m_pRightHandShadow->ChangeSequence("Down");

						CColliderRect* pRC = m_pRightHand->FindComByType<CColliderRect>();

						pRC->Enable(false);

						SAFE_RELEASE(pRC);
					}
				}

				CObj* pObj = m_pScene->GetGameMode()->GetPlayer();

				Vector3 vObjPos = pObj->GetWorldPos() + Vector3(0.f, m_fLimitDist, 0.f);

				Vector3 vPos = {};

				if (m_bLeft)
				{
					vPos = m_pLeftHand->GetWorldPos();

					Vector3 vDir = vObjPos - vPos;

					float fLength = vDir.Length();
					
					if (fLength < 10.f)
					{
						m_pLeftHand->AddWorldPos(vDir / fLength * 5.f * fTime);
					}

					else if (fLength < m_fMoveSpeed)
					{
						m_pLeftHand->AddWorldPos(vDir / fLength * m_fMoveSpeed * fTime);
					}

					else
					{
						m_pLeftHand->AddWorldPos(vDir * fTime);
					}

					m_fDownDist -= fTime * m_fMoveSpeed;

					if (m_fDownDist <= 0.f)
					{
						m_fDownDist = 0.f;
					}

					else
					{
						m_pLeftHandShadow->AddRelativePos(0.f, -m_fMoveSpeed * fTime, 0.f);

						m_pLeftHandShadow->AddRelativeScale(-60.f * fTime, -40.f * fTime, 0.f);
					}

					Vector3 vBodyDir = m_pBody->GetWorldPos() - m_pRightHand->GetWorldPos();

					float fBodyDist = vBodyDir.Length();

					vBodyDir /= fBodyDist;

					if (fBodyDist > 5.f)
					{
						m_pRightHand->AddWorldPos(vBodyDir * m_fMoveSpeed * fTime);
					}

					else
					{
						m_pRightHand->AddWorldPos(vBodyDir * 5.f * fTime);
					}
				}

				else
				{
					vPos = m_pRightHand->GetWorldPos();

					Vector3 vDir = vObjPos - vPos;

					float fLength = vDir.Length();

					if (fLength < 10.f)
					{
						m_pRightHand->AddWorldPos(vDir / fLength * 5.f * fTime);
					}
					
					else if (fLength < m_fMoveSpeed)
					{
						m_pRightHand->AddWorldPos(vDir / fLength * m_fMoveSpeed * fTime);
					}

					else
					{
						m_pRightHand->AddWorldPos(vDir * fTime);
					}

					m_fDownDist -= fTime * m_fMoveSpeed;

					if (m_fDownDist <= 0.f)
					{
						m_fDownDist = 0.f;
					}

					else
					{
						m_pRightHandShadow->AddRelativePos(0.f, -m_fMoveSpeed * fTime, 0.f);

						m_pRightHandShadow->AddRelativeScale(-60.f * fTime, -40.f * fTime, 0.f);
					}

					Vector3 vBodyDir = m_pBody->GetWorldPos() - m_pLeftHand->GetWorldPos();

					float fBodyDist = vBodyDir.Length();

					vBodyDir /= fBodyDist;

					if (fBodyDist > 5.f)
					{
						m_pLeftHand->AddWorldPos(vBodyDir * m_fMoveSpeed * fTime);
					}

					else
					{
						m_pLeftHand->AddWorldPos(vBodyDir * 5.f * fTime);
					}
				}

				SAFE_RELEASE(pObj);
			}
				break;
			case ATTACK_STAGE::DOWN:
			{
				m_fDownDist += fTime * m_fHandSpeed;

				if (m_fDownDist >= m_fLimitDist)
				{

					m_eAttackStage = ATTACK_STAGE::IDLE;

					((CTileMode*)m_pScene->GetGameMode())->ShakeCam(150.f, 10.f);

					if (m_bLeft)
					{
						m_pLeftHandShadow->SetRelativePos(0.f, 0.f, 0.f);

						m_pLeftHandShadow->SetRelativeScale(48.f, 32.f, 0.f);
					}

					else
					{
						m_pRightHandShadow->SetRelativePos(0.f, 0.f, 0.f);

						m_pRightHandShadow->SetRelativeScale(48.f, 32.f, 0.f);
					}
				}

				else if (m_fLimitDist - m_fDownDist <= 16.f)
				{
					if (m_bLeft)
					{
						CColliderRect* pRC = m_pLeftHand->FindComByType<CColliderRect>();

						pRC->Enable(true);

						SAFE_RELEASE(pRC);
					}

					else
					{
						CColliderRect* pRC = m_pRightHand->FindComByType<CColliderRect>();

						pRC->Enable(true);

						SAFE_RELEASE(pRC);
					}
				}

					if (m_bLeft)
					{
						m_pLeftHand->ChangeSequence("Down");

						m_pLeftHand->AddWorldPos(0.f, -m_fHandSpeed * fTime, 0.f);

						m_pLeftHandShadow->AddRelativePos(0.f, m_fHandSpeed * fTime, 0.f);

						m_pLeftHandShadow->AddRelativeScale(60.f * fTime, 40.f * fTime, 0.f);

						Vector3 vBodyDir = m_pBody->GetWorldPos() - m_pRightHand->GetWorldPos();

						float fBodyDist = vBodyDir.Length();

						vBodyDir /= fBodyDist;

						if (fBodyDist > 5.f)
						{
							m_pRightHand->AddWorldPos(vBodyDir * m_fMoveSpeed * fTime);
						}

						else
						{
							m_pRightHand->AddWorldPos(vBodyDir * 5.f * fTime);
						}
					}

					else
					{
						m_pRightHand->ChangeSequence("Down");

						m_pRightHand->AddWorldPos(0.f, -m_fHandSpeed * fTime, 0.f);

						m_pRightHandShadow->AddRelativePos(0.f, m_fHandSpeed * fTime, 0.f);

						m_pRightHandShadow->AddRelativeScale(60.f * fTime, 40.f * fTime, 0.f);

						Vector3 vBodyDir = m_pBody->GetWorldPos() - m_pLeftHand->GetWorldPos();

						float fBodyDist = vBodyDir.Length();

						vBodyDir /= fBodyDist;

						if (fBodyDist > 5.f)
						{
							m_pLeftHand->AddWorldPos(vBodyDir * m_fMoveSpeed * fTime);
						}

						else
						{
							m_pLeftHand->AddWorldPos(vBodyDir * 5.f * fTime);
						}
					}
			}
				break;

			}
		}
			break;
		case TITAN_STATE::STOP:
			break;
		case TITAN_STATE::DIE:
			break;
		}
	}

}

void CColossus::PostUpdate(float fTime)
{
	CObj::PostUpdate(fTime);
}

void CColossus::Collision(float fTime)
{
	CObj::Collision(fTime);
}

void CColossus::PreRender(float fTime)
{
	CObj::PreRender(fTime);
}

void CColossus::Render(float fTime)
{
	CObj::Render(fTime);
}

void CColossus::PostRender(float fTime)
{
	CObj::PostRender(fTime);
}

CColossus* CColossus::Clone()
{
	return new CColossus(*this);
}

void CColossus::Save(FILE* pFile)
{
	CObj::Save(pFile);
}

void CColossus::Load(FILE* pFile)
{
	CObj::Load(pFile);
}

void CColossus::ColStart(CCollider* pSrc, CCollider* pDst, float fTime)
{
	std::string strDst = pDst->GetName();

	if (strDst == "BulletBody")
	{
		if (m_eState == TITAN_STATE::SLEEP)
		{
			m_bRising = true;

			CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

			pCam->SetFocus(this);

			SAFE_RELEASE(pCam);

			CScene* pScene = GET_SINGLE(CSceneManager)->GetScene();

			((CTileMode*)m_pScene->GetGameMode())->Lock(true);
		}

		else if (m_eState == TITAN_STATE::ATTACK)
		{
			SetState(TITAN_STATE::STOP);
		}
	}
}

void CColossus::ColStay(CCollider* pSrc, CCollider* pDst, float fTime)
{
}

void CColossus::ColLast(CCollider* pSrc, CCollider* pDst, float fTime)
{
}

void CColossus::SetState(TITAN_STATE eState)
{
	TITAN_STATE ePrevState = m_eState;

	m_eState = eState;

	switch (m_eState)
	{
	case TITAN_STATE::SLEEP:
		break;
	case TITAN_STATE::IDLE:
		break;
	case TITAN_STATE::ATTACK:
		break;
	case TITAN_STATE::STOP:
	{
		m_pHead->ChangeSequence("Stop");

		FADE tFade = {};

		tFade.fGray = 1.f;

		GET_SINGLE(CShaderManager)->UpdateCBuffer("Fade", &tFade);
	}
		break;
	case TITAN_STATE::DIE:
		break;
	}
}

void CColossus::Cry(float fTime)
{
	CTileMode* pMode = (CTileMode*)m_pScene->GetGameMode();

	pMode->ShakeCam(1500.f, 10.f, 10.f);

	CSound* pSnd = CreateComponent<CSound>("Cry");

	pSnd->SetSound("Roar");

	pSnd->Play(fTime);

	SAFE_RELEASE(pSnd);
}

void CColossus::ReturnCam(float fTime)
{
	CObj* pObj = m_pScene->GetGameMode()->GetPlayer();

	CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

	pCam->SetFocus(pObj);

	SAFE_RELEASE(pCam);

	SAFE_RELEASE(pObj);

	SetState(TITAN_STATE::IDLE);
}

void CColossus::StopShaking(float fTime)
{
	CTileMode* pMode = (CTileMode*)m_pScene->GetGameMode();

	pMode->StopShaking();
}
