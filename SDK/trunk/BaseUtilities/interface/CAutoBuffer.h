/*!	\file CAutoBuffer.h
	\author Michael Olsen
	\date 16/Dec/2005
	\date 16/Dec/2005
*/

/*! \class CAutoBuffer
 * \brief An CAutoBuffer is used to automatically destroy an array ("buffer") when it goes out of scope.
 *
 *	The use of this class is different in that it doesn't have an interface. This means it is created and destroyed "directly", i.e. through constructor and destructor.
*/
template<class _T> class CAutoBuffer
{
public:
	//! Constructor which doesn't allocate buffer
	CAutoBuffer()
		: mpBuffer(NULL)
		{}

	//! Constructor which allocates buffer
	/*!
		\parm uiSize [in]: Size (in elements) of buffer to allocate
	*/
	explicit CAutoBuffer(tuint32 uiSize)
		: mpBuffer(NULL)
		{
			Allocate(uiSize);
		}

	//! Destructor
	~CAutoBuffer()
		{
			if (mpBuffer) {
				delete[] mpBuffer;
			}
		}

	//! Allocate buffer. If buffer is already allocated it will be reallocated (previous content is destroyed). After allocation the buffer is cleared.
	/*!
		\parm uiSize [in]: Size (in elements) of buffer to allocate
	*/
	void Allocate(tuint32 uiSize)
		{
			if (mpBuffer) {
				delete[] mpBuffer;
			}
			mpBuffer = new _T[uiSize];
			memset(mpBuffer, 0, uiSize * sizeof(_T));
		}

	//! operator *. Returns a reference to buffer that we point to
	/*!
		\return _T&: Reference to buffer that we point to
	*/
	_T& operator*() const
		{
			return *Get();
		}

	//! operator ->. Returns the buffer pointer
	/*!
		\return _T&: The buffer pointer
	*/
	_T* operator->() const
		{
			return Get();
		}

	//! Returns the buffer pointer
	/*!
		\return _T&: The buffer pointer
	*/
	_T* Get() const
		{
			return mpBuffer;
		}

	//! operator (). Returns the buffer pointer
	/*!
		\return _T&: The buffer pointer
	*/
	operator _T*() const
	{
		return Get();
	}

protected:
	//! The array ("buffer") that we store. Is NULL is not allocated yet
	_T* mpBuffer;
};


