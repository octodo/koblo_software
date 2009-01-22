#ifndef _JDKMIDI_ADVANCEDSEQUENCER_H

#define _JDKMIDI_ADVANCEDSEQUENCER_H

#include "jdkmidi/midi.h"
#include "jdkmidi/msg.h"
#include "jdkmidi/sysex.h"
#include "jdkmidi/multitrack.h"
#include "jdkmidi/filereadmultitrack.h"
#include "jdkmidi/sequencer.h"
#include "jdkmidi/manager.h"
#include "jdkmidi/driver.h"


#include "jdkmidi/driverdump.h"

#include <string>
#include <vector>

#define MAX_WARP_POSITIONS (128)
#define MEASURES_PER_WARP (4)

namespace jdkmidi
{
  class AdvancedSequencer
    {
    public:
      AdvancedSequencer();
      virtual ~AdvancedSequencer();
      
      bool OpenMIDI( int in_port, int out_port, int timer_resolution=5 );
      void CloseMIDI();
      
      void SetMIDIThruEnable( bool f );
      bool GetMIDIThruEnable() const;
      
      void SetMIDIThruChannel( int chan );
      int GetMIDIThruChannel() const;
      
      void SetMIDIThruTranspose( int val );
      int GetMIDIThruTranspose() const;
      
      bool Load( const char *fname );
      void Reset();
      
      void GoToMeasure( int measure, int beat=0 );
      void GoToTime( MIDIClockTime t );
      void Play( int clock_offset = 0 );
      void RepeatPlay( bool enable, int start_measure, int end_measure );
      void Pause();
      void Stop();
      
      bool IsPlay()
        {
          return mgr.IsSeqPlay();
        }
      
      void UnmuteAllTracks();
      void SoloTrack( int trk );
      void UnSoloTrack();
      void SetTrackMute( int trk, bool f );
      
      void SetTempoScale( double scale );
      double GetTempoWithoutScale() const;
      double GetTempoWithScale() const;
      
      int GetMeasure() const;
      int GetBeat() const;
      
      int GetTimeSigNumerator() const;
      int	GetTimeSigDenominator() const;
      
      int GetTrackNoteCount( int trk ) const;
      const char *GetTrackName( int trk ) const;
      int GetTrackVolume( int trk ) const;
      
      void SetTrackVelocityScale( int trk, int scale );
      int GetTrackVelocityScale( int trk ) const;
      
      void SetTrackRechannelize( int trk, int chan );
      int GetTrackRechannelize( int trk ) const;
      
      void SetTrackTranspose( int trk, int trans );
      int GetTrackTranspose( int trk ) const;
      
      void ExtractMarkers( std::vector< std::string > *list );
      int GetCurrentMarker() const;
      
      int FindFirstChannelOnTrack(int trk);
      
      void ExtractWarpPositions();
      
      bool IsChainMode() const { return chain_mode; }
      
      MIDIMultiProcessor thru_processor;
      MIDIProcessorTransposer thru_transposer;
      MIDIProcessorRechannelizer thru_rechannelizer;

      MIDIDriverDump driver;

      MIDIMultiTrack tracks;


      MIDISequencerGUIEventNotifierText notifier;

      MIDISequencer seq;
      
      MIDIClockTime marker_times[1024];
      int num_markers;
      
      MIDIManager mgr;
      
      long repeat_start_measure;
      long repeat_end_measure;
      bool repeat_play_mode;
      
      int num_warp_positions;
      MIDISequencerState *warp_positions[MAX_WARP_POSITIONS];
      
      bool file_loaded;
      bool chain_mode;
    };
  
}

#endif
