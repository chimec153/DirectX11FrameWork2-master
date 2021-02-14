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

	CMaterial* pMtrl = pMaterial;

	m_pMaterial = pMtrl->Clone();
}

void CRenderer::SetMaterial(const std::string& strTag)
{
	SAFE_RELEASE(m_pMaterial);

	CMaterial* pMtrl = GET_SINGLE(CResourceManager)->FindMaterial(strTag);

	m_pMaterial = pMtrl->Clone();

	SAFE_RELEASE(pMtrl);
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

CShader* CRenderer::GetShader() const
{
	if (m_pShader)
		m_pShader->AddRef();

	return m_pShader;
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

void CRenderer::SetTexture(REGISTER_TYPE eType, const std::string& strTag, int iRegister, int iCount, unsigned int iType)
{
	m_pMaterial->SetTexture(eType, strTag, iRegister, iCount, iType);
}

void CRenderer::SetTexture(REGISTER_TYPE eType, CTexture* pTex, int iRegister, int iCount, unsigned int iType)
{
	m_pMaterial->SetTexture(eType, pTex, iRegister, iCount, iType);
}

void CRenderer::SetTexture(REGISTER_TYPE eType, const std::string& strTag, const TCHAR* pFileName, const std::string& strPathName, int iRegister, int iCount, unsigned int iType)
{
	m_pMaterial->SetTexture(eType, strTag, pFileName, strPathName, iRegister, iCount, iType);
}

void CRenderer::SetTextureFromFullPath(REGISTER_TYPE eType, const std::string& strTag, const TCHAR* pFullPath, int iRegister, int iCount, unsigned int iType)
{
	m_pMaterial->SetTextureFromFullPath(eType, strTag, pFullPath, iRegister, iCount, iType);
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
	if(m_pShader)
		m_pShader->SetShader();

	if(m_pMaterial)
		m_pMaterial->SetMaterial();

	if(m_pMesh)
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

	int iSize = (int)m_vecRenderState.size();
	fwrite(&iSize, 4, 1, pFile);
	for (int i = 0; i < iSize; ++i)
	{
		std::string strTag = m_vecRenderState[i]->GetName();

		int iLength = (int)strTag.length();
		fwrite(&iLength, 4, 1, pFile);
		fwrite(strTag.c_str(), 1, iLength, pFile);
	}

	bool bMesh = false;

	if (m_pMesh)
	{
		bMesh = true;
	}

	fwrite(&bMesh, 1, 1, pFile);

	if (m_pMesh)
	{
		std::string strTag = m_pMesh->GetName();

		int iLength = (int)strTag.length();
		fwrite(&iLength, 4, 1, pFile);
		fwrite(strTag.c_str(), 1, iLength, pFile);
	}

	bool bMtrl = false;

	if (m_pMaterial)
	{
		bMtrl = true;
	}

	fwrite(&bMtrl, 1, 1, pFile);

	if (m_pMaterial)
	{
		std::string strTag = m_pMaterial->GetName();
		int iLength = (int)strTag.length();
		fwrite(&iLength, 4, 1, pFile);
		fwrite(strTag.c_str(), 1, iLength, pFile);
		m_pMaterial->Save(pFile);
	}

	bool bShader = false;

	if (m_pShader)
	{
		bShader = true;
	}

	fwrite(&bShader, 1, 1, pFile);

	if (m_pShader)
	{
		std::string strTag = m_pShader->GetName();

		int iLength = (int)strTag.length();
		fwrite(&iLength, 4, 1, pFile);
		fwrite(strTag.c_str(), 1, iLength, pFile);
	}
}

void CRenderer::Load(FILE* pFile)
{
	CComponent::Load(pFile);

	int iSize = 0;
	fread(&iSize, 4, 1, pFile);
	for (int i = 0; i < iSize; ++i)
	{
		char strTag[256] = {};
		int iLength = 0;
		fread(&iLength, 4, 1, pFile);
		if (iLength > 0)
		{
			fread(strTag, 1, iLength, pFile);
		}

		AddRenderState(strTag);
	}

	bool bMesh = false;

	fread(&bMesh, 1, 1, pFile);

	if (bMesh)
	{
		char strTag[256] = {};
		int iLength = 0;
		fread(&iLength, 4, 1, pFile);
		if (iLength > 0)
		{
			fread(strTag, 1, iLength, pFile);
		}

		SetMesh(strTag);
	}

	bool bMtrl = false;

	fread(&bMtrl, 1, 1, pFile);

	if (bMtrl)
	{
		char strTag[256] = {};
		int iLength = 0;
		fread(&iLength, 4, 1, pFile);
		if (iLength > 0)
		{
			fread(strTag, 1, iLength, pFile);
		}
		SetMaterial(strTag);
		
		CMaterial* pMtrl = GetMaterial();
		pMtrl->Load(pFile);
	}

	bool bShader = false;

	fread(&bShader, 1, 1, pFile);

	if (bShader)
	{
		char strTag[256] = {};
		int iLength = 0;
		fread(&iLength, 4, 1, pFile);
		if (iLength > 0)
		{
			fread(strTag, 1, iLength, pFile);
		}

		SetShader(strTag);
	}
}

void CRenderer::SpawnWindow()
{
	if (ImGui::Begin("Renderer"))
	{
		CMaterial* pMtrl = GetMaterial();

		if (pMtrl)
		{
			Vector4 vDiff = pMtrl->GetDif();
			Vector4 vAmb = pMtrl->GetAmb();

			ImGui::SliderFloat4("DiffuseColor", &vDiff.x, 0.f, 1.f);
			ImGui::SliderFloat4("AmbientColor", &vAmb.x, 0.f, 1.f);

			pMtrl->SetDiffuseColor(vDiff);
			pMtrl->SetAmbientColor(vAmb);
		}

		SAFE_RELEASE(pMtrl);
		static int iItem = 0;
		char strState[256] = {};

		size_t iSize = m_vecRenderState.size();

		for (size_t i = 0; i < iSize; ++i)
		{
			strcat_s(strState, m_vecRenderState[i]->GetName().c_str());
			strcat_s(strState, "0");
		}

		ImGui::Combo("RenderState", &iItem, strState);
	}
	ImGui::End();
}
