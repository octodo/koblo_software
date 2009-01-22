#include "KSOS.h"


CMIDI_Sequencer::CMIDI_Sequencer()
{
	

}

CMIDI_Sequencer::~CMIDI_Sequencer()
{
	
}

void CMIDI_Sequencer::Init(char *pszIn_File, char *pszOut_File)
{
	const char *infile_name		= pszIn_File;
    const char *outfile_name	= pszOut_File;
    
    // the stream used to read the input file
    jdkmidi::MIDIFileReadStreamFile rs( infile_name );
/*	
    // the object which will hold all the tracks
    jdkmidi::MIDIMultiTrack tracks;
	
    // the object which loads the tracks into the tracks object
    jdkmidi::MIDIFileReadMultiTrack track_loader( &tracks );
	
    // the object which parses the midifile and gives it to the multitrack loader
    jdkmidi::MIDIFileRead reader( &rs, &track_loader );
	
    // load the midifile into the multitrack object
    reader.Parse();
    
    // create the output stream
    jdkmidi::MIDIFileWriteStreamFileName out_stream( outfile_name );
*/	
}