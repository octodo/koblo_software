/*
   Schroeder / Moorer reverb model implementation

   original algorithm by Jezar at Dreampoint, June 2000
   http://www.dreampoint.co.uk/

   optimization & integration into Koblo Studio by Sinus at SinusWeb, June 2008
   http://www.sinusweb.de/

*/

#ifndef _denormals_
#define _denormals_
 #define undenormalise(sample) if(((*(unsigned int*)&sample)&0x7f800000)==0) sample=0.0f
#endif//_denormals_

#ifndef _tuning_
#define _tuning_

const int	numcombs		= 10; // best quality
const int	numallpasses	= 4;
const float	muted			= 0;
const float	fixedgain		= 0.015f; 
const float scalewet		= 3;
const float scaledry		= 2;
const float scaledamp		= 0.55f; // 0.4f
const float scaleroom		= 0.33f; // 0.28
const float offsetroom		= 0.65f; // 0.7 
const float initialroom		= 0.5f;
const float initialdamp		= 0.5f;
const float initialwet		= 1.0f; //1/scalewet;
const float initialdry		= 0;
const float initialwidth	= 1;
const float freezemode		= 0.5f;
const int	stereospread	= 23;

const int combtuningL1		= 1116;
const int combtuningR1		= 1116+stereospread;
const int combtuningL2		= 1188;
const int combtuningR2		= 1188+stereospread;
const int combtuningL3		= 1277;
const int combtuningR3		= 1277+stereospread;
const int combtuningL4		= 1356;
const int combtuningR4		= 1356+stereospread;
const int combtuningL5		= 1422;
const int combtuningR5		= 1422+stereospread;
const int combtuningL6		= 1491;
const int combtuningR6		= 1491+stereospread;
const int combtuningL7		= 1557;
const int combtuningR7		= 1557+stereospread;
const int combtuningL8		= 1617;
const int combtuningR8		= 1617+stereospread;
const int combtuningL9		= 1678;
const int combtuningR9		= 1678+stereospread;

const int combtuningL10	= 1044;
const int combtuningR10	= 1044+stereospread;

const int allpasstuningL1	= 556;                
const int allpasstuningR1	= 556+stereospread;   
const int allpasstuningL2	= 441;              
const int allpasstuningR2	= 441+stereospread; 
const int allpasstuningL3	= 341;              
const int allpasstuningR3	= 341+stereospread;
const int allpasstuningL4	= 225;             
const int allpasstuningR4	= 225+stereospread;

#endif//_tuning_

#ifndef _allpass_
#define _allpass_

class allpass
{
public:
					allpass();
			void	setbuffer(float *buf, int size);
   	inline  float	process(float inp);
			void	mute();
			void	setfeedback(float val);
			float	getfeedback();

    float	feedback;
	float	*buffer;
	int	bufsize;
	int	bufidx;
};


// Big to inline - but crucial for speed

inline float allpass::process(float input)
{
	float bufout;
	
	bufout = buffer[bufidx];
	undenormalise(bufout);
	
	buffer[bufidx] = input + (bufout*feedback);

	if(++bufidx>=bufsize) bufidx = 0;

	return (bufout-input);
}

#endif // allpass

#ifndef _comb_
#define _comb_

class comb
{
public:
					comb();
			void	setbuffer(float *buf, int size);

	inline  float	process(float inp);
	inline  float	process2(float inp);

			void	mute();
			void	setdamp(float val);
			float	getdamp();
			void	setfeedback(float val);
			float	getfeedback();
private:
	float	feedback;
	float	filterstore;
	float	damp1;
	float	damp2;
	float	*buffer;
	int	bufsize;
	int	bufidx;
};


// Big to inline - but crucial for speed

inline float comb::process(float input)
{
	float output;

	output = buffer[bufidx];
	undenormalise(output);

	filterstore = (output*damp2) + (filterstore*damp1);
	undenormalise(filterstore);

    buffer[bufidx] = input + (filterstore*feedback);

	if(++bufidx>=bufsize) bufidx = 0;

	return output;
}

inline float comb::process2(float input)
{

	filterstore = buffer[bufidx];
	undenormalise(filterstore);

	buffer[bufidx] = input + (filterstore*feedback);

	if(++bufidx>=bufsize) bufidx = 0;

	return filterstore;
}

#endif // comb

#ifndef _revmodel_
#define _revmodel_

class revmodel
{
public:
	revmodel();
	void mute();
	void processReplace(float **inputs, float **outputs, long numsamples);
	void setReverbOn(tbool bReverb_On);
	void setRoomSize(float value);
	void setPredelay(float value);
	void setLP(float value);
	void setHP(float value);
	void setDamp(float value);
	void setWet(float value);
	void setDry(float value);
	void setSampleRate(tuint32 uiFreqHz);
    void flush();
    float maxL, maxR;

private:
	void	update();
	
	tbool mbReverb_On;

    unsigned short pre;
    float revcolor;
    float revcolor2;
    unsigned char revquality;
	float gain;
	float roomsize, roomsize1;
	float damp,damp1;
	float wet; 
	float dry;
	tfloat32 mfSampleRatekHz;
	float width;
	float mode;
    float *predelay;
    unsigned short poi;
    float l1L,l2L,l3L,l1R,l2R,l3R;
    float semiout1L,semiout1R;

	// Comb filters
	comb combL[numcombs];
	comb combR[numcombs];

	// Allpass filters
	allpass	allpassL[numallpasses];
	allpass	allpassR[numallpasses];

	// Buffers for the combs
	float	bufcombL1[combtuningL1];
	float	bufcombR1[combtuningR1];
	float	bufcombL2[combtuningL2];
	float	bufcombR2[combtuningR2];
	float	bufcombL3[combtuningL3];
	float	bufcombR3[combtuningR3];
	float	bufcombL4[combtuningL4];
	float	bufcombR4[combtuningR4];
	float	bufcombL5[combtuningL5];
	float	bufcombR5[combtuningR5];
	float	bufcombL6[combtuningL6];
	float	bufcombR6[combtuningR6];
	float	bufcombL7[combtuningL7];
	float	bufcombR7[combtuningR7];
	float	bufcombL8[combtuningL8];
	float	bufcombR8[combtuningR8];
	float	bufcombL9[combtuningL9];
	float	bufcombR9[combtuningR9];
	float	bufcombL10[combtuningL10];
	float	bufcombR10[combtuningR10];

	// Buffers for the allpasses
	float	bufallpassL1[allpasstuningL1];
	float	bufallpassR1[allpasstuningR1];
	float	bufallpassL2[allpasstuningL2];
	float	bufallpassR2[allpasstuningR2];
	float	bufallpassL3[allpasstuningL3];
	float	bufallpassR3[allpasstuningR3];
	float	bufallpassL4[allpasstuningL4];
	float	bufallpassR4[allpasstuningR4];
};

#endif//_revmodel_

//ends