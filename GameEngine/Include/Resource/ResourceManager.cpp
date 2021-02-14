#include "ResourceManager.h"
#include "Mesh2D.h"
#include "ShaderManager.h"
#include "Material.h"
#include "Texture.h"
#include "../Device.h"
#include "Animation2DSequence.h"
#include "StaticMesh.h"
#include "ParticleSystem.h"

DEFINITION_SINGLE(CResourceManager)

CResourceManager::CResourceManager()	:
	m_pDefaultMesh(nullptr),
	m_pDefaultUIMesh(nullptr),
	m_pNoiseTex(nullptr),
	m_pPoint(nullptr),
	m_pLinear(nullptr),
	m_pAnisotropic(nullptr)
{

}

CResourceManager::~CResourceManager()
{
	SAFE_RELEASE(m_pDefaultMesh);
	SAFE_RELEASE(m_pDefaultUIMesh);
	SAFE_RELEASE(m_pNoiseTex);
	SAFE_RELEASE_MAP(m_mapMesh);
	SAFE_RELEASE_MAP(m_mapMaterial);
	SAFE_RELEASE_MAP(m_mapAni2DSeq);
	SAFE_RELEASE_MAP(m_mapTexture);
	SAFE_RELEASE_MAP(m_mapParticle);
	DESTROY_SINGLE(CShaderManager);
	SAFE_RELEASE(m_pPoint);
	SAFE_RELEASE(m_pLinear);
	SAFE_RELEASE(m_pAnisotropic);
}

CMesh2D* CResourceManager::GetDefaultMesh() const
{
	if (m_pDefaultMesh)
		m_pDefaultMesh->AddRef();

	return m_pDefaultMesh;
}

CMesh2D* CResourceManager::GetUIMesh() const
{
	if (m_pDefaultUIMesh)
		m_pDefaultUIMesh->AddRef();

	return m_pDefaultUIMesh;
}

CTexture* CResourceManager::GetNoiseTex() const
{
	if (m_pNoiseTex)
		m_pNoiseTex->AddRef();

	return m_pNoiseTex;
}

bool CResourceManager::Init()
{
	// Shader Manager �ʱ�ȭ
	if (!GET_SINGLE(CShaderManager)->Init())
		return false;

	m_pNoiseTex = new CTexture;

	m_pNoiseTex->SetName("Noise");

	//m_pNoiseTex->CreateRandomTexture1DSRV();

	m_pNoiseTex->LoadTexture("Noise", TEXT("Noise.png"));

	m_pNoiseTex->SetTexture(10, (int)SHADER_CBUFFER_TYPE::CBUFFER_COMPUTE |
		(int)SHADER_CBUFFER_TYPE::CBUFFER_PIXEL);

	m_pDefaultMesh = new CMesh2D;

	m_pDefaultMesh->SetName("DefaultMesh");

	if (!m_pDefaultMesh->CreateMesh())
		return false;

	m_pDefaultMesh->AddRef();

	m_mapMesh.insert(std::make_pair("DefaultMesh", m_pDefaultMesh));

	CMaterial* pMaterial = CreateMaterial("Color");

	//pMaterial->SetShader("Standard2D");

	SAFE_RELEASE(pMaterial);

	m_pDefaultUIMesh = new CMesh2D;

	m_pDefaultUIMesh->SetName("DefaultUIMesh");

	if (!m_pDefaultUIMesh->CreateMesh())
		return false;

	CMaterial* pMtrl = CreateMaterial("UI2D");

	//pMtrl->SetShader("UI2D");

	SAFE_RELEASE(pMtrl);
	
	CMaterial* pParticleMtrl = CreateMaterial("Particle");

	//pParticleMtrl->SetShader("ParticleShader");

	SAFE_RELEASE(pParticleMtrl);

	m_pPoint = CreateSample(D3D11_FILTER_MIN_MAG_MIP_POINT);
	m_pLinear = CreateSample(D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	m_pAnisotropic = CreateSample(D3D11_FILTER_ANISOTROPIC);

	CONTEXT->VSSetSamplers(0, 1, &m_pPoint);
	CONTEXT->PSSetSamplers(0, 1, &m_pPoint);
	CONTEXT->HSSetSamplers(0, 1, &m_pPoint);
	CONTEXT->DSSetSamplers(0, 1, &m_pPoint);
	CONTEXT->GSSetSamplers(0, 1, &m_pPoint);
	CONTEXT->CSSetSamplers(0, 1, &m_pPoint);

	CONTEXT->VSSetSamplers(1, 1, &m_pLinear);
	CONTEXT->PSSetSamplers(1, 1, &m_pLinear);
	CONTEXT->HSSetSamplers(1, 1, &m_pLinear);
	CONTEXT->DSSetSamplers(1, 1, &m_pLinear);
	CONTEXT->GSSetSamplers(1, 1, &m_pLinear);
	CONTEXT->CSSetSamplers(1, 1, &m_pLinear);

	CONTEXT->VSSetSamplers(2, 1, &m_pAnisotropic);
	CONTEXT->PSSetSamplers(2, 1, &m_pAnisotropic);
	CONTEXT->HSSetSamplers(2, 1, &m_pAnisotropic);
	CONTEXT->DSSetSamplers(2, 1, &m_pAnisotropic);
	CONTEXT->GSSetSamplers(2, 1, &m_pAnisotropic);
	CONTEXT->CSSetSamplers(2, 1, &m_pAnisotropic);

	CMaterial* pColMat = CreateMaterial("Collider");

	//pColMat->SetShader("Collider");

	SAFE_RELEASE(pColMat);

	Vector3 vVtx[5] =
	{
		Vector3(-0.5f, 0.5f, 0.f),
		Vector3(0.5f, 0.5f, 0.f),
		Vector3(0.5f, -0.5f, 0.f),
		Vector3(-0.5f, -0.5f, 0.f),
		Vector3(-0.5f, 0.5f, 0.f)
	};

	CreateMesh(MESH_TYPE::MT_2D, "Collider2D", D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, vVtx, sizeof(Vector3), sizeof(vVtx) / sizeof(vVtx[0]),
		D3D11_USAGE_IMMUTABLE);

	Vector3 vLnVtx[2] =
	{
		Vector3(0.f, -0.5f, 0.f),
		Vector3(0.f, 0.5f, 0.f)
	};

	CreateMesh(MESH_TYPE::MT_2D, "Collider2DLine", D3D_PRIMITIVE_TOPOLOGY_LINELIST, vLnVtx, sizeof(Vector3), sizeof(vLnVtx) / sizeof(vLnVtx[0]),
		D3D11_USAGE_IMMUTABLE);

	Vector3 vLn[2] =
	{
		Vector3(0.f, 0.f, 0.f),
		Vector3(1.f, 0.f, 0.f)
	};

	CreateMesh(MESH_TYPE::MT_2D, "Line", D3D_PRIMITIVE_TOPOLOGY_LINELIST, vLn,
		sizeof(Vector3), sizeof(vLn) / sizeof(vLn[0]), D3D11_USAGE_IMMUTABLE);

	CMaterial* pLineMtrl = CreateMaterial("Line");

	//pLineMtrl->SetShader("LineShader");

	SAFE_RELEASE(pLineMtrl);

	Vector3 vCircle[361] = {};

	for (int i = 0; i < 361; ++i)
	{
		float fAngle = DirectX::XMConvertToRadians((float)i);

		vCircle[i] = Vector3(cosf(fAngle) * 0.5f, sinf(fAngle) * 0.5f, 0.f);
	}

	CreateMesh(MESH_TYPE::MT_2D, "Circle2D", D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
		vCircle, sizeof(Vector3), sizeof(vCircle) / sizeof(vCircle[0]), D3D11_USAGE_IMMUTABLE);

	VertexColor vCircle2[361] = {};

	for (int i = 0; i < 361; ++i)
	{
		float fAngle = DirectX::XMConvertToRadians((float)i);

		vCircle2[i].vPos = Vector3(cosf(fAngle) * 0.5f, 0.f, sinf(fAngle) * 0.5f);
		vCircle2[i].vColor = Vector4((sinf(fAngle) + 1.f) / 2.f, (cosf(fAngle) + 1.f) / 2.f, (sinf(fAngle) + 1.f) / 2.f, 1.f);
		vCircle2[i].vUV = Vector2(0.f, -1.f);
	}

	CreateMesh(MESH_TYPE::MT_2D, "Circle", D3D_PRIMITIVE_TOPOLOGY_LINESTRIP,
		vCircle2, sizeof(VertexColor), sizeof(vCircle2) / sizeof(vCircle2[0]), D3D11_USAGE_IMMUTABLE);

	CMaterial* pC2CMtrl = CreateMaterial("C2C");

	//pC2CMtrl->SetShader("C2CShader");

	SAFE_RELEASE(pC2CMtrl);

	VertexColor tPoint = {};

	CreateMesh(MESH_TYPE::MT_2D, "Particle", D3D_PRIMITIVE_TOPOLOGY_POINTLIST,
		&tPoint, sizeof(VertexColor), 1, D3D11_USAGE_IMMUTABLE);

	Basic tVertex[4] = {};

	tVertex[0].vPos = Vector3(0.f, 1.f, 0.f);
	tVertex[0].vUV = Vector2(0.f, 0.f);

	tVertex[1].vPos = Vector3(1.f, 1.f, 0.f);
	tVertex[1].vUV = Vector2(1.f, 0.f);

	tVertex[2].vPos = Vector3(1.f, 0.f, 0.f);
	tVertex[2].vUV = Vector2(1.f, 1.f);

	tVertex[3].vPos = Vector3(0.f, 0.f, 0.f);
	tVertex[3].vUV = Vector2(0.f, 1.f);

	unsigned short pBasicIdx[6] =
	{
		0, 1, 2, 0, 2,3
	};

	CreateMesh(MESH_TYPE::MT_2D, "AngleMesh", D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		tVertex, sizeof(Basic), 4, D3D11_USAGE_IMMUTABLE, pBasicIdx, 2, 6, D3D11_USAGE_IMMUTABLE, DXGI_FORMAT_R16_UINT);

	CMaterial* pAngleMtrl = CreateMaterial("Angle");

	//pAngleMtrl->SetShader("AngleShader");

	SAFE_RELEASE(pAngleMtrl);

	std::vector<const TCHAR*> vecFileName;

	for (int i = 0; i < 13; ++i)
	{
		TCHAR* pFile = new TCHAR[MAX_PATH];

		swprintf_s(pFile, MAX_PATH, TEXT("mouse\\Mouse%d.png"), i);

		vecFileName.push_back(pFile);
	}

	LoadTexture("Mouse", vecFileName);

	SAFE_DELETE_VECLIST(vecFileName);

	LoadTexture("TopButton", TEXT("Up.png"));
	LoadTexture("BotButton", TEXT("Down.png"));
	LoadTexture("Slider", TEXT("Slider.png"));
	LoadTexture("SliderBack", TEXT("SliderBack.png"));

	return true;
}

bool CResourceManager::CreateMesh(MESH_TYPE eType, const std::string& strName, 
	D3D_PRIMITIVE_TOPOLOGY eTop, void* pVtxData, int iVtxSz, int iVtxCnt, 
	D3D11_USAGE eVtxUsg, void* pIdxData, int iIdxSz, int iIdxCnt, 
	D3D11_USAGE eIdxUsg, DXGI_FORMAT eFmt)
{
	CMesh* pMesh = FindMesh(strName);

	if (pMesh)
	{
		SAFE_RELEASE(pMesh);
		return false;
	}
		
	switch (eType)
	{
	case MESH_TYPE::MT_2D:
		pMesh = new CMesh2D;
		break;
	case MESH_TYPE::MT_STATIC:
		pMesh = new CStaticMesh;
		break;
	case MESH_TYPE::MT_ANIMATION:
		break;
	}

	pMesh->SetName(strName);

	if (!pMesh->CreateMesh(eTop, pVtxData, iVtxSz, iVtxCnt, eVtxUsg, 
		pIdxData, iIdxSz, iIdxCnt, eIdxUsg, eFmt))
	{
		SAFE_RELEASE(pMesh);
		return false;
	}

	m_mapMesh.insert(std::make_pair(strName, pMesh));

	return true;
}

CMesh* CResourceManager::FindMesh(const std::string& strName)
{
	std::unordered_map<std::string, CMesh*>::iterator iter = m_mapMesh.find(strName);

	if (iter == m_mapMesh.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

void CResourceManager::ReleaseMesh(const std::string& strName)
{
	std::unordered_map<std::string, CMesh*>::iterator iter = m_mapMesh.find(strName);

	if (iter == m_mapMesh.end())
		return;
		
	if (iter->second->Release() == 0)
		m_mapMesh.erase(iter);
}

CMaterial* CResourceManager::CreateMaterial(const std::string& strName)
{
	CMaterial* pMaterial = FindMaterial(strName);
	
	if (pMaterial)
		return pMaterial;

	pMaterial = new CMaterial;

	pMaterial->SetName(strName);

	pMaterial->AddRef();

	m_mapMaterial.insert(std::make_pair(strName, pMaterial));

	return pMaterial;
}

CMaterial* CResourceManager::FindMaterial(const std::string& strName)
{
	std::unordered_map<std::string, CMaterial*>::iterator iter = m_mapMaterial.find(strName);

	if (iter == m_mapMaterial.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

void CResourceManager::ReleaseMaterial(const std::string& strName)
{
	std::unordered_map<std::string, CMaterial*>::iterator iter = m_mapMaterial.find(strName);

	if (iter == m_mapMaterial.end())
		return;

	if (iter->second->Release() == 0)
		m_mapMaterial.erase(iter);
}

bool CResourceManager::LoadTexture(const std::string& strTag, const TCHAR* pFileName, const std::string& strRootPath)
{
	CTexture* pTex = FindTexture(strTag);

	if (pTex)
	{
		SAFE_RELEASE(pTex);
		return false;
	}

	pTex = new CTexture;

	if (!pTex->LoadTexture(strTag, pFileName, strRootPath))
	{
		SAFE_RELEASE(pTex);
		return false;
	}

	m_mapTexture.insert(std::make_pair(strTag, pTex));

	return true;
}

bool CResourceManager::LoadTextureFromFullPath(const std::string& strTag, const TCHAR* pFullPath)
{
	CTexture* pTex = FindTexture(strTag);

	if (pTex)
	{
		SAFE_RELEASE(pTex);
		return true;
	}

	pTex = new CTexture;

	if (!pTex->LoadTextureFromFullPath(strTag, pFullPath))
	{
		SAFE_RELEASE(pTex);
		return false;
	}

	m_mapTexture.insert(std::make_pair(strTag, pTex));

	return true;
}

bool CResourceManager::LoadTexture(const std::string& strTag, const std::vector<const TCHAR*>& vecFileName, const std::string& strRootPath)
{
	CTexture* pTex = FindTexture(strTag);

	if (pTex)
	{
		SAFE_RELEASE(pTex);
		return true;
	}

	pTex = new CTexture;

	if (!pTex->LoadTexture(strTag, vecFileName, strRootPath))
	{
		SAFE_RELEASE(pTex);
		return false;
	}

	m_mapTexture.insert(std::make_pair(strTag, pTex));

	return true;
}

bool CResourceManager::LoadTextureFromFullPath(const std::string& strTag, const std::vector<const TCHAR*>& vecFullPath)
{
	CTexture* pTex = FindTexture(strTag);

	if (pTex)
	{
		SAFE_RELEASE(pTex);
		return true;
	}

	pTex = new CTexture;

	if (!pTex->LoadTextureFromFullPath(strTag, vecFullPath))
	{
		SAFE_RELEASE(pTex);
		return false;
	}

	m_mapTexture.insert(std::make_pair(strTag, pTex));

	return true;
}

void CResourceManager::ReleaseTexture(const std::string& strTag)
{
	std::unordered_map<std::string, CTexture*>::iterator iter = m_mapTexture.find(strTag);

	if (iter == m_mapTexture.end())
		return;

	if (iter->second->Release() == 0)
		m_mapTexture.erase(iter);
}

CTexture* CResourceManager::FindTexture(const std::string& strTag)
{
	std::unordered_map<std::string, CTexture*>::iterator iter = m_mapTexture.find(strTag);

	if (iter == m_mapTexture.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

CTexture* CResourceManager::LoadTexture(FILE* pFile)
{
	return nullptr;
}

bool CResourceManager::CreateAni2DSeq(const std::string& strTag, CTexture* pTex, float fMaxTime, float fPlayRate)
{
	CAnimation2DSequence* pSeq = FindAni2DSeq(strTag);

	if (pSeq)
	{
		SAFE_RELEASE(pSeq);
		return false;
	}

	pSeq = new CAnimation2DSequence;

	pSeq->SetName(strTag);

	if (!pSeq->Init(pTex, fMaxTime, fPlayRate))
	{
		SAFE_RELEASE(pSeq);
		return false;
	}

	m_mapAni2DSeq.insert(std::make_pair(strTag, pSeq));
	return true;
}

bool CResourceManager::CreateAni2DSeq(const std::string& strTag, const std::string& strKey, float fMaxTime, float fPlayRate)
{
	CAnimation2DSequence* pSeq = FindAni2DSeq(strTag);

	if (pSeq)
	{
		SAFE_RELEASE(pSeq);
		return false;
	}

	pSeq = new CAnimation2DSequence;

	pSeq->SetName(strTag);

	if (!pSeq->Init(strKey, fMaxTime, fPlayRate))
	{
		SAFE_RELEASE(pSeq);
		return false;
	}

	m_mapAni2DSeq.insert(std::make_pair(strTag, pSeq));
	return true;
}

bool CResourceManager::LoadAni2DSeq(const std::string& strTag, const TCHAR* pFileName, const std::string& strRootPath, float fMaxTime, float fPlayRate)
{
	CAnimation2DSequence* pSeq = FindAni2DSeq(strTag);

	if (pSeq)
	{
		SAFE_RELEASE(pSeq);
		return false;
	}

	pSeq = new CAnimation2DSequence;

	pSeq->SetName(strTag);

	if (!pSeq->Init(pFileName, strRootPath, fMaxTime, fPlayRate))
	{
		SAFE_RELEASE(pSeq);
		return false;
	}

	m_mapAni2DSeq.insert(std::make_pair(strTag, pSeq));

	return true;
}

void CResourceManager::SaveAni2DSeqFromFullPath(const std::string& strTag, const char* pFullPath)
{
	CAnimation2DSequence* pSeq = FindAni2DSeq(strTag);

	if (!pSeq)
		return;

	FILE* pFile = nullptr;

	fopen_s(&pFile, pFullPath, "wb");

	if (pFile)
	{
		pSeq->Save(pFile);

		fclose(pFile);
	}

	SAFE_RELEASE(pSeq);
}

void CResourceManager::AddFrame(const std::string& strTag, const Vector2& vStart, const Vector2& vEnd)
{
	CAnimation2DSequence* pSeq = FindAni2DSeq(strTag);

	if (!pSeq)
		return;

	pSeq->AddFrame(vStart, vEnd);

	SAFE_RELEASE(pSeq);
}

void CResourceManager::AddNotify(const std::string& strTag, const std::string& strNot, int iFrame)
{
	CAnimation2DSequence* pSeq = FindAni2DSeq(strTag);

	if (!pSeq)
		return;

	pSeq->AddNotify(strNot, iFrame);

	SAFE_RELEASE(pSeq);
}

void CResourceManager::AddNotify(const std::string& strTag, const std::string& strNot, float fTime)
{
	CAnimation2DSequence* pSeq = FindAni2DSeq(strTag);

	if (!pSeq)
		return;

	pSeq->AddNotify(strNot, fTime);

	SAFE_RELEASE(pSeq);
}

CAnimation2DSequence* CResourceManager::FindAni2DSeq(const std::string& strTag)
{
	std::unordered_map<std::string, CAnimation2DSequence*>::iterator iter = m_mapAni2DSeq.find(strTag);

	if (iter == m_mapAni2DSeq.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

void CResourceManager::ReleaseAni2DSeq(const std::string& strTag)
{
	std::unordered_map<std::string, CAnimation2DSequence*>::iterator iter = m_mapAni2DSeq.find(strTag);

	if (iter == m_mapAni2DSeq.end())
		return;

	if (iter->second->Release() == 0)
		m_mapAni2DSeq.erase(iter);
}

ID3D11SamplerState* CResourceManager::CreateSample(D3D11_FILTER eFilter)
{
	D3D11_SAMPLER_DESC tDesc = {};

	tDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	tDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	tDesc.MaxLOD = FLT_MAX;
	tDesc.MinLOD = -FLT_MAX;
	tDesc.Filter = eFilter;

	ID3D11SamplerState* pSampler = nullptr;

	DEVICE->CreateSamplerState(&tDesc, &pSampler);

	return pSampler;
}

CParticleSystem* CResourceManager::CreateParticle(const std::string& strTag, bool bDynamic, int iCount, int iSize,int iRegister, int iType)
{
	CParticleSystem* pParticle = FindParticle(strTag);

	if (pParticle)
	{
		SAFE_RELEASE(pParticle);
		return nullptr;
	}

	pParticle = new CParticleSystem;

	pParticle->SetName(strTag);

	if (!pParticle->Init( bDynamic, iCount, iSize, iRegister, iType))
	{
		SAFE_RELEASE(pParticle);
		return nullptr;
	}

	m_mapParticle.insert(std::make_pair(strTag, pParticle));

	pParticle->AddRef();

	return pParticle;
}

CParticleSystem* CResourceManager::FindParticle(const std::string& strTag)
{
	std::unordered_map<std::string, CParticleSystem*>::iterator iter = m_mapParticle.find(strTag);

	if (iter == m_mapParticle.end())
	{
		return nullptr;
	}

	iter->second->AddRef();

	return iter->second;
}

ID3D11ShaderReflectionVariable* InputA;
ID3D11ShaderReflectionVariable* InputB;
ID3D11ShaderReflectionVariable* Output;
