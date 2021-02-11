#pragma once

#include "GameMode.h"

enum class Stage
{
	PAD,
	AN,
	LOGO,
	TITLE,
	END
};

class CTileMode :
	public CGameMode
{
	friend class CScene;

protected:
	CTileMode();
	virtual ~CTileMode();

protected:
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
	class CObj* m_pFadeObj;
	class CMesh2DComponent* m_pFade;
	float           m_fFadeStart;
	float           m_fFadeEnd;
	float           m_fFadeScale;
	bool			m_bFade;
	class CMesh2DComponent* m_pBackFade;
	bool		m_bHub;
	
public:
	const Vector3& GetDoorDir()	const
	{
		return m_vDoorDir;
	}

	const bool GetLock()	const
	{
		return m_bLock;
	}

	void Lock(bool bLock);

public:
	bool Init();
	virtual void Update(float fTime);
	virtual void PreRender(float fTime);

public:
	virtual bool LoadXml(const char* pFileName, const std::string& strPathKey = DATA_PATH);
	virtual void searchXMLData(class TiXmlElement* pElem);

public:
	void ShakeCam(float fSpeed = 150.f, float fAmp = 5.f, float fLimit = 0.25f);
	void StopShaking();

public:
	void FadeOut(float fTime);
	void FadeIn(float fTime);
	void SetFade(float fStart, float fEnd, float fScale);
	void SetFadeColor(float r, float g, float b);

public:
	void SpawnFade();
};

void ColInit(class CCollider* pSrc, class CCollider* pDst, float fTime);
void ColStay(class CCollider* pSrc, class CCollider* pDst, float fTime);
void ColLast(class CCollider* pSrc, class CCollider* pDst, float fTime);

