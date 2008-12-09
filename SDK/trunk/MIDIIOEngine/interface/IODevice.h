/*!	\file IODevice.h
	\author Michael Olsen
	\date 09/Feb/2005
	\date 09/Feb/2005
*/

/*! \class IODevice
 * \brief A device with 16 output channels
*/
class IODevice : public virtual IDestructable
{
public:
	//! Create a new IODevice. Should only be called by MIDI Manager.
	/*!
		\param p [in]: System specific. Windows: Pointer to tint, holding index of device to create
		\return IODevice*: Pointer to created device, or NULL if error
	*/
	static IODevice* Create(void* p);
};

