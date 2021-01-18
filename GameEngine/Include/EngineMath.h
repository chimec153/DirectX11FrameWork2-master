#pragma once

#include <DirectXMath.h>
#include <Windows.h>

using namespace DirectX;

#include "Flag.h"

#define PI	3.14159f

static float DegToRad(float fDeg)
{
	return fDeg * PI / 180.f;
}

static float RadToDeg(float fRad)
{
	return fRad * 180.f / PI;
}

namespace MathHelper
{

	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	static float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	XMVECTOR MathHelper::RandUnitVec3()
	{
		XMVECTOR One = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		XMVECTOR Zero = DirectX::XMVectorZero();

		while (true)
		{
			XMVECTOR v = DirectX::XMVectorSet(
				MathHelper::RandF(-1.0f, 1.0f),
				MathHelper::RandF(-1.0f, 1.0f),
				MathHelper::RandF(-1.0f,1.0f),0.0f);

			if (XMVector3Greater(XMVector3LengthSq(v), One))
				continue;

			return XMVector3Normalize(v);
		}
	}
}