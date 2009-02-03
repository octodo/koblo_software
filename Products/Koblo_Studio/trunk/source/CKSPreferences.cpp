

#include "KSOS.h"

tbool SKSPrefs_1::IsSane()
{
	return (
			(iStructSize == sizeof(*this))
			&&
			((cbAudioDevice_LastOpenCrashed == (tchar)false) || (cbAudioDevice_LastOpenCrashed == (tchar)true))
			&&
			((cbAudioDevice_LastOpenSuccess == (tchar)false) || (cbAudioDevice_LastOpenSuccess == (tchar)true))
			&&
			((cbMidiDevice_LastOpenCrashed == (tchar)false) || (cbMidiDevice_LastOpenCrashed == (tchar)true))
			&&
			((cbMidiDevice_LastOpenSuccess == (tchar)false) || (cbMidiDevice_LastOpenSuccess == (tchar)true))
			&&
			(iAudioManagerIx >= 0)
			&&
			(iAudioDeviceIx >= -1)
			&&
			(iAudioSampleRate > 0)
			&&
			(
				(iAudioBufferSize == -1)
				||
				((iAudioBufferSize > 0)) && ((iAudioBufferSize % 32) == 0)
			)
			&&
			(iMIDIDevice >= 0)
			);
} // SKSPrefs_1::IsSane


tbool SKSPrefs_1::Load(IFile* pFile)
{
	tint32 iActualRead = (tint32)pFile->Read((tchar*)this, sizeof(*this));
	if (iActualRead == sizeof(*this)) {
		iAudioManagerIx = CPSwap(iAudioManagerIx);
		iAudioDeviceIx = CPSwap(iAudioDeviceIx);
		iAudioSampleRate = CPSwap(iAudioSampleRate);
		iAudioBufferSize = CPSwap(iAudioBufferSize);
		//
		iMIDIDevice = CPSwap(iMIDIDevice);
		
		tbool bIsStructSane = IsSane();
		if (bIsStructSane) {
			return true;
		}
	}

	return false;
} // SKSPrefs_1::Load


tbool SKSPrefs_1::Save(IFile* pFile)
{
	if (!IsSane())
		return false;

	SKSPrefs_1 prefsTemp = *this;

	prefsTemp.iAudioManagerIx = CPSwap(prefsTemp.iAudioManagerIx);
	prefsTemp.iAudioDeviceIx = CPSwap(prefsTemp.iAudioDeviceIx);
	prefsTemp.iAudioSampleRate = CPSwap(prefsTemp.iAudioSampleRate);
	prefsTemp.iAudioBufferSize = CPSwap(prefsTemp.iAudioBufferSize);
	//
	prefsTemp.iMIDIDevice = CPSwap(prefsTemp.iMIDIDevice);

	tint64 iActuallyWritten = pFile->Write((tchar*)&prefsTemp, sizeof(prefsTemp));

	return (iActuallyWritten == sizeof(prefsTemp));
} // SKSPrefs_1::Save


tbool CKSPrefs_2::IsSane()
{
	return (
			(iStructSize == sizeof(*this))
			&&
			((cbInputDevice_LastOpenCrashed == (tchar)false) || (cbInputDevice_LastOpenCrashed == (tchar)true))
			&&
			((cbInputDevice_LastOpenSuccess == (tchar)false) || (cbInputDevice_LastOpenSuccess == (tchar)true))
			&&
			(iInputManagerIx >= 0)
			&&
			(iInputDeviceIx >= -1)
			&&
			(
				(iInputBufferSize == -1)
				||
				((iInputBufferSize > 0)) && ((iInputBufferSize % 32) == 0)
			)
		);
} // CKSPrefs_2::IsSane


tbool CKSPrefs_2::Load(IFile* pFile)
{
	tint32 iActualRead = (tint32)pFile->Read((tchar*)this, sizeof(*this));
	if (iActualRead == sizeof(*this)) {
		tbool bIsStructSane = IsSane();
		if (bIsStructSane) {
			return true;
		}
	}

	return false;
} // CKSPrefs_2::Load


tbool CKSPrefs_2::Save(IFile* pFile)
{
	if (!IsSane())
		return false;

	tint64 iActuallyWritten = pFile->Write((tchar*)this, sizeof(*this));

	return (iActuallyWritten == sizeof(*this));
} // CKSPrefs_2::Save


void CKSPreferences::LoadAll(SKSPrefs_1* pPrefs_1, CKSPrefs_2* pPrefs_2)
{
	std::string sPathName;
	GetPrefsFileName(sPathName);

	CAutoDelete<IFile> pFile(IFile::Create());
	if (pFile->Open((const tchar*)(sPathName.c_str()), IFile::FileRead) == false) {
		// Cannot open file
		return;
	}

	// Load backwards compatible settings
	SKSPrefs_1 prefsTemp_1;
	if (!prefsTemp_1.Load(pFile)) {
		// Error
		return;
	}
	if (pPrefs_1)
		*pPrefs_1 = prefsTemp_1;

	// Load extended settings
	CKSPrefs_2 prefsTemp_2;
	if (!prefsTemp_2.Load(pFile)) {
		// Error
		return;
	}
	if (pPrefs_2)
		*pPrefs_2 = prefsTemp_2;
} // CKSPreferences::LoadAll


void CKSPreferences::SaveAll(SKSPrefs_1* pPrefs_1, CKSPrefs_2* pPrefs_2)
{
	std::string sPathName;
	GetPrefsFileName(sPathName);

	CAutoDelete<IFile> pFile(IFile::Create());
	if (pFile->Open((const tchar*)(sPathName.c_str()), IFile::FileCreate) == false) {
		// Cannot create file
		return;
	}

	if (pPrefs_1 == NULL) {
		// Huh?
		return;
	}
	if (!pPrefs_1->Save(pFile)) {
		// error
		return;
	}

	if (pPrefs_2 == NULL) {
		// Huh?
		return;
	}
	if (!pPrefs_2->Save(pFile)) {
		// error
		return;
	}
} // CKSPreferences::SaveAll


void CKSPreferences::GetPrefsFileName(std::string& rsPrefsFileName)
{
	tchar pszPrefPath[512];
	IFile::GetSystemDirectory(IFile::SystemDirPreferences, pszPrefPath);

	rsPrefsFileName = pszPrefPath;
	rsPrefsFileName += "Koblo:Koblo Studio:";
	IFile::CreateDirectory(rsPrefsFileName.c_str());

	rsPrefsFileName += "KS.prefs";
} // CKSPreferences::GetPrefsFileName
