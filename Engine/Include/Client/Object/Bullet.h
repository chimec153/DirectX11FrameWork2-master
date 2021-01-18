#pragma once
#include "Object\Obj.h"
class CBullet :
	public CObj
{
public:
	CBullet();
	CBullet(const CBullet& bullet);
	virtual ~CBullet();

private:
	float						m_fDist;
	float						m_fLimitDist;
	float						m_fSpeed;
	class CSpriteComponent* m_pMesh;
	class CTileMap* m_pMap;
	class CSound* m_pSnd;
	int		m_iCol;

public:
	void SetLimitDist(float fDist)
	{
		m_fLimitDist = fDist;

		if (m_fLimitDist >= 700.f)
			m_fLimitDist = 700.f;
	}

	void SetSpeed(float fSpeed)
	{
		m_fSpeed = fSpeed;
	}

	float GetSpeed()	const
	{
		return m_fSpeed;
	}

	void SetDist(float fDist)
	{
		m_fDist = fDist;
	}

	int GetCol()	const
	{
		return m_iCol;
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
	virtual CBullet* Clone();

public:
	void ColInit(class CCollider* pSrc, class CCollider* pDest, float fTime);
	void ColStay(class CCollider* pSrc, class CCollider* pDest, float fTime);
	void ColEnd(class CCollider* pSrc, class CCollider* pDest, float fTime);
};

