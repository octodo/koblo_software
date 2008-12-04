
#include "SToolsInternalOS.h"

#include <string.h>

CResampler::CResampler()
{
}

CResampler::~CResampler()
{
}

IResampler* IResampler::Create()
{
	return dynamic_cast<IResampler*>(new CResampler());
}

void CResampler::Destroy()
{
	delete dynamic_cast<CResampler*>(this);
}

void CResampler::Reset(void)
{
	Phase = 0.0f;

	ResampleBufL[0] = ResampleBufL[1] = ResampleBufL[2] = ResampleBufL[3] = 0.0f;
	ResampleBufR[0] = ResampleBufR[1] = ResampleBufR[2] = ResampleBufR[3] = 0.0f;
}

int CResampler::GetNrOfInputSamplesNeeded(float fResampleFactor, unsigned int uiOutputBufferSize)
{
	return ((int)(Phase + fResampleFactor * (float)uiOutputBufferSize));
}

void CResampler::Resample(unsigned int len, unsigned int in_len, float Delta, float *ptOut, float *ptIn)
{
	memcpy(ptIn, ResampleBufL, 4*sizeof(float));

	if (Delta < 1.0F)
		HermiteInterpolate1(len, Delta, ptOut, ptIn);
	else if (Delta < 1.25F)
		HermiteInterpolate2(len, Delta, ptOut, ptIn);
	else if (Delta < 2.0F)
		Decimate1(len, Delta, ptOut, ptIn);
	else
		Decimate2(len, Delta, ptOut, ptIn);

	memcpy(ResampleBufL, ptIn + in_len, 4*sizeof(float));
}


void CResampler::ResampleDual(unsigned int len, unsigned int in_len, float Delta, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2)
{
	memcpy(ptIn1, ResampleBufL, 4*sizeof(float));
	memcpy(ptIn2, ResampleBufR, 4*sizeof(float));

	if (Delta < 1.0F)
		HermiteInterpolate1D(len, Delta, ptOut1, ptOut2, ptIn1, ptIn2);
	else if (Delta < 1.25F)
		HermiteInterpolate2D(len, Delta, ptOut1, ptOut2, ptIn1, ptIn2);
	else if (Delta < 2.0F)
		Decimate1D(len, Delta, ptOut1, ptOut2, ptIn1, ptIn2);
	else
		Decimate2D(len, Delta, ptOut1, ptOut2, ptIn1, ptIn2);

	memcpy(ResampleBufL, ptIn1 + in_len, 4*sizeof(float));
	memcpy(ResampleBufR, ptIn2 + in_len, 4*sizeof(float));
}


void CResampler::HermiteInterpolate1(unsigned int len, float Delta, float *ptOut, float *ptIn)
{
unsigned int i;
float x = Phase;
float c1, c2, c3;

	for (i = 0; i < len; i++)
	{
		c1 = 0.5F * (ptIn[2] - ptIn[0]);
		c2 = ptIn[0] - 2.5F*ptIn[1] + 2.0F*ptIn[2] - 0.5F*ptIn[3];
    c3 = 0.5F*(ptIn[3]-ptIn[0]) + 1.5F*(ptIn[1] - ptIn[2]);
		ptOut[i] = ((c3*x + c2) * x + c1) * x + ptIn[1];

		x += Delta;
		if (x >= 1.0F) { x -= 1.0F; ptIn++; }
	}

	Phase = x;
}

void CResampler::HermiteInterpolate1D(unsigned int len, float Delta, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2)
{
unsigned int i;
float x = Phase;
float c0, c1, c2, c3;

	for (i = 0; i < len; i++)
	{
		c0 = ((     -x+2.0F) * x-1.0F) * x;
		c1 = (( 3.0F*x-5.0F) * x     ) * x + 2.0F;
		c2 = ((-3.0F*x+4.0F) * x+1.0F) * x;
		c3 = ((      x-1.0F) * x     ) * x;
		ptOut1[i] = 0.5F*(c0 * ptIn1[0] + c1 * ptIn1[1] + c2 * ptIn1[2] + c3 * ptIn1[3]);
		ptOut2[i] = 0.5F*(c0 * ptIn2[0] + c1 * ptIn2[1] + c2 * ptIn2[2] + c3 * ptIn2[3]);

		x += Delta;
		if (x >= 1.0F) { x -= 1.0F; ptIn1++; ptIn2++; }
	}

	Phase = x;
}


void CResampler::HermiteInterpolate2(unsigned int len, float Delta, float *ptOut, float *ptIn)
{
unsigned int i;
float x = Phase, OptimizedDelta = Delta - 1.0F;
float c1, c2, c3;

	for (i = 0; i < len; i++)
	{
		c1 = 0.5F * (ptIn[2] - ptIn[0]);
		c2 = ptIn[0] - 2.5F*ptIn[1] + 2.0F*ptIn[2] - 0.5F*ptIn[3];
    c3 = 0.5F*(ptIn[3]-ptIn[0]) + 1.5F*(ptIn[1] - ptIn[2]);
		ptOut[i] = ((c3*x + c2) * x + c1) * x + ptIn[1];

		x += OptimizedDelta;
		ptIn += 1;
		if (x >= 1.0F) { x -= 1.0F; ptIn++; }
	}

	Phase = x;
}


void CResampler::HermiteInterpolate2D(unsigned int len, float Delta, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2)
{
unsigned int i;
float x = Phase, OptimizedDelta = Delta - 1.0F;
float c0, c1, c2, c3;

	for (i = 0; i < len; i++)
	{
		c0 = ((     -x+2.0F) * x-1.0F) * x;
		c1 = (( 3.0F*x-5.0F) * x     ) * x + 2.0F;
		c2 = ((-3.0F*x+4.0F) * x+1.0F) * x;
		c3 = ((      x-1.0F) * x     ) * x;
		ptOut1[i] = 0.5F*(c0 * ptIn1[0] + c1 * ptIn1[1] + c2 * ptIn1[2] + c3 * ptIn1[3]);
		ptOut2[i] = 0.5F*(c0 * ptIn2[0] + c1 * ptIn2[1] + c2 * ptIn2[2] + c3 * ptIn2[3]);

		x += OptimizedDelta;
		ptIn1 += 1;
		ptIn2 += 1;
		if (x >= 1.0F) { x -= 1.0F; ptIn1++; ptIn2++; }
	}

	Phase = x;
}


#define filter0 (0.171899F * 1.1852F)
#define filter1 (0.500000F * 1.1852F)
#define filter2 (0.171899F * 1.1852F)
#define filter3 (0.121548F * 2.028F)
#define filter4 (0.250000F * 2.028F)
#define filter5 (0.121548F * 2.028F)

const float coeff1[] = {filter0, filter0 - filter1, filter1,
												filter1 - filter2, filter2};

void CResampler::Decimate1(unsigned int len, float Delta, float *ptOut, float *ptIn)
{
unsigned int i;
float x = Phase, OptimizedDelta = Delta - 1.0F;

	x = Phase;
	for (i = 0; i < len; i++)
	{
		ptOut[i] = ptIn[0]*(coeff1[0]-coeff1[0]*x) + ptIn[1]*(coeff1[1]*x+coeff1[2]) +
							 ptIn[2]*(coeff1[3]*x+coeff1[4]) + ptIn[3]*(coeff1[4]*x);
		x += OptimizedDelta;
		ptIn += 1;
		if (x >= 1.0F) { x -= 1.0F; ptIn++; }
	}
	Phase = x;
}

void CResampler::Decimate1D(unsigned int len, float Delta, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2)
{
unsigned int i;
float x = Phase, OptimizedDelta = Delta - 1.0F;

	x = Phase;
	for (i = 0; i < len; i++)
	{
		ptOut1[i] = ptIn1[0]*(coeff1[0]-coeff1[0]*x) + ptIn1[1]*(coeff1[1]*x+coeff1[2]) +
							  ptIn1[2]*(coeff1[3]*x+coeff1[4]) + ptIn1[3]*(coeff1[4]*x);
		ptOut2[i] = ptIn2[0]*(coeff1[0]-coeff1[0]*x) + ptIn2[1]*(coeff1[1]*x+coeff1[2]) +
							  ptIn2[2]*(coeff1[3]*x+coeff1[4]) + ptIn2[3]*(coeff1[4]*x);
		x += OptimizedDelta;
		ptIn1 += 1;
		ptIn2 += 1;
		if (x >= 1.0F) { x -= 1.0F; ptIn1++; ptIn2++; }
	}
	Phase = x;
}



const float coeff2[] = {filter3, filter3 - filter4, filter4,
												filter4 - filter5, filter5};

void CResampler::Decimate2(unsigned int len, float Delta, float *ptOut, float *ptIn)
{
unsigned int i;
float x = Phase, OptimizedDelta = Delta - 2.0F;

	for (i = 0; i < len; i++)
	{
		ptOut[i] = ptIn[0]*(coeff2[0]-coeff2[0]*x) + ptIn[1]*(coeff2[1]*x+coeff2[2]) +
							 ptIn[2]*(coeff2[3]*x+coeff2[4]) + ptIn[3]*(coeff2[4]*x);
		x += OptimizedDelta;
		ptIn += 2;
		while (x >= 1.0F) { x -= 1.0F; ptIn++; }
	}
	Phase = x;
}

void CResampler::Decimate2D(unsigned int len, float Delta, float *ptOut1, float *ptOut2, float *ptIn1, float *ptIn2)
{
unsigned int i;
float x = Phase, OptimizedDelta = Delta - 2.0F;

	for (i = 0; i < len; i++)
	{
		ptOut1[i] = ptIn1[0]*(coeff2[0]-coeff2[0]*x) + ptIn1[1]*(coeff2[1]*x+coeff2[2]) +
							  ptIn1[2]*(coeff2[3]*x+coeff2[4]) + ptIn1[3]*(coeff2[4]*x);
		ptOut2[i] = ptIn2[0]*(coeff2[0]-coeff2[0]*x) + ptIn2[1]*(coeff2[1]*x+coeff2[2]) +
							  ptIn2[2]*(coeff2[3]*x+coeff2[4]) + ptIn2[3]*(coeff2[4]*x);
		x += OptimizedDelta;
		ptIn1 += 2;
		ptIn2 += 2;
		while (x >= 1.0F) { x -= 1.0F; ptIn1++; ptIn2++; }
	}
	Phase = x;
}
