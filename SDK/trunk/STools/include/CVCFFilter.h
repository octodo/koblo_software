// Copyright 2004, 2005,2006,2007,2008 Koblo http://koblo.com
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



class CVCFFilter : public virtual IVCFFilter
{
public:
	CVCFFilter();
	virtual ~CVCFFilter();

	// IVCFFilter overrides
	virtual void ST_CALL Destroy();
	virtual void ST_CALL SetSampleRate(tfloat fSampleRate);
	virtual void ST_CALL Reset();
	virtual void ST_CALL Process(tfloat32* pf, tfloat fFreq, tfloat fQ, tint lC);
	virtual void ST_CALL Process(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC);
	virtual void ST_CALL Process3(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC);
	virtual void ST_CALL Process(tfloat32* pf, const tfloat32* psFreq, const tfloat32* psQ, tint lC);
	virtual void ST_CALL ProcessQ(tfloat32* pf, const tfloat32* psFreq, const tfloat32* psQ, tint lC);
	virtual void ST_CALL ProcessQ(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC);
	virtual void ST_CALL Process(tfloat32* pf, const tfloat32* psFreq, tint lC);

protected:
	float mfSampleRate;
	float mfOneDivSampleRate;
	float mfOneDivSampleRateMulPI;

	double y1;
	double y2;
	double y3;
	double y4;
	double oldx;
	double oldy1;
	double oldy2;
	double oldy3;
};

