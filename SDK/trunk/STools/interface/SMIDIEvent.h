


struct SMIDIEvent
{
	// Time offset from start of next buffer.
	tuint32 ulOffset;
	// Raw MIDI data.
	tchar pchData[4];

	bool operator==(const SMIDIEvent& a) {if ((ulOffset == a.ulOffset) && (pchData[0] == a.pchData[0]) && (pchData[1] == a.pchData[1]) && (pchData[2] == a.pchData[2]) && (pchData[3] == a.pchData[3])) {return true;} else {return false;}}
};

