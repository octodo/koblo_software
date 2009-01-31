

class CTake_Data: public virtual CKSUUID
{
public:
	//! Constructor
	CTake_Data();

	//! Destructor
	virtual ~CTake_Data();

	
	
	
	
	//! Which stereo channels to map: 1 = left, 2 = right, 3 = both
	tint32 iOriginalChannelMask;
	
	//! Is this displayed as a stereo clip in list?
	tbool bIsStereoInList;
	
	// -------------
	// Variables below here aren't saved but regenerated upon load
	std::string sWavePathNameL;
	std::string sWavePathNameR;
	
	//! get the Sample UUID
	void Set_Description(std::string sDescription ){msDescription =  sDescription;};
	
	//! get the description
	std::string Get_Description(){ return msDescription;};
	
	//! get the Sample UUID
	void Set_URL(std::string sURL ){ msURL = sURL;};
	
	//! get the description
	std::string Get_URL(){ return msURL;};
	
	//! path to the left peak file
	std::string sLeft_Peak_File_Path;
	
	//! path to the right peak file
	std::string sRight_Peak_File_Path;
	
	//! is the take in stereo
	std::string Mode(){ return muiChannels ? "stereo": "mono";};
	
	void Mode(std::string sMode) { msMode = sMode;};
	
	//! get channel name
	std::string Name(tuint uiChannel);
	
	//! set name and how many channels there is in the sampel
	void Name(std::string sName, tuint32 uiChannels);
	
	//! set take number
	void uiTake(tuint uiTake_Nr){ muiTake_Nr = uiTake_Nr;};
	

	
	
protected:
	
	//! take name
	std::string msName;
	
	
	//! Name of the left side (or mono) wave file for clip
//	std::string sWaveNameL;
	//! Name of the right side wave file for clip
//	std::string sWaveNameR;
	
	//! description
	std::string msDescription;
	//! channels
	tuint32 muiChannels;
	//! take_nr
	tuint32 muiTake_Nr;
	//! url
	std::string msURL;
	//! url
	std::string msMode;
	
	
	
};






