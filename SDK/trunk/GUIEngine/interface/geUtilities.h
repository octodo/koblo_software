/*!	\file geUtilities.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 27/Dec/2004
	\todo Move gControlMin and gControlMax
*/




// This should be moved to something called geConstants.h
const tint32 gControlMin = -32768;
const tint32 gControlMax = 32767;
//const tint32 gControlMin = 0 - (1 << 31);
//const tint32 gControlMax = (tint32)(((tuint32)1 << 31) - 1);



#if defined(WIN32)
	inline int WidthBytes(int bits)
	{
		return ((int)(((bits)+31) & (~31)) / 8);
	}
#endif
#ifdef _Mac
	inline int WidthBytes(int bits)
	{
		return ((bits + 31) / 32 * 4);
	}
#endif



// This really doesn't belong here

//! Enum to define keys. Note that small and capital letters are considered different keys
enum EKey
{
	Keya = 0,
	Keyb,
	Keyc,
	Keyd,
	Keye,
	Keyf,
	Keyg,
	Keyh,
	Keyi,
	Keyj,
	Keyk,
	Keyl,
	Keym,
	Keyn,
	Keyo,
	Keyp,
	Keyq,
	Keyr,
	Keys,
	Keyt,
	Keyu,
	Keyv,
	Keyw,
	Keyx,
	Keyy,
	Keyz,
	KeyA,
	KeyB,
	KeyC,
	KeyD,
	KeyE,
	KeyF,
	KeyG,
	KeyH,
	KeyI,
	KeyJ,
	KeyK,
	KeyL,
	KeyM,
	KeyN,
	KeyO,
	KeyP,
	KeyQ,
	KeyR,
	KeyS,
	KeyT,
	KeyU,
	KeyV,
	KeyW,
	KeyX,
	KeyY,
	KeyZ,
	Key0,
	Key1,
	Key2,
	Key3,
	Key4,
	Key5,
	Key6,
	Key7,
	Key8,
	Key9,
	KeySpace,
	KeyPlus,
	KeyMinus,
	KeyComma,
	KeyDot,
	KeyReturn,
	KeyEscape,
	KeyDelete,
	KeyBackSpace,
	KeyLeft,
	KeyRight,
	KeyUp,
	KeyDown,
	
	KeyExclamation,
	KeyQuotation,
	KeyPound,
	KeyDollar,
	KeyPercent,
	KeyAnd,
	KeyNote,
	KeyBracketLeft,
	KeyBracketRight,
	KeyMul,
	KeySlash,
	KeyColon,
	KeySemiColon,
	KeyLarger,
	KeyEqual,
	KeySmaller,
	KeyQuestion,
	KeyAdd,
	KeySquaredBrackedLeft,
	KeyBackSlash,
	KeySquaredBrackedRight,
	KeyHat,
	KeyUnderLine,
	KeyNote2,
	KeyCurlyBrackedLeft,
	KeyLine,
	KeyCurlyBrackedRight,
	KeyTilde,

	// Lasse: Added 2006-09-18:
	KeyBigUp,
	KeyBigDown,
	// .. Lasse

	// Lasse - Added 2007-05-14
	KeyHome,
	KeyEnd,
	KeyBigHome,
	KeyBigEnd,
	// .. Lasse

};

