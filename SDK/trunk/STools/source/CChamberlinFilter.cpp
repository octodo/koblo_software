

#include "SToolsInternalOS.h"


IChamberlinFilter* IChamberlinFilter::Create(float* pfFreqToCutoff,int iSize)
{
	return dynamic_cast<IChamberlinFilter*>(new CChamberlinFilter( pfFreqToCutoff, iSize));
}

void CChamberlinFilter::Destroy()
{
	delete dynamic_cast<CChamberlinFilter*>(this);
}

CChamberlinFilter::CChamberlinFilter(float* pfFreqToCutoff,int iSize)
{
	mpfFreqToCutoff		=	pfFreqToCutoff;
	iFreqToCutoffSize	=	iSize;	
}

CChamberlinFilter::~CChamberlinFilter()
{

}

void CChamberlinFilter::SetSampleRate(float fSampleRate)
{
	mfSampleRate		= fSampleRate;
	mfOneDivSampleRate	= 1 / mfSampleRate;

	for( int i=1; i<iFreqToCutoffSize; i++) {

		float fFreq	= (float)i;

		if(fFreq < 16274.0f) {

			float fDelta		=	gfPI *(fFreq / fSampleRate);
			mpfFreqToCutoff[i]	=	2.0f * sin(fDelta);

			if(mpfFreqToCutoff[i] > 1) {
				mpfFreqToCutoff[i]= 1;
			}
		}
		else {

			mpfFreqToCutoff[i]	=	1.0f;
		}
	}
	mpfFreqToCutoff[0] = mpfFreqToCutoff[1];
}

void CChamberlinFilter::Reset()
{
	mfB			=	0;
	mfBp		=	0;
	mfL			=	0;
	mfH			=	0;
}
void CChamberlinFilter::ProcessHP(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tint lC)
{

}

void CChamberlinFilter::ProcessBP(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tint lC)
{

}

void CChamberlinFilter::ProcessLP(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tint lC)
{
	//-------------------------------------------
	// Set filter cutoff
	//-------------------------------------------
	float	fCutoff, fDelta;				
	int	iIndex;

	INDEX_DELTA(fFreq,iIndex, fDelta);

	if(iIndex >=iFreqToCutoffSize-1) {

		fCutoff =  1.0f;
	}
	else {

		fCutoff			=	mpfFreqToCutoff[iIndex]		* (1.0f-fDelta);
		fCutoff			+=	mpfFreqToCutoff[iIndex+1]	* fDelta;
	}	
	//--------------------------------------------
	// Filter
	//--------------------------------------------
	for (int i = 0; i < lC; i++) {	
		// Chamberlin Filter
		mfB			=	mfBp;
		mfL			=	mfB * fCutoff + mfL;
		mfH			=	psAudio[i] - mfL - mfB * fRes;	
		mfBp		=	mfH * fCutoff + mfBp;
		// Output from filter
		psAudio[i]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));
	}

	// Avoid denomarlization problems
	mfBp			+=	gfAnti_denormal;
}

void CChamberlinFilter::ProcessLP32(tfloat32* psAudio, float fFreq, float fRes)
{
	//-------------------------------------------
	// Set filter cutoff
	//-------------------------------------------
	float	fCutoff, fDelta;				
	int	iIndex;

	INDEX_DELTA(fFreq,iIndex, fDelta);

	if(iIndex >=iFreqToCutoffSize-1) {

		fCutoff =  1.0f;
	}
	else {

		fCutoff			=	mpfFreqToCutoff[iIndex]		* (1.0f-fDelta);
		fCutoff			+=	mpfFreqToCutoff[iIndex+1]	* fDelta;
	}	

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[0] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[0]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[1] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[1]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[2] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[2]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[3] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[3]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[4] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[4]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[5] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[5]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[6] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[6]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[7] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[7]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[8] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[8]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[9] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[9]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[10] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[10]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[11] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[11]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[12] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[12]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[13] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[13]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[14] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[14]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[15] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[15]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[16] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[16]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[17] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[17]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[18] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[18]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[19] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[19]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[20] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[20]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[21] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[21]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[22] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[22]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[23] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[23]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[24] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[24]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[25] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[25]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[26] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[26]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[27] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[27]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[28] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[28]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[29] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[29]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[30] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[30]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfB			=	mfBp;
	mfL			=	mfB * fCutoff + mfL;
	mfH			=	psAudio[31] - mfL - mfB * fRes;	
	mfBp		=	mfH * fCutoff + mfBp;
	psAudio[31]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)));

	mfBp			+=	gfAnti_denormal;
}


void CChamberlinFilter::Process(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tfloat32 fLP, tfloat32 fBP, tfloat32 fHP, tint lC)
{
	//-------------------------------------------
	// Set filter cutoff
	//-------------------------------------------
	float	fCutoff, fDelta;				
	int	iIndex;
	
	INDEX_DELTA(fFreq,iIndex, fDelta);
	
	if(iIndex >=iFreqToCutoffSize-1) {
		
		fCutoff =  1.0f;
	}
	else {
		
		fCutoff			=	mpfFreqToCutoff[iIndex]		* (1.0f-fDelta);
		fCutoff			+=	mpfFreqToCutoff[iIndex+1]	* fDelta;
	}	
	//--------------------------------------------
	// Filter
	//--------------------------------------------
	for (int i = 0; i < lC; i++) {	
		// Chamberlin Filter
		mfB			=	mfBp;
		mfL			=	mfB * fCutoff + mfL;
		mfH			=	psAudio[i] - mfL - mfB * fRes;	
		mfBp		=	mfH * fCutoff + mfBp;
		// Output from filter
		psAudio[i]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes))) * fLP;
		psAudio[i]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes))) * fBP;
		psAudio[i]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes))) * fHP;
	}
	
	// Avoid denomarlization problems
	mfBp			+=	gfAnti_denormal;
	
}

void CChamberlinFilter::Process32(tfloat32* psAudio, tfloat fFreq, tfloat fRes, tfloat32 fLP, tfloat32 fBP, tfloat32 fHP, tint lC)
{
	//-------------------------------------------
	// Set filter cutoff
	//-------------------------------------------
	float	fCutoff, fDelta;				
	int	iIndex;
	
	INDEX_DELTA(fFreq,iIndex, fDelta);
	
	if(iIndex >=iFreqToCutoffSize-1) {
		
		fCutoff =  1.0f;
	}
	else {
		
		fCutoff			=	mpfFreqToCutoff[iIndex]		* (1.0f-fDelta);
		fCutoff			+=	mpfFreqToCutoff[iIndex+1]	* fDelta;
	}	
	//--------------------------------------------
	// Filter
	//--------------------------------------------
	
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[0] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[0]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[0]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[0]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[1] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[1]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[1]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[1]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[2] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[2]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[2]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[2]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[3] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[3]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[3]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[3]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[4] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[4]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[4]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[4]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[5] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[5]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[5]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[5]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[6] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[6]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[6]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[6]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[7] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[7]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[7]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[7]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[8] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[8]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[8]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[8]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[9] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[9]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[9]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[9]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[10] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[10]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[10]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[10]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[11] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[11]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[11]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[11]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[12] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[12]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[12]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[12]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[13] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[13]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[13]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[13]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[14] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[14]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[14]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[14]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[15] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[15]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[15]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[15]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[16] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[16]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[16]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[16]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[17] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[17]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[17]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[17]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[18] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[18]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[18]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[18]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[19] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[19]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[19]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[19]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[20] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[20]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[20]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[20]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[21] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[21]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[21]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[21]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[22] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[22]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[22]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[22]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[23] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[23]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[23]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[23]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[24] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[24]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[24]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[24]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[25] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[25]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[25]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[25]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[26] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[26]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[26]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[26]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[27] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[27]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[27]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[27]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[28] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[28]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[28]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[28]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[29] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[29]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[29]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[29]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[30] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[30]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[30]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[30]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	//--------------------------------------------------------------------
	mfB				=	mfBp;
	mfL				=	mfB * fCutoff + mfL;
	mfH				=	psAudio[31] - mfL - mfB * fRes;	
	mfBp			=	mfH * fCutoff + mfBp;
	// Output from filter
	psAudio[31]		=	(tfloat32)(mfL * (0.5 + (0.5*fRes)))	* fLP;
	psAudio[31]		+=	(tfloat32)(mfBp * (0.5 + (0.5*fRes)))	* fBP;
	psAudio[31]		+=	(tfloat32)(mfH * (0.5 + (0.5*fRes)))	* fHP;
	
	
	// Avoid denomarlization problems
	mfBp			+=	gfAnti_denormal;
	
}


