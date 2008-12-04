

#include "SToolsInternalOS.h"

IAmp* IAmp::Create()
{
	return dynamic_cast<IAmp*>(new CAmp());
}

void CAmp::Destroy()
{
	delete dynamic_cast<CAmp*>(this);
}

CAmp::CAmp()
{
	
}

CAmp::~CAmp()
{
	
}

void CAmp::Process(tfloat fAmp, tfloat32* pfLeft, tfloat32* pfRight, tint lC)
{
	
	
	for (long i = 0; i < lC; i++) {
		
		
		
		pfLeft[i]	*= fAmp;
		pfRight[i]	*= fAmp;
		
	}	
}

void CAmp::Process32(tfloat fAmp, tfloat32* pfLeft, tfloat32* pfRight)
{
	
	pfLeft[0]	*= fAmp;
	pfLeft[1]	*= fAmp;
	pfLeft[2]	*= fAmp;
	pfLeft[3]	*= fAmp;
	pfLeft[4]	*= fAmp;
	pfLeft[5]	*= fAmp;
	pfLeft[6]	*= fAmp;
	pfLeft[7]	*= fAmp;
	
	pfLeft[8]	*= fAmp;
	pfLeft[9]	*= fAmp;
	pfLeft[10]	*= fAmp;
	pfLeft[11]	*= fAmp;
	pfLeft[12]	*= fAmp;
	pfLeft[13]	*= fAmp;
	pfLeft[14]	*= fAmp;
	pfLeft[15]	*= fAmp;
	
	pfLeft[16]	*= fAmp;
	pfLeft[17]	*= fAmp;
	pfLeft[18]	*= fAmp;
	pfLeft[19]	*= fAmp;
	pfLeft[20]	*= fAmp;
	pfLeft[21]	*= fAmp;
	pfLeft[22]	*= fAmp;
	pfLeft[23]	*= fAmp;
	
	pfLeft[24]	*= fAmp;
	pfLeft[25]	*= fAmp;
	pfLeft[26]	*= fAmp;
	pfLeft[27]	*= fAmp;
	pfLeft[28]	*= fAmp;
	pfLeft[29]	*= fAmp;
	pfLeft[30]	*= fAmp;
	pfLeft[31]	*= fAmp;
	
	
	
	pfRight[0]	*= fAmp;
	pfRight[1]	*= fAmp;
	pfRight[2]	*= fAmp;
	pfRight[3]	*= fAmp;
	pfRight[4]	*= fAmp;
	pfRight[5]	*= fAmp;
	pfRight[6]	*= fAmp;
	pfRight[7]	*= fAmp;
	
	pfRight[8]	*= fAmp;
	pfRight[9]	*= fAmp;
	pfRight[10]	*= fAmp;
	pfRight[11]	*= fAmp;
	pfRight[12]	*= fAmp;
	pfRight[13]	*= fAmp;
	pfRight[14]	*= fAmp;
	pfRight[15]	*= fAmp;
	
	pfRight[16]	*= fAmp;
	pfRight[17]	*= fAmp;
	pfRight[18]	*= fAmp;
	pfRight[19]	*= fAmp;
	pfRight[20]	*= fAmp;
	pfRight[21]	*= fAmp;
	pfRight[22]	*= fAmp;
	pfRight[23]	*= fAmp;
	
	pfRight[24]	*= fAmp;
	pfRight[25]	*= fAmp;
	pfRight[26]	*= fAmp;
	pfRight[27]	*= fAmp;
	pfRight[28]	*= fAmp;
	pfRight[29]	*= fAmp;
	pfRight[30]	*= fAmp;
	pfRight[31]	*= fAmp;
	

		
}