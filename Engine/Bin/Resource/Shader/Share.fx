struct PS_OUTPUT_COLOR
{
	float4 vColor	:	SV_TARGET;
};

cbuffer Transform	:	register(b0)
{
	matrix	g_matWorld;
	matrix	g_matView;
	matrix	g_matProj;
	matrix	g_matWV;
	matrix	g_matWVP;
	float3	g_vPivot;
	float	g_fTime;
	float3	g_vMeshSize;
	float	g_f2;
};

cbuffer Material	:	register(b1)
{
	float4	g_vDif;
	float4	g_vAmb;
	float4	g_vSpe;
	int		g_iDif;
	int		g_iNrm;
	int		g_iSpe;
	float	g_iGray;
};

cbuffer Sprite	:	register(b2)
{
	int		g_iImageType;
	int		g_iAniType;
	float2	g_vStart;
	float2	g_vEnd;
	float2	g_vSize;
};

cbuffer Global	:	register(b3)
{
	float	g_fDeltaTime;
	float	g_fAccTime;
	float2	g_vNoiseSize;
};

cbuffer Fade	:	register(b10)
{
	float4	g_vFade;
	float	g_fGray;
	float3	g_vE;
};

Texture2D gDiffuseMap	:	register(t0);
Texture2DArray gArrayTex	:	register(t9);
Texture2D gRandomTex	:	register(t10);

SamplerState g_sPoint	:	register(s0);
SamplerState g_sLinear	:	register(s1);
SamplerState g_sAnisotropic	:	register(s2);

#define ATLAS	0
#define ARRAY	1

float2 ComputeUV(float2 vUV)
{
	if (g_iImageType == ARRAY)
		return vUV;

	float2 output = (float2)0.f;

	output.x = ((g_vEnd.x - g_vStart.x) * vUV.x + g_vStart.x) / g_vSize.x;
	output.y = ((g_vEnd.y - g_vStart.y) * vUV.y + g_vStart.y) / g_vSize.y;

	return output;
}

static float gaussian5x5[25] =	
{
	0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
	0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
	0.023792, 0.094907, 0.150342, 0.094907, 0.023792,
	0.015019, 0.059912, 0.094907, 0.059912, 0.015019,
	0.003765, 0.015019, 0.023792, 0.015019, 0.003765,
};

float gaussian5x5Sample(in int2 vUV, in texture2D RandomTex)
{
	float4 vRand = (float4)0.f;

	vRand += RandomTex[vUV];

	//for (int i = 0; i < 5; ++i)
	//{
	//	for (int j = 0; j < 5; ++j)
	//	{
	//		vRand += RandomTex[vUV + int2(i-2, j-2)] * gaussian5x5[i * 5 + j];
	//	}
	//}

	return vRand.x;
}

float Rand(in float fRand)
{
	float2 vUV = float2(fRand + g_fAccTime, g_fAccTime);

	vUV.y += sin(vUV.x * 6.283184f);

	if (vUV.x < 0)
	{
		vUV.x= -(int)vUV.x + 1 + vUV.x;
	}

	else
	{
		vUV.x = frac(vUV.x);
	}

	if (vUV.y < 0)
	{
		vUV.y = -(int)vUV.y + 1 + vUV.y;
	}

	else
	{
		vUV.y = frac(vUV.y);
	}
	
	vUV = vUV * g_vNoiseSize;

	return gaussian5x5Sample(vUV, gRandomTex);
}

float3 RandUnitVec3(float offset)
{
	float u = (g_fDeltaTime + offset);

	float3 v = gRandomTex.SampleLevel(g_sLinear, u, 0).xyz;

	return normalize(v);
}