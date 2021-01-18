#pragma once
#include "SceneComponent.h"
class CParticle :
    public CSceneComponent
{
    friend class CObj;

private:
    CParticle();
    CParticle(const CParticle& com);
    virtual ~CParticle();

private:
    

public:
    virtual bool Init();
    virtual void Update(float fTime);
    virtual void PostUpdate(float fTime);
    virtual void Collision(float fTime);
    virtual void PreRender(float fTime);
    virtual void Render(float fTime);
    virtual void PostRender(float fTime);
    virtual CParticle* Clone();
    virtual void Save(FILE* pFile);
    virtual void Load(FILE* pFile);
};

