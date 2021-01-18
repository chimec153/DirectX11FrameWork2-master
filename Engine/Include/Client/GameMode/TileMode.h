#pragma once

#include "GameMode.h"

class CTileMode :
	public CGameMode
{
	friend class CScene;

protected:
	CTileMode();
	virtual ~CTileMode();

protected:
	Vector2	m_vTileSize;
	int		m_pWidth[(int)TILE_TEX_TYPE::END];
	int		m_pHeight[(int)TILE_TEX_TYPE::END];
	const char* m_pTex[(int)TILE_TEX_TYPE::END];
	int		m_iMapWidth;
	int		m_iMapHeight;
	TILE_TEX_TYPE	m_eTexType;
	const char* m_pTag;
	int		m_iZ;
	bool	m_bAni;
	bool	m_bTileEnable;
	float	m_fTileOpacity;
	Vector3	m_vPos;
	Vector3 m_vSpawn;
	bool	m_bGlobal;
	float	m_fMaxRange;
	float	m_fMinRange;
	float	m_fMaxVol;
	float	m_fMinVol;
	float	m_fShakeTime;
	float	m_fShakeLimit;
	bool	m_bShake;
	Vector3 m_vPrevCamPos;
	int		m_iShake;
	float	m_fAmplitude;
	float	m_fDist;
	float	m_fSpeed;
	float	m_fFade;
	Vector3 m_vDoorDir;
	bool	m_bLock;
	class CCamera*	m_pCam;

public:
	const Vector3& GetDoorDir()	const
	{
		return m_vDoorDir;
	}

	const bool GetLock()	const
	{
		return m_bLock;
	}

	void Lock(bool bLock)
	{
		m_bLock = bLock;
	}

public:
	bool Init();
	virtual bool LoadXml(const char* pFileName, const std::string& strPathKey = DATA_PATH);
	virtual void searchXMLData(class TiXmlElement* pElem);
	void ShakeCam(float fSpeed = 150.f, float fAmp = 5.f, float fLimit = 0.25f);
	void StopShaking();
	virtual void Update(float fTime);	
};

void ColInit(class CCollider* pSrc, class CCollider* pDst, float fTime);
void ColStay(class CCollider* pSrc, class CCollider* pDst, float fTime);
void ColLast(class CCollider* pSrc, class CCollider* pDst, float fTime);

