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




class CAmpSim : public virtual IAmpSim
{
public:
	CAmpSim();
	virtual ~CAmpSim();

	// IAmpSim overrides
	virtual void ST_CALL Destroy();
	virtual void ST_CALL SetSampleRate(float fSampleRate);
	virtual void ST_CALL Reset();
	virtual void ST_CALL Drive(float fDrive);
	virtual void ST_CALL Process(tfloat32* pfLeft, tfloat32* pfRight, tint32 iSamples);

protected:

	void CalckDrive();

	unsigned int muiRandomLast;

	float mfDrive;

	float mfLc1_z1_L;
	float mfLc2_z1_L;
	float mfLc3_z1_L;

	float mfLc1_z1_R;
	float mfLc2_z1_R;
	float mfLc3_z1_R;

	float mfLc_b1;
	float mfLc_a;
	// Left
	float mfA1_L;
	float mfA2_L;
	float mfA3_L;
	float mfA4_L;
	float mfA5_L;
	float mfA6_L;
	float mfA7_L;
	float mfA8_L;
	float mfA9_L;
	float mfA10_L;
	// Right
	float mfA1_R;
	float mfA2_R;
	float mfA3_R;
	float mfA4_R;
	float mfA5_R;
	float mfA6_R;
	float mfA7_R;
	float mfA8_R;
	float mfA9_R;
	float mfA10_R;

	float mfFollowerFactor;
	float mfPreAmpGain;
	float mfOutputGain;
};

