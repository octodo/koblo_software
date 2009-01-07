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


class CRBJFilter : public virtual IRBJFilter
{
public:
	CRBJFilter();
	virtual ~CRBJFilter();

	// IRBJFilter overrides
	virtual void Destroy();
	virtual void SetSampleRate(tfloat fSampleRate);
	virtual void Reset();
	virtual void CreateLowPass(tfloat fFreq, tfloat fQ = 0.5);
	virtual void Process(tfloat32* pf, tint lC);
	virtual void ProcessLP(tfloat32* pf, const tfloat32* psFreq, tfloat32 sQ, tint lC);

protected:
	float mfSampleRate;
	float mfOneDivSampleRate;

	enum EFilterType {
		FilterTypeNone = 0,
		FilterTypeLowPass,
	};
	EFilterType meFilterType;
	float mfFreq;
	float mfQ;

	double x0, x1, y0, y1;
	double a1, a2;
	double b0, b1, b2;

	void CalcFilter();
	void CalcEmptyFilter();
	void CalcLowPass();
};

