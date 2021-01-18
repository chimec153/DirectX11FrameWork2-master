#pragma once

#include "Object/Obj.h"

enum class STATE
{
	IDLE,
	WALK,
	ROLL,
	SWIM,
	ATTACK,
	CALL,
	DIE,
	END
};

class CPlayer :
	public CObj
{
public:
	CPlayer();
	CPlayer(const CPlayer& player);
	virtual ~CPlayer();

private:
	class CSpriteComponent* m_pMesh;
	float						m_fSpeed;
	bool						m_bIdleEnable;
	float						m_fScaleX;
	float						m_fScaleY;
	DIR_8						m_eDir;
	bool						m_bCharge;
	bool						m_bRolling;
	class CArrow* m_pArrow;
	class CBow* m_pBow;
	class CBullet* m_pBullet;
	float			m_fBowAngle;
	class CTileMap* m_pTileMap;
	class CUIFont*	m_pCoord;
	class CSpriteComponent* m_pShadow;
	class CColliderRect* m_pRC;
	STATE						m_eState;
	float						m_fPushTime;
	float						m_fRollTime;
	float						m_fPrevSpeed;
	class CSound* m_pSnd;
	class CTileMap* m_pMtrlMap;
	float						m_fBackTime;
	bool						m_bBack;
	float						m_fBackLimit;
	class CTileMap* m_pAirMap;
	bool	m_bWalkIn;
	float	m_fWalkTime;
	std::string	m_strMap;
	class CMesh2DComponent* m_pFade;
	float		m_fDieDelay;

public:
	void SetMeshComponent(class CSpriteComponent* pMesh);
	virtual void Notify(const std::string& strTag);
	void Charge();
	bool IsCharged()	const
	{
		return m_bCharge;
	}

public:
	virtual bool Init();
	virtual void Start();
	virtual void Input(float fTime);
	virtual void Update(float fTime);
	virtual void PostUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PreRender(float fTime);
	virtual void Render(float fTime);
	virtual void PostRender(float fTime);
	virtual CPlayer* Clone();

public:
	virtual void Save(FILE* pFile);
	virtual void Load(FILE* pFile);

public:
	void MoveV(float fScale, float fTime);
	void MoveH(float fScale, float fTime);
	void RotZ(float fScale, float fTime);
	void Attack(float fPushTime, float fTime);
	void AttackFire(float fPushTime, float fTime);
	void AttackEnd();
	void Fire1(float fPushTime, float fTime);
	void ShiftDown(float fPushTIme, float fTime);
	void ShiftUp(float fPushTIme, float fTime);
	float GetAngle();
	void ReturnArrow(float fPushTime, float fTime);
	void RollEnd();
	void RollEnd(float fTime);

public:
	void ColStart(class CCollider* pSrc, class CCollider* pDst, float fTime);
	void ColStay(class CCollider* pSrc, class CCollider* pDst, float fTime);
	void ColEnd(class CCollider* pSrc, class CCollider* pDst, float fTime);

public:
	void Option(float fPushTime, float fTime);
	void Fire2(float fScale, float fTime);
	void TileMatToggle(float fScale, float fTime);
	void Stage1(float fScale, float fTime);
	void CloseUp(float fScale, float fTime);
	void CloseUpEnd(float fScale, float fTime);
	char TileMapCol(class CTileMap* pMap);
	void ChangeSequenceAll(const char* pSeq);
	void SetDefaultClipAll(const char* pSeq);
	void SetWorldRotYAll(float fRot);
	void SetState(STATE eState);
	void Water(float fTime);
	void LookBack();
	void ChangeStage(const char* pStage);
	void Die(float fTime);
};

