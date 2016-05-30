#ifndef HARMONIZERENGINE
#define HARMONIZERENGINE

#include <math.h>
#include <cstring>
#include <string>
#include "PitchDetect.h"
#include "PitchShift.h"
#include "PitchDecide.h"
#include "HarmonyControl.h"
#include "MyerViterbi.h"
#include "MyerFFT.h"
#include "NNLSChroma.h"
#include <boost/lexical_cast.hpp>
#include "lpfcoeffM5.h"
//====
//filter coefficients for LP filter
#define M2 5
//====
const unsigned short pitchBufferSize = 1024;	//value is fixed to reduce complexity 
const unsigned short chordBufferSize = 1024;	//value is fixed to reduce complexity 
const char chordNames[5][4]={"","m","dim","aug","sus"};
const char noteNames[12][3]={"C","C#","D","Eb","E","F","F#","G","Ab","A","Bb","B"};
const short kMinMidiNote = 21;  // A0
const short kMaxMidiNote = 108; // C8
class HarmonizerEngine 
{
private:
	unsigned short pitchHopSize;
	unsigned short pitchBufferReadIndex;
	unsigned short pitchBufferWriteIndex;
	unsigned short chordHopSize;
	unsigned short chordBufferReadIndex;
	unsigned short chordBufferWriteIndex;
	unsigned short sampleIncrement;
	unsigned short pitchIncrement;
	unsigned short chordDownsample;
	unsigned short pitchDownsample;
	float sampleRate; 
	float fftSampleRate; 
	float pitchSampleRate;

	//Storage arrays for audio I/O
	float shiftBufferIn[pitchBufferSize];
	float audioBufferIn[pitchBufferSize];
	float audioBufferOut[pitchBufferSize];
	float chordBufferIn[chordBufferSize];
	float audioBufferOut2[pitchBufferSize];
	float inQueue[pitchBufferSize*2]; //Data buffer fot input
	float outQueue[pitchBufferSize*3];//Data buffer for output
	float inQueue2[pitchBufferSize*2]; //Data buffer fot input 2
	float outQueue2[pitchBufferSize*3];//Data buffer for output 2
	float pitch[4]; 
	float chord[4]; 
	float pitchView[pitchBufferSize/2]; // for analysing the result of multiple frames.
	float buffer[M2];	// for FIR filter
	float buffer2[M2]; // for FIR filter

	//Variables of type class
	PitchDetect* yin;
	PitchShift* transpose;
	PitchShift* transpose2;
	MyerFFT* fft;
	NNLSChroma* nnlsChroma;
	
	Viterbi viterbiModule;
	HarmonyControl decision;
	PitchDecide autotune;

	//Chord detection variables
	bool isChordFrameUpdated;
	short chordId, noteId, typeId;
	float bassChroma[12];
	float midChroma[12];
	float magnitude[chordBufferSize/2];
	std::vector<float> spectrum;	

	//Pitch detection variables
	bool isPitchFrameUpdated;
    short minPeriod;
	float shiftFactor;
	float periodRatio;
	float maxHz;

	//Pitch Shifting variables
	bool doesPitchExist;
	unsigned short shiftWriteIndex;
	float harmony1ShiftFactor;
	float harmony2ShiftFactor;
	float hannWin[2*pitchBufferSize];
	float hannWin2[2*pitchBufferSize];	
	float harm1Pitch;
	float harm2Pitch;
	float previousPitch;
	float previousPeriod;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonizerEngine)
public:
	//Adjustable JUCE cooked parameters

	int shiftTypePar;
	int autotunePar;
	float referenceHzPar;
	float outputGainPar;
	float voice1GainPar;
	float voice2GainPar;
	float transposePar;
	float panLPar;
	float panRPar;
	float whiteningPar;
	int keyPar;
	int scalePar;
	int attackPar;
	int shiftPar;
	int rollOnPar;
	int harmonyPar;
	int tunePar;
	int harm1AttackPar;
	int harm2AttackPar;

	//GUI variables
	float panLPercent;
	float panRPercent;
	float whiteningPercent;
	float semitoneTranspose;
	float outputGaindB;
	float voice1GaindB;
	float voice2GaindB;
	float attackTimeMilliseconds;
	float harm1AttackTimeMilliseconds;
	float harm2AttackTimeMilliseconds;
	float vRate;
	float vDepth;
	int check;
	float detectedPeriod;
	float detectedPitch;
	float transposePitch;
	float transposePeriod;
	int keyGUI;
	int scaleGUI;
    int MIDINote, cents, MIDINoteHarm1, MIDINoteHarm2;
	std::string chordStr;
	std::string pitchValue;
	std::string harm1Value;
	std::string harm2Value;
	bool wasPreviousFramePitch;

	//Enumerations for GUI helper functions
	enum notes
	{
		C = 1, CSharp, D, DSharp, E, F, FSharp, G, GSharp, A, ASharp, B
	};

	enum scales
	{
		Major = 1, Minor, Pentatonic, Chromatic
	};

	enum pitchShiftAlgorithms
	{
		lent = 1,
		delay
	};

	enum OnOff
	{
		on = 1,
		off
	};

	enum harmonies
	{

		HighAndHigher = 1,
		LowAndLower,
		HigherAndLower,
		HighAndLow
	};

	HarmonizerEngine(float sampleRate)
	{
		if (sampleRate <= 0)
            sampleRate = 44100.0f;
		
		this->sampleRate =  sampleRate;
		pitchIncrement = (int)ceil(sampleRate/(sampleRate/4));
		sampleIncrement = (int)ceil(sampleRate/(sampleRate/4));
		pitchSampleRate = sampleRate/pitchIncrement; 
		fftSampleRate = sampleRate/sampleIncrement;
		pitchHopSize = pitchBufferSize/4; //size 256
		pitchBufferReadIndex = 0;
		pitchBufferWriteIndex = 0;
		chordHopSize = chordBufferSize/2; //size 1024
		chordBufferReadIndex = 0;
		chordBufferWriteIndex = 0;
		check = 0;
		for (unsigned short n = 0; n < 2*pitchBufferSize; n++)
		{
			hannWin[n] = 0.5f*(1.0f - cos(2*pi*(float)n/(2*pitchBufferSize)));
			hannWin2[n] = 0.5f*(1.0f + cos(2*pi*(float)n/(2*pitchBufferSize)));
		}

		chordDownsample = 0;
		pitchDownsample = 0;
		noteId = -1, 
		typeId = -1;
		shiftWriteIndex = 0;

		rollOnPar = 5;
		whiteningPercent = 100.f;
		harmonyPar = HighAndHigher;
		yin = new PitchDetect(pitchSampleRate,pitchBufferSize);
		fft = new MyerFFT(chordBufferSize, sampleRate);
		nnlsChroma  = new NNLSChroma(fftSampleRate);
		transpose = new PitchShift(sampleRate,pitchBufferSize);
		transpose2 = new PitchShift(sampleRate,pitchBufferSize);
		previousPitch = 0;
		previousPeriod = pitchBufferSize/2;
	    harm1Pitch = 0;
	    harm2Pitch = 0;
		doesPitchExist =false;
		wasPreviousFramePitch = false;
		//Default settings for parameters
		transposePar = 1.f;
		shiftTypePar = lent;
		autotunePar = off;
		tunePar = off;
		referenceHzPar = 440.f;
		shiftPar = off;
		panLPercent = 50.f;
		panRPercent = 50.f;
		semitoneTranspose = 0.f;
		MIDINote = cents = 0;
		MIDINoteHarm1 = 0;
		MIDINoteHarm2 = 0;
		outputGaindB = voice1GaindB = voice2GaindB =  0;
		attackTimeMilliseconds = 0.f;
		harm1AttackTimeMilliseconds = 0.f;
		harm2AttackTimeMilliseconds = 0.f;
		vRate = 4.0;
		vDepth = 0.f;

		keyGUI = C;
		scaleGUI = Major;
		detectedPeriod = pitchBufferSize/2;
		detectedPitch = transposePitch = transposePeriod = 0.f;
		shiftFactor = 1.f;
		harmony1ShiftFactor = -1;
		harmony2ShiftFactor = -1;
		maxHz = 0.1f*sampleRate;
		minPeriod = (unsigned short)(sampleRate/maxHz + 0.5);	
		nnlsChroma->initialise(1,chordHopSize,chordBufferSize);		
		cookGain();
		cookPan();
		cookMusic();
	}

	~HarmonizerEngine()
	{
		delete yin;	
		delete fft;
		delete nnlsChroma;
		delete transpose;

		delete transpose2;
	}

	void prepare()
	{
		reset();
		transpose->resume();
	}

	void reset()
	{
		memset(shiftBufferIn,  0, pitchBufferSize*sizeof(float));
		memset(audioBufferIn,  0, pitchBufferSize*sizeof(float));
		memset(audioBufferOut, 0, pitchBufferSize*sizeof(float));
		memset(chordBufferIn, 0, chordBufferSize*sizeof(float));
		memset(audioBufferOut2,0,pitchBufferSize*sizeof(float));
		memset(inQueue, 0, 2*pitchBufferSize*sizeof(float));
		memset(outQueue, 0, 3*pitchBufferSize*sizeof(float));
		memset(inQueue2, 0, 2*pitchBufferSize*sizeof(float));
		memset(outQueue2, 0, 3*pitchBufferSize*sizeof(float));
		memset(magnitude, 0, (chordBufferSize/2)*sizeof(float));
		memset(pitch, 0,  4*sizeof(float));
		memset(chord, 0,  4*sizeof(float));
		memset(buffer, 0, 5*sizeof(float));
		memset(buffer2,0, 5*sizeof(float));
		memset(pitchView,0, (pitchBufferSize/2)*sizeof(float));
		viterbiModule.reset();
		nnlsChroma->reset();
	}

	void cookGain()
	{
		if(outputGaindB != -30)
			outputGainPar = pow(10.,(outputGaindB/20.));
		else
			outputGainPar = 0;

		if(voice1GaindB != -30)
			voice1GainPar = pow(10.,(voice1GaindB/20.));
		else
			voice1GainPar = 0;

		if(voice2GaindB != -30)
			voice2GainPar = pow(10.,(voice2GaindB/20.));
		else
			voice2GainPar = 0;
	}

	void cookPan()
	{
		panLPar = panLPercent/100.0;
		panRPar = panRPercent/100.0;
		whiteningPar = whiteningPercent/100.0;
	}

	void cookMusic() //Current Issue when switching between Pitch shifting algorithms, race condition
	{
		if(shiftTypePar == delay)
		transposePar = pow(2., (semitoneTranspose/12.));
		else
		transposePar = pow(2., -(semitoneTranspose/12.));

		keyPar = (keyGUI-1); //JUCE fix
		scalePar = (scaleGUI-1);
		attackPar = (int)((attackTimeMilliseconds * 441) / 1024.0); //44100 = 1 SEC 441 
		harm1AttackPar = (int)((harm1AttackTimeMilliseconds * 441) / 1024.0); //44100 = 1 SEC 441 
		harm2AttackPar = (int)((harm2AttackTimeMilliseconds * 441) / 1024.0); //44100 = 1 SEC 441 
	}

	void pitchFactor()
	{
		if (transposePar == 1 || transposePar == 0.5 || transposePar == 0.25 || transposePar == 2 || transposePar == 4)
			periodRatio = (transposePar)*(shiftFactor);
		else
			periodRatio = (transposePar)/(shiftFactor);
	}

	void process(float* inputL, float* inputR)
	{
		float vocal = *inputL;
		float instrument = *inputR;
		float harmonyOutput1 = 0.f;
		float harmonyOutput2 = 0.f;
		
		// limit signal
        if (vocal > 1.0f) vocal = 1.0f;
        if (vocal < -1.0f) vocal = -1.0f;
        if (instrument > 1.0f) instrument = 1.0f;
        if (instrument < -1.0f) instrument = -1.0f;

		//noise gate - cubase fix
		if(vocal > 0 && vocal < 0.0000001) vocal = 0.f;
		if(vocal < 0 && vocal > -0.0000001) vocal = 0.f;
		if(instrument > 0 && instrument < 0.0000001) instrument = 0.f;
		if(instrument < 0 && instrument > -0.0000001) instrument = 0.f;

		pitch[pitchDownsample] = vocal;
		chord[chordDownsample] = instrument;
		++pitchDownsample;
		++chordDownsample;

		processShift(vocal,&harmonyOutput1,&harmonyOutput2);

		if(pitchDownsample == pitchIncrement)
		{
			short i;
			for(i = 0; i < pitchDownsample; i++)
				pitch[i] = fir(M2-1, lowpass2_blackman_filter_1200Hz, buffer2, pitch[i]); 

			processPitch(pitch[i-1]);
			pitchDownsample = 0;
		}

		if(chordDownsample == sampleIncrement)
		{
			short i;
			for(i = 0; i < chordDownsample; i++)
				chord[i] = fir(M2-1, lowpass2_blackman_filter_1200Hz, buffer, chord[i]); 

			processChord(chord[i-1]);
			chordDownsample = 0;
		}
		//True Stereo Panning In Built
		*inputL = (((harmonyOutput1*voice1GainPar)*sqrt(1-panLPar))+((harmonyOutput2*voice2GainPar)*sqrt(1-panRPar)))*outputGainPar;
		*inputR = (((harmonyOutput1*voice1GainPar)*sqrt(panLPar))+((harmonyOutput2*voice2GainPar)*sqrt(panRPar)))*outputGainPar;
	}

	void processChord(float input)
	{
		float maxmag = -10000;
		memset(midChroma, 0, 12*sizeof(float));
		memset(bassChroma, 0, 12*sizeof(float));
		float energysum = 0;
		isChordFrameUpdated=false;
		chordBufferIn[chordBufferWriteIndex] = input;
		chordBufferWriteIndex++;
		chordBufferWriteIndex%=chordBufferSize;

		if(chordBufferWriteIndex == chordBufferReadIndex)
		{
			spectrum=fft->windowed_fft(chordBufferIn,chordBufferReadIndex,chordBufferSize);

			//Spectral Whitening
			unsigned short vecSize = spectrum.size();
			for(unsigned short iBin = 0; iBin < vecSize; iBin++)
			{
				magnitude[iBin] = spectrum[iBin];
				if (maxmag < magnitude[iBin]) 
					maxmag = magnitude[iBin];
				
				energysum += pow(magnitude[iBin],2);
			}

		float cumenergy = 0;
        for (unsigned short iBin = 2; iBin < vecSize; iBin++) 
		{
            cumenergy +=  pow(magnitude[iBin],2);
            if (cumenergy < energysum * rollOnPar / 100) 
				magnitude[iBin-2] = 0;
            else 
				break;
        } 
		if(maxmag > 0.1)
			{
				for(unsigned short iBin = 0; iBin < vecSize; iBin++)
					spectrum[iBin] = magnitude[iBin];
				
				nnlsChroma->getRemainingFeatures(magnitude, whiteningPar, tunePar, referenceHzPar);
				unsigned short vectorSize = nnlsChroma->chroma.size();
				
				for(unsigned short i = 0; i < vectorSize; i++)
				{
					if(3+i >= 12)
					{
						for(int j = 0; j < 3; j++)
						{
							midChroma[9+j] = nnlsChroma->chroma[j];
							bassChroma[9+j] = nnlsChroma->basschroma[j];
						}
						break;
					}
					else
					{
						midChroma[i] = nnlsChroma->chroma[3+i];
						bassChroma[i] = nnlsChroma->basschroma[3+i];
					}
				}
				
				chordId=viterbiModule.viterbi(bassChroma,midChroma);
				noteId=chordId%12;
				typeId=chordId/12;	
				chordStr=noteNames[noteId];
				chordStr+=chordNames[typeId];
			}
			else
				{
					//No chord state
					viterbiModule.reset();
					chordStr = "NONE";
					noteId= -1;
					typeId= -1;
				}
			isChordFrameUpdated=true;
			chordBufferReadIndex+=chordHopSize;
			chordBufferReadIndex%=chordBufferSize;
		}
	}

	void processShift(float input, float *out, float *out2)
	{
		shiftBufferIn[shiftWriteIndex] = input;
		*out = audioBufferOut[shiftWriteIndex];
		*out2 = audioBufferOut2[shiftWriteIndex];
		++shiftWriteIndex;
		while(shiftWriteIndex == pitchBufferSize)
		{
			shiftWriteIndex = 0;
			unsigned short i;
			// First, transfer inputs and outputs 
			for(i = 0; i < pitchBufferSize; ++i)
			{
				//Voice 1 Output
				inQueue[i] = inQueue[pitchBufferSize+i];//previous frame of input shifted
				inQueue[pitchBufferSize+i] = shiftBufferIn[i]; //current input stored into input delay line
				audioBufferOut[i] = outQueue[i];

				//Voice 2 Output
				inQueue2[i] = inQueue2[pitchBufferSize+i];
				inQueue2[pitchBufferSize+i] = shiftBufferIn[i];
				audioBufferOut2[i] = outQueue2[i];
			}
			//finish shifting output buffer
			for(i = 0; i < 2*pitchBufferSize; ++i)
			{
				outQueue[i] = outQueue[pitchBufferSize+i];
				outQueue2[i] = outQueue2[pitchBufferSize+i];
			}

			for(i = 0; i < pitchBufferSize; ++i)
			{
				outQueue[2*pitchBufferSize+i] = 0;
				outQueue2[2*pitchBufferSize+i] = 0;
			}

			shiftFactor = 1.f;

			while(doesPitchExist)
			{	
				if(autotunePar == on)
					shiftFactor = autotune.processDecision(detectedPitch,&referenceHzPar,&keyPar,&scalePar,&attackPar);
				else
					shiftFactor = 1.f;

				pitchFactor();

				transposePitch = detectedPitch/periodRatio;
				pitchView[check] = transposePitch; //checking the pitch for 512 frames( roughly up to 12 secs at 44.1kHz)
				check++;

					if(check >= (pitchBufferSize/2))
						check = 0;

				if(shiftTypePar == delay)
					transposePitch = detectedPitch*periodRatio;	

				PitchToMidiNote(transposePitch, MIDINote, cents);
				pitchValue = GetNoteName(MIDINote);

				if(shiftPar == on && noteId >= 0 && noteId <= 11 && typeId >= 0 && typeId <= 4)
				{
					decision.processHarmony(transposePitch,detectedPitch,noteId,typeId,&harmony1ShiftFactor, &harmony2ShiftFactor, harmonyPar, referenceHzPar, harm1AttackPar, harm2AttackPar, vRate, vDepth, sampleRate, pitchBufferSize);	
					if(shiftTypePar == delay)
					{
						harmony1ShiftFactor = 1/harmony1ShiftFactor;
						harmony2ShiftFactor = 1/harmony2ShiftFactor;
					}
				}
				else
				{
					harmony1ShiftFactor = periodRatio;
					harmony2ShiftFactor = periodRatio;
				}				
				if(shiftTypePar == delay)
				{
					harm1Pitch = detectedPitch*(harmony1ShiftFactor);
					harm2Pitch = detectedPitch*(harmony2ShiftFactor);
				}
				else
				{
					harm1Pitch = detectedPitch/harmony1ShiftFactor;
					harm2Pitch = detectedPitch/harmony2ShiftFactor;
				}
				PitchToMidiNote(harm1Pitch, MIDINoteHarm1, cents);
				PitchToMidiNote(harm2Pitch, MIDINoteHarm2, cents);
				if (MIDINoteHarm1 > kMinMidiNote && MIDINoteHarm2 > kMinMidiNote && MIDINoteHarm1 < kMaxMidiNote && MIDINoteHarm2 < kMaxMidiNote)
				{
					harm1Value = GetNoteName(MIDINoteHarm1);
					harm2Value = GetNoteName(MIDINoteHarm2);
				}
				
					switch(shiftTypePar)
					{
						case lent:
						{
							transpose->processLent(detectedPeriod,inQueue,outQueue,harmony1ShiftFactor);
							transpose2->processLent(detectedPeriod,inQueue2,outQueue2,harmony2ShiftFactor);
						}
						break;
						case delay:
						{		
							transpose->smbPitchShift(harmony1ShiftFactor,inQueue,outQueue);
							transpose2->smbPitchShift(harmony2ShiftFactor,inQueue2,outQueue2);
						}
						break;
					}

					if(!wasPreviousFramePitch)
					{
						for(int n = 0; n < 2*pitchBufferSize; n++)
						{
							outQueue[n] = ((outQueue[n])*hannWin2[n]) + (((inQueue[n])*hannWin[n]));
							outQueue2[n] = ((outQueue2[n])*hannWin2[n]) + (((inQueue2[n])*hannWin[n]));
						}
						wasPreviousFramePitch = true;
					}

					previousPitch = detectedPitch;
					previousPeriod = detectedPeriod;
					return;
			}
				
				detectedPitch = sampleRate/previousPeriod;
				pitchFactor();
				
				transposePitch = detectedPitch/periodRatio;
				
				if(shiftTypePar == delay)
					transposePitch = detectedPitch*periodRatio;	
				if(shiftPar == on && noteId >= 0 && noteId <= 11 && typeId >= 0 && typeId <= 4)
				{
					decision.processHarmony(transposePitch,detectedPitch,noteId,typeId,&harmony1ShiftFactor, &harmony2ShiftFactor, harmonyPar, referenceHzPar, harm1AttackPar, harm2AttackPar, vRate, vDepth, sampleRate, pitchBufferSize);	
					if(shiftTypePar == delay)
					{
						harmony1ShiftFactor = 1/harmony1ShiftFactor;
						harmony2ShiftFactor = 1/harmony2ShiftFactor;
					}
				}
				else
				{
					harmony1ShiftFactor = periodRatio;
					harmony2ShiftFactor = periodRatio;
				}		
				
			harm1Value = "NONE";
			harm2Value = "NONE";
			pitchValue = "NONE";
					switch(shiftTypePar)
					{
						case lent:
						{
							transpose->processLent(previousPeriod,inQueue,outQueue,harmony1ShiftFactor);
							transpose2->processLent(previousPeriod,inQueue2,outQueue2,harmony2ShiftFactor);
						}
						break;
						case delay:
						{		
							transpose->smbPitchShift(harmony1ShiftFactor,inQueue,outQueue);
							transpose2->smbPitchShift(harmony2ShiftFactor,inQueue2,outQueue2);
						}
						break;
					}

					if(wasPreviousFramePitch)
					{
						for(int n = 0; n < 2*pitchBufferSize; n++) //mixing the original input with the already shifted output
							{
								outQueue[n] = ((outQueue[n])*hannWin2[n]) + (((inQueue[n])*hannWin[n])/2);
								outQueue2[n] = ((outQueue2[n])*hannWin2[n]) + (((inQueue2[n])*hannWin[n])/2);		
							}			
						wasPreviousFramePitch = false;
					}
				return;	
		}
	}
	
	void processPitch(float input)
	{
		isPitchFrameUpdated = false;
		audioBufferIn[pitchBufferWriteIndex] = input;
		++pitchBufferWriteIndex;
		pitchBufferWriteIndex%=pitchBufferSize;
		if(pitchBufferWriteIndex == pitchBufferReadIndex)
		{
			detectedPeriod = yin->process(audioBufferIn,pitchBufferReadIndex);
			detectedPeriod *= pitchIncrement;
			if(detectedPeriod <= minPeriod || detectedPeriod >= pitchBufferSize/2) 
			{
				detectedPeriod = previousPeriod;
				detectedPitch = sampleRate/previousPeriod;
				doesPitchExist = false;
			}
			else
			{
				detectedPitch = (sampleRate/detectedPeriod);
				doesPitchExist = true;
			}
			isPitchFrameUpdated=true;
			pitchBufferReadIndex+=pitchHopSize;
			pitchBufferReadIndex%=pitchBufferSize;
		}
	}
        /// Get the MIDI note and cents of the pitch 
    void PitchToMidiNote(float pitch, int& note, int& cents)
        {
            if (pitch < 20.0f)
            {
                note = 0;
                cents = 0;
                return;
            }
			float fNote= log(pitch/(C0))/log(semitone);
			note = (int)(fNote + 0.5f);
            cents = (int)((( fNote- note ) * 100));
           
        }
        /// Format a midi note to text
    static string GetNoteName(int note)
        {
            if (note < kMinMidiNote || note > kMaxMidiNote)
                return NULL;
            note -= kMinMidiNote;
            int octave = (note + 9) / 12;
            note = note % 12;
            string noteText;
			
			string s = boost::lexical_cast<string>( octave );
			switch (note)
            {
                case 0:
                    noteText = "A";
                    break;

                case 1:
                    noteText = "Bb";
                    break;

                case 2:
                    noteText = "B";
                    break;

                case 3:
                    noteText = "C";
                    break;

                case 4:
					noteText = "C#";
                    break;

                case 5:
                    noteText = "D";
                    break;
				case 6:
                    noteText = "Eb";
                    break;

                case 7:
                    noteText = "E";
                    break;

                case 8:
                    noteText = "F";
                    break;

                case 9:
                    noteText = "F#";
                    break;

                case 10:
                    noteText = "G";
                    break;

                case 11:
                    noteText = "Ab";
                    break;
            }
                noteText += "" + s;

            return noteText;
		}
// fir.c - FIR filter in direct form with linear buffer
// Usage: y = fir(M, h, w, x);
// ----------------------------------------------------------------
	float fir(int MLength, const float *h, float *w, float x) //Function used from http://eceweb1.rutgers.edu/~orfanidi/ece348/lab3.pdf [Accessed 26th August 2013]
{
int i;

float y; // y=output sample

w[0] = x; // read current input sample x

for (y=0, i=0; i<=MLength; i++) // process current output sample

y += h[i] * w[i]; // dot-product operation

for (i=MLength; i>=1; i--) // update states for next call

w[i] = w[i-1]; // done in reverse order

return y;
}
};


#endif