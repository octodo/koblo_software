/*!	\file SRGBA.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
*/

#if WIN32
#pragma pack(push, 1)
#endif

/*! \class SRGBA
 * \brief Struct that contains r+g+b+a info (red / green / blue / alpha)
*/
struct SRGBA {
	//! Constructor (no parameters, all values set to 0)
	SRGBA() { u.gun.r=0; u.gun.g=0; u.gun.b=0; u.gun.a=0; }

	//! Contructor
	/*!
		\param r [in]: Initial red value
		\param g [in]: Initial green value
		\param b [in]: Initial blue value
		\param a [in]: Initial alpha value
	*/
	SRGBA(tuchar r, tuchar g, tuchar b, tuchar a) { u.gun.r=r; u.gun.g=g; u.gun.b=b; u.gun.a=a; }

	union srgbau {
		//! Struct to split up r/g/b/a
		struct SGun {
#if	defined(WIN32)
			//! Blue
			tuchar b;
			//! Green
			tuchar g;
			//! Red
			tuchar r;
			//! Alpha
			tuchar a;
#endif
#ifdef	_Mac
			tuchar a;
			tuchar r;
			tuchar g;
			tuchar b;
#endif
		} gun;

		//! Pixel value (r+g+b+a)
		tuint32 pixel;
	} u;
};

#if WIN32
#pragma pack(pop)
#endif








