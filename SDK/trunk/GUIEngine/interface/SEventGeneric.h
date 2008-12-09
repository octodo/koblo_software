//!
enum EEventGenericType {
	giEventIllegal = 0,

	giEventCustom,
	giEventKeyboard,
	giEventMouse
};

//!
struct SEventGeneric {
	EEventGenericType eType;

	tbool bInsideControlRect;

	tint32 iMsg;
	ge::SPos pos;
};
