#include "UIFont.h"
#include "../Device.h"
#include "../FontManager.h"
#include "../Camera/CameraManager.h"
#include "../Component/Camera.h"

CUIFont::CUIFont() :
	m_pTarget(nullptr),
	m_bShadow(false),
	m_bAlpha(false),
	m_bShadowAlpha(false),
	m_vShadowOffset(),
	m_iColor(0),
	m_iShadowColor(0),
	m_pColor(nullptr),
	m_pShadowColor(nullptr),
	m_iLength(256),
	m_pLayout(nullptr),
	m_fSize(0.f),
	m_fOpacity(1.f),
	m_fShadowOpacity(1.f),
	m_tRect(),
	m_eAlignH(TEXT_ALIGN_HORISONTAL::LEFT),
	m_eAlignV(TEXT_ALIGN_VERTICAL::TOP)
{
	m_eSceneComponentClassType = SCENECOMPONENT_CLASS_TYPE::UI_FONT;

	m_pText = new TCHAR[m_iLength];
	m_pFont = new TCHAR[64];

	memset(m_pText, 0, sizeof(TCHAR) * m_iLength);
	memset(m_pFont, 0, sizeof(TCHAR) * 64);

	lstrcpy(m_pText, TEXT("Text"));
	lstrcpy(m_pFont, TEXT("�ü�ü"));

	m_pFormat = GET_SINGLE(CFontManager)->FindFormat("Normal");

	SetRect(0, 100, 100, 0);

	SetColor(1.f, 1.f, 1.f, 1.f);

	CreateTextLayout();
}

CUIFont::CUIFont(const CUIFont& ui)	:
	CUIControl(ui)
{
	m_pTarget = ui.m_pTarget;
	m_bShadow = ui.m_bShadow;
	m_bAlpha = ui.m_bAlpha;
	m_bShadowAlpha = ui.m_bShadowAlpha;
	m_vShadowOffset = ui.m_vShadowOffset;
	m_iColor = ui.m_iColor;
	m_iShadowColor = ui.m_iShadowColor;
	m_pColor = ui.m_pColor;
	m_pShadowColor = ui.m_pShadowColor;
	m_iLength = ui.m_iLength;
	m_pFormat = ui.m_pFormat;
	m_pLayout = ui.m_pLayout;

	if (m_pLayout)
	{
		ui.m_pLayout->AddRef();
	}

	m_fSize = ui.m_fSize;
	m_fOpacity = ui.m_fOpacity;
	m_tRect = ui.m_tRect;
	m_eAlignH = ui.m_eAlignH;
	m_eAlignV = ui.m_eAlignV;

	m_pText = ui.m_pText;
	m_pFont = ui.m_pFont;

	m_pText = new TCHAR[m_iLength];
	m_pFont = new TCHAR[64];

	memset(m_pText, 0, sizeof(TCHAR) * m_iLength);
	memset(m_pFont, 0, sizeof(TCHAR) * 64);

	lstrcpy(m_pText, ui.m_pText);
	lstrcpy(m_pFont, ui.m_pFont);
}

CUIFont::~CUIFont()
{
	SAFE_RELEASE(m_pLayout);
	delete[] m_pText;
	delete[] m_pFont;
}

bool CUIFont::Init()
{
	if (!CUIControl::Init())
		return false;

	m_pTarget = RENDERTARGET2D;

	return true;
}

void CUIFont::Start()
{
	CUIControl::Start();
}

void CUIFont::Update(float fTime)
{
	CUIControl::Update(fTime);
}

void CUIFont::PostUpdate(float fTime)
{
	CUIControl::PostUpdate(fTime);
}

void CUIFont::Collision(float fTime)
{
	CUIControl::Collision(fTime);
}

void CUIFont::PreRender(float fTime)
{
	CUIControl::PreRender(fTime);
}

void CUIFont::Render(float fTime)
{
	m_pTarget->BeginDraw();

	Resolution tRS = RESOLUTION;

	Vector3 vPos = GetWorldPos();

	if (m_eSceneComponentType == SCENE_COMPONENT_TYPE::SCT_2D)
	{
		CCamera* pCam = GET_SINGLE(CCameraManager)->GetMainCam();

		vPos -= pCam->GetWorldPos() - Vector3((float)tRS.iWidth, (float)tRS.iHeight, 0.f) * pCam->GetPivot();

		SAFE_RELEASE(pCam);
	}

	float fHeight = m_tRect.bottom - m_tRect.top;

	vPos.x += m_tRect.left;
	vPos.y = tRS.iHeight - fHeight - vPos.y;

	if (m_bShadow)
	{
		Vector3 vShadowPos = vPos + m_vShadowOffset;

		if (m_bShadowAlpha)
			m_pShadowColor->SetOpacity(m_fShadowOpacity);

		else
			m_pShadowColor->SetOpacity(1.f);

		m_pTarget->DrawTextLayout(D2D1_POINT_2F(D2D1::Point2F(vShadowPos.x, vShadowPos.y)), m_pLayout, m_pShadowColor);
	}

	if (m_bShadowAlpha)
		m_pColor->SetOpacity(m_fOpacity);

	else
		m_pColor->SetOpacity(1.f);

	m_pTarget->DrawTextLayout(D2D1_POINT_2F(D2D1::Point2F(vPos.x, vPos.y)), m_pLayout, m_pColor);

	HRESULT tResult = m_pTarget->EndDraw();
}

void CUIFont::PostRender(float fTime)
{
	CUIControl::PostRender(fTime);
}

CUIFont* CUIFont::Clone()
{
	return new CUIFont(*this);
}

void CUIFont::Save(FILE* pFile)
{
	CUIControl::Save(pFile);

	fwrite(&m_bShadow, 1, 1, pFile);
	fwrite(&m_bAlpha, 1, 1, pFile);
	fwrite(&m_bShadowAlpha, 1, 1, pFile);
	fwrite(&m_vShadowOffset, sizeof(Vector3), 1, pFile);
	fwrite(&m_iColor, sizeof(UINT), 1, pFile);
	fwrite(&m_iShadowColor, sizeof(UINT), 1, pFile);
	fwrite(&m_iLength, 4, 1, pFile);
	fwrite(m_pText, sizeof(TCHAR), m_iLength, pFile);
	int iLength = (int)lstrlen(m_pFont);
	fwrite(&iLength, 4, 1, pFile);
	fwrite(m_pFont, sizeof(TCHAR), iLength, pFile);
	fwrite(&m_fSize, 4, 1, pFile);
	fwrite(&m_fOpacity, 1, 1, pFile);
	fwrite(&m_fShadowOpacity, 1, 1, pFile);
	fwrite(&m_tRect, sizeof(D2D1_RECT_F), 1, pFile);
	fwrite(&m_eAlignH, 4, 1, pFile);
	fwrite(&m_eAlignV, 4, 1, pFile);
}

void CUIFont::Load(FILE* pFile)
{
	CUIControl::Load(pFile);

	fread(&m_bShadow, 1, 1, pFile);
	fread(&m_bAlpha, 1, 1, pFile);
	fread(&m_bShadowAlpha, 1, 1, pFile);
	fread(&m_vShadowOffset, sizeof(Vector3), 1, pFile);
	fread(&m_iColor, sizeof(UINT), 1, pFile);
	fread(&m_iShadowColor, sizeof(UINT), 1, pFile);
	fread(&m_iLength, 4, 1, pFile);
	fread(m_pText, sizeof(TCHAR), m_iLength, pFile);
	int iLength = 0;
	fread(&iLength, 4, 1, pFile);
	fread(m_pFont, sizeof(TCHAR), iLength, pFile);
	fread(&m_fSize, 4, 1, pFile);
	fread(&m_fOpacity, 1, 1, pFile);
	fread(&m_fShadowOpacity, 1, 1, pFile);
	fread(&m_tRect, sizeof(D2D1_RECT_F), 1, pFile);
	fread(&m_eAlignH, 4, 1, pFile);
	fread(&m_eAlignV, 4, 1, pFile);

	m_pTarget = RENDERTARGET2D;

	SetColor(m_iColor);
	SetShadowColor(m_iShadowColor);
	CreateTextLayout();
}

void CUIFont::SetText(const TCHAR* pText)
{
	size_t iLength = lstrlen(pText);

	if (iLength > m_iLength)
	{
		SAFE_DELETE_ARRAY(m_pText);

		m_iLength = (int)iLength;

		m_pText = new TCHAR[m_iLength];
	}

	memset(m_pText, 0, sizeof(TCHAR)* m_iLength);

	lstrcpy(m_pText, pText);

	CreateTextLayout();
}

void CUIFont::AddText(const TCHAR* pText)
{
	size_t iLength = lstrlen(pText);

	size_t iCurLength = lstrlen(m_pText);

	if (iLength + iCurLength > m_iLength)
	{
		m_iLength = (int)(iLength + iCurLength) * 2;

		TCHAR* strText = new TCHAR[m_iLength];

		memset(strText, 0, sizeof(TCHAR) * m_iLength);

		lstrcpy(strText, m_pText);

		SAFE_DELETE_ARRAY(m_pText);

		m_pText = strText;
	}

	lstrcat(m_pText, pText);

	CreateTextLayout();
}

void CUIFont::SetShadowOffset(const Vector3& vOff)
{
	m_vShadowOffset = vOff;
}

void CUIFont::SetOpacity(float fOpa)
{
	m_fOpacity = fOpa;

	if (m_fOpacity < 1.f)
		m_bAlpha = true;
}

void CUIFont::SetShadowOpacity(float fOpa)
{
	m_fShadowOpacity = fOpa;

	if (m_fShadowOpacity < 1.f)
		m_bShadowAlpha = true;
}

void CUIFont::SetAlpha(bool bAlp)
{
	m_bAlpha = bAlp;
}

void CUIFont::SetShadow(bool bSha)
{
	m_bShadow = bSha;
}

void CUIFont::SetShadowAlpha(bool bAlp)
{
	m_bShadowAlpha = bAlp;
}

void CUIFont::SetShadowColor(float r, float g, float b, float a)
{
	m_iShadowColor = GET_SINGLE(CFontManager)->ConvertColor(r, g, b, a);

	m_pShadowColor = GET_SINGLE(CFontManager)->CreateBrush(r, g, b, a);

	//m_bShadow = true;
}

void CUIFont::SetShadowColor(BYTE r, BYTE g, BYTE b, BYTE a)
{
	m_iShadowColor = GET_SINGLE(CFontManager)->ConvertColor(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

	m_pShadowColor = GET_SINGLE(CFontManager)->CreateBrush(r, g, b, a);

	//m_bShadow = true;
}

void CUIFont::SetShadowColor(UINT iColor)
{
	m_iShadowColor = iColor;

	m_pShadowColor = GET_SINGLE(CFontManager)->CreateBrush(iColor);

	//m_bShadow = true;
}

void CUIFont::SetColor(float r, float g, float b, float a)
{
	m_iColor = GET_SINGLE(CFontManager)->ConvertColor(r, g, b, a);

	m_pColor = GET_SINGLE(CFontManager)->CreateBrush(r, g, b, a);
}

void CUIFont::SetColor(BYTE r, BYTE g, BYTE b, BYTE a)
{
	m_iColor = GET_SINGLE(CFontManager)->ConvertColor(r / 255.f, g / 255.f, b / 255.f, a / 255.f);

	m_pColor = GET_SINGLE(CFontManager)->CreateBrush(r, g, b, a);
}

void CUIFont::SetColor(UINT iColor)
{
	m_iColor = iColor;

	m_pColor = GET_SINGLE(CFontManager)->CreateBrush(iColor);
}

void CUIFont::SetFormat(const std::string& strKey)
{
	m_pFormat = GET_SINGLE(CFontManager)->FindFormat(strKey);
}

void CUIFont::SetFormat(const std::string& strKey, const TCHAR* pFont, int iWeight, int iStyle, int iStretch, float fSize, const TCHAR* pLocal)
{
	if (!GET_SINGLE(CFontManager)->CreateFormat(strKey, pFont, iWeight, iStyle, iStretch, fSize, pLocal))
		return;

	m_pFormat = GET_SINGLE(CFontManager)->FindFormat(strKey);
}

void CUIFont::CreateTextLayout()
{
	SAFE_RELEASE(m_pLayout);

	m_pLayout = GET_SINGLE(CFontManager)->CreateLayout(m_pText, (unsigned int)lstrlen(m_pText), m_pFormat, m_tRect.right - m_tRect.left, m_tRect.bottom - m_tRect.top);
}

void CUIFont::SetRect(float l, float t, float r, float b)
{
	Resolution tRS = RESOLUTION;

	m_tRect = D2D1::RectF(l, (float)tRS.iHeight - t, r, (float)tRS.iHeight - b);

	CreateTextLayout();
}

void CUIFont::SetAlignH(TEXT_ALIGN_HORISONTAL eAlign)
{
	switch (eAlign)
	{
	case TEXT_ALIGN_HORISONTAL::LEFT:
		m_pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		break;
	case TEXT_ALIGN_HORISONTAL::CENTER:
		m_pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		break;
	case TEXT_ALIGN_HORISONTAL::RIGHT:
		m_pLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		break;
	}

	m_eAlignH = eAlign;
}

void CUIFont::SetAlignV(TEXT_ALIGN_VERTICAL eAlign)
{
	switch (eAlign)
	{
	case TEXT_ALIGN_VERTICAL::TOP:
		m_pLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case TEXT_ALIGN_VERTICAL::MID:
		m_pLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case TEXT_ALIGN_VERTICAL::BOT:
		m_pLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
		break;
	}

	m_eAlignV = eAlign;
}
