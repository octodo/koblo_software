/*!	\file EMouseMsg.h
	\author Michael Olsen
	\date 27/Dec/2004
	\date 01/May/2008
*/

//! Enumeration to desribe which type of mouse message was the reason for the call
enum EMouseMsg {
	//! Left button was pressed down
	LeftButtonDown = 0,

	//! Left button was released
	LeftButtonUp,

	//! Left button was double-clicked
	LeftButtonDblClk,

	//! Right button was pressed down
	RightButtonDown,

	//! Right button was released
	RightButtonUp,

	//! Right button was double-clicked
	RightButtonDblClk,

	//! Mouse was moved
	MouseMove,

	//! Mouse was moved during a drag 'n drop operation
	MouseDrag,

	//! Mouse drag and drop was finished
	MouseDrop,

	//! Number of possible mouse messages
	NrOfMouseMessages
};
