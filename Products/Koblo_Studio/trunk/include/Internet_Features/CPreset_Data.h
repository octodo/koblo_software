

class CPreset_Data:  public virtual CKSUUID
{
public:
	//! Constructor
	CPreset_Data();

	//! Destructor
	virtual ~CPreset_Data();
	
	//! set insert track
	void Insert_Track(tuint32 uiInsert_Track){ muiInsert_Track = uiInsert_Track; };
	
	//! set insert slot
	void Insert_Slot(tuint32 uiInsert_Slot){ muiInsert_Slot = uiInsert_Slot; };
	

	
protected:
	
	// track where the insert preset belongs
	tuint32 muiInsert_Track;
	
	// slot where the insert preset belongs
	tuint32 muiInsert_Slot;
	
	// slot where the insert preset belongs
	tuint32 muiProduct_ID;
	
	// slot where the insert preset belongs
	tuint32 muiVendor_ID;
	
	
	
};






