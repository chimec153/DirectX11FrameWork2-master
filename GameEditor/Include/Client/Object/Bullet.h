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
	bool	m_bFire;
	class CSpriteComponent* m_pFire;
	bool	m_bFixed;
	class CObj* m_pFixingObj;
	class CTileMap* m_pMtrlMap;

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

	void ChangeSprite(const std::string& strTag);

	void SetFire(bool bFire);

	bool IsFire()	const
	{
		return m_bFire;
	}

	void SetFix(bool bFix);

	bool IsFixed()	const
	{
		return m_bFixed;
	}

	void SetFixObj(class CObj* pObj);
	class CObj* GetFixObj()	const;

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

