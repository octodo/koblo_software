/*
 *  libjdkmidi-2004 C++ Class Library for MIDI
 *
 *  Copyright (C) 2004  J.D. Koftinoff Software, Ltd.
 *  www.jdkoftinoff.com
 *  jeffk@jdkoftinoff.com
 *
 *  *** RELEASED UNDER THE GNU GENERAL PUBLIC LICENSE (GPL) April 27, 2004 ***
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/*
**	Copyright 1986 to 1998 By J.D. Koftinoff Software, Ltd.
**
**	All rights reserved.
**
**	No one may duplicate this source code in any form for any reason
**	without the written permission given by J.D. Koftinoff Software, Ltd.
**
*/

#ifndef _JDKMIDI_TRACK_H
#define _JDKMIDI_TRACK_H

#include "jdkmidi/midi.h"
#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"

namespace jdkmidi
{
  
  const int MIDITrackChunkSize=512;
  
  class  MIDITrackChunk 	
    {
    public:
      const MIDITimedBigMessage * GetEventAddress( int event_num ) const;
      MIDITimedBigMessage * GetEventAddress( int event_num );
      
    protected:
      
    private:
      MIDITimedBigMessage buf[MIDITrackChunkSize];
    }; 
  
  const int MIDIChunksPerTrack=512;
  
  class  MIDITrack 	
    {
    public:
      MIDITrack( int size=0 );
      
      MIDITrack( const MIDITrack &t );
      
      ~MIDITrack();
      
      void	Clear();
      void	Shrink();
      
      void	ClearAndMerge( const MIDITrack *src1, const MIDITrack *src2 );
      
//	bool	Insert( int start_event, int num_events );
//    bool 	Delete( int start_event, int num_events);
//    void 	Sort();
      
      bool	Expand( int increase_amount=(MIDITrackChunkSize) );
      
      MIDITimedBigMessage * GetEventAddress( int event_num );
      
      const MIDITimedBigMessage * GetEventAddress( int event_num ) const;
      
      const 	MIDITimedBigMessage *GetEvent( int event_num ) const;
      MIDITimedBigMessage *GetEvent( int event_num );
      bool	GetEvent( int event_num, MIDITimedBigMessage *msg ) const;
      
      bool	PutEvent( const MIDITimedBigMessage &msg );
      bool	PutEvent( const MIDITimedMessage &msg, MIDISystemExclusive *sysex );
      bool	SetEvent( int event_num, const MIDITimedBigMessage &msg );
      
      bool	MakeEventNoOp( int event_num );
      
      bool	FindEventNumber( MIDIClockTime time, int *event_num ) const;
      
      int	GetBufferSize()	const;
      int	GetNumEvents() const;
      
    private:
      
//	void 	QSort( int left, int right );
      
      MIDITrackChunk * chunk[MIDIChunksPerTrack];
      
      int buf_size;
      int num_events;
    }; 
  
}

#endif

