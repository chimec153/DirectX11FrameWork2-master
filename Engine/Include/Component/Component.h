#pragma once

#include "../Ref.h"

class CComponent :
	public CRef
{
	friend class CObj;

protected:
	CComponent();
	CComponent(const CComponent& com);
	virtual ~CComponent() = 0;

protected:
	class CScene*		m_pScene;
	class CLayer*		m_pLayer;
	class CObj*			m_pObj;
	COMPONENT_TYPE		m_eType;

public:
	class CObj* GetObj()	const;
	COMPONENT_TYPE GetType()	const;
	class CLayer* GetLayer()	const;
	class CScene* GetScene()	const;
	void DeleteLayer();

protected:
	bool		m_bStart;

public:
	bool IsStart()	const;

public:
	virtual bool Init();
	virtual bool Init(const char* pFileName, const std::string& strPathKey = DATA_PATH);
	virtual bool Init(FILE* pFile);
	virtual void Start();
	virtual void Update(float fTime);
	virtual void PostUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PreRender(float fTime);
	virtual void Render(float fTime);
	virtual void PostRender(float fTime);
	virtual CComponent* Clone() = 0;
	virtual void Save(FILE* pFile);
	virtual void Load(FILE* pFile);

public:
	void SpawnWindow();
};

