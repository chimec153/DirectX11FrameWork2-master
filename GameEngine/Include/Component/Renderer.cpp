#include "Renderer.h"
#include "../RenderManager.h"
#include "../Render/RenderState.h"
#include "../Resource/Mesh.h"
#include "../Resource/Material.h"
#include "../Resource/ResourceManager.h"

CRenderer::CRenderer()	:
	m_pMesh(nullptr)
	, m_pMaterial(nullptr)
{
}

CRenderer::CRenderer(const CRenderer& com)
{
	m_pMesh = com.m_pMesh;

	if (m_pMesh)
	{
		m_pMesh->AddRef();
	}

	if (com.m_pMaterial)
		m_pMaterial = com.m_pMaterial->Clone();

	else
		m_pMaterial = nullptr;

	m_vecRenderState.clear();

	size_t iSz = com.m_vecRenderState.size();

	for (int i = 0; i < iSz; ++i)
	{
		m_vecRenderState.push_back(GET_SINGLE(CRenderManager)->FindState(com.m_vecRenderState[i]->GetTag()));
	}
}

CRenderer::~CRenderer()
{
	SAFE_RELEASE_VECLIST(m_vecRenderState);
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pMesh);
}

void CRenderer::AddRenderState(const std::string& strKey)
{
	CRenderState* pState = GET_SINGLE(CRenderManager)->FindState(strKey);

	if (!pState)
		return;

	m_vecRenderState.push_back(pState);
}

CMesh* CRenderer::GetMesh() const
{
	if (m_pMesh)
		m_pMesh->AddRef();

	return m_pMesh;
}

void CRenderer::SetMesh(const std::string& strName)
{
	SAFE_RELEASE(m_pMesh);

	m_pMesh = GET_SINGLE(CResourceManager)->FindMesh(strName);

	if (m_pMesh)
	{
		CMaterial* pMaterial = m_pMesh->GetMaterial();
		CMaterial* pClone = pMaterial->Clone();

		SAFE_RELEASE(pMaterial);

		m_pMaterial = pClone;

		SAFE_RELEASE(pClone);
	}
}

void CRenderer::SetMesh(CMesh* pMesh)
{
	SAFE_RELEASE(m_pMesh);

	m_pMesh = pMesh;

	if (m_pMesh)
	{
		m_pMesh->AddRef();

		CMaterial* pMaterial = m_pMesh->GetMaterial();
		CMaterial* pClone = pMaterial->Clone();

		SAFE_RELEASE(pMaterial);

		SetMaterial(pClone);

		SAFE_RELEASE(pClone);
	}
}

CMaterial* CRenderer::GetMaterial() const
{
	if (m_pMaterial)
		m_pMaterial->AddRef();

	return m_pMaterial;
}

void CRenderer::SetMaterial(CMaterial* pMaterial)
{
	SAFE_RELEASE(m_pMaterial);

	m_pMaterial = pMaterial;

	if (m_pMaterial)
		m_pMaterial->AddRef();
}

void CRenderer::SetState()
{
	size_t iSz = m_vecRenderState.size();

	for (size_t i = 0; i < iSz; ++i)
	{
		m_vecRenderState[i]->SetState();
	}
}

void CRenderer::ResetState()
{
	size_t iSz = m_vecRenderState.size();

	for (size_t i = 0; i < iSz; ++i)
	{
		m_vecRenderState[i]->ResetState();
	}
}

bool CRenderer::Init()
{
	if (!CComponent::Init())
		return false;

	return true;
}

void CRenderer::Update(float fTime)
{
	CComponent::Update(fTime);
}

void CRenderer::PostUpdate(float fTime)
{
	CComponent::PostUpdate(fTime);
}

void CRenderer::Collision(float fTime)
{
	CComponent::Collision(fTime);
}

void CRenderer::PreRender(float fTime)
{
	size_t iSz = m_vecRenderState.size();

	for (size_t i = 0; i < iSz; ++i)
	{
		m_vecRenderState[i]->SetState();
	}
}

void CRenderer::Render(float fTime)
{
	CComponent::Render(fTime);
}

void CRenderer::PostRender(float fTime)
{
	size_t iSz = m_vecRenderState.size();

	for (size_t i = 0; i < iSz; ++i)
	{
		m_vecRenderState[i]->ResetState();
	}
}

CRenderer* CRenderer::Clone()
{
	return new CRenderer(*this);
}

void CRenderer::Save(FILE* pFile)
{
	CComponent::Save(pFile);
}

void CRenderer::Load(FILE* pFile)
{
	CComponent::Load(pFile);
}
