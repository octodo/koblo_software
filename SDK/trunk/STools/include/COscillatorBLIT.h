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
// along with the Koblo Stools. If not, see <http://www.gnu.org/licenses/>.



class COscillatorBLIT : public virtual IOscillatorBLIT
{
public:
	COscillatorBLIT();
	virtual ~COscillatorBLIT();

	// IOscillatorBLIT overrides
	virtual void Destroy();
	virtual void SetSampleRate(tfloat fSampleRate);
	virtual void Reset();
	virtual void ProcessSquare(tfloat32* pfOut, const tfloat32* pfPitch, tfloat fPWM, tint lC);
	virtual void ProcessSquareFM(tfloat32* pfOut, const tfloat32* pfPitch, tfloat fPWM, const tfloat32* pfFM, const tfloat32* pfFMAmt, tint lC);
	virtual void ProcessSquareFM(tfloat32* pfOut, const tfloat pfPitch, tfloat fPWM, const tfloat32* pfFM, const tfloat pfFMAmt, tint lC);
	virtual void ProcessSquare(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfPWM, tint lC);
	virtual void ProcessSquare(tfloat32* pfOut, const tfloat pfPitch, const tfloat pfPWM, tint lC);
	virtual void ProcessSquare32(tfloat32* pfOut, const tfloat pfPitch, const tfloat pfPWM);
	virtual void ProcessSquareSH(tfloat32* pfOut, const tfloat32* pfPitch, tint lC);
	virtual void ProcessSquareSync(tfloat32* pfOut, const tfloat32* pfPitchSlave, const tfloat32* pfPitchMaster, const tfloat32* pfPWM, tint lC);
	virtual void ProcessSaw(tfloat32* pfOut, const tfloat32* pfPitch, tint lC);
	virtual void ProcessSaw(tfloat32* pfOut, tfloat fPitch, tint lC);
	virtual void ProcessSaw32(tfloat32* pfOut, tfloat fPitch);
	virtual void ProcessSawFM(tfloat32* pfOut, const tfloat32* pfPitch,const tfloat32* pfFMInput,const tfloat32* pfFMAmt, tint lC);
	virtual void ProcessSawFM(tfloat32* pfOut, const tfloat pfPitch,const tfloat32* pfFMInput,const tfloat pfFMAmt, tint lC);
	virtual void ProcessSawSync(tfloat32* pfOut, const tfloat32* pfPitchSlave, const tfloat32* pfPitchMaster, tint lC);
	virtual void ProcessSawAndSquare(tfloat32* pfOut, const tfloat32* pfPitch, const tfloat32* pfPWM, tfloat fSawVolume, tfloat fSquareVolume, tint lC);
	virtual void ProcessSawAndSquareSync(tfloat32* pfOut, const tfloat32* pfPitchSlave, const tfloat32* pfPitchMaster, const tfloat32* pfPWM, tfloat fSawVolume, tfloat fSquareVolume, tint lC);

protected:
	float mfSampleRate;
	float mfOneDivSampleRate;
	float mfPhase;
	double mdVolume;
	float mfPhaseMaster;

	void AddBLEP(double dOffset, double dVol);

	double* mpdBuffer;
	long mlBufferSize;
	long mlBufferIndex;

	double mdSHLeak;
};

