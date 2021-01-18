#include "Bullet.h"
#include "Component/Mesh2DComponent.h"
#include "Resource/Material.h"
#include "Component/Camera.h"
#include "Scene/Scene.h"
#include "Scene/CameraManager.h"
#include "Component/SpriteComponent.h"
#include "Component/ColliderRect.h"
#include "Player.h"
#include "Component/ColliderCircle.h"
#include "Tile/TileMap.h"
#include "Tile/Tile.h"
#include "Component/Sound.h"

CBullet::CBullet() :
	m_fDist(0.f),
	m_fLimitDist(1800.f),
	m_fSpeed(1000.f),
	m_pMesh(nullptr),
	m_pMap(nullptr),
	m_pSnd(nullptr),
	m_iCol(0)
{
	m_iObjClassType = (int)OBJ_CLASS_TYPE::OCT_BULLET;
}

CBullet::CBullet(const CBullet& bullet) :
 	CObj(bullet)
{
	m_fDist = 0.f;
	m_fLimitDist = bullet.m_fLimitDist;
	m_fSpeed = bullet.m_fSpeed;

	m_pMesh = (CSpriteComponent*)FindSceneComponent("Mesh");

	SetRootComponent(m_pMesh);

	CCollider* pRC = (CCollider*)FindSceneComponent("BulletBody");

	pRC->SetCallBack<CBullet>(COLLISION_STATE::INIT, this, &CBullet::ColInit);
	pRC->SetCallBack<CBullet>(COLLISION_STATE::STAY, this, &CBullet::ColStay);
	pRC->SetCallBack<CBullet>(COLLISION_STATE::END, this, &CBullet::ColEnd);

	SAFE_RELEASE(pRC);
	//m_pMesh->SetEndFunc<CBullet>(this, &CBullet::Destroy);

	m_pMap = bullet.m_pMap;

	if (m_pMap)
		m_pMap->AddRef();

	if (bullet.m_pSnd)
	{
		m_pSnd = bullet.m_pSnd->Clone();
	}

	else
	{
		m_pSnd = nullptr;
	}

	m_iCol = 0;
}

CBullet::~CBullet()
{
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pMap);
	SAFE_RELEASE(m_pSnd);
}

bool CBullet::Init()
{
	m_pMesh = CreateComponent<CSpriteComponent>("Mesh", m_pLayer);

	m_pMesh->SetWorldRotX(180.f);
	m_pMesh->SetTexture(REGISTER_TYPE::RT_DIF, "ani2", TEXT("pngegg.png"), TEXTURE_PATH, 0, 1, (int)SHADER_CBUFFER_TYPE::CBUFFER_PIXEL);
	m_pMesh->CreateSprite("Arrow", "Arrow", LOOP_OPTION::LOOP);

	CMaterial* pMtrl = m_pMesh->GetMaterial();

	pMtrl->SetGray(false);

	SAFE_RELEASE(pMtrl);

	CColliderCircle* pRC = CreateComponent<CColliderCircle>("BulletBody", m_pLayer);

	pRC->SetRadius(8.f);
	pRC->SetProfile("Base");

	pRC->SetCallBack<CBullet>(COLLISION_STATE::INIT, this, &CBullet::ColInit);
	pRC->SetCallBack<CBullet>(COLLISION_STATE::STAY, this, &CBullet::ColStay);
	pRC->SetCallBack<CBullet>(COLLISION_STATE::END, this, &CBullet::ColEnd);

	m_pMesh->AddChild(pRC);

	SAFE_RELEASE(pRC);

	CLayer* pGround = m_pScene->FindLayer("Ground");

	CSpriteComponent* pShadow = CreateComponent<CSpriteComponent>("Shadow", pGround);

	pShadow->SetWorldScale(16.f, 16.f, 1.f);
	pShadow->SetPivot(0.5f, 0.5f, 0.f);
	pShadow->CreateSprite("ArrowShadow", "ArrowShadow", LOOP_OPTION::LOOP);
	pShadow->SetInheritScale(false);
	pShadow->SetInheritRotZ(false);
	pShadow->SetRelativePos(0.f, -4.f, 0.f);
	pShadow->SetInheritPos(false);
	pShadow->AddRenderState("AlphaBlend");

	pMtrl = pShadow->GetMaterial();

	pMtrl->SetDiffuseColor(1.f, 1.f, 1.f, 0.5f);

	SAFE_RELEASE(pMtrl);

	m_pMesh->AddChild(pShadow);

	SAFE_RELEASE(pShadow);

	SetRootComponent(m_pMesh);

	m_pMesh->SetRelativeScale(16.f, 16.f, 1.f);
	m_pMesh->SetPivot(0.5f, 0.5f, 0.f);

	m_pSnd = CreateComponent<CSound>("Sound");

	m_pSnd->SetSound("ArrowImpact1");

	m_pMesh->AddChild(m_pSnd);

	return true;
}

void CBullet::Start()
{
}

void CBullet::Update(float fTime)
{
	CObj::Update(fTime);

	if (!m_pMap)
	{
		CLayer* pLayer = m_pScene->FindLayer("1_COL");

		m_pMap = pLayer->GetTileMap();
	}

	Vector3 vPos = GetWorldPos() -GetWorldAxis(WORLD_AXIS::AXIS_Y) * GetWorldScale() * GetPivot();

	char cCol = m_pMap->GetTileCol(Vector2(vPos.x, vPos.y));

	if (cCol != 0)
	{
		CTile* pTile = m_pMap->GetTile(Vector2(vPos.x, vPos.y));

		Vector3 vDir = {};

		if (pTile)
		{
			Vector3 vTilePos = pTile->GetWorldPos();// -pTile->GetWorldScale() * pTile->GetPivot();

			vDir = vPos - vTilePos;
		}			

		vDir.Normalize();

		Vector2 vDir2D = Vector2(vDir.x, vDir.y);

		Vector2 vAxis = Vector2(1.f, 0.f);

		float fAngle = RadToDeg(acosf(vDir2D.Dot(vAxis)));

		if (vDir2D.y < 0.f)
			fAngle *= -1.f;

		Vector3 vNorm = Vector3::Zero;

		if (fAngle > 45.f && fAngle < 135.f)	// ╩С
		{
			vNorm = Vector3(0.f, 1.f, 0.f);
		}

		else if (fAngle > 135.f || fAngle < -135.f)	//	аб
		{
			vNorm = Vector3(-1.f, 0.f, 0.f);
		}

		else if (fAngle < 45.f && fAngle > -45.f)	//	©Л
		{
			vNorm = Vector3(1.f, 0.f, 0.f);
		}

		else	//	го
		{
			vNorm = Vector3(0.f, -1.f, 0.f);
		}

		Vector3 vArrowDir = -GetWorldAxis(WORLD_AXIS::AXIS_Y);

		Vector3 vReflect = vArrowDir - vNorm.Dot(vArrowDir) * vNorm * 2;

		fAngle = RadToDeg(acosf(vReflect.Dot(vArrowDir)));

		if (vReflect.x * vReflect.y > 0.f)
			fAngle *= -1.f;

		if (vNorm.y == 0.f)
			fAngle *= -1.f;

		fAngle = 180.f - fAngle;

		AddWorldRotZ(-fAngle);
	}

	AddRelativePos(m_pMesh->GetRelativeAxis(WORLD_AXIS::AXIS_Y) * 
		m_fSpeed * (m_fLimitDist - m_fDist ) / m_fLimitDist* -fTime);

	m_fDist += fTime * m_fSpeed;

	if (m_fDist >= m_fLimitDist)
	{
		m_fSpeed = 0.f;
	}
}

void CBullet::PostUpdate(float fTime)
{
	CObj::PostUpdate(fTime);
}

void CBullet::Collision(float fTime)
{
	CObj::Collision(fTime);

}

void CBullet::PreRender(float fTime)
{
	CObj::PreRender(fTime);
}

void CBullet::Render(float fTime)
{
	CObj::Render(fTime);
}

void CBullet::PostRender(float fTime)
{
	CObj::PostRender(fTime);
}

CBullet* CBullet::Clone()
{
	return new CBullet(*this);
}

void CBullet::ColInit(CCollider* pSrc, CCollider* pDest, float fTime)
{
	++m_iCol;

	std::string strDst = pDest->GetName();

	if (strDst == "PlayerBody")
	{
		CPlayer* pObj = (CPlayer*)pDest->GetObj();

		if (!pObj->IsCharged())
		{
			CObj* pCharge = m_pScene->CreateCloneObj("Charge", "Charge", m_pLayer, m_pScene->GetSceneType());

			pCharge->SetWorldPos(pSrc->GetCross());

			CSpriteComponent* pCom = pCharge->FindComByType<CSpriteComponent>();

			pCom->SetEndFunc<CObj>("Charge", pCharge, &CObj::Destroy);

			SAFE_RELEASE(pCom);

			SAFE_RELEASE(pCharge);

			pObj->Charge();
			Destroy();
		}
	}

	else if (strDst == "LeftHandBody" || 
		strDst == "RightHandBody")
	{
		Vector3 vSrcPos = pSrc->GetWorldPos();

		Vector3 vDstPos = pDest->GetWorldPos();

		Vector2 vExtent = ((CColliderRect*)pDest)->GetExtent();

		Vector3 vCrs = pSrc->GetCross();

		Vector3 vNrm = {};

		if (vExtent.y / vExtent.x * (vCrs.x - vDstPos.x) + vDstPos.y < vCrs.y)
		{
			if (- vExtent.y / vExtent.x * (vCrs.x - vDstPos.x) + vDstPos.y < vCrs.y)
			{
				vNrm = Vector3(0.f, 1.f, 0.f);
			}

			else
			{
				vNrm = Vector3(-1.f, 0.f, 0.f);
			}
		}

		else
		{
			if (-vExtent.y / vExtent.x * (vCrs.x - vDstPos.x) + vDstPos.y < vCrs.y)
			{
				vNrm = Vector3(1.f, 0.f, 0.f);
			}

			else
			{
				vNrm = Vector3(0.f, -1.f, 0.f);
			}
		}

		Vector3 vReflect = {};

		Vector3 v = GetWorldAxis(WORLD_AXIS::AXIS_Y);

		vReflect = v - 2 * v.Dot(vNrm) * vNrm;

		float fAngle = RadToDeg(acosf(v.Dot(vReflect)));

		AddWorldRotZ(fAngle);
	}

}

void CBullet::ColStay(CCollider* pSrc, CCollider* pDest, float fTime)
{
}

void CBullet::ColEnd(CCollider* pSrc, CCollider* pDest, float fTime)
{
	--m_iCol;
}
