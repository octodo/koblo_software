/*!	\file CAutoDelete.h
	\author Michael Olsen
	\date 10/Jan/2005
	\date 10/Jan/2005
*/

/*! \class CAutoDelete
 * \brief An CAutoDelete is used to automatically destroy an object (inherited from IDestructable) when it goes out of scope. It is a variant on the stl auto_ptr, in that where auto_ptr delete's an object, CAutoDelete uses Destroy()
 *
 *	The use of this class is different in that it doesn't have an interface. This means it is created and destroyed "directly", i.e. through constructor and destructor.
*/
template<class _T> class CAutoDelete
{
public:
	//! Constructor
	/*!
		\parm pObject [in]: Pointer to object to be stored as allocated object
	*/
	explicit CAutoDelete(_T* pObject = 0)
		: mbOwns(pObject != 0), mpObject(pObject) {}

	//! Constructor
	/*!
		\parm Object [in]: Object, whose ownership is transfered to this object
	*/
	CAutoDelete(const CAutoDelete<_T>& Object)
		: mbOwns(Object.mbOwns), mpObject(Object.Release()) {}

	//! operator =
	/*!
		\parm Object [in]: Object, whose ownership is transfered to this object. However, transfer is only done if Objects stored pointer differes from the stored pointer of this object
		\return CAutoDelete<_T>&: Reference to this
	*/
	CAutoDelete<_T>& operator=(const CAutoDelete<_T>& Object)
	{
		if (this != &Object) {
			if (mpObject != Object.Get()) {
				if (mbOwns) {
					mpObject->Destroy();
				}
				mbOwns = Object.mbOwns;
			}
			else {
				if (Object.mbOwns) {
					mbOwns = true;
				}
			}

			mpObject = Object.Release();
		}

		return (*this);
	}

	//! Destructor
	~CAutoDelete()
	{
		if (mbOwns) {
			mpObject->Destroy();
		}
	}

	//! operator *.  Returns a reference to object that our pointer points to
	/*!
		\return _T&: Reference to object that our pointer points to
	*/
	_T& operator*() const
	{
		return (*Get());
	}

	//! operator ->.  Returns the stored pointer
	/*!
		\return _T&: The stored pointer
	*/
	_T* operator->() const
	{
		return (Get());
	}

	//! Returns the stored pointer
	/*!
		\return _T&: The stored pointer
	*/
	_T* Get() const
	{
		return (mpObject);
	}

	//! Releases the object, and returns the pointer we previously stored
	/*!
		\return _T*: Pointer we're releasing
	*/
	_T* Release() const
	{
		((CAutoDelete<_T>*)this)->mbOwns = false;
		return (mpObject);
	}

	//! operator ().  Returns the stored pointer
	/*!
		\return _T&: The stored pointer
	*/
	operator _T*() const
	{
		return mpObject;
	}

protected:
	//! Indicates whether we own the object or not. An owned object is destroyed at destruction time
	tbool mbOwns;

	//! The pointer to object that we store
	_T* mpObject;
};


