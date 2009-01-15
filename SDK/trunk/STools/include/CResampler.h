// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
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

class CResampler : public virtual IResampler
{
public:
	CResampler();

	~CResampler();

	virtual void Destroy();

	//! IResampler override
	virtual void Reset();
	//! IResampler override
	virtual int GetNrOfInputSamplesNeeded(float fResampleFactor, unsigned int uiOutputBufferSize);
	//! IResampler override
	virtual void Resample(unsigned int uiOutputBufferSize, unsigned int uiInputBufferSize, float fResampleFactor, float *ptOut, float *ptIn);
	//! IResampler override
	virtual void ResampleDual(unsigned int uiOutputBufferSize, unsigned int uiInputBufferSize, float fResampleFactor, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2);

protected:
	float Phase;
	float ResampleBufL[4], ResampleBufR[4];

	void HermiteInterpolate1(unsigned int len, float Delta, float *ptOut, float *ptIn);
	void HermiteInterpolate1D(unsigned int len, float Delta, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2);
	void HermiteInterpolate2(unsigned int len, float Delta, float *ptOut, float *ptIn);
	void HermiteInterpolate2D(unsigned int len, float Delta, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2);
	void Decimate1(unsigned int len, float Delta, float *ptOut, float *ptIn);
	void Decimate1D(unsigned int len, float Delta, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2);
	void Decimate2(unsigned int len, float Delta, float *ptOut, float *ptIn);
	void Decimate2D(unsigned int len, float Delta, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2);
};
