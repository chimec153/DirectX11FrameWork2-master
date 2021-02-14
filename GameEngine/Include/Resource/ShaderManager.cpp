#include "ShaderManager.h"
#include "Shader.h"
#include "GraphicShader.h"
#include "../Device.h"
#include "ComputeShader.h"

DEFINITION_SINGLE(CShaderManager)

CShaderManager::CShaderManager()	:
	m_iInputSize(0)
	, m_pBuffer(nullptr)
	, m_pInputSRV(nullptr)
	, m_pOutputBuffer(nullptr)
	, m_pOutputUAV(nullptr)
{

}

CShaderManager::~CShaderManager()
{
	SAFE_RELEASE_MAP(m_mapShader);
	SAFE_RELEASE_MAP(m_mapLayout);

	auto iter = m_mapCBuffer.begin();
	auto iterEnd = m_mapCBuffer.end();

	for (; iter != iterEnd; ++iter)
		SAFE_RELEASE(iter->second->pBuffer);

	SAFE_DELETE_MAP(m_mapCBuffer);

	SAFE_RELEASE(m_pBuffer);
	SAFE_RELEASE(m_pInputSRV);
	SAFE_RELEASE(m_pOutputBuffer);
	SAFE_RELEASE(m_pOutputUAV);
}

bool CShaderManager::Init()
{
	//	gloval shader √ ±‚»≠
	CGraphicShader* pShader = CreateShader<CGraphicShader>("Standard2D");

	if (!LoadVertexShader("Standard2D", "StandardVS", TEXT("VertexShader.fx")))
		return false;

	if (!LoadPixelShader("Standard2D", "StandardPS", TEXT("VertexShader.fx")))
		return false;

	pShader->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pShader->AddInputLayoutDesc("TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pShader->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (!pShader->CreateInputLayout())
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	SAFE_RELEASE(pShader);

	pShader = CreateShader<CGraphicShader>("Sprite");

	if (!LoadVertexShader("Sprite", "StandardSpriteVS", TEXT("VertexShader.fx")))
		return false;

	if (!LoadPixelShader("Sprite", "StandardSpritePS", TEXT("VertexShader.fx")))
		return false;

	pShader->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pShader->AddInputLayoutDesc("TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pShader->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (!pShader->CreateInputLayout())
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	SAFE_RELEASE(pShader);

	pShader = CreateShader<CGraphicShader>("Collider");

	if (!LoadVertexShader("Collider", "ColliderVS", TEXT("Collider.fx")))
		return false;

	if (!LoadPixelShader("Collider", "ColliderPS", TEXT("Collider.fx")))
		return false;

	pShader->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (!pShader->CreateInputLayout())
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	SAFE_RELEASE(pShader);

	pShader = CreateShader<CGraphicShader>("UI2D");

	if (!LoadVertexShader("UI2D", "VS", TEXT("UI.fx")))
		return false;

	if (!LoadPixelShader("UI2D", "PS", TEXT("UI.fx")))
		return false;

	pShader->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pShader->AddInputLayoutDesc("TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pShader->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (!pShader->CreateInputLayout())
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	SAFE_RELEASE(pShader);

	pShader = CreateShader<CGraphicShader>("Instance2D");

	if (!LoadVertexShader("Instance2D", "StandardVS", TEXT("VertexShader.fx")))
		return false;

	if (!LoadPixelShader("Instance2D", "StandardPS", TEXT("VertexShader.fx")))
		return false;

	pShader->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_INSTANCE_DATA, 0);
	pShader->AddInputLayoutDesc("TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_INSTANCE_DATA, 0);
	pShader->AddInputLayoutDesc("WORLD", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("WORLD", 1,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("WORLD", 2,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("WORLD", 3,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);

	if (!pShader->CreateInputLayout())
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	SAFE_RELEASE(pShader);

	pShader = CreateShader<CGraphicShader>("Bar");

	if (!pShader->LoadVertexShader("BarVS", TEXT("UI.fx")))
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	if (!pShader->LoadPixelShader("BarPS", TEXT("UI.fx")))
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	pShader->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pShader->AddInputLayoutDesc("TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pShader->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (!pShader->CreateInputLayout())
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	SAFE_RELEASE(pShader);

	pShader = CreateShader<CGraphicShader>("Inst");

	if (!pShader->LoadVertexShader("VS_INST", TEXT("Inst.fx")))
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	if (!pShader->LoadPixelShader("PS_INST", TEXT("Inst.fx")))
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	pShader->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pShader->AddInputLayoutDesc("TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pShader->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pShader->AddInputLayoutDesc("WORLD", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("WORLD", 1,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("WORLD", 2,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("WORLD", 3,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("PIVOT", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 1, 12,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("SIZE", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 1, 12,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("TEXSIZE", 0,
		DXGI_FORMAT_R32G32_FLOAT, 1, 8,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("START", 0,
		DXGI_FORMAT_R32G32_FLOAT, 1, 8,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pShader->AddInputLayoutDesc("END", 0,
		DXGI_FORMAT_R32G32_FLOAT, 1, 8,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);

	if (!pShader->CreateInputLayout())
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	SAFE_RELEASE(pShader);

	CGraphicShader* pInstCollider = CreateShader<CGraphicShader>("InstCollider");

	if (!pInstCollider->LoadVertexShader("VS_INST_COLLIDER", TEXT("Inst.fx")))
	{
		SAFE_RELEASE(pInstCollider);
		return false;
	}

	if (!pInstCollider->LoadPixelShader("PS_INST_COLLIDER", TEXT("Inst.fx")))
	{
		SAFE_RELEASE(pInstCollider);
		return false;
	}

	pInstCollider->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pInstCollider->AddInputLayoutDesc("WORLD", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pInstCollider->AddInputLayoutDesc("WORLD", 1,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pInstCollider->AddInputLayoutDesc("WORLD", 2,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pInstCollider->AddInputLayoutDesc("WORLD", 3,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pInstCollider->AddInputLayoutDesc("PIVOT", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 1, 12,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pInstCollider->AddInputLayoutDesc("SIZE", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 1, 12,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);
	pInstCollider->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16,
		D3D11_INPUT_PER_INSTANCE_DATA, 1);

	if (!pInstCollider->CreateInputLayout())
	{
		SAFE_RELEASE(pInstCollider);
		return false;
	}

	SAFE_RELEASE(pInstCollider);

	CGraphicShader* pShadow = CreateShader<CGraphicShader>("Shadow");

	if (!pShadow->LoadVertexShader("StandardSpriteVS", TEXT("VertexShader.fx")))
	{
		SAFE_RELEASE(pShadow);
		return false;
	}

	if (!pShadow->LoadPixelShader("ShadowPS", TEXT("VertexShader.fx")))
	{
		SAFE_RELEASE(pShadow);
		return false;
	}

	pShadow->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pShadow->AddInputLayoutDesc("TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pShadow->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (!pShadow->CreateInputLayout())
	{
		SAFE_RELEASE(pShadow);
		return false;
	}

	SAFE_RELEASE(pShadow);

	CGraphicShader* pParticle = CreateShader<CGraphicShader>("ParticleShader");

	pParticle->LoadVertexShader("VS", TEXT("Particle.fx"));
	pParticle->LoadGeometryShader("GS", TEXT("Particle.fx"));
	pParticle->LoadPixelShader("PS", TEXT("Particle.fx"));

	pParticle->AddInputLayoutDesc("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pParticle->AddInputLayoutDesc("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pParticle->AddInputLayoutDesc("COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pParticle->CreateInputLayout();

	SAFE_RELEASE(pParticle);

	CComputeShader* pComputeShader = CreateShader<CComputeShader>("ComputeShader");

	pComputeShader->LoadComputeShader("ParticleMain", TEXT("Particle.fx"));	

	SAFE_RELEASE(pComputeShader);

	CGraphicShader* pCircle2CylinderShader = CreateShader<CGraphicShader>("C2CShader");

	if (!pCircle2CylinderShader->LoadVertexShader("VS", TEXT("Geo.fx")))
	{
		return false;
	}

	if (!pCircle2CylinderShader->LoadGeometryShader("GS", TEXT("Geo.fx")))
	{
		return false;
	}

	if (!pCircle2CylinderShader->LoadPixelShader("PS", TEXT("Geo.fx")))
	{
		return false;
	}

	pCircle2CylinderShader->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pCircle2CylinderShader->AddInputLayoutDesc("SIZE", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pCircle2CylinderShader->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	if (!pCircle2CylinderShader->CreateInputLayout())
	{
		return false;
	}

	SAFE_RELEASE(pCircle2CylinderShader);

	CGraphicShader* pLineShader = CreateShader<CGraphicShader>("LineShader");

	if (!pLineShader->LoadVertexShader("LineVS", L"Line.fx"))
	{
		return false;
	}

	if (!pLineShader->LoadGeometryShader("LineGS", L"Line.fx"))
	{
		return false;
	}

	if (!pLineShader->LoadPixelShader("LinePS", L"Line.fx"))
	{
		return false;
	}

	pLineShader->AddInputLayoutDesc("POSITION", 
		0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12, 
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pLineShader->CreateInputLayout();

	SAFE_RELEASE(pLineShader);

	D3D11_SO_DECLARATION_ENTRY pEntry[5] = {};

	pEntry[0].SemanticName = "POSITION";
	pEntry[0].SemanticIndex = 0;
	pEntry[0].StartComponent = 0;
	pEntry[0].ComponentCount = 3;
	pEntry[0].OutputSlot = 0;
	pEntry[0].Stream = 0;

	pEntry[1].SemanticName = "VELOCITY";
	pEntry[1].SemanticIndex = 0;
	pEntry[1].StartComponent = 0;
	pEntry[1].ComponentCount = 3;
	pEntry[1].OutputSlot = 0;
	pEntry[1].Stream = 0;

	pEntry[2].SemanticName = "SIZE";
	pEntry[2].SemanticIndex = 0;
	pEntry[2].StartComponent = 0;
	pEntry[2].ComponentCount = 2;
	pEntry[2].OutputSlot = 0;
	pEntry[2].Stream = 0;

	pEntry[3].SemanticName = "AGE";
	pEntry[3].SemanticIndex = 0;
	pEntry[3].StartComponent = 0;
	pEntry[3].ComponentCount = 1;
	pEntry[3].OutputSlot = 0;
	pEntry[3].Stream = 0;

	pEntry[4].SemanticName = "TYPE";
	pEntry[4].SemanticIndex = 0;
	pEntry[4].StartComponent = 0;
	pEntry[4].ComponentCount = 1;
	pEntry[4].OutputSlot = 0;
	pEntry[4].Stream = 0;

	/*CGraphicShader* pUpdateShader = CreateShader<CGraphicShader>("ParticleUpdate");

	pUpdateShader->LoadVertexShader("VS", L"Particle");

	pUpdateShader->LoadGeometryShaderWithSO("GS", pEntry, 5, sizeof(VERTEX), 1, L"Particle");

	SAFE_RELEASE(pUpdateShader);*/

	CGraphicShader* pAngleShader = CreateShader<CGraphicShader>("AngleShader");

	pAngleShader->LoadVertexShader("VS", L"Line.fx");
	pAngleShader->LoadPixelShader("PS", L"Line.fx");

	pAngleShader->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pAngleShader->AddInputLayoutDesc("TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pAngleShader->CreateInputLayout();

	SAFE_RELEASE(pAngleShader);

	CGraphicShader* pGray = CreateShader<CGraphicShader>("GrayShader");

	pGray->LoadVertexShader("StandardSpriteVS", TEXT("VertexShader.fx"));
	pGray->LoadPixelShader("GrayPS", TEXT("VertexShader.fx"));

	pGray->AddInputLayoutDesc("POSITION", 0,
		DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pGray->AddInputLayoutDesc("TEXCOORD", 0,
		DXGI_FORMAT_R32G32_FLOAT, 0, 8,
		D3D11_INPUT_PER_VERTEX_DATA, 0);
	pGray->AddInputLayoutDesc("COLOR", 0,
		DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16,
		D3D11_INPUT_PER_VERTEX_DATA, 0);

	pGray->CreateInputLayout();

	SAFE_RELEASE(pGray);

	CreateCBuffer("Transform", sizeof(TransformCBuffer), 0, 
		(int)SHADER_CBUFFER_TYPE::CBUFFER_VERTEX
		|(int)SHADER_CBUFFER_TYPE::CBUFFER_GEOMETRY);
	CreateCBuffer("Material", sizeof(ShaderCBuffer), 1,
		(int)SHADER_CBUFFER_TYPE::CBUFFER_PIXEL);
	CreateCBuffer("Sprite", sizeof(SpriteCBuffer), 2, 
		(int)SHADER_CBUFFER_TYPE::CBUFFER_VERTEX
		| (int)SHADER_CBUFFER_TYPE::CBUFFER_PIXEL
		|(int)SHADER_CBUFFER_TYPE::CBUFFER_GEOMETRY);
	CreateCBuffer("Global", sizeof(GLOBALCBUFFER), 3, 
		(int)SHADER_CBUFFER_TYPE::CBUFFER_COMPUTE);
	CreateCBuffer("Bar", sizeof(BARCBUFFER), 10,
		(int)SHADER_CBUFFER_TYPE::CBUFFER_PIXEL);
	CreateCBuffer("Particle", sizeof(PARTICLECBUFFER), 11,
		(int)SHADER_CBUFFER_TYPE::CBUFFER_ALL);
	CreateCBuffer("ParticleSO", sizeof(CBPERFRAME), 12,
		(int)SHADER_CBUFFER_TYPE::CBUFFER_GEOMETRY
		| (int)SHADER_CBUFFER_TYPE::CBUFFER_COMPUTE);

	CreateCBuffer("Fade", sizeof(FADE), 10);

	CreateCBuffer("Angle", sizeof(ANGLE), 12, (int)SHADER_CBUFFER_TYPE::CBUFFER_GEOMETRY);

	FADE tFade = {};

	tFade.vFade = Vector4(1.f, 1.f, 1.f, 1.f);
	tFade.fGray = 0.f;

	UpdateCBuffer("Fade", (void*)&tFade);

	return true;
}

bool CShaderManager::CreateLayout(const std::string& strName)
{
	CShader* pShader = FindShader(strName);

	if (!pShader)
		return false;

	bool bResult = pShader->CreateInputLayout();

	SAFE_RELEASE(pShader);

	return bResult;
}

bool CShaderManager::CreateInputDesc(const std::string& strName, const char* pSemanticName, UINT iSemanticIndex, 
	DXGI_FORMAT eFormat, UINT iInputSlot, UINT iSize, 
	D3D11_INPUT_CLASSIFICATION eInputSlotClass, UINT iInstanceDataStepRate)
{
	CShader* pShader = FindShader(strName);

	if (!pShader)
		return false;

	bool bResult = pShader->AddInputLayoutDesc(pSemanticName, iSemanticIndex,
		eFormat, iInputSlot, iSize, eInputSlotClass, iInstanceDataStepRate);

	SAFE_RELEASE(pShader);

	return bResult;
}

CShader* CShaderManager::FindShader(const std::string& strName)
{
	std::unordered_map<std::string, CShader*>::iterator iter = m_mapShader.find(strName);

	if (iter == m_mapShader.end())
		return nullptr;		

	iter->second->AddRef();

	return iter->second;
}

void CShaderManager::ReleaseShader(const std::string& strName)
{
	std::unordered_map<std::string, CShader*>::iterator iter = m_mapShader.find(strName);

	if (iter == m_mapShader.end())
		return;

	if (iter->second->Release() == 0)
		m_mapShader.erase(iter);
}

ID3D11InputLayout* CShaderManager::FindLayout(const std::string& strName)
{
	std::unordered_map<std::string, ID3D11InputLayout*>::iterator iter = m_mapLayout.find(strName);

	if (iter == m_mapLayout.end())
		return nullptr;

	iter->second->AddRef();

	return iter->second;
}

void CShaderManager::ReleaseLayout(const std::string& strName)
{
	std::unordered_map<std::string, ID3D11InputLayout*>::iterator iter = m_mapLayout.find(strName);

	if (iter == m_mapLayout.end())
		return;

	iter->second->Release();
	m_mapLayout.erase(iter);
}

bool CShaderManager::CreateCBuffer(const std::string& strTag, int iSize, int iRegister, int iType)
{
	PCBuffer pBuffer = FindCBuffer(strTag);

	if (pBuffer)
		return false;

	pBuffer = new CBuffer;

	pBuffer->iSize = iSize;
	pBuffer->iType = iType;
	pBuffer->iRegister = iRegister;

	D3D11_BUFFER_DESC tDesc = {};

	tDesc.ByteWidth = iSize;
	tDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, nullptr, &pBuffer->pBuffer)))
	{
		SAFE_DELETE(pBuffer);	
		return false;
	}	

	m_mapCBuffer.insert(std::make_pair(strTag, pBuffer));

	return true;
}

bool CShaderManager::UpdateCBuffer(const std::string& strTag, void* pData)
{
	PCBuffer pBuffer = FindCBuffer(strTag);

	if (!pBuffer)
		return false;

	D3D11_MAPPED_SUBRESOURCE tMap = {};

	CONTEXT->Map(pBuffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &tMap);

	memcpy(tMap.pData, pData, pBuffer->iSize);	

	CONTEXT->Unmap(pBuffer->pBuffer, 0);

	if (pBuffer->iType & (int)SHADER_CBUFFER_TYPE::CBUFFER_VERTEX)
		CONTEXT->VSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	if (pBuffer->iType & (int)SHADER_CBUFFER_TYPE::CBUFFER_PIXEL)
		CONTEXT->PSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	if (pBuffer->iType & (int)SHADER_CBUFFER_TYPE::CBUFFER_HULL)
		CONTEXT->HSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	if (pBuffer->iType & (int)SHADER_CBUFFER_TYPE::CBUFFER_DOMAIN)
		CONTEXT->DSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	if (pBuffer->iType & (int)SHADER_CBUFFER_TYPE::CBUFFER_GEOMETRY)
		CONTEXT->GSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	if (pBuffer->iType & (int)SHADER_CBUFFER_TYPE::CBUFFER_COMPUTE)
		CONTEXT->CSSetConstantBuffers(pBuffer->iRegister, 1, &pBuffer->pBuffer);

	return true;
}

PCBuffer CShaderManager::FindCBuffer(const std::string& strTag)
{
	std::unordered_map<std::string, PCBuffer>::iterator iter = m_mapCBuffer.find(strTag);

	if (iter == m_mapCBuffer.end())
		return nullptr;

	return iter->second;
}


bool CShaderManager::LoadVertexShader(const std::string& strName, const char* pEntryName, const TCHAR* pFileName, const std::string& strRootPath)
{
	CShader* pShader = FindShader(strName);

	if (!pShader)
		return false;

	else if (pShader->GetShaderType() == SHADER_TYPE::ST_COMPUTE)
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	bool bResult = ((CGraphicShader*)pShader)->LoadVertexShader(pEntryName, pFileName, strRootPath);

	SAFE_RELEASE(pShader);

	return bResult;
}

bool CShaderManager::LoadPixelShader(const std::string& strName, const char* pEntryName, const TCHAR* pFileName, const std::string& strRootPath)
{
	CShader* pShader = FindShader(strName);

	if (!pShader)
		return false;

	else if (pShader->GetShaderType() == SHADER_TYPE::ST_COMPUTE)
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	bool bResult = ((CGraphicShader*)pShader)->LoadPixelShader(pEntryName, pFileName, strRootPath);

	SAFE_RELEASE(pShader);

	return bResult;
}

bool CShaderManager::LoadHullShader(const std::string& strName, const char* pEntryName, const TCHAR* pFileName, const std::string& strRootPath)
{
	CShader* pShader = FindShader(strName);

	if (!pShader)
		return false;

	else if (pShader->GetShaderType() == SHADER_TYPE::ST_COMPUTE)
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	bool bResult = ((CGraphicShader*)pShader)->LoadHullShader(pEntryName, pFileName, strRootPath);

	SAFE_RELEASE(pShader);

	return bResult;
}

bool CShaderManager::LoadGeometryShader(const std::string& strName, const char* pEntryName, const TCHAR* pFileName, const std::string& strRootPath)
{
	CShader* pShader = FindShader(strName);

	if (!pShader)
		return false;

	else if (pShader->GetShaderType() == SHADER_TYPE::ST_COMPUTE)
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	bool bResult = ((CGraphicShader*)pShader)->LoadGeometryShader(pEntryName, pFileName, strRootPath);

	SAFE_RELEASE(pShader);

	return bResult;
}

bool CShaderManager::LoadGeometryShaderWithSO(const std::string& strName, const char* pEntryName, 
	D3D11_SO_DECLARATION_ENTRY* pEntry, UINT iNumEntries, UINT iStride, UINT iNumStrides, 
	const TCHAR* pFileName, const std::string& strRootName)
{
	CShader* pShader = FindShader(strName);

	if (!pShader)
		return false;

	else if (pShader->GetShaderType() == SHADER_TYPE::ST_COMPUTE)
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	bool bResult = ((CGraphicShader*)pShader)->LoadGeometryShaderWithSO(pEntryName, pEntry, 
		iNumEntries, iStride, iNumStrides, pFileName, strRootName);

	SAFE_RELEASE(pShader);

	return bResult;
}

bool CShaderManager::LoadDomainShader(const std::string& strName, const char* pEntryName, const TCHAR* pFileName, const std::string& strRootPath)
{
	CShader* pShader = FindShader(strName);

	if (!pShader)
		return false;

	else if (pShader->GetShaderType() == SHADER_TYPE::ST_COMPUTE)
	{
		SAFE_RELEASE(pShader);
		return false;
	}

	bool bResult = ((CGraphicShader*)pShader)->LoadDomainShader(pEntryName, pFileName, strRootPath);

	SAFE_RELEASE(pShader);

	return bResult;
}
