/*
   Schroeder / Moorer reverb model implementation

   original algorithm by Jezar at Dreampoint, June 2000
   http://www.dreampoint.co.uk/

   optimization & integration into Koblo Studio by Sinus at SinusWeb, June 2008
   http://www.sinusweb.de/

*/

#include "KSOS.h"
//#include "CKobloVerb.h"
#include <math.h>

allpass::allpass()
{
	bufidx = 0;
}

void allpass::setbuffer(float *buf, int size) 
{
	buffer = buf; 
	bufsize = size;
}

void allpass::mute()
{
	for (int i=0; i<bufsize; i++) {
		buffer[i]=0;
	}
}

void allpass::setfeedback(float val) 
{
	feedback = val;
}

float allpass::getfeedback() 
{
	return feedback;
}
 
comb::comb()
{
	filterstore = 0;
	bufidx = 0;
}

void comb::setbuffer(float *buf, int size) 
{
	buffer = buf; 
	bufsize = size;
}

void comb::mute()
{
	for (int i=0; i<bufsize; i++)
		buffer[i]=0;
}

void comb::setdamp(float val) 
{
	damp1 = val; 
	damp2 = 1-val;
}

float comb::getdamp() 
{
	return damp1;
}

void comb::setfeedback(float val) 
{
	feedback = val;
}

float comb::getfeedback() 
{
	return feedback;
}

revmodel::revmodel()
{

	// defaults
	revquality=10; // always best quality
	mfSampleRatekHz = 44.1f;
	pre=(unsigned short)(10 * mfSampleRatekHz);
	poi=0;
	revcolor=0.5f;
	revcolor2=0.5f;
	l1L=l2L=l3L=l1R=l2R=l3R=0;
	semiout1L=semiout1R=0;

	predelay = new float[96000];
	flush();

 // Tie the components to their buffers
	combL[0].setbuffer(bufcombL1,combtuningL1);
	combR[0].setbuffer(bufcombR1,combtuningR1);
	combL[1].setbuffer(bufcombL2,combtuningL2);
	combR[1].setbuffer(bufcombR2,combtuningR2);
	combL[2].setbuffer(bufcombL3,combtuningL3);
	combR[2].setbuffer(bufcombR3,combtuningR3);
	combL[3].setbuffer(bufcombL4,combtuningL4);
	combR[3].setbuffer(bufcombR4,combtuningR4);
	combL[4].setbuffer(bufcombL5,combtuningL5);
	combR[4].setbuffer(bufcombR5,combtuningR5);
	combL[5].setbuffer(bufcombL6,combtuningL6);
	combR[5].setbuffer(bufcombR6,combtuningR6);
	combL[6].setbuffer(bufcombL7,combtuningL7);
	combR[6].setbuffer(bufcombR7,combtuningR7);
	combL[7].setbuffer(bufcombL8,combtuningL8);
	combR[7].setbuffer(bufcombR8,combtuningR8);
	combL[8].setbuffer(bufcombL9,combtuningL9);
	combR[8].setbuffer(bufcombR9,combtuningR9);
	combL[9].setbuffer(bufcombL10,combtuningL10);
	combR[9].setbuffer(bufcombR10,combtuningR10);


	allpassL[0].setbuffer(bufallpassL1,allpasstuningL1);
	allpassR[0].setbuffer(bufallpassR1,allpasstuningR1);
	allpassL[1].setbuffer(bufallpassL2,allpasstuningL2);
	allpassR[1].setbuffer(bufallpassR2,allpasstuningR2);
	allpassL[2].setbuffer(bufallpassL3,allpasstuningL3);
	allpassR[2].setbuffer(bufallpassR3,allpasstuningR3);
	allpassL[3].setbuffer(bufallpassL4,allpasstuningL4);
	allpassR[3].setbuffer(bufallpassR4,allpasstuningR4);

	// Set default values
	allpassL[0].setfeedback(0.5f);
	allpassR[0].setfeedback(0.5f);
	allpassL[1].setfeedback(0.5f);
	allpassR[1].setfeedback(0.5f);
	allpassL[2].setfeedback(0.5f);
	allpassR[2].setfeedback(0.5f);
	allpassL[3].setfeedback(0.5f);
	allpassR[3].setfeedback(0.5f);

	setWet(initialwet);
	setRoomSize(initialroom);
	setDry(initialdry);
	setDamp(initialdamp);
	//setwidth(initialwidth);

	// Buffer will be full of rubbish - so we MUST mute them
	mute();
}

void revmodel::mute()
{
    int i;

	for (i=0;i<numcombs;i++)
	{
		combL[i].mute();
		combR[i].mute();
	}
	for (i=0;i<numallpasses;i++)
	{
		allpassL[i].mute();
		allpassR[i].mute();
	}
}

void revmodel::flush()
{
  for (int i=0; i<96000;i++)
  {
    predelay[i]=0;
  }
}

// master effect
void revmodel::processReplace(float **inputs, float **outputs, long numsamples)
{
	float *inputL = inputs[0];
	float *inputR = inputs[1];
	float *outputL = outputs[0];
	float *outputR = outputs[1];

	if(mbReverb_On){

		 //float absmL;
		 register float mL,mR;

		 unsigned short p_pre=pre;
		 float p_revcolor=revcolor;
		 unsigned char p_revquality=revquality;
		 unsigned short p_poi=poi;
		 float p_wet=wet;

		 float p_dry=dry*0.5f;
		 register float p_l1L=l1L;
		 register float p_l2L=l2L;
		 register float p_l3L=l3L;
		 register float p_l1R=l1R;
		 register float p_l2R=l2R;
		 register float p_l3R=l3R;

		 register float p_gain=gain*0.5f;
		 
		 register float p_maxL=maxL;
		 register float p_maxR=maxR;

		 float absL,absR;

		 register float semioutL;
		 register float semioutR;
		 register float ppp;

		 bool damped;
		 if (damp1==0) { damped=false; } 
				  else { damped=true;  }

		 float p_semiout1L=semiout1L;
		 float p_semiout1R=semiout1R;

		 float alpha = 200.0f*(revcolor2+0.005525f); // 49.1312f
		 float a0, b0a0, a1a0;

		 a0 = 1.0f+alpha;
		 b0a0=2.0f/a0; 
		 a1a0=(1.0f-alpha)/a0;        
		 
		 while(numsamples-- > 0)
		 {

		   mL = mR = 0;
		   ppp=predelay[p_poi];

			if (damped) {
				for(int i=0; i<p_revquality; i++) {
					mL += combL[i].process(ppp);
					mR += combR[i].process(ppp); 
				}
			}
			else{
				for(int i=0; i<p_revquality; i++){ 
					mL += combL[i].process2(ppp);
					mR += combR[i].process2(ppp); }
			}
				 
		   mL = allpassL[3].process(allpassL[2].process(allpassL[1].process(allpassL[0].process(mL))))*p_wet;
		   mR = allpassR[3].process(allpassR[2].process(allpassR[1].process(allpassR[0].process(mR))))*p_wet;

		   // ------------ Filters ------------------------

		   semioutL=((mL)-(p_revcolor*(3.0f*p_l1L + p_l2L+p_l2L +p_l3L)));
		   semioutR=((mR)-(p_revcolor*(3.0f*p_l1R + p_l2R+p_l2R +p_l3R)));  

		   semioutL = b0a0*semioutL-(a1a0*p_semiout1L)+0.00000000001f;
		   semioutR = b0a0*semioutR-(a1a0*p_semiout1R)+0.00000000001f;

		   p_semiout1L=semioutL;
		   p_semiout1R=semioutR;

		   semioutL+=(*inputL)*p_dry;
		   semioutR+=(*inputR)*p_dry;

		   if (semioutL>0) { absL=(semioutL); } else { absL=-(semioutL); }
		   if (semioutR>0) { absR=(semioutR); } else { absR=-(semioutR); }
		   if (absL>p_maxL) { p_maxL=absL; }
		   if (absR>p_maxR) { p_maxR=absR; }

		   p_l3L=p_l2L;
		   p_l3R=p_l2R;
		   p_l2L=p_l1L;
		   p_l2R=p_l1R;
		   p_l1L=mL;
		   p_l1R=mR;

		   predelay[p_poi]=(*inputL+*inputR)*p_gain; // Originalinput im PreDelayBuffer merken
		   if(++p_poi>=p_pre) p_poi = 0;

		   (*outputL)= semioutL; 
		   (*outputR)= semioutR; 

		   inputL++; 
		   inputR++; 
		   outputL++;
		   outputR++; 

		} // while

		poi=p_poi;

		l1L=p_l1L;
		l2L=p_l2L;
		l3L=p_l3L;
		l1R=p_l1R;
		l2R=p_l2R;
		l3R=p_l3R;

		semiout1L=p_semiout1L;
		semiout1R=p_semiout1R;

		maxL=p_maxL;
		maxR=p_maxR;
	}
		else{
			for(long i=0; i< numsamples; i++){
				outputL[i]	=	inputL[i];
				outputR[i]	=	inputR[i];
		}
	}
}

void revmodel::update()
{
    // Recalculate internal values after parameter change

	int i;

	roomsize1 = roomsize;
	damp1 = damp;
	gain = fixedgain;

	for(i=0; i<numcombs; i++)
	{
		combL[i].setfeedback(roomsize1);
		combR[i].setfeedback(roomsize1);
	}

	for(i=0; i<numcombs; i++)
	{
		combL[i].setdamp(damp1);
		combR[i].setdamp(damp1);
	}
}

// The following get/set functions are not inlined, because
// speed is never an issue when calling them, and also
// because as you develop the reverb model, you may
// wish to take dynamic action when they are called.

void revmodel::setReverbOn(tbool bReverb_On) 
{
	mbReverb_On = bReverb_On;
	if(!bReverb_On){
		mute();
		flush();
	}
		
		

}

void revmodel::setRoomSize(float value)
{
	roomsize = (float)((log10((value*20.0f)+1.0f))*0.75630f*scaleroom) + offsetroom;
	update();
}


void revmodel::setPredelay(float value)
{
    pre = (unsigned short)(value * mfSampleRatekHz);
}

void revmodel::setDamp(float value)
{
	damp = value*scaledamp;
	update();
}

void revmodel::setHP(float value)
{
	revcolor = value;
}

void revmodel::setLP(float value)
{
	revcolor2 = value;
}

void revmodel::setWet(float value)
{
	wet = value*scalewet;
	update();
}

void revmodel::setDry(float value)
{
	dry = value*scaledry;
}


void revmodel::setSampleRate(tuint32 uiFreqHz)
{
	tfloat32 fSampleRatekHz = uiFreqHz / 1000.0f;

	if (fSampleRatekHz != mfSampleRatekHz) {
		// Sample-rate actually did change

		// Recalculate values that depend on samplerate
		{
			// pre
			tfloat64 f64PreValue = (tfloat64)pre / mfSampleRatekHz;
			pre = (unsigned short)((tfloat32)(f64PreValue * fSampleRatekHz));
		}

		// Set new frequency
		mfSampleRatekHz = fSampleRatekHz;

		// Kill effect-buffers
		flush();
		mute();
	}
}

//ends
