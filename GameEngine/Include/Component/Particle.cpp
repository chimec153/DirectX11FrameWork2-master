#include "Particle.h"
#include "Camera.h"
#include "../Device.h"

CParticle::CParticle()
{
}

CParticle::CParticle(const CParticle& com)	:
	CSceneComponent(com)
{
}

CParticle::~CParticle()
{
}

bool CParticle::Init()
{
	if (!CSceneComponent::Init())
		return false;

	return true;
}

void CParticle::Update(float fTime)
{
	CSceneComponent::Update(fTime);
}

void CParticle::PostUpdate(float fTime)
{
	CSceneComponent::PostUpdate(fTime);
}

void CParticle::Collision(float fTime)
{
	CSceneComponent::Collision(fTime);
}

void CParticle::PreRender(float fTime)
{
	CSceneComponent::PreRender(fTime);
}

void CParticle::Render(float fTime)
{
	CSceneComponent::Render(fTime);
}

void CParticle::PostRender(float fTime)
{
	CSceneComponent::PostRender(fTime);
}

CParticle* CParticle::Clone()
{
	return new CParticle(*this);
}

void CParticle::Save(FILE* pFile)
{
	CSceneComponent::Save(pFile);
}

void CParticle::Load(FILE* pFile)
{
	CSceneComponent::Load(pFile);
}
