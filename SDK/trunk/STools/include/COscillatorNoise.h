// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
//
// This file is part of the Koblo Stools.
//
// the Koblo Stools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo Stools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.


//! Truncates a floating point number and converts it to int
#define TRUNC(a) \
	(unsigned int)((a))

//! Linear interpolation for floating point. a: Accumulator [in]. b: Integer index [out]. c: Fraction [out]
#define INDEX_DELTA(a, b, c) \
		b = TRUNC(a); \
		c = a - b

//! Frequency to filter cytoff table size
//const int		giFreqToCutoffSize				=	16385;
const float		gfPI							=	3.1415926535897932384626433832795f;
const float		gfScaleNoise					=	1.0f / 0x7FFFFFFF;
const float		gfAnti_denormal					=	1e-10f;

class COscillatorNoise : public virtual IOscillatorNoise
{
public:
	COscillatorNoise(tfloat* pfFreqToCutoff, tint iSize);
	virtual ~COscillatorNoise();

	// IOscillatorNoise overrides
	virtual void Destroy();
	virtual void SetSampleRate(tfloat fSampleRate);
	virtual void SetNoise(tfloat* pfNoise, tint iNoiseSize);
	virtual void Reset();
	virtual void ProcessNoise(tfloat32* pfOut, const tfloat32* pfPitch, tint lC);
	virtual void ProcessNoise(tfloat32* pfOut, const tfloat pfPitch, tint lC);
	virtual void ProcessNoiseFM(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC);
	
protected:

	float mfSampleRate;
	float mfOneDivSampleRate;

	float* mpfFreqToCutoff;
	int	 iFreqToCutoffSize;

	float mfB;
	float mfBp;
	float mfL;
	float mfH;

	float* mpfNoise;
	int	miNoiseSize;
	int miNoise;

};

