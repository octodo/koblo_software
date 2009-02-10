
// Forward definition
class CSample_Data;

class CTake_Data: public virtual CKSUUID
{
public:
	//! Constructor
	CTake_Data(CSample_Data* pSampleThatOwnsThis);

	//! Destructor
	virtual ~CTake_Data();

	//! Assignment operator
//	CTake_Data operator=(const CTake_Data&);

	//! Get the UUID for the sample that "owns" this take
	std::string GetUUID_ForOwningSample();
	
	//! Get the name of the sample that "owns" this take
	std::string GetName_ForOwningSample();

	//! get the Sample UUID
	void Set_Description(const std::string& sDescription ){msDescription =  sDescription;};
	
	//! get the description
	std::string Get_Description() const { return msDescription;};
	
	//! get the Sample URL
	void URL(const std::string& sURL ){ msURL = sURL;};
	
	//! get the description
	std::string URL() const { return msURL;};
	
	//! is the take in stereo
	std::string Mode() const { return msMode;};
	
	//! mode stereo and mono
	void Mode(const std::string& sMode);
	
	//! get take name based on channel
	std::string Disk_Name(tuint uiChannel);
	
	//! set screen name
	void Screen_Name(const std::string& sScreen_Name){ msScreen_Name = sScreen_Name;};
	
	//! get screen name
	std::string Screen_Name() const {return msScreen_Name;};
	
	//! set left peak file path
	void Left_Peak_File_Path(std::string sLeft_Peak_File_Path){ msLeft_Peak_File_Path = sLeft_Peak_File_Path;};
	
	//! get left peak file path
	std::string Left_Peak_File_Path(){ return msLeft_Peak_File_Path;};
	
	//! set left peak file path
	void Right_Peak_File_Path(std::string sRight_Peak_File_Path){ msRight_Peak_File_Path = sRight_Peak_File_Path;};
	
	//! get left peak file path
	std::string Right_Peak_File_Path(){ return msRight_Peak_File_Path;};
	
	
	//! set left wave file path
	void Left_Wave_File_Path(std::string sLeft_Wave_File_Path){ msLeft_Wave_File_Path = sLeft_Wave_File_Path;};
	
	// get left wave file path
	std::string Left_Wave_File_Path() const { return msLeft_Wave_File_Path;};
	
	//! set right wave file path
	void Right_Wave_File_Path(std::string sRight_Wave_File_Path){ msRight_Wave_File_Path = sRight_Wave_File_Path;};
	
	//! get right wave file path
	std::string Right_Wave_File_Path() const { return msRight_Wave_File_Path;};
	
	//! check if take is in a given folder
	tbool In_Folder(std::string sFolder, std::string sExtencion);
	
	/*! check it the wave form pict files are in the "Pict Files" folder
	\ if they are store the path
	*/
	tbool Needs_Pict_Files();
	
	std::string GetMp3FilePath();
	std::string GetOggFilePath();
	
	
	tuint32 Channels() {return muiChannels; };
	
	tuint32 Channels() const {return muiChannels; };
	

	
	
protected:

	//! The sample that "owns" this take
	CSample_Data* mpOwningSample;

	//! take name
	std::string msScreen_Name;
	
	//! description
	std::string msDescription;

	//! url
	std::string msURL;
	
	//! stereo / mono
	std::string msMode;
	
	//! channels
	tuint muiChannels;
	
	//! calculates the number of channels based on the mode
	tuint32 Get_Channels_From_Mode();
	
	//! path to the left peak file
	std::string msLeft_Peak_File_Path;
	
	//! path to the right peak file
	std::string msRight_Peak_File_Path;
	
	//! path to the left peak file
	std::string msLeft_Wave_File_Path;
	
	//! path to the right peak file
	std::string msRight_Wave_File_Path;
	
	
	
};






