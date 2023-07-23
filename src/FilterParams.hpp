#pragma once

#include <hip/hip_runtime.h>

struct FilterParams
{
	struct Sepia
	{
		float k;
	} m_sepia;

	struct BoxBlux
	{
		uint filterDim;
	} m_boxBlur;

	struct GausBlur
	{
		uint filterDim;
		float sigma;
	} m_gausBlur;
};
