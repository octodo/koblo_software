// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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


enum EExportDSPOrder {
	geExportDSP_Start,

	geExportDSP_Peak_Before,
	geExportDSP_Peak_Action,
	geExportDSP_Peak_After,

	geExportDSP_Before,
	geExportDSP_Action,
	geExportDSP_After,

	geExportDSP_Done
}; // EExportDSPOrder


class CExportDSPTask : public CProgressTask {
public:

	tuint64 muiPosSaved;

	tint32 miTrack;
	std::string msTrackName;

	std::string sDestFolder;
	std::string sDestNameAndExt;

	IFile* mpfOutput;
	ac::IEncoder* mpEncoder;
	ac::EAudioCodec meCodec;
	ac::EQuality meQuality;
	tint32 miChannels;

	tint64 miFirstSample;
	tint64 miFinalSample;
	tbool mbRemoveInitialSilence;
	tint32 miTailMS;

	tfloat32 mfNormalizationFactor;
	tbool mbSkipFileOutput;

	tint32 miPortionSize;
	tint32 miBytesPerPortion;

	tint32 miActionOrder;

	CExportDSPTask(
		tint32 iTrackNb, const tchar* pszTrackName,
		ac::EAudioCodec eCodec, ac::EQuality eQuality, tint32 iChannels, tint32 iTailMS
		);

	virtual ~CExportDSPTask();
	virtual void Destroy();

	virtual tbool DoWork();
	virtual tbool IsDone();

protected:
	tfloat32* ppfInputs[2];
	tfloat32* ppfOutputs[2];
	tchar* pcOutputL;
	tchar* pcOutputR;
	void AllocBuffers(tint32 iSamples);

	std::string msIntermediateFiles[2];
	IFile* mpfIntermediateFiles[2];

	tuint64 muiEndPosPlusTail;
	tuint64 muiPortionSize;
	tuint64 uiWholePortions_SilenceBefore;
	tuint64 uiWholePortions;
	tuint64 uiLastPortionSize;

	tint64 iAccumOverflows;

	tfloat32 mfPeak;


	tbool IsOutMix()
	{ return miTrack < 0; };

	tbool IsSampleRangeLimited()
	{ return ((miFirstSample > 0) || (miFinalSample >= 0)); };

	tbool DoEncode_InALoop(tbool bWrite);
	tbool DoEncode(tbool bWrite);
	tbool DoEncode_FirstTimeHere();



}; // CExportDSPTask
