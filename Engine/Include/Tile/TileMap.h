#pragma once
#include "../Component/SceneComponent.h"

typedef struct _tagTileAnim
{
	std::vector<int> vTexIndexs;
	float	fTime;
}TILEANIM, *PTILEANIM;

class CTileMap :
	public CSceneComponent
{
	friend class CObj;

private:
	CTileMap();
	CTileMap(const CTileMap& map);
	virtual ~CTileMap();

private:
	int							m_iCountX;
	int							m_iCountY;
	Vector3						m_vTileSize;
	TILE_TYPE					m_eTileType;
	std::vector<class CTile*>	m_vecTile;
	std::vector<class CTile*>	m_vecUpdateTile;
	char*						m_pTexIdx;
	char*						m_pColIdx;
	bool						m_bAni; 
	static std::vector<PTILEANIM>		m_vecAnim;

public:
	void CreateTile(int x, int y, const Vector3& vSize, const Vector2& vStart, const Vector3& vPivot, TILE_TYPE eType);
	int GetIndex(int x, int y)	const;
	int GetIndex(const Vector2& vPos)	const;
	void SetTileTexture(const std::string& strKey);
	void SetTileTexture(class CTexture* pTex);
	bool LoadTileIdx(const char* pFileName, int iWidth, int iHegiht);
	bool LoadColIdx(const char* pFileName, int iWidth, int iHegiht);
	char GetTileCol(const Vector2& vPos)	const;
	int GetCountX()	const;
	int GetCountY()	const;
	void SetTileAnim(bool bAni);
	void SetTileOpacity(float fOp);
	class CTile* GetTile(const Vector2& vPos)	const;
	class CTile* GetTile(int x, int y)	const;
	class CTile* GetTile(int idx)	const;
	void AddTileRenderState(const std::string& strKey);
	static void AddAnim(const std::vector<int>& vecFrame,float fTime);
	const Vector2 GetTextureSize()	const;
	static void ClearAnim();

public:
	virtual bool Init();
	virtual void Start();
	virtual void Update(float fTime);
	virtual void PostUpdate(float fTime);
	virtual void Collision(float fTime);
	virtual void PreRender(float fTime);
	virtual void Render(float fTime);
	virtual void PostRender(float fTime);
	virtual CTileMap* Clone();

public:
	virtual void Save(FILE* pFile);
	virtual void Load(FILE* pFile);
};

