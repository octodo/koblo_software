

class CKSUUID
{
public:
	//! Constructor
	CKSUUID();

	//! Destructor
	virtual ~CKSUUID();
	
	//! copy constructor
	CKSUUID::CKSUUID(const CKSUUID& Data_Old);

	//! check if there is a UUID if not create one 
	void Check_UUID();
	
	//! get the uuid
	std::string Get_UUID();
	
	//! set the uuid
	void Set_UUID(std::string  sUUID){ msUUID_X = sUUID;};
	
	//! set the uuid
	void Set_UUID(){ Generate_UUID();};
	
	//! UUID is set
	tbool UUID_Is_Set();
	
	//! UUID is set
	void Clear_UUID(){msUUID_X.clear();};
	
	//! make a new uuid
	//! get the uuid
	std::string Get_New_UUID();
	
	std::string msUUID_X;
	
	
protected:
	
	void Generate_UUID();
	
	//! UUID
	//std::string msUUID;
	
};






