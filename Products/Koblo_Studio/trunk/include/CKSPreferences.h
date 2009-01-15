// Copyright 2004, 2005,2006,2007,2008,2009 Koblo http://koblo.com
//
// This file is part of the Koblo SDK.
//
// the Koblo SDK is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// the Koblo SDK is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



//! Audio + Midi Preference format - this struct/class is backwards compatible and shouldn't be modified!
struct SKSPrefs_1 {
	tint32 iStructSize;
	//
	tint32 iAudioManagerIx;
	tint32 iAudioDeviceIx;
	tint32 iAudioSampleRate;
	tint32 iAudioBufferSize;
	tchar pszAudioDevName[1024];
	tchar cbAudioDevice_LastOpenCrashed;
	tchar cbAudioDevice_LastOpenSuccess;
	//
	tint32 iMIDIDevice;
	tchar pszMidiDevName[1024];
	tchar cbMidiDevice_LastOpenCrashed;
	tchar cbMidiDevice_LastOpenSuccess;

	SKSPrefs_1() {
		// Set default values
		iStructSize = sizeof(SKSPrefs_1);
#ifdef WIN32
		iAudioManagerIx = 1;		// WAVE IO (16 bit stereo backward compatible driver)
		iAudioDeviceIx = -1;		// Default WAVE mapper
		pszAudioDevName[0] = '\0';	// Ignore name
		iAudioSampleRate = 44100;
		iAudioBufferSize = -1;		// Default buffer size
#endif // WIN32
#ifdef _Mac
		iAudioManagerIx = 0;							// Core Audio
		iAudioDeviceIx = -1;							// Ignore index
		sprintf(pszAudioDevName, "Built-in Output");	// Set by name
		iAudioSampleRate = 44100;
		iAudioBufferSize = -1;							// Default buffer size
#endif // _Mac
		cbAudioDevice_LastOpenCrashed = (tchar)false;
		cbAudioDevice_LastOpenSuccess = (tchar)true;
		//
		iMIDIDevice = 0;
		pszMidiDevName[0] = '\0';
		cbMidiDevice_LastOpenCrashed = (tchar)false;
		cbMidiDevice_LastOpenSuccess = (tchar)false;
	};
	
	tbool IsSane();

	tbool Load(IFile* pFile);
	tbool Save(IFile* pFile);
}; // SKSPrefs_1


//! Extended preferences
class CKSPrefs_2 {
public:
	CKSPrefs_2() {
		// Set default values
		iStructSize = sizeof(CKSPrefs_2);
#ifdef WIN32
		iInputManagerIx = 1;			// WAVE IO (16 bit stereo backward compatible driver)
		iInputDeviceIx = -1;			// Default WAVE mapper
		pszInputDevName[0] = '\0';	// Ignore name
		iInputBufferSize = -1;		// Default buffer size
#endif // WIN32
#ifdef _Mac
		iInputManagerIx = 0;							// Core Audio
		iInputDeviceIx = -1;							// Ignore index
		sprintf(pszInputDevName, "Built-in Input");	// Set by name
		iInputBufferSize = -1;						// Default buffer size
#endif // _Mac
		cbInputDevice_LastOpenCrashed = (tchar)false;
		cbInputDevice_LastOpenSuccess = (tchar)true;
	};
	
	tbool IsSane();

	tbool Load(IFile* pFile);
	tbool Save(IFile* pFile);

	tint32 iStructSize;
	//
	tint32 iInputManagerIx;
	tint32 iInputDeviceIx;
	tint32 iInputBufferSize;
	tchar pszInputDevName[1024];
	tchar cbInputDevice_LastOpenCrashed;
	tchar cbInputDevice_LastOpenSuccess;
}; // CKSPrefs_2


//! 
class CKSPreferences {
public:
	static void LoadAll(SKSPrefs_1* pPrefs_1, CKSPrefs_2* pPrefs_2);
	static void SaveAll(SKSPrefs_1* pPrefs_1, CKSPrefs_2* pPrefs_2);

protected:
	static void GetPrefsFileName(std::string& rsPrefsFileName);
}; // CKSPreferences
