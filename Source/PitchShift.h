#ifndef PITCHSHIFT
#define PITCHSHIFT

#include <math.h>
#include <string.h>
//#include "fftw3.h"
#include <kiss_fft.h>
#define M_PI 3.14159265358979323846
#define MAX_FRAME_LENGTH 1024 ///HACK! <--- this value MUST be the same as pitchBufferSize in HarmonizerEngine.h

class PitchShift
{
private:
	//====
  float gInFIFO[MAX_FRAME_LENGTH];
  float gOutFIFO[MAX_FRAME_LENGTH];

float gFFTworksp[2*MAX_FRAME_LENGTH];
  float gLastPhase[MAX_FRAME_LENGTH / 2 + 1];
  float gSumPhase[MAX_FRAME_LENGTH / 2 + 1];
  float gOutputAccum[2 * MAX_FRAME_LENGTH];
  float gAnaFreq[MAX_FRAME_LENGTH];
  float gAnaMagn[MAX_FRAME_LENGTH];
  float gSynFreq[MAX_FRAME_LENGTH];
  float gSynMagn[MAX_FRAME_LENGTH];

  double window[MAX_FRAME_LENGTH];
  unsigned short gRover;
  float magn, phase, tmp, real, imag; //instead of double
  unsigned short osamp;
  float freqPerBin, expct; //instead of double
  long i,k, qpd, index, inFifoLatency, stepSize,fftFrameSize, fftFrameSize2;

	kiss_fft_cpx *fftw_in;
	kiss_fft_cpx *fftw_out;

    //fftwf_complex fftw_in[MAX_FRAME_LENGTH], fftw_out[MAX_FRAME_LENGTH];
	//fftwf_plan ftPlanForward, ftPlanInverse;
	kiss_fft_cfg ftPlanForward; 
	kiss_fft_cfg ftPlanInverse;

  unsigned short n;
  unsigned short lastPeriod; // Result of the last pitch tracking loop
  unsigned short bufferSize;
	int inputPtr;
	float outputPtr;
	float sampleRate;
	float periodRatio; // Ratio of modification of the signal period	
	float *hannWindow; // Hanning window used for the input portion extraction
	float  *interp;     // Coefficients for the linear interpolation when modifying the output samples
	//=========================================

	//===
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PitchShift)
public:

	PitchShift(float sampleRate,unsigned short bufferSize)
	{
		this->sampleRate = sampleRate;
		this->bufferSize = bufferSize;
		fftFrameSize = bufferSize;
		inputPtr = 0;
		outputPtr = 0.f;
		lastPeriod = 0;
		periodRatio = 1.f;
		hannWindow = new float[bufferSize];
		interp = new float[2];
		osamp = 4;
        // set up some handy variables 
        fftFrameSize2 = bufferSize/2;
        stepSize = bufferSize/osamp;
        freqPerBin = sampleRate/(float)bufferSize; //
        expct = 2.*M_PI*stepSize/(float)bufferSize;//
        inFifoLatency = bufferSize-stepSize;
		gRover = inFifoLatency;

		  //create FFTW plan
    int nfftFrameSize = (int) fftFrameSize;
    //printf("nfs= %d, lfs= %ld\n", nfftFrameSize, fftFrameSize);

    //ftPlanForward = fftwf_plan_dft_1d(nfftFrameSize, fftw_in, fftw_out, FFTW_FORWARD, FFTW_MEASURE);
   // ftPlanInverse = fftwf_plan_dft_1d(nfftFrameSize, fftw_in, fftw_out, FFTW_BACKWARD, FFTW_MEASURE);
	//kiss_fft_alloc( nfft ,is_inverse_fft ,0,0 );	
	fftw_in = (kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*nfftFrameSize);
	fftw_out = (kiss_fft_cpx*)KISS_FFT_MALLOC(sizeof(kiss_fft_cpx)*nfftFrameSize);
   	ftPlanForward = kiss_fft_alloc(nfftFrameSize,0,NULL, NULL);
	ftPlanInverse = kiss_fft_alloc(nfftFrameSize,1,NULL,NULL);
		resume();
		for (int n =0; n < MAX_FRAME_LENGTH; n++)
		window[n] = 0.5f*(1.0f - cos(2*pi*(double)n/(MAX_FRAME_LENGTH)));
        // initialize arrays
	}

	~PitchShift()
	{
		delete[] hannWindow;
		delete[] interp;
		kiss_fft_cleanup();   
		free(fftw_in);
		free(fftw_out);
		free(ftPlanForward);
		free(ftPlanInverse);
		//fftwf_destroy_plan(ftPlanForward);
		//fftwf_destroy_plan(ftPlanInverse);
	}

	void resume()
	{
		memset(hannWindow, 0, bufferSize*sizeof(float));
		memset(interp, 0, 2*sizeof(float));
		memset(gInFIFO, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(gOutFIFO, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(gFFTworksp, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
		memset(gLastPhase, 0, (MAX_FRAME_LENGTH/2+1)*sizeof(float));
		memset(gSumPhase, 0, (MAX_FRAME_LENGTH/2+1)*sizeof(float));
		memset(gOutputAccum, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
		memset(gAnaFreq, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(gAnaMagn, 0, MAX_FRAME_LENGTH*sizeof(float));

		//memset (window, 0.0, MAX_FRAME_LENGTH * sizeof (double));
	}

	void processLent(float period, float *inputQueue, float *outputQueue, float ratio) //Function modified from https://ccrma.stanford.edu/software/stk/LentPitShift_8h_source.html [Accessed 25th August 2013]
	{
		lastPeriod = (unsigned short)(period + 0.5);
		periodRatio = ratio;
		 int M; // Index of reading in the input delay line
		 int N;  // Index of writing in the output delay line
		double sample; // Temporary storage for the new coefficient
		// Initialization of the Hanning window used in the algorithm
		for(int n=-lastPeriod; n<lastPeriod; n++)
			hannWindow[n+lastPeriod] = 0.5f*(1 + cos(pi*n/lastPeriod));

		// We loop for all the frames of length periodFinal presents between inputPtr and 2*N
		for (; inputPtr<(int)(bufferSize-lastPeriod); inputPtr+=lastPeriod ) 
		{
			// Test for the decision of compression/expansion
			while ( outputPtr < inputPtr ) 
			{
				// Coefficients for the linear interpolation
				interp[1] = fmodf(outputPtr + bufferSize, 1.0 );
				interp[0] = 1.0 - interp[1];

				 M = bufferSize + inputPtr - lastPeriod - 1; // New reading pointer
				 N = (unsigned long)floor(outputPtr + bufferSize)  - 1; // New writing pointer
				
				 
			        // Linear interpolation  
				 for (unsigned int j=0; j<2*lastPeriod; j++,M++,N++) 
				{
					sample = inputQueue[M] * hannWindow[j];
					outputQueue[N]+= interp[0]*sample;
					outputQueue[N-1]+= interp[1]*sample;
				}
				 outputPtr += (lastPeriod * periodRatio); //new output
			 }
		}
		// Shifting of the pointers waiting for the new frame of length N.
		outputPtr -= bufferSize;
		inputPtr  -= bufferSize;
	}

	void smbPitchShift (float pitchShift, float *indata, float *outdata) // Function modified from http://downloads.dspdimension.com/smbPitchShift.cpp [Accessed 26th August 2013]
	{
		    float maxmag = 0.0f;
			float tunefrq = 0.0f;
		for (i = 0; i < bufferSize; i++)
		{
			// As long as we have not yet collected enough data just read in 
			gInFIFO[gRover] = indata[i+bufferSize];
			outdata[i+bufferSize] = gOutFIFO[gRover-inFifoLatency];
			++gRover;

			if (gRover >= bufferSize) 
			{
				gRover = inFifoLatency;
			/* do windowing and re,im interleave */
			for (k = 0; k < fftFrameSize;k++) 
			{
				
                fftw_in[k].r = gInFIFO[k] * window[k];
                fftw_in[k].i = 0.0;
			}
			
			/* do transform */
			// fftwf_execute(ftPlanForward);
			 kiss_fft(ftPlanForward,fftw_in, fftw_out);
			/* this is the analysis step */
			for (k = 0; k <= fftFrameSize2; k++) 
			{
				/* de-interlace FFT buffer */
				real = fftw_out[k].r;
                imag = fftw_out[k].i;
				/* compute magnitude and phase */
				magn = 2.*sqrt(real*real + imag*imag);
				phase = atan2(imag,real);

				/* compute phase difference */
				tmp = phase - gLastPhase[k];
				gLastPhase[k] = phase;

				/* subtract expected phase difference */
				tmp -= (double)k*expct;

				/* map delta phase into +/- Pi interval */
				qpd = tmp/M_PI;
				if (qpd >= 0) 
					qpd += qpd&1;
				else 
					qpd -= qpd&1;
				tmp -= M_PI*(double)qpd;

				/* get deviation from bin frequency from the +/- Pi interval */
				tmp = osamp*tmp/(2.*M_PI);

				/* compute the k-th partials' true frequency */
				tmp = (double)k*freqPerBin + tmp*freqPerBin;

				/* store magnitude and true frequency in analysis arrays */
				gAnaMagn[k] = magn;
				gAnaFreq[k] = tmp;

                if(magn>maxmag) {
                    maxmag = magn;
                    tunefrq = tmp;
                }
	}	
	
		/* ***************** PROCESSING ******************* */
			/* this does the actual pitch shifting */
			memset(gSynMagn, 0, fftFrameSize*sizeof(float));
			memset(gSynFreq, 0, fftFrameSize*sizeof(float));
			for (k = 0; k <= fftFrameSize2; k++) { 
				index = k*pitchShift;
				if (index <= fftFrameSize2) { 
					gSynMagn[index] += gAnaMagn[k]; 
					gSynFreq[index] = gAnaFreq[k] * pitchShift; 
				} 
			}
			
			/* ***************** SYNTHESIS ******************* */
			/* this is the synthesis step */
			for (k = 0; k <= fftFrameSize2; k++) {

				/* get magnitude and true frequency from synthesis arrays */
				magn = gSynMagn[k];
				tmp = gSynFreq[k];

				/* subtract bin mid frequency */
				tmp -= (double)k*freqPerBin;

				/* get bin deviation from freq deviation */
				tmp /= freqPerBin;

				/* take osamp into account */
				tmp = 2.*M_PI*tmp/osamp;

				/* add the overlap phase advance back in */
				tmp += (double)k*expct;

				/* accumulate delta phase to get bin phase */
				gSumPhase[k] += tmp;
				phase = gSumPhase[k];

				/* get real and imag part and re-interleave */
                fftw_in[k].r = magn * cos (phase);
                fftw_in[k].i = magn * sin (phase);
			} 

			/* zero negative frequencies */
			for (k = 2 + fftFrameSize2; k < fftFrameSize; k++) 
			{
                //fftw_in[k][0] = 0.;
               // fftw_in[k-1][1] = 0.;
                fftw_in[k].r = 0.;
                fftw_in[k-1].i = 0.;
            }
			/* do inverse transform */
			//fftwf_execute(ftPlanInverse);
			kiss_fft(ftPlanInverse,fftw_in, fftw_out);
			/* do windowing and add to output accumulator */ 
			for(k=0; k < fftFrameSize; k++) 
			{
				gOutputAccum[k] += 2.* window[k]*fftw_out[k].r/(fftFrameSize2*osamp);
			}
			for (k = 0; k < stepSize; k++) gOutFIFO[k] = gOutputAccum[k];

			/* shift accumulator */
			memmove(gOutputAccum, gOutputAccum+stepSize, fftFrameSize*sizeof(float));

			/* move input FIFO */
			for (k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k+stepSize];
		}
	}
}
};

#endif
