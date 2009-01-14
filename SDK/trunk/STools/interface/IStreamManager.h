/*!	\file IStreamManager.h
	\author Michael Olsen
	\date 19/Jun/2007
*/

/*! \brief The stream manager creates and manages hard disk streams
*
*	The stream manager creates and manages hard disk streams. It also acts as a sort of primitive polyphony manager (with no stealing)
*/
class IStreamManager : public virtual IDestructable
{
public:
	//! Creates IStreamManager
	static IStreamManager* Create();

	//! Creates the streams
	/*!
		\param iPolyphony [in]: Number of streams to create
	*/
	virtual void CreateStreams(tint32 iPolyphony) = 0;

	//! Get a stream to use. Each call must match a call to ReleaseStream().
	/*!
		\return IStream*: Stream to use
	*/
	virtual IStream* GetStream() = 0;

	//! Release stream. Call this when you're done with the stream, i.e. voice is no longer playing
	/*!
		\param pStream [in]: Stream to release. It will be asynchronously deleted when not being accessed by OnTimer() anymore
	*/
	virtual void ReleaseStream(IStream* pStream) = 0;

	//! Call this to see if sound playback has stuttered resently (flag will be reset after)
	virtual tbool GetStutter() = 0;
};
