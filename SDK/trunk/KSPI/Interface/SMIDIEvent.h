// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo Stools.
//
// the Koblo Stools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo Stools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo SDK. If not, see <http://www.gnu.org/licenses/>.


//! Single MIDI event
struct SMIDIEvent
{
	union {
		//! Standard MIDI data
		tuchar pcData[4];

		//! Standard MIDI data, accessed as 32-bit integer
		tuint32 iData;
	};

	//! Timestamp of event. Measured in samples, with 0 being first frame of next processing buffer.
	tuint32 iTimeStamp;
};

