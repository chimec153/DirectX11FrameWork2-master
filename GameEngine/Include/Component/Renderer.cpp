#include "Renderer.h"
#include "../RenderManager.h"
#include "../Render/RenderState.h"
#include "../Resource/Mesh.h"
#include "../Resource/Material.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/Shader.h"
#include "../Resource/ShaderManager.h"

CRenderer::CRenderer()	:
	m_pMesh(nullptr)
	, m_pMaterial(nullptr)
	, m_pShader(nullptr)
{
}

CRenderer::CRenderer(const CRenderer& com)	:
	CComponent(com)
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

	m_pShader = com.m_pShader;

	if (m_pShader)
	{
		m_pShader->AddRef();
	}

	m_vecRenderState.clear();

	size_t iSz = com.m_vecRenderState.size();

	for (int i = 0; i < iSz; ++i)
	{
		CRenderState* pState = GET_SINGLE(CRenderManager)->FindState(com.m_vecRenderState[i]->GetName());

		if (pState)
		{
			m_vecRenderState.push_back(pState);
		}
	}
}

CRenderer::~CRenderer()
{
	SAFE_RELEASE_VECLIST(m_vecRenderState);
	SAFE_RELEASE(m_pMaterial);
	SAFE_RELEASE(m_pMesh);
	SAFE_RELEASE(m_pShader);
}

void CRenderer::AddRenderState(const std::string& strKey)
{
	CRenderState* pState = FindState(strKey);

	if (pState)
	{
		SAFE_RELEASE(pState);
		return;
	}

	pState = GET_SINGLE(CRenderManager)->FindState(strKey);

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
}

void CRenderer::SetMesh(CMesh* pMesh)
{
	SAFE_RELEASE(m_pMesh);

	m_pMesh = pMesh;

	if (m_pMesh)
	{
		m_pMesh->AddRef();
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

void CRenderer::SetShader(const std::string& strTag)
{
	SAFE_RELEASE(m_pShader);

	m_pShader = GET_SINGLE(CShaderManager)->FindShader(strTag);
}

void CRenderer::SetShader(CShader* pShader)
{
	SAFE_RELEASE(m_pShader);

	m_pShader = pShader;

	if (m_pShader)
		m_pShader->AddRef();
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

const std::vector<class CRenderState*>& CRenderer::GetVecRenderState() const
{
	return m_vecRenderState;
}

void CRenderer::DeleteRenderState(const std::string& strKey)
{
	size_t iSz = m_vecRenderState.size();

	for (size_t i = 0; i < iSz; ++i)
	{
		if (m_vecRenderState[i]->GetName() == strKey)
		{
			m_vecRenderState[i]->ResetState();

			SAFE_RELEASE(m_vecRenderState[i]);

			std::vector<CRenderState*>::iterator iter = m_vecRenderState.begin() + i;

			m_vecRenderState.erase(iter);

			return;
		}
	}
}

CRenderState* CRenderer::FindState(const std::string& strKey)
{
	size_t iSz = m_vecRenderState.size();

	for (size_t i = 0; i < iSz; ++i)
	{
		if (m_vecRenderState[i]->GetName() == strKey)
		{
			m_vecRenderState[i]->AddRef();

			return m_vecRenderState[i];
		}
	}

	return nullptr;
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

	m_pShader->SetShader();

	m_pMaterial->SetMaterial();

	m_pMesh->Render(fTime);
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
