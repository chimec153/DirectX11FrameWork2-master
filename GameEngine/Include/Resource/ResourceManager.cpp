#include "ResourceManager.h"
#include "Mesh2D.h"
#include "ShaderManager.h"
#include "Material.h"
#include "Texture.h"
#include "../Device.h"
#include "Animation2DSequence.h"
#include "StaticMesh.h"
#include <iostream>
#include <fstream>

DEFINITION_SINGLE(CResourceManager)

CResourceManager::CResourceManager()	:
	m_pDefaultMesh(nullptr),
	m_pDefaultUIMesh(nullptr),
	m_pDefaultTex(nullptr),
	m_pPoint(nullptr),
	m_pLinear(nullptr),
	m_pAnisotropic(nullptr)
{

}

CResourceManager::~CResourceManager()
{
	SAFE_RELEASE(m_pDefaultMesh);
	SAFE_RELEASE(m_pDefaultUIMesh);
	SAFE_RELEASE(m_pDefaultTex);
	SAFE_RELEASE_MAP(m_mapMesh);
	SAFE_RELEASE_MAP(m_mapMaterial);
	SAFE_RELEASE_MAP(m_mapAni2DSeq);
	SAFE_RELEASE_MAP(m_mapTexture);
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

CTexture* CResourceManager::GetDTex() const
{
	if (m_pDefaultTex)
		m_pDefaultTex->AddRef();

	return m_pDefaultTex;
}

bool CResourceManager::Init()
{
	// Shader Manager 초기화
	if (!GET_SINGLE(CShaderManager)->Init())
		return false;

	m_pDefaultMesh = new CMesh2D;

	m_pDefaultMesh->SetName("DefaultMesh");

	if (!m_pDefaultMesh->CreateMesh())
		return false;

	CMaterial* pMaterial = CreateMaterial("Color");

	pMaterial->SetShader("Standard2D");

	m_pDefaultMesh->SetMaterial(pMaterial);

	SAFE_RELEASE(pMaterial);

	m_pDefaultUIMesh = new CMesh2D;

	m_pDefaultUIMesh->SetName("DefaultUIMesh");

	if (!m_pDefaultUIMesh->CreateMesh())
		return false;

	CMaterial* pMtrl = CreateMaterial("UI2D");

	pMtrl->SetShader("UI2D");

	m_pDefaultUIMesh->SetMaterial(pMtrl);

	SAFE_RELEASE(pMtrl);

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

	pColMat->SetShader("Collider");

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

	Vector3 vCircle[361] = {};

	for (int i = 0; i < 361;++i)
	{
		float fAngle = DirectX::XMConvertToRadians((float)i);

		vCircle[i] = Vector3(cosf(fAngle) * 0.5f, sinf(fAngle) * 0.5f, 0.f);
	}

	CreateMesh(MESH_TYPE::MT_2D, "Circle2D", D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, 
		vCircle, sizeof(Vector3), sizeof(vCircle) / sizeof(vCircle[0]), D3D11_USAGE_IMMUTABLE);

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

//	연습용 코드

ID3D11ShaderResourceView* CResourceManager::CreateRandomTexture1DSRV(ID3D11Device* device)
{
	XMFLOAT4 randomValues[1024];

	for (int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].y = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].z = MathHelper::RandF(-1.0f, 1.0f);
		randomValues[i].w = MathHelper::RandF(-1.0f, 1.0f);
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024 * sizeof(XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	D3D11_TEXTURE1D_DESC texDesc;
	texDesc.Width = 1024;
	texDesc.MipLevels = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;

	ID3D11Texture1D* randomTex = 0;

	if (FAILED(DEVICE->CreateTexture1D(&texDesc, &initData, &randomTex)))
	{
		return nullptr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	ID3D11ShaderResourceView* randomTexSRV = 0;
	if (FAILED(DEVICE->CreateShaderResourceView(randomTex, &viewDesc, &randomTexSRV)))
	{
		SAFE_RELEASE(randomTex);
	}

	return randomTexSRV;
}

void CResourceManager::CreateBufferStreamOutput()
{

	struct Vertex
	{

	};

#define MAX_VERTICES 10;

	ID3D11Buffer* mStreamOutVB = nullptr;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex) * MAX_VERTICES;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	if (FAILED(DEVICE->CreateBuffer(&vbd, 0, &mStreamOutVB)))
	{
		return;
	}
}

void CResourceManager::ReleaseVBSO()
{
	UINT offset = 0;

	ID3D11Buffer* bufferArray[1] = { 0 };
	CONTEXT->SOSetTargets(1, bufferArray, &offset);
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

ID3D11ShaderReflectionVariable* InputA;
ID3D11ShaderReflectionVariable* InputB;
ID3D11ShaderReflectionVariable* Output;

void CResourceManager::CreateBufferComputeShader()
{
	InputA = mFX->GetVariableByName("gInputA")->AsShaderResource();
	InputB = mFX->GetVariableByName("gInputB")->AsShaderResource();
	Output = mFX->GetVariableByName("gOutput")->AsUnorderedAccessView();

	D3D11_BUFFER_DESC inputDesc;
	inputDesc.Usage = D3D11_USAGE_DEFAULT;
	inputDesc.ByteWidth = sizeof(Data) * mNumElements;
	inputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	inputDesc.CPUAccessFlags = 0;
	inputDesc.StructureByteStride = sizeof(Data);
	inputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA vinitDataA;
	vinitDataA.pSysMem = &dataA[0];

	ID3D11Buffer* bufferA = 0;

	if (FAILED(DEVICE->CreateBuffer(&inputDesc, &vinitDataA, &bufferA)))
	{
		return;
	}

	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = sizeof(Data) * mNumElements;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = sizeof(Data);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	ID3D11Buffer* mOutputBuffer;
	if (FAILED(DEVICE->CreateBuffer(&outputDesc, 0, &mOutputBuffer)))
	{
		return;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = mNumElements;

	DEVICE->CreateShaderResourceView(bufferA, &srvDesc, &mInputASRV);
	DEVICE->CreateShaderResourceView(bufferB, &srvDesc, &mInputBSRV);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = mNumElements;

	DEVICE->CreateUnorderedAccessView(mOutputBuffer, &uavDesc, &mOutputUAV);
}

void CResourceManager::SetInputA(ID3D11ShaderResourceView* srv)
{
	InputA->SetResource(srv);
}

void CResourceManager::SetInputB(ID3D11ShaderResourceView* srv)
{
	InputB->SetResource(srv);
}

void CResourceManager::SetOutput(ID3D11UnorderedAccessView* uav)
{
	Output->SetUnorderedAccessView(uav);
}

void CResourceManager::CreateBufferCS()
{
	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.ByteWidth = sizeof(Data) * mNumElements;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	outputDesc.StructureByteStride = sizeof(Data);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	ID3D11Buffer* mOutputDebugBuffer;
	if (FAILED(DEVICE->CreateBuffer(&outputDesc, 0, &mOutputDebugBuffer)))
	{
		return;
	}

	CONTEXT->CopyResource(mOutputDebugBuffer, mOutputBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedData;
	CONTEXT->Map(mOutputDebugBuffer, 0, D3D11_MAP_READ, 0, &mappedData);
	Data* dataView = reinterpret_cast<Data*>(mappedData.pData);

	std::ofstream fout;

	for (int i = 0; i < mNumElements; ++i)
	{
		fout << "(" << dataView[i].v1.x << ", " <<
			dataView[i].v1.y << ", " <<
			dataView[i].v1.z << ", " <<
			dataView[i].v2.x << ", " <<
			dataView[i].v2.y << ") " << std::endl;
	}

	CONTEXT->Unmap(mOutputDebugBuffer, 0);

	fout.close();

	std::vector<Data> dataA(mNumElements);
	std::vector<Data> dataB(mNumElements);

	for (int i = 0; i < mNumElements; ++i)
	{
		dataA[i].v1 = XMFLOAT3(i, i, i);
		dataA[i].v2 = XMFLOAT2(i, 0);

		dataB[i].v1 = XMFLOAT3(-i, i, 0.0f);
		dataB[i].v2 = XMFLOAT2(0, -i);
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	uavDesc.Buffer.NumElements = mNumElements;
}

// 연습용 코드
