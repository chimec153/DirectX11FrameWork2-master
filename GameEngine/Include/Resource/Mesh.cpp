#include "Mesh.h"
#include "Material.h"
#include "ResourceManager.h"
#include "../Device.h"

CMesh::CMesh()	:
	m_ePrimitive(D3D_PRIMITIVE_TOPOLOGY_UNDEFINED)
	, m_pInstancingBuffer(nullptr)
{
	memset(&m_tVB, 0, sizeof(VertexBuffer));
	memset(&m_tIB, 0, sizeof(IndexBuffer));
}

CMesh::~CMesh()
{
	SAFE_RELEASE(m_tVB.pBuffer);
	SAFE_RELEASE(m_tIB.pBuffer);
}

bool CMesh::Init()
{
	return true;
}

bool CMesh::CreateInstancingBuffer(int iCount, int iSize)
{
	m_pInstancingBuffer = new INSTANCINGBUFFER;

	m_pInstancingBuffer->iCount = iCount;
	m_pInstancingBuffer->iSize = iSize;
	m_pInstancingBuffer->pData = new char[m_pInstancingBuffer->iCount * m_pInstancingBuffer->iSize];
	m_pInstancingBuffer->eUsg = D3D11_USAGE_DYNAMIC;

	D3D11_BUFFER_DESC tDesc = {};

	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	tDesc.ByteWidth = m_pInstancingBuffer->iCount * m_pInstancingBuffer->iSize;
	tDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tDesc.Usage = D3D11_USAGE_DYNAMIC;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, nullptr, &m_pInstancingBuffer->pBuffer)))
	{
		delete m_pInstancingBuffer;
		return false;
	}

	return true;
}

bool CMesh::CreateMesh(D3D_PRIMITIVE_TOPOLOGY eTop, void* pVtxData, int iVtxSz, int iVtxCnt, 
	D3D11_USAGE eVtxUsg,  void* pIdxData, int iIdxSz, int iIdxCnt, 
	D3D11_USAGE eIdxUsg, DXGI_FORMAT eFmt)
{
	return false;
}

bool CMesh::CreateParticleMesh(D3D_PRIMITIVE_TOPOLOGY eTop, void* pVtxData, 
	int iVtxSz, int iVtxCnt, D3D11_USAGE eVtxUsg, void* pIdxData, 
	int iIdxSz, int iIdxCnt, D3D11_USAGE eIdxUsg, DXGI_FORMAT eFmt)
{
	m_ePrimitive = eTop;

	m_tVB.iSize = iVtxSz;
	m_tVB.iCount = iVtxCnt;
	m_tVB.eUsage = eVtxUsg;

	D3D11_BUFFER_DESC tDesc = {};

	tDesc.ByteWidth = m_tVB.iSize * m_tVB.iCount;
	tDesc.Usage = eVtxUsg;
	tDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	tDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA tData = {};

	tData.pSysMem = pVtxData;

	if (FAILED(DEVICE->CreateBuffer(&tDesc, &tData, &m_tVB.pBuffer)))
		return false;

	return true;
}

void CMesh::Render(float fTime)
{
}

void CMesh::RenderInstancing(void* pData, int iCount, int iSize)
{
}

void CMesh::RenderParticle(int iCount)
{
}

void CMesh::Save(FILE* pFile)
{
	CRef::Save(pFile);

	fwrite(&m_tMax, sizeof(Vector3), 1, pFile);
	fwrite(&m_tMin, sizeof(Vector3), 1, pFile);
}

void CMesh::Load(FILE* pFile)
{
	CRef::Load(pFile);

	fread(&m_tMax, sizeof(Vector3), 1, pFile);
	fread(&m_tMin, sizeof(Vector3), 1, pFile);

}
