#pragma once

struct FilterParams
{
	struct Sepia
	{
		float k;
	} m_sepia;

	struct BoxBlur
	{
		int filterDim;
	} m_boxBlur;

	struct GausBlur
	{
		int filterDim;
		float sigma;
	} m_gausBlur;

	bool operator==(const FilterParams& other)
	{
		return m_sepia.k == other.m_sepia.k
			&& m_boxBlur.filterDim == other.m_boxBlur.filterDim
			&& m_gausBlur.filterDim == other.m_gausBlur.filterDim
			&& m_gausBlur.sigma == other.m_gausBlur.sigma;
	}

	bool operator!=(const FilterParams& other)
	{
		return (*this == other) == false;
	}
};
