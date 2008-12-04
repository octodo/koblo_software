
class IDrive
{
public:
	static IDrive* Create();
	virtual void Destroy() = 0;

	// Drive / saturation
	virtual void ProcessHarch(tfloat32* pfData, tfloat32 fDrive, tint lC) = 0;
	// Drive / saturation
	virtual void ProcessHarch(tfloat32* pfOut0,tfloat32* pfOut1, tfloat32 fDrive, tint lC) = 0;
	// Soft drive
	virtual void ProcessSoft(tfloat32* pfData, tfloat32 fDrive, tint lC) = 0;
	
};



