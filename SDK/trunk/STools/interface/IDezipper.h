

/*

Class for dezipping of single parameter

Accepts parameters of any value in double format

*/

class IDezipper
{
public:
	static IDezipper* Create();
	virtual void Destroy() = 0;

	virtual void SetSampleRate(float fSampleRate) = 0;

	// Jump to target (useful for initialization)
	virtual void JumpToTarget(double dTarget) = 0;

	// Set target value
	virtual void SetTarget(double dTarget) = 0;

	// Process lC samples, and return current parameter value
	virtual double Dezip(long lC) = 0;
};



