

class CKSUUID
{
public:
	//! Constructor
	CKSUUID();

	//! Destructor
	virtual ~CKSUUID();

	//! check if there is a UUID if not create one 
	void Check_UUID();
	//! get the uuid
	std::string Get_UUID();
	
	//! set the uuid
	void Set_UUID(std::string  sUUID){ msUUID = sUUID;};
	
	//! set the uuid
	void Set_UUID(){ Generate_UUID();};
	
	
protected:
	
	void Generate_UUID();
	
	//! UUID
	std::string msUUID;
	
};






