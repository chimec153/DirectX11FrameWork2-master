#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <Windows.h>
#include <list>
#include <vector>
#include <unordered_map>
#include <string>
#include <tchar.h>
#include <crtdbg.h>
#include <functional>
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include <iostream>
#include "Resource/DirectXTex.h"
#include "fmod.hpp"
#include <d2d1.h>
#include <dwrite.h>
#include <queue>
#include <stack>
#include <mutex>

#include "UI/imgui/imgui.h"
#include "UI/imgui/imgui_impl_dx11.h"
#include "UI/imgui/imgui_impl_win32.h"

#include "Macro.h"
#include "resource.h"
#include "EngineMath.h"

#include "Matrix.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "d2d1.lib")

#ifdef _WIN64
#pragma comment(lib, "fmod64_vc.lib")
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#endif

#else
#pragma comment(lib, "fmod_vc.lib")
#ifdef _DEBUG
#pragma comment(lib, "DirectXTex32_Debug.lib")
#else
#pragma comment(lib, "DirectXTex32.lib")
#endif

#endif

typedef struct _tagTransformCBuffer
{
	Matrix		matWorld;
	Matrix		matView;
	Matrix		matProj;
	Matrix		matWV;
	Matrix		matWVP;
	Vector3		vPivot;
	float		fE1;
	Vector3		vMeshSize;
	float		fE;
}TransformCBuffer, *PTransformCBuffer;

typedef struct _tagShaderCBuffer
{
	Vector4	vDif;
	Vector4 vAmp;
	Vector4	vSpe;
	int		iDifTex;
	int		iNrmTex;
	int		iSpeTex;
	int		iGray;
}ShaderCBuffer, *PShaderCBuffer;

typedef struct _tagCBuffer
{
	ID3D11Buffer*	pBuffer;
	int				iSize;
	int				iRegister;
	int				iType;
}CBuffer, *PCBuffer;

typedef struct _tagHierarchy
{
	std::string strName;
	std::string strParent;
}Hierarchy, *PHierarchy;

typedef struct _tagSpriteCBuffer
{
	int		iImageType;
	int		iAniType;
	Vector2	vStart;
	Vector2	vEnd;
	Vector2	vSize;
}SpriteCBuffer, *PSpriteCBuffer;

typedef struct _tagGlobalCBuffer
{
	float	fDeltaTime;
	float	fAccTime;
	Vector2 vNoiseSize;
}GLOBALCBUFFER, *PGLOBALCBUFFER;

typedef struct _tagParticle
{
	Vector3 vPos;
	Vector3 vMoveDir;
	float fSpeed;
	float fAddSpeed;
	float fLifeTime;
	float fMaxLifeTime;
	int iLive;
	float	fAngle;
	int	iFrame;
}PARTICLE, * PPARTICLE;

typedef struct _tagParticleShare
{
	unsigned int iMaxCount;
	unsigned int iCount;

	Vector3 vStartScale;
	Vector3 vEndScale;

	Vector4 vStartColor;
	Vector4 vEndColor;
}PARTICLESHARE, * PPARTICLESHARE;

typedef struct _tagParticleCBuffer
{
	Vector3 vPos;
	unsigned int iCount;
	Vector3 vStartScale;
	unsigned int iMaxCount;
	Vector3 vEndScale;
	float fMinLifeTime;
	Vector3 vRange;
	float fMaxLifeTime;
	Vector4 vStartColor;
	Vector4 vEndColor;
	float fMinSpeed;
	float fMaxSpeed;
	int iMove;
	int i2D;
	Vector3 vMoveDir;
	float fTurnAngle;
	Vector3 vMoveAngle;
	float fFrameTime;
	int		iFrameMax;
	Vector3	vAcc;
}PARTICLECBUFFER, *PPARTICLECBUFFER;

typedef struct _tagCBPerFrame
{
	Vector3 vCamPosW;
	Vector3 vEmitPosW;
	Vector3 vEmitDirW;
	float	fGameTime;
	float	fTimeStep;
	Matrix matViewProj;
}CBPERFRAME, *PCBPERFRAME;

typedef struct _tagRectInfo
{
	float fL;
	float fT;
	float fR;
	float fB;

	_tagRectInfo() :
		fL(0.f),
		fT(0.f),
		fR(0.f),
		fB(0.f)
	{
	}
}RectInfo, *PRectInfo;

typedef struct _tagLineInfo
{
	Vector2	vStart;
	Vector2	vEnd;

	_tagLineInfo() :
		vStart(),
		vEnd()
	{
	}
}LineInfo, *PLineInfo;

typedef struct _tagCircleInfo
{
	float		fRadius;
	Vector3		vCenter;

	_tagCircleInfo() :
		fRadius(0.f),
		vCenter()
	{
	}
}CircleInfo, *PCircleInfo;

typedef struct _tagOBBInfo
{
	Vector3	vPos;
	Vector3 vAxis[(int)WORLD_AXIS::AXIS_END];
	float fLength[(int)WORLD_AXIS::AXIS_END];

	_tagOBBInfo() :
		vPos()
	{
		for (int i = 0; i < (int)WORLD_AXIS::AXIS_END; ++i)
			vAxis[i] = Vector3::Axis[i];
		memset(fLength, 0, sizeof(float) * (int)WORLD_AXIS::AXIS_END);
	}
}OBBInfo, *POBBInfo;

typedef struct _tagColor
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	_tagColor() :
		r(0),
		g(0),
		b(0),
		a(0)
	{
	}
}Color, *PColor;

typedef struct _tagPixelInfo
{
	_tagColor*				pPixel;
	int						iHeight;
	int						iWidth;
	int						iSize;
	PIXEL_COLLISION_TYPE	eType;
	_tagColor				tColor;
	_tagColor				tChkColor;
	RectInfo				tRI;

	_tagPixelInfo()	:
		pPixel(nullptr),
		iHeight(0),
		iWidth(0),
		iSize(4),
		eType(PIXEL_COLLISION_TYPE::COLOR_IGN),
		tRI()
	{
		memset(&tColor, 0, sizeof(Color));
		memset(&tChkColor, 0, sizeof(Color));
	}

	~_tagPixelInfo()
	{
		SAFE_DELETE_ARRAY(pPixel);
	}
}PixelInfo, *PPixelInfo;

typedef struct _tagCollisionChannel
{
	std::string			strTag;
	COLLISION_CHANNEL	eChannel;
	COLLISION_TYPE		eType;
}CollisionChannel, *PCollisionChannel;

typedef struct _tagCollisionProfile
{
	std::string						strTag;
	COLLISION_CHANNEL				eChannel;
	std::vector<CollisionChannel>	vecChannel;
}CollisionProfile, *PCollisionProfile;

typedef struct _tagBarCBuffer
{
	float	fPercent;
	int		iDir;
	Vector2	vE;
}BARCBUFFER, *PBARCBUFFER;

typedef struct _tagSoundInfo
{
	FMOD::Sound*			pSound;
	SOUND_TYPE				eType;
	FMOD::Channel*			pChannel;
	float					fVol;

	_tagSoundInfo() :
		pSound(nullptr),
		eType(SOUND_TYPE::END),
		pChannel(nullptr),
		fVol(1.f)
	{

	}

	~_tagSoundInfo()
	{
		pSound->release();
	}
}SOUNDINFO, * PSOUNDINFO;

typedef struct _tagInstancingBuffer
{
	ID3D11Buffer*	pBuffer;
	void*			pData;
	int				iCount;
	int				iSize;
	D3D11_USAGE		eUsg;

	_tagInstancingBuffer() :
		pBuffer(nullptr),
		pData(nullptr),
		iCount(0),
		iSize(0),
		eUsg(D3D11_USAGE_DEFAULT)
	{
	}
}INSTANCINGBUFFER, *PINSTANCINGBUFFER;

typedef struct _tagFrame
{
	Vector2		vStart;
	Vector2		vEnd;
}Frame, * PFrame;

typedef struct _tagFade
{
	Vector4 vFade;
	float	fGray;
	Vector3 vE;

	_tagFade()	:
		vFade(),
		fGray(0.f),
		vE()
	{
	}
	
}FADE, * PFADE;

typedef struct _tagAngle
{
	float fAngle;
	Vector3 vE;
}ANGLE, *PANGLE;