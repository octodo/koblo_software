/*!	\file IEQDisplayCallback.h
	\author Michael Olsen
	\date 15/Dec/2005
	\date 15/Dec/2005
*/

/*! \class IEQDisplayCallback.h
 * \brief Callback for IEQDisplay
 *
 *	Called by IEQDisplay to get curve info
*/
class IEQDisplayCallback
{
public:
	//! Called to get curve info. Since information is not cached by the control, the callee should cache it.
	/*!
		\param iCallbackUserDefined [in]: User defined value, passed when setting callback
		\param uiBand [in]: Band to get curve for
		\param pfData [out]: Pointer to buffer to fill with curve data. The excact content is up to the callee, but typically it will contain data for 20Hz to 20KHz on a logarithmic scale. Each value in the buffer is a dB value representing the value at that particular point.
		\param uiDataLength [in]: Length of the buffer (equals size of eq display in x direction)
	*/
	virtual void GetCurve(tint32 iCallbackUserDefined, tuint uiBand, tfloat32* pfData, tuint32 uiDataLength) = 0;
};
