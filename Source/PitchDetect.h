#ifndef PITCHDETECT
#define PITCHDETECT

#include <math.h>
#include <string.h>
#include "Biquad.h"
//#include "fftw3.h"
#include <kiss_fftr.h>
#include <kiss_fft.h>


#ifndef max
#define max(x, y) ((x) > (y)) ? (x) : (y)
#endif
#ifndef min
#define min(x, y) ((x) < (y)) ? (x) : (y)
#endif

const double pi = 3.14159265358979;

class PitchChecker
{
public:
	double prevPitch;
	int pitchConfidence;
	PitchChecker(){}
	~PitchChecker(){}
};

class PitchDetect
{
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchDetect)
public:
	kiss_fft_cpx *spectrum;
	kiss_fft_scalar *real;
	kiss_fftr_cfg fft;
	kiss_fftr_cfg ifft;
	//fftwf_complex* outFFT;
	//fftwf_complex* in;
	//fftwf_plan pFFT, pIFFT;
	float* difference;
	float* tempBuffer;
	float* cumDifference;
	float* r;
	float* hannWindow;
	float* accum;
	float* mag;
	float sampleRate;
	float threshold; //default YIN threshold value, best value between 0.1-0.2
	float maxHz; //highest frequency detectable by YIN
	float minHz;
	float periodLength;
	unsigned short bufferSize;
	unsigned short halfBufferSize;
	unsigned short periodIndex;
	unsigned short lastPeriod; 
	unsigned short minPeriod; 
	unsigned short read; 

	Biquad* lpFilter;
	PitchChecker check;
	PitchDetect(float sampleRate, unsigned short pitchBufferSize)
	{
		this->sampleRate	= sampleRate;
		this->bufferSize	= pitchBufferSize;
		threshold = 0.2f;
		maxHz = 0.2f*sampleRate;
		minPeriod = (unsigned short)(sampleRate/maxHz + 0.5);
		periodLength = 0.f;
		mag = new float[2*bufferSize];
		halfBufferSize = bufferSize/2;
		lastPeriod = halfBufferSize;
		periodIndex = 0;
		read = 0;

		//FFTW set up
		//outFFT = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) *2*bufferSize);
		//in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) *2*bufferSize);
		//pFFT = fftwf_plan_dft_1d(2*bufferSize, in, outFFT, FFTW_FORWARD, FFTW_MEASURE);
		//pIFFT = fftwf_plan_dft_1d(2*bufferSize, in , outFFT , FFTW_BACKWARD, FFTW_MEASURE);
		fft = kiss_fftr_alloc(bufferSize*2,0,NULL, NULL);
		ifft = kiss_fftr_alloc(bufferSize*2,1,0,0);
		spectrum =(kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*bufferSize*2);
		real = (kiss_fft_scalar*)malloc(sizeof(kiss_fft_scalar)*bufferSize*2);	

		tempBuffer = new float[bufferSize];
		cumDifference = new float[bufferSize];
		r = new float[bufferSize];
		difference = new float[bufferSize];
		hannWindow = new float[bufferSize];
		accum = new float[halfBufferSize];
		for (unsigned short n = 0; n < bufferSize; n++)
			hannWindow[n] = 0.5f*(1.0f + sin(2*pi*(float)n/(bufferSize)));

		lpFilter = new Biquad();	// create a Biquad, lpFilter;
		lpFilter->setBiquad(bq_type_lowpass, 500 / sampleRate, 0.707, -12); //improve pitch detection with LP filter

		// initialize our arrays 
		memset(tempBuffer, 0, bufferSize*sizeof(float));
		memset(cumDifference, 0, bufferSize*sizeof(float));
		memset(r, 0, bufferSize*sizeof(float));
		memset(difference, 0, bufferSize*sizeof(float));
		memset(mag, 0, 2*bufferSize*sizeof(float));
		memset(accum, 0, halfBufferSize*sizeof(float));
	}

	~PitchDetect()
	{
		//fftwf_destroy_plan(pFFT); // FFTW clean up
		//fftwf_destroy_plan(pIFFT);
		//fftwf_free(outFFT);
		//fftwf_free(in);


		kiss_fft_cleanup();   
		free(fft);
		free(ifft);
		delete[] hannWindow;
		delete[] tempBuffer;
		delete[] cumDifference;
		delete[] r;
		delete[] difference;
		delete lpFilter;
		delete[] mag;
		if(accum) delete[] accum;
	}

	void cmnd() //this function is taken from https://ccrma.stanford.edu/~mhorgan/220c/yin_helpers.cpp [Accessed 26th August 2013]
	{		
		float runningSum = 0;
		cumDifference[0] = 1;
		for (unsigned short tau = 1; tau < halfBufferSize; tau++)
		{
			runningSum += difference[tau];
			if (runningSum == 0) 
				cumDifference[tau] = 0;
			else
			cumDifference[tau] = (tau*difference[tau])/ runningSum;
		}
	}
 	void absThresh()
	{		
		//limit search to target range 86Hz-1kHz
		unsigned short tMin = minPeriod;
		// initialize global_min to a value > minimia
		float globalMin = 0.9;
		float periodicity = 0;
		//find fundamental period
		for(;tMin < halfBufferSize; tMin++)
		{
			//Look for a minimum first
			if (cumDifference[tMin] < cumDifference[tMin-1] && cumDifference[tMin] < cumDifference[tMin+1])
			{
				//Check if the minimum is under the threshold
				if (cumDifference[tMin]< threshold) 
				{
				//The threshold determines the list of candidates admitted to the set, and can be interpreted as the
				// proportion of aperiodic power tolerated within a periodic signal.
						lastPeriod = tMin;
						break;
				}
				else if (cumDifference[tMin]  < globalMin)
					// Otherwise we store it if it is less than the current global minimum
				{
					globalMin = cumDifference[tMin];
					lastPeriod = tMin;					
				}	
			}		
		}

		// Since we want the periodicity and and not aperiodicity:
		// periodicity = 1 - aperiodicity
		periodicity = 1 - cumDifference[lastPeriod];

		if(lastPeriod >= halfBufferSize || lastPeriod <= minPeriod || cumDifference[lastPeriod] > globalMin || periodicity >= 1.0 || periodicity < 0.1)
			lastPeriod = 0;

		periodIndex = lastPeriod;
	}

	float paraInter(float *buffer, int index) //this function is taken from https://github.com/JorenSix/TarsosDSP/blob/master/TarsosDSP/src/main/java/be/hogent/tarsos/dsp/pitch/FastYin.java [Accessed 26th August 2013]
	{
		//parabolic interpolation on period length is 
			float betterTau;
			int x0,x2;
			if ( index < 1)
			x0 = index;
			else
				x0 = index -1;
			if (index+1 < halfBufferSize)
				x2 = index + 1;
			else 
				x2 = index;
			if(x0 == index)
			{
				if(buffer[index] <= buffer[x2])
					betterTau = index;
				else
					betterTau = x0;		
			}
			else
			{
				float s0 = buffer[x0];
				float s1 = buffer[index];
				float s2 = buffer[x2];
			betterTau = index + (s2-s0) / (2*(2*s1-s2-s0));
			}
			return betterTau;
	}
	void dywapitch_inittracking(PitchChecker *pitchtracker)
	{
		pitchtracker->prevPitch = -1.;
		pitchtracker->pitchConfidence = -1;
	}

	float dywapitch_dynamicprocess(PitchChecker *pitchtracker, float pitch) //this function is taken from  http://www.schmittmachine.com/dywapitchtrack/dywapitchtrack.zip [Accessed 26th August 2013]
	{
			// ***********************************
// the dynamic postprocess
// ***********************************

/***
It states: 
 - a pitch cannot change much all of a sudden (29%) (impossible humanly,
 so if such a situation happens, consider that it is a mistake and drop it. 
 - a lonely voiced pitch cannot happen, nor can a sudden drop in the middle
 of a voiced segment. Smooth the plot. 
**/
		// equivalence
		if( pitch == 0.0 || pitch > maxHz)
			 pitch = -1.0;

	double estimatedPitch = -1;
	double acceptedError = 0.29f;
	int maxConfidence = 5;
	
	if (pitch != -1) 
	{
		// I have a pitch here
		
		if (pitchtracker->prevPitch == -1) 
		{
			// no previous
			estimatedPitch = pitch;
			pitchtracker->prevPitch = pitch;
			pitchtracker->pitchConfidence = 1;
			
		} 
		else if (abs(pitchtracker->prevPitch - pitch)/pitch < acceptedError) 
		{
			// similar : remember and increment pitch
			pitchtracker->prevPitch = pitch;
			estimatedPitch = pitch;
			pitchtracker->pitchConfidence = min(maxConfidence, pitchtracker->pitchConfidence + 1); // maximum 3
			
		} 
		else 
		{
			// nothing like this : very different value
			if (pitchtracker->pitchConfidence >= 1) 
			{
				// previous trusted : keep previous
				estimatedPitch = pitchtracker->prevPitch;
				pitchtracker->pitchConfidence = max(0, pitchtracker->pitchConfidence - 1);
			} 
			else 
			{
				// previous not trusted : take current
				estimatedPitch = pitch;
				pitchtracker->prevPitch = pitch;
				pitchtracker->pitchConfidence = 1;
			}
		}
		
	} 
	else 
	{
		// no pitch now
		if (pitchtracker->prevPitch != -1) 
		{
			// was pitch before
			if (pitchtracker->pitchConfidence >= 1) 
			{
				// continue previous
				estimatedPitch = pitchtracker->prevPitch;
				pitchtracker->pitchConfidence = max(0, pitchtracker->pitchConfidence - 1);
			} 
			else 
			{
				pitchtracker->prevPitch = -1;
				estimatedPitch = -1.;
				pitchtracker->pitchConfidence = 0;
			}
		}
	}
	
	// put "_pitchConfidence="&pitchtracker->_pitchConfidence
	if (pitchtracker->pitchConfidence >= 1) 
		// ok
		pitch = estimatedPitch;
	else
		pitch = -1;
	
	
	// equivalence
	if (pitch == -1) 
		pitch = 0.0;
	
	return pitch;
	}

	float process(float *input, unsigned short startPosition)
	{
		for(unsigned short i = 0; i < bufferSize; i++)
		{
			tempBuffer[i] = lpFilter->process(input[(startPosition+i)%bufferSize]);
			tempBuffer[i] = tempBuffer[i]*hannWindow[i];
			real[i] = tempBuffer[i];
		}
			// zero-pad
		for (unsigned short i=bufferSize; i<2*bufferSize; i++) 
  		{
			real[i] = 0;
		}
		 
		float maxmag = -10000;
		//fftwf_execute(pFFT);
		kiss_fftr(fft,(kiss_fft_scalar*)real, spectrum);
		//Step 1 of YIN algorithm: ACF calculate by finding power spectral density 
		for(unsigned short n = 0; n < 2*bufferSize; n++) 
		{	
			//calculate power spectral density by finding mag squared  of the autocorrelation
			 spectrum[n].r = spectrum[n].r*spectrum[n].r + spectrum[n].i*spectrum[n].i;
			 spectrum[n].i = 0;
			
			
			//in[n][0] = (outFFT[n][0]*outFFT[n][0]) + (outFFT[n][1]*outFFT[n][1]);
			//outFFT[n][1] = 0;	
			//mag[n] = sqrt((outFFT[n][0]*outFFT[n][0]) + (outFFT[n][1]*outFFT[n][1]));
			mag[n] = sqrt(spectrum[n].r);

			if(maxmag < mag[n]) 
				maxmag = mag[n];
		}

		if(maxmag < 0.001)
			return 0; //Saves processing time

		kiss_fftri(ifft,spectrum,(kiss_fft_scalar*)real);	
		//fftwf_execute(pIFFT);
		//normalise FFTW 
		for(unsigned short out = 0; out < bufferSize; out++) 
			//r[out] = (outFFT[out][0]/(2*bufferSize))*((float)(bufferSize/(bufferSize-out))); //remove bias;
			r[out] = real[out]/(2*bufferSize)*((float)(bufferSize/(bufferSize-out)));
		//Step 2 of YIN algorithm: difference function
		float rtTau = r[0];
  
		for (int i=0; i< halfBufferSize; i++)
		{
			difference[i] = r[0] + rtTau - (2 * r[i]);
			rtTau = rtTau + (tempBuffer[(halfBufferSize)+i]*tempBuffer[(halfBufferSize)+i]) - (tempBuffer[i]*tempBuffer[i]);
		}
		

		//Step 3 of YIN algorithm: The cumulative mean normalized difference function 
		cmnd();
		//Step 4 of YIN algorithm: The absolute threshold 
		absThresh();
		if(periodIndex)
		{
			//Step 5 of YIN algorithm: Parabolic interpolation
			periodLength = paraInter(cumDifference,periodIndex);
			float initialPitch = sampleRate/periodLength;
			//Step 6 of YIN algorithm: Local estimate
			float finalPitch = dywapitch_dynamicprocess(&check,initialPitch);
			accum[read] = finalPitch;
			read++;
			//float bas =0.f; //testing the pitch detection by observing the mean of 10 frames(approx 200ms at fs 44.1khz)
			/*if(read == 10)
			{
				for(int b = 0; b <=9; b++)
				{
					bas +=accum[b];
				}

				bas /= 10;

			} */
			if(read >= halfBufferSize)
				read = 0; 
			if(finalPitch <= 0)
				return 0;
			else
			{
				float finalPeriod = sampleRate/finalPitch;
				return finalPeriod;
			}
		}
		else 
			return 0; 
	}
};
#endif