

/*

	Original code by Torben Kjærhus, Modifyed by Max Grønlund
	may not work on samplerates above 96.000 kHz

*/
class IAmpSim
{
public:
	static IAmpSim* ST_CALL Create();
	virtual void ST_CALL Destroy() = 0;
	// Ste sample rate
	virtual void ST_CALL SetSampleRate(tfloat fSampleRate) = 0;

	virtual void ST_CALL Reset() = 0;
	// Set drive amount
	virtual void ST_CALL Drive(tfloat fDrive) = 0;
	// Stereo version only
	virtual void ST_CALL Process(tfloat32* pfLeft, tfloat32* pfRight, tint iSamples) = 0;
};



