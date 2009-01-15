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


enum EAudioImportOrder {
	geAudioImport_Start,

	geAudioImport_Decode_Before,
	geAudioImport_Decode_Action,
	geAudioImport_Decode_After,

	geAudioImport_Copy_Before,
	geAudioImport_Copy_Action,
	geAudioImport_Copy_After,

	geAudioImport_Peak_Before,
	geAudioImport_Peak_Action,
	geAudioImport_Peak_After,

	geAudioImport_Region_Before,
	geAudioImport_Region_Action,
	geAudioImport_Region_After,

	geAudioImport_Done
}; // EAudioImportOrder


class CImportAudioTask : public CProgressTask {
public:


	enum EStereoBehavior { geStereoDoAsk, geStereoDoSplit, geStereoDoKeep };

	std::string msSrcPathName;
	std::string msPathOnly;
	std::string msClipName;
	std::string msExt;

	std::string msDstNameL;
	std::string msDstNameR;
	std::string msDstPathNameL;
	std::string msDstPathNameR;
	std::string msDstPathName_ForCopy;

	ac::EAudioCodec meCodec;
	ac::EQuality meSrcQuality;
	tbool mbSrcLossyCompressed;
	tbool mbStereo;
	tbool mbSplit;
	tint32 miBitWidth;

	IFile* mpfSrc;
	IDecoder* mpDecoder;
	tint32 miDecodeIx;
	IFile* mpfDstL;
	IFile* mpfDstR;

	tbool mbDstIsAlreadyThere;
	tbool mbCopyLossyOriginal;

	tint32 miRegion_TrackID;
	tint64 miRegion_TrackPos;
	tbool mbDeleteNonLossyOriginal;

	tint32 miAudioImportOrder;

	CImportAudioTask() {
	

		meCodec = ac::geAudioCodecUndefined;

		mpfSrc = mpfDstL = mpfDstR = NULL;
		mpDecoder = NULL;
		miAudioImportOrder = geAudioImport_Start;

		mbDstIsAlreadyThere = false;
		mbCopyLossyOriginal = false;

		miRegion_TrackID = -1;
		miRegion_TrackPos = -1;
		mbDeleteNonLossyOriginal = false;
	};

	virtual ~CImportAudioTask();

	virtual void Destroy();

	tbool Init( const tchar* pszSrcPath, tbool bDoesWaveAlreadyExist /*= false*/, EStereoBehavior eStereoBehavior /*= geStereoDoAsk*/, tbool bForceOriginalIsLossy /*= false*/);
	void Init_InsertAsRegionAfterImport(tint32 iTrackID, tint64 iTrackPos);

	tbool IsOpened() { return (mpfDstL != NULL); };
	tbool Open();

	virtual tbool DoWork();
	virtual tbool IsDone();
};