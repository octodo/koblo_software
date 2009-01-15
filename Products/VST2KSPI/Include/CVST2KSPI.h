

class CEcho : public virtual CBaseDSPEngine
{
public:
	//! Constructor
	CEcho();

	//! Destructor
	virtual ~CEcho();

	//! IDestructable override
	virtual void Destroy();

	// CBaseDSPEngine override
	virtual void Process(tfloat** ppfSamplesOut, const tfloat** ppfSamplesIn, tuint32 iNrOfSamples);
	// CBaseDSPEngine override
	virtual void Start();
	// CBaseDSPEngine override
	virtual void Stop();
	// CBaseDSPEngine override
	virtual void SetSampleRate(tuint32 uiSampleRate);

	void SetChannels(tuint32 uiChannels);

	// In dB
	void SetThreshold(float fThreshold);
	// In dB
	void SetGain(float fGain);
	void SetRatio(float fRatio);
	// In ms
	void SetAttack(float fAttack);
	// In ms
	void SetRelease(float fRelease);
	// In percent
	void SetKnee(int iKnee);
	
	void ProcessMono(float** ppfOut, const float** ppfIn, long lC);
	void ProcessStereo(float** ppfOut, const float** ppfIn, long lC);
	
protected:
	tuint32 muiChannels;
	
	double mEnvelope1;
	double mEnvelope2;
	double mAttack;
	double mRelease;
	double mdHKCoeffA;
	double mdHKCoeffB;
	double mdSKCoeffA;
	double mdSKCoeffB;
	double mdSKCoeffC;
	double mdSKThLinLow;
	double mdSKThLinHigh;
	
	float mfThresholddB;
	float mfGain;
	float mfRatio;
	int miKnee;

	void CookCurve();

	float mfHPCoeff;
	double mdHPHistoryL;
	double mdHPHistoryR;
};
