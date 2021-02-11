#include "Particle.h"
#include "Camera.h"
#include "../Device.h"
#include "../Resource/Mesh.h"
#include "../Resource/ComputeShader.h"
#include "../Resource/ShaderManager.h"
#include "../Resource/ResourceManager.h"
#include "../Resource/ParticleSystem.h"
#include "../Resource/Texture.h"
#include "../Resource/Material.h"

CParticle::CParticle()	:
	m_pShader(nullptr)
	, m_pUpdateShader(nullptr)
	, m_fSpawnTime(0.f)
	, m_fSpawnTimeLimit(0.2f)
	, m_tParticle()
	, m_tParticleShare()
	, m_tCBuffer()
	, m_pNoiseTex(nullptr)
	, m_tSpriteCBuffer()
	, m_iFrame(0)
	, m_iMaxFrame(0)
	, m_fFrameTime(0.f)
	, m_fFrameTimeMax(0.f)
	, m_vStart()
	, m_vEnd()
	, m_bStop()
	, m_iSpawnCount(-1)
{
	m_eSceneComponentClassType = SCENECOMPONENT_CLASS_TYPE::PARTICLE;
	memset(&m_tCBuffer, 0, sizeof(m_tCBuffer));
	m_tSpriteCBuffer.iImageType = (int)IMAGE_TYPE::IT_ARRAY;
}

CParticle::CParticle(const CParticle& com)	:
	CSceneComponent(com)
	, m_pShader(com.m_pShader)
	, m_pUpdateShader(com.m_pUpdateShader)
	, m_fSpawnTime(com.m_fSpawnTime)
	, m_fSpawnTimeLimit(com.m_fSpawnTimeLimit)
	, m_tParticle(com.m_tParticle)
	, m_tParticleShare(com.m_tParticleShare)
	, m_tCBuffer(com.m_tCBuffer)
	, m_pNoiseTex(com.m_pNoiseTex)
	, m_tSpriteCBuffer(com.m_tSpriteCBuffer)
	, m_iFrame(com.m_iFrame)
	, m_iMaxFrame(com.m_iMaxFrame)
	, m_fFrameTime(com.m_fFrameTime)
	, m_fFrameTimeMax(com.m_fFrameTimeMax)
	, m_vStart(com.m_vStart)
	, m_vEnd(com.m_vEnd)
	, m_bStop(com.m_bStop)
	, m_iSpawnCount(com.m_iSpawnCount)
{
	if (m_pShader)
	{
		m_pShader->AddRef();
	}

	if (m_pUpdateShader)
	{
		m_pUpdateShader->AddRef();
	}

	m_vecParticle.clear();

	size_t iSz = com.m_vecParticle.size();

	for (size_t i = 0; i < iSz; ++i)
	{
		m_vecParticle.push_back(com.m_vecParticle[i]->Clone());
	}

	if (m_pNoiseTex)
	{
		m_pNoiseTex->AddRef();
	}
}

CParticle::~CParticle()
{
	SAFE_RELEASE(m_pShader);
	SAFE_RELEASE(m_pUpdateShader);
	SAFE_RELEASE_VECLIST(m_vecParticle);
	SAFE_RELEASE(m_pNoiseTex);
}

void CParticle::AddParticle(const std::string& strTag)
{
	CParticleSystem* pParticle = GET_SINGLE(CResourceManager)->FindParticle(strTag);

	if (!pParticle)
		return;
	
	pParticle->CloneCBuffer(m_tCBuffer);

	CParticleSystem* pCloneSystem = pParticle->Clone();

	SAFE_RELEASE(pParticle);

	m_vecParticle.push_back(pCloneSystem);
}

void CParticle::SetSpawnLimit(float fTime)
{
	m_fSpawnTimeLimit = fTime;
}

void CParticle::SetUVStart(const Vector2& vStart)
{
	m_tSpriteCBuffer.iImageType = (int)IMAGE_TYPE::IT_ATLAS;
	m_tSpriteCBuffer.vStart = vStart;
	m_vStart = vStart;
}

void CParticle::SetUVEnd(const Vector2& vEnd)
{
	m_tSpriteCBuffer.iImageType = (int)IMAGE_TYPE::IT_ATLAS;
	m_tSpriteCBuffer.vEnd = vEnd;
	m_vEnd = vEnd;
}

void CParticle::SetUVSize(const Vector2& vSize)
{
	m_tSpriteCBuffer.iImageType = (int)IMAGE_TYPE::IT_ATLAS;
	m_tSpriteCBuffer.vSize = vSize;
}

void CParticle::SetUVStart(float x, float y)
{
	m_tSpriteCBuffer.iImageType = (int)IMAGE_TYPE::IT_ATLAS;
	m_tSpriteCBuffer.vStart = Vector2(x, y);
	m_vStart = Vector2(x, y);
}

void CParticle::SetUVEnd(float x, float y)
{
	m_tSpriteCBuffer.iImageType = (int)IMAGE_TYPE::IT_ATLAS;
	m_tSpriteCBuffer.vEnd = Vector2(x, y);
	m_vEnd = Vector2(x, y);
}

void CParticle::SetUVSize(float x, float y)
{
	m_tSpriteCBuffer.iImageType = (int)IMAGE_TYPE::IT_ATLAS;
	m_tSpriteCBuffer.vSize = Vector2(x,y);
}

void CParticle::SetAnim(int iMaxFrame, float fFrameTimeMax)
{
	m_iMaxFrame = iMaxFrame;
	m_fFrameTimeMax = fFrameTimeMax;
}

void CParticle::Stop()
{
	m_bStop = true;
}

void CParticle::Resume()
{
	m_bStop = false;
}

void CParticle::SetSpawnCount(int iCount)
{
	m_iSpawnCount = iCount;
}

bool CParticle::Init()
{
	if (!CSceneComponent::Init())
		return false;

	m_pUpdateShader = (CComputeShader*)GET_SINGLE(CShaderManager)->FindShader("ComputeShader");

	m_pMesh = GET_SINGLE(CResourceManager)->FindMesh("Particle");

	CMaterial* pMtrl = GET_SINGLE(CResourceManager)->FindMaterial("Particle");

	m_pMaterial = pMtrl->Clone();

	SAFE_RELEASE(pMtrl);

	m_pNoiseTex = GET_SINGLE(CResourceManager)->GetNoiseTex();

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
	GET_SINGLE(CShaderManager)->UpdateCBuffer("Sprite", &m_tSpriteCBuffer);

	if (!m_bStop)
	{
		m_fSpawnTime += fTime;

		if (m_fSpawnTime >= m_fSpawnTimeLimit)
		{
			m_fSpawnTime -= m_fSpawnTimeLimit;

			if (m_iSpawnCount == -1)
			{
				m_tCBuffer.iCount = 1;
			}

			else if (m_iSpawnCount > 0)
			{
				m_iSpawnCount--;
				m_tCBuffer.iCount = 1;
			}

			else
			{
				m_tCBuffer.iCount = 0;
			}
		}

		else
		{
			m_tCBuffer.iCount = 0;
		}
	}


	else
	{
		m_tCBuffer.iCount = 0;
	}

	m_tCBuffer.vPos = GetWorldPos();

	GET_SINGLE(CShaderManager)->UpdateCBuffer("Particle", &m_tCBuffer);

	size_t iSz = m_vecParticle.size();

	for (size_t i = 0; i < iSz; ++i)
	{
		m_vecParticle[i]->SetShader((int)i);
	}

	m_pUpdateShader->SetShader();

	int iX = ((m_tCBuffer.iMaxCount - 1) / 64) + 1;

	if (iX > 65535)
	{
		iX = 0;
	}

	m_pUpdateShader->Dispatch(iX, 1, 1);

	m_pUpdateShader->Clear();

	for (size_t i = 0; i < iSz; ++i)
	{
		m_vecParticle[i]->ResetShader((int)i);
	}

	for (size_t i = 0; i < iSz; ++i)
	{
		m_vecParticle[i]->SetShader((int)i + 30, (int)SHADER_CBUFFER_TYPE::CBUFFER_GEOMETRY);
	}

	CSceneComponent::Render(fTime);

	m_pMesh->RenderParticle(m_tCBuffer.iMaxCount );

	for (size_t i = 0; i < iSz; ++i)
	{
		m_vecParticle[i]->ResetShader((int)i + 30, (int)SHADER_CBUFFER_TYPE::CBUFFER_GEOMETRY);
	}
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

void CParticle::SpawnWindow()
{
	if (ImGui::Begin("Particles"))
	{
		ImGui::Text(GetName().c_str());
		bool bEnable = IsEnable();
		ImGui::Checkbox("Enable", &bEnable);
		Enable(bEnable);
		ImGui::SliderFloat3("Pos", &m_tCBuffer.vPos.x, 0.f, 5000.f);
		ImGui::SliderFloat3("StartScale", &m_tCBuffer.vStartScale.x, 0.f, 320.f);
		ImGui::SliderFloat3("EndScale", &m_tCBuffer.vEndScale.x, 0.f, 320.f);
		ImGui::ColorPicker4("StartColor", &m_tCBuffer.vStartColor.x);
		ImGui::ColorPicker4("EndColor", &m_tCBuffer.vEndColor.x);
		ImGui::SliderFloat("MaxSpeed", &m_tCBuffer.fMaxSpeed, 0.f, 320.f);
		ImGui::SliderFloat("MinSpeed", &m_tCBuffer.fMinSpeed, 0.f, 320.f);
		ImGui::SliderFloat("MaxLifeTime", &m_tCBuffer.fMaxLifeTime, 0.f, 20.f);
		ImGui::SliderFloat("MinLifeTime", &m_tCBuffer.fMinLifeTime, 0.f, 20.f);
		ImGui::SliderAngle("TurnAngle", &m_tCBuffer.fTurnAngle, -1440.f, 1440.f);
		ImGui::SliderFloat3("Range", &m_tCBuffer.vRange.x, 0.f, 512.f);
		ImGui::SliderFloat3("MoveDir", &m_tCBuffer.vMoveDir.x, -1.f, 1.f);
		ImGui::SliderFloat("Acceleration", &m_tCBuffer.vAcc.x, -256.f, 256.f);
		int iMaxCount = (int)m_tCBuffer.iMaxCount;
		ImGui::SliderInt("MaxCount", &iMaxCount, 0, 512);
		m_tCBuffer.iMaxCount = (unsigned int)iMaxCount;
		int iCount = (int)m_tCBuffer.iCount;
		ImGui::SliderInt("Count", &iCount, 0, 64);
		m_tCBuffer.iCount = (unsigned int)iCount;
		ImGui::SliderFloat("LimitTime", &m_fSpawnTimeLimit, 0.f, 20.f);
		ImGui::SliderInt("Move", &m_tCBuffer.iMove, 0, 1);
		ImGui::SliderInt("Frame", &m_iFrame, 0, 1);
		ImGui::SliderInt("MaxFrame", &m_iMaxFrame, 0, 1);
		ImGui::SliderFloat2("StartFrame", &m_tSpriteCBuffer.vStart.x, 0, 512);
		ImGui::SliderFloat2("EndFrame", &m_tSpriteCBuffer.vEnd.x, 0, 512);
		ImGui::SliderFloat2("FrameSize", &m_tSpriteCBuffer.vSize.x, 0, 512);
	}
	ImGui::End();
}
