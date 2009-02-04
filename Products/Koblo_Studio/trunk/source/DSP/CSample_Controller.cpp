
#include "KSOS.h"

CSample_Controller::CSample_Controller()
{
	
} 

CSample_Controller::~CSample_Controller()
{

} // destructor

CSample_Data* CSample_Controller::Get_Sample_Data_From_Name(const tchar* pszListName) const
{
	std::list<CSample_Data*> pSample_Data_List = gpApplication->Get_Sample_Data_List();
	std::list<CSample_Data*>::iterator  itSample_Data = pSample_Data_List.begin();
	
	
	for (; itSample_Data != pSample_Data_List.end(); itSample_Data++) {
		
		CSample_Data* pSample_Data = *itSample_Data;
		
		if (stricmp(pSample_Data->Name().c_str(), pszListName) == 0)
			return pSample_Data;
	}
	return NULL;
} 

tuint64 CSample_Controller::Get_Sample_Duration_From_Name( const tchar* pszListName)
{
	
	CSample_Data* pSample_Data = Get_Sample_Data_From_Name(pszListName);  
	
	CTake_Data* pTake_Info		=	pSample_Data->Get_Take_Data();
	
	std::string sWavePathNameL;
	std::string sWavePathNameR;
	Set_Wave_Path(pTake_Info, pszListName, sWavePathNameL, sWavePathNameR);
	
	
	CSample_DSP* mpSample;
	mpSample = new CSample_DSP(sWavePathNameL);
	
	tuint64 uiDuration = mpSample->GetLength();
	
	delete(mpSample);
	
	return uiDuration;
	
	
		
	
}

void CSample_Controller::Set_Wave_Path( CTake_Data* pTake_Info, 
									   const tchar* pszListName, 
									   std::string& rsWavePathNameL,
									   std::string& rsWavePathNameR ) const
{
	//CTake_Data* pTake_Info		=	pSample_Data->Get_Take_Data();
	
	std::string sMode				=	pTake_Info->Mode();
	if (stricmp("stereo", sMode.c_str()) == 0) {
		// STEREO
		// Get two streams for stereo channel
		rsWavePathNameL = pTake_Info->Left_Wave_File_Path();
		rsWavePathNameR = pTake_Info->Right_Wave_File_Path();
	}
	else if (stricmp("mono", sMode.c_str()) == 0) {
		// MONO
		// Get a single stream for mono channel
		if (pTake_Info->Left_Wave_File_Path().length()) {
			// Using left (or only) side of original for mono channel
			rsWavePathNameL = pTake_Info->Left_Wave_File_Path();
			rsWavePathNameR = "";
		}
		else if (pTake_Info->Right_Wave_File_Path().length()) {
			// Using right side of original for mono channel
			// Note! It *IS* correct to output into *left* side always!
			rsWavePathNameL = pTake_Info->Right_Wave_File_Path();
			rsWavePathNameR = "";
		}
	}	
}