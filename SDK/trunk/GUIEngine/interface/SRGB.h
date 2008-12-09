/*!	\file SRGB.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

#if WIN32
#pragma pack(push, 1)
#endif

/*! \class SRGB
 * \brief Struct that contains r+g+b info (red / green / blue)
*/
struct SRGB {
	//! Constructor (no parameter, initial values all 0)
	SRGB() { u.gun.r=0; u.gun.g=0; u.gun.b=0; u.gun.reserved=0; }

	//! Constructor
	/*!
		\param r [in]: Initial red value
		\param g [in]: Initial green value
		\param b [in]: Initial blue value
	*/
	SRGB(tuchar r, tuchar g, tuchar b) { u.gun.r=r; u.gun.g=g; u.gun.b=b; u.gun.reserved=0; }

	union srgbu {
		//! Struct to split up r+g+b+pad byte
		struct SGun {
#if	defined(WIN32)
			//! Blue
			tuchar b;
			//! Green
			tuchar g;
			//! Red
			tuchar r;
			//! Padding
			tuchar reserved;
#endif
#ifdef	_Mac
			tuchar reserved;
			tuchar r;
			tuchar g;
			tuchar b;
#endif
		} gun;

		//! Pixel value (r+g+b+padding)
		tuint32 pixel;
	} u;
};

#if WIN32
#pragma pack(pop)
#endif