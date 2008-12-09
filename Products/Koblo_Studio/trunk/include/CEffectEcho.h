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


//! RAM allocated for echo effect (per channel). Must be a power of 2.
#define EFFECT_ECHO_BUFFER_SIZE 131072*2

/*! \brief Echo effect
*/
class CEffectEcho : public virtual CEffect, public virtual de::IParameterManagerCallback
{
public:
	//! Constructor
	CEffectEcho();

	//! Destructor
	virtual ~CEffectEcho();

	//! IPlugIn override
	virtual void KSPI_CALL Destroy();

	//! CEffect override
	virtual void KSPI_CALL Initialize();
	//! CEffect override
	virtual void KSPI_CALL DeInitialize();
	//! CEffect override
	virtual void KSPI_CALL Start();
	//! CEffect override
	virtual void KSPI_CALL ProcessInPlace(tfloat** ppfSamples, tuint32 iNrOfSamples);
	//! CEffect override
	virtual tuint32 KSPI_CALL GetNrOfParameters();
	//! CEffect override
	virtual void KSPI_CALL GetParameterInfo(tuint32 iIndex, kspi::SParameterInfo* pInfo);
	//! CEffect override
	virtual void KSPI_CALL SetParameter(tuint32 iIndex, tint32 iValue);
	//! CEffect override
	virtual void SetBPM(tfloat fBPM);

	//! IParameterManagerCallback
	virtual void GUINotify(tint32 iPaneID, tint32 iControlID, tint32 iValue);
	//! IParameterManagerCallback
	virtual void AutomationNotify(de::IParameterManager::EParameterType Type, tint32 iExtra, tint32 iParameterID, tint32 iValue);

	virtual void UpdateData(tint32 iID, tint32 iValue);

protected:
	virtual void SetDelayTime();
	//! Feedback
	tfloat mfFeedback;
	//! Damping
	tfloat mfDamping;
	//! High freq Damping
	tfloat mfHFDamping;
	//! Buffer (left channel)
	tfloat* mpfBuffer1;
	//! Buffer (right channel)
	tfloat* mpfBuffer2;
	//! Current writing position into buffer
	tint32 miPos;
	//! Current delay time in samples
	tint32 miInterFaceDelayTime;
	//! Current delay time in samples
	tint32 miDelayTime;
	//! Current Host delay time in samples
	tint32 miHostDelayTime;
	//! Synck to host time
	tbool  mbSyncToHost;
	//! Low pass history
	tfloat mpfLPHistory[2];
	//! High pass history
	tfloat mpfHPHistory[2];
};






