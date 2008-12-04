/*!	\file IFFT.h
	\author Michael Olsen
	\date 13/Dec/2005
	\date 13/Dec/2005
*/

/*! \class IFFT
 * \brief Control which draws a spectral analyze
*/
class IFFT : public virtual IControl, public virtual IDestructable
{
public:
	//! Create IFFT
	static IFFT* Create();

	//! Initializer
	/*!
		\param iID [in]: Control ID
		\param Colour [in]: Colour to draw with
		\param pParentWindow [in]: Window to draw in
		\param uiSampleRate [in]: Samplerate of system
		\param uiChannels [in]: Number of channels in sound data delivered. Must be 1 or 2
	*/
	virtual void Init(tint32 iID, const SRGB& Colour, IWindow* pParentWindow, tuint32 uiSampleRate, tuint uiChannels) = 0;

	//! Queue some input (doesn't actually process it)
	/*
		\param ppfSamples [in]: Array of pointers to channel data (sound)
		\param uiSampleCount [in]: Number of samples in each buffer
	*/
	virtual void Input(const tfloat32** ppfSamples, tuint32 uiSampleCount) = 0;
};