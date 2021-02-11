#pragma once

#include "Component.h"

class CRenderer :
    public CComponent
{
    friend class CSceneComponent;

private:
    CRenderer();
    CRenderer(const CRenderer& com);
    virtual ~CRenderer();

private:
    std::vector<class CRenderState*>    m_vecRenderState;
    class CMesh*                        m_pMesh;
    class CMaterial*                    m_pMaterial;
    class CShader*                      m_pShader;

public:
    void AddRenderState(const std::string& strKey);
    class CMesh* GetMesh()	const;
    void SetMesh(const std::string& strName);
    void SetMesh(class CMesh* pMesh);
    class CMaterial* GetMaterial()	const;
    void SetMaterial(class CMaterial* pMaterial);
    void SetShader(const std::string& strTag);
    void SetShader(class CShader* pShader);
    void SetState();
    void ResetState();
    const std::vector<class CRenderState*>& GetVecRenderState() const;
    void DeleteRenderState(const std::string& strKey);

private:
    class CRenderState* FindState(const std::string& strKey);

public:
    virtual bool Init();
    virtual void Update(float fTime);
    virtual void PostUpdate(float fTime);
    virtual void Collision(float fTime);
    virtual void PreRender(float fTime);
    virtual void Render(float fTime);
    virtual void PostRender(float fTime);
    virtual CRenderer* Clone();
    virtual void Save(FILE* pFile);
    virtual void Load(FILE* pFile);
};

