

class CTake_Data: public virtual CKSUUID
{
public:
	//! Constructor
	CTake_Data();

	//! Destructor
	virtual ~CTake_Data();
	
	//! Assignment operator
//	CTake_Data operator=(const CTake_Data&);

	//! get the Sample UUID
	void Set_Description(const std::string& sDescription ){msDescription =  sDescription;};
	
	//! get the description
	std::string Get_Description(){ return msDescription;};
	
	//! get the Sample URL
	void URL(const std::string& sURL ){ msURL = sURL;};
	
	//! get the description
	std::string URL(){ return msURL;};
	
	//! is the take in stereo
	std::string Mode(){ return msMode;};
	
	//! mode stereo and mono
	void Mode(const std::string& sMode) { msMode = sMode;};
	
	//! get channel name
	std::string Disk_Name(tuint uiChannel);
	
	//! set screen name
	void Screen_Name(const std::string& sScreen_Name){ msScreen_Name = sScreen_Name;};
	
	//! get screen name
	std::string Screen_Name(){return msScreen_Name;};
	
	// set left peak file path
	void Left_Peak_File_Path(std::string sLeft_Peak_File_Path){ msLeft_Peak_File_Path = sLeft_Peak_File_Path;};
	
	// get left peak file path
	std::string Left_Peak_File_Path(){ return msLeft_Peak_File_Path;};
	
	// set left peak file path
	void Right_Peak_File_Path(std::string sRight_Peak_File_Path){ msRight_Peak_File_Path = sRight_Peak_File_Path;};
	
	// get left peak file path
	std::string Right_Peak_File_Path(){ return msRight_Peak_File_Path;};
	
	
	// set left wave file path
	void Left_Wave_File_Path(std::string sLeft_Wave_File_Path){ msLeft_Wave_File_Path = sLeft_Wave_File_Path;};
	
	// get left wave file path
	std::string Left_Wave_File_Path(){ return msLeft_Wave_File_Path;};
	
	// set right wave file path
	void Right_Wave_File_Path(std::string sRight_Wave_File_Path){ msRight_Wave_File_Path = sRight_Wave_File_Path;};
	
	// get right wave file path
	std::string Right_Wave_File_Path(){ return msRight_Wave_File_Path;};
	

	
	
protected:
	
	//! take name
	std::string msScreen_Name;
	
	//! description
	std::string msDescription;

	//! url
	std::string msURL;
	//! url
	std::string msMode;
	
	//! path to the left peak file
	std::string msLeft_Peak_File_Path;
	
	//! path to the right peak file
	std::string msRight_Peak_File_Path;
	
	//! path to the left peak file
	std::string msLeft_Wave_File_Path;
	
	//! path to the right peak file
	std::string msRight_Wave_File_Path;
	
	
	
};






