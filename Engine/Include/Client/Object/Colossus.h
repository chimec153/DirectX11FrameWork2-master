#pragma once
#include "Object\Obj.h"

enum class TITAN_STATE
{
	SLEEP,
	IDLE,
	ATTACK,
	STOP,
	DIE,
	END
};

enum class ATTACK_STAGE
{
	IDLE,
	FIND,
	READY,
	DOWN,
	END
};

class CColossus :
	public CObj
{
	friend class CScene;
	friend class CLayer;

private:
	CColossus();
	CColossus(const CColossus& obj);
	virtual ~CColossus();

private:
	class CSpriteComponent* m_pHead;
	class CSpriteComponent* m_pBody;
	class CSpriteComponent* m_pLeftHand;
	class CSpriteComponent* m_pRightHand;
	class CSpriteComponent* m_pLeftShoulder;
	class CSpriteComponent* m_pRightShoulder;
	class CSpriteComponent* m_pLight;
	class CSpriteComponent* m_pLeftHandShadow;
	class CSpriteComponent* m_pRightHandShadow;
	TITAN_STATE	m_eState;
	bool		m_bRising;
	bool		m_bLeft;
	ATTACK_STAGE	m_eAttackStage;
	float		m_fReadyTime;
	float		m_fDownDist;
	float		m_fLimitDist;
	float		m_fHandSpeed;
	float		m_fInertiaTime;
	float		m_fInertiaLimit;
	float		m_fMoveSpeed;

public:
	const TITAN_STATE GetState()	const
	{
		return m_eState;
	}

	const ATTACK_STAGE GetStage()	const
	{
		return m_eAttackStage;
	}

public:
	virtual bool Init();
	virtual void Start();
	virtual void Update(float fTime);
	virtual void PostUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PreRender(float fTime);
	virtual void Render(float fTime);
	virtual void PostRender(float fTime);
	virtual CColossus* Clone();

public:
	virtual void Save(FILE* pFile);
	virtual void Load(FILE* pFile);

public:
	void ColStart(class CCollider* pSrc, class CCollider* pDst, float fTime);
	void ColStay(class CCollider* pSrc, class CCollider* pDst, float fTime);
	void ColLast(class CCollider* pSrc, class CCollider* pDst, float fTime);

public:
	void SetState(TITAN_STATE eState);

public:
	void Cry(float fTime);
	void ReturnCam(float fTime);
	void StopShaking(float fTime);
};

