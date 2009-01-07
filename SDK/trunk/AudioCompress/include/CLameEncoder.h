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

#ifndef _ac_clame_encoder_h
#define _ac_clame_encoder_h
// Created by Lasse Steen Bohnstedt, 2008-03-xx

#include <lame.h>

class CLameEncoder : public virtual IEncoder, public virtual CEncoder
{
public:
	CLameEncoder();
	virtual ~CLameEncoder();

	//! IDestructable override
	static IEncoder* Create();
	//! IDestructable override
	virtual void Destroy();

	// CEncoder overload
	virtual tbool Init_Descendant(EQuality eQuality);
	// CEncoder overload
	virtual tbool Process_Init_Descendant();
	// CEncoder overload
	virtual tbool Process_Exit_Descendant();
	// CEncoder overload
	virtual tbool ProcessRaw_Descendant(tchar* pcInput1, tchar* pcInput2, tint32 iInputBytes, tint32 iSamples, tint64* piAccumOverflows);
	// CEncoder overload
	virtual tbool Finalize_Descendant();
	
	// IEncoder overload
	virtual tbool GetQualityApproximateBitRate(tint32 iQualityNb, tint32* piKBPS_Mono, tint32* piKBPS_Stereo);
	
protected:	
	// Global record that keeps track of everything
	lame_global_flags* mpLameGlobalFlags;
	
	void my_warningf_hook(const char *format, va_list ap);
	void my_errorf_hook(const char *format, va_list ap);

	//! How much time to use for encoder approximation. 0=slowest (best quality), 9=fastest (low quality).
	tint32 miLoopQuality;
	//! How much bandwidth to use for encoding. 0=largest (best quality), 9=smallest (low quality).
	tint32 miVbrQuality;
	//! Whether we're encoding with fixed bitate
	tbool mbFixedBitRate;
};

#endif //#infdef _ac_clame_encoder_h
