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
	int		g_iGray;
};

cbuffer Sprite	:	register(b2)
{
	int		g_iImageType;
	int		g_iAniType;
	float2	g_vStart;
	float2	g_vEnd;
	float2	g_vSize;
};

cbuffer Fade	:	register(b10)
{
	float4	g_vFade;
	float	g_fGray;
	float3	g_vE;
};

Texture2D gDiffuseMap	:	register(t0);
Texture2D gRandomTex	:	register(t1);
Texture2DArray gArrayTex	:	register(t9);

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

	if (vUV.x > 0.f)
		output.x = g_vEnd.x / g_vSize.x;

	else
		output.x = g_vStart.x / g_vSize.x;

	if (vUV.y > 0.f)
		output.y = g_vEnd.y / g_vSize.y;

	else
		output.y = g_vStart.y / g_vSize.y;

	return output;
}

// 연습용 코드
float3 RandUnitVec3(float offset)
{
	float u = (gGameTime + offset);

	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;

	return normalize(v);
}

GeometryShader gsStreamOut = ConstructGSWithSO(ComplieShader(gs_5_0, GS()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");

technique11 SOTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(gsStreamOut);
		SetPixelShader(CompileShader(ps_5_0, PS()));
	};
};

struct Vertex
{
	float3 InitialPosW	:	POSITION;
	float3 InitialVelW	:	VELOCITY;
	float2 SizeW	:	SIZE;
	float Age : AGE;
	uint Type	:	TYPE;
};

DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

GeometryShader gsStreamOut = ConstructGSWithSO(
	CompileShader(gs_5_0, StreamOutGS()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x");

technique10 StreamOutTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, StreamOutVS()));
		SetGeometryShader(gsStreamOut);

		SetPixelShader(NULL);

		SetDepthStencilState(DisableDepth, 0);
	};
};

cbuffer cbPerFrame
{
	float3 gEyePosW;

	float3 gEmitPosW;
	float3 gEmitDirW;
	float gGameTime;
	float gTimeStep;
	float4x4 gViewProj;
};

cbuffer cbFixed
{
	float3 gAccelW = { 0.0f, 7.8f, 0.0f };

	float2 gQuadTexC[4] =
	{
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f)
	};
};

Texture2DArray gTexArray;

Texture1D gRandomTex;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

DepthStencilState DisableDepth
{
	DepthEnable = FALSE;
	DepthWriteMask = ZERO;
};

DepthStencilState NoDepthWrites
{
	DepthEnable = TRUE;
	DepthWriteMask = ZERO;
};

BlendState AdditiveBlending
{
	AlphaToCoverageEnable = FALSE;
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = ONE;
	BlendOp = ADD;
	SrcBlendAlpha = ZERO;
	DestBlendAlpha = ZERO;
	BlendOpAlpha = ADD;
	RenderTargetWriteMask[0] = 0x0F;
};

#define PT_EMITTER	0
#define PT_FLARE	1

struct Particle
{
	float3 InitialPosW	:	POSITION;
	float3	InitialVelW	:	VELOCITY;
	float2	SizeW	:	SIZE;
	float	Age : AGE;
	uint	Type	:	TYPE;
};

Particle StreamOutVS(Particle vin)
{
	return vin;
}

[maxvertexcount(2)]
void StreamOutGS(point Particle gin[1],
	inout PointStream<Particle> ptStream)
{
	gin[0].Age += gTimeStep;

	if (gin[0].Type == PT_EMITTER)
	{
		if (gin[0].Age > 0.005f)
		{
			float3 vRandom = RandUnitVec3(0.0f);
			vRandom.x *= 0.5f;
			vRandom.z *= 0.5f;

			Particle p;
			p.InitialPosW = gEmitPosW.xyz;
			p.InitialVelW = 4.0f * vRandom;
			p.SizeW = float2(3.0f, 3.0f);
			p.Age = 0.0f;
			p.Type = PT_FLARE;

			ptStream.Append(p);

			gin[0].Age = 0.0f;
		}

		ptStream.Append(gin[0]);
	}

	else
	{
		if (gin[0].Age <= 1.0f)
			ptStream.Append(gin[0]);
	}
}

struct VertexOut
{
	float3 PosW	:	POSITION;
	float2 SizeW	:	SIZE;
	float4 Color	:	COLOR;
	uint Type	:	TYPE;
};

VertexOut DrawVS(Particle vin)
{
	VertexOut vout;

	float t = vin.Age;

	vout.PosW = 0.5f * t * t * gAccelW + t * vin.InitialVelW + vin.InitialPosW;

	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t / 1.0f);
	vout.Color = float4(1.0f, 1.0f, 1.0f, opacity);

	vout.SizeW = vin.SizeW;
	vout.Type = vin.Type;

	return vout;
}

struct GeoOut
{
	float4 PosH	:	SV_Position;
	float4 Color	:	COLOR;
	float2 Tex	:	TEXCOORD;
};

[maxvertexcount(4)]
void DrawGS(point VertexOut gin[1],
	inout TriangleStream<GeoOut> triStream)
{
	if (gin[0].Type != PT_EMITTER)
	{
		float3 look = normalize(gEyePosW.xyz - gin[0].PosW);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = cross(look, right);

		float halfWidth = 0.5f * gin[0].SizeW.x;
		float halfHeight = 0.5f * gin[0].SizeW.y;

		float4 v[4];
		v[0] = float4(gin[0].PosW + halfWidth * right -
			halfHeight * up, 1.0f);
		v[1] = float4(gin[0].PosW + halfWidth * right +
			halfHeight * up, 1.0f);
		v[2] = float4(gin[0].PosW - halfWidth * right -
			halfHeight * up, 1.0f);
		v[3] = float4(gin[0].PosW - halfWidth * right +
			halfHeight * up, 1.0f);

		GeoOut gout;
		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			gout.PosH = mul(v[i], gViewProj);
			gout.Tex = gQuadTexC[i];
			gout.Color = gin[0].Color;
			triStream.Append(gout);
		}
	}
}

float4 DrawPS(GeoOut pin) : SV_TARGET
{
	return gTexArray.Sample(samLinear, float3(pin.Tex, 0)) * pin.Color;
}

technique11 DrawTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, DrawVS()));
		SetGeometryShader(CompileShader(gs_5_0, DrawGS()));
		SetPixelShader(CompileShader(ps_5_0, DrawPS()));

		SetBlendState(AdditiveBlending, float4(0.0f, 0.0f, 0.0f, 0.0f),
			0xffffffff);
		SetDepthStencilState(NoDepthWrites, 0);
	};
};

struct Data
{
	float3 v1;
	float2 v2;
};

StructuredBuffer<Data> gInputA;
StructuredBuffer<Data> gInputB;

RWStructuredBuffer<Data> gOutput;

Buffer<float4> typedBuffer1;
Buffer<float> typedBuffer2;
Buffer<float2> typedBuffer3;

[numthreads(32,1,1)]
void CS(int3 dtid : SV_DispatchThreadID)
{
	gOutput[dtid.x].v1 = gInputA[dtid.x].v1 + gInputB[dtid.x].v1;
	gOutput[dtid.x].v2 = gInputB[dtid.x].v2 + gInputB[dtid.x].v2;
}

Texture2D gInputA;
Texture2D gInputB;
RWTexture2D<float4> gOutput;

[numthreads(16,16,1)]
void CS(int3 dispatchThreadID : SV_DispatchThreadID)
{
	gOutput[dispatchThreadID.xy] =
		gInputA[dispatchThreadID.xy] +
		gInputB[dispatchThreadID.xy];
}

struct Particle
{
	float3 Position;
	float3 Velocity;
	float3 Acceleration;
};

float TimeStep = 1.0f / 60.0f;

ConsumeStructuredBuffer<Particle> gInput;
AppendStructuredBuffer<Particle> gOutput;
[numthreads(16,16,1)]
void CS()
{
	Particle p = gInput.Consume();

	p.Velocity += p.Acceleration * TimeStep;
	p.Position += p.Velocity * TimeStep;

	gOutput.Append(p);
}

groupshared float4 gCache[256];


Texture2D gInput;
RWTexture2D<float4> gOutput;

groupshared float4 gCache[256];

[numthreads(256,1,1)]
void CS(int3 groupThreadID : SV_GroupThreadID,
	int3 dispatchThreadID : SV_DispatchThreadID)
{
	gCache[groupThreadID.x] = gInput[dispatchThreadID.xy];

	GroupMemoryBarrierWithGroupSync();

	float4 left = gCache[groupThreadID.x - 1];
	float4 right = gCache[groupThreadID.x + 1];
}

// 연습용 코드