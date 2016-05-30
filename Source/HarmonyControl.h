
#ifndef __HARMONYCONTROL__
#define __HARMONYCONTROL__
#define semitone 1.059463094359	// 12th root of 2
#define C0 6.875*semitone*semitone*semitone //First note of MIDI representation C

enum harmony
	{

		HighAndHigher = 1,
		LowAndLower,
		HigherAndLower,
		HighAndLow
	};

#include <math.h>

const int chordQualityTable[5][16] =
{ -20,-17,-12, -8, -5, 0, 4, 7, 12, 16, 19, 0, 0, 0, 0, 10+1,		//Major Chord Intervals
  -21,-17,-12, -9, -5, 0, 3, 7, 12, 15, 19, 0, 0, 0, 0, 10+1,		//Minor Chord Intervals
  -21,-18,-12, -9, -6, 0, 3, 6, 12, 16, 19, 0, 0, 0, 0, 10+1,		//Diminished Chord Intervals
  -20,-16,-12, -8, -4, 0, 4, 8, 12, 17, 21, 0, 0, 0, 0, 10+1,		//Augmented Chord Intervals
  -22,-19,-17,-12, -10, -7, -5, 0, 2, 5, 7, 12, 14, 17, 19, 14+1	//Suspended Chord Intervals
};
  

class HarmonyControl
{
public:
	float currPitch;
	float currPitch2;
	float g_time;
	int numElapsed;
	int numElapsed2;
	int chord;
	int destinationChordNote;
	int destinationChordNote2;
	float currentPeriod;
	int midiNoteBackVox;
	int midiNoteBackVox2;
	float correctedHz, correctedHz2;
	float referenceHzPar;
	float detectedPitch;
	float rootChord;
	float qualityChord;
	int attack;
	int attack2;
	HarmonyControl()
	{
		chord = 0;
		referenceHzPar = 440;
        g_time = 0.0;
		currPitch = 0;
		currPitch2 = 0;
        numElapsed = 0;
		numElapsed2 = 0;
		attack = 0;		
		attack2 = 0;
	}

	~HarmonyControl()
	{}

    float addVibrato(int nFrames, float sampleRate, float& vibratoRate, float& vibratoDepth) //Function modified from http://voicerecorder.codeplex.com/SourceControl/latest [Accessed 25th August 2013]
    {
		g_time += nFrames;
		float d = (float)(sin(2 * 3.14159265358979 * vibratoRate * g_time / sampleRate) * vibratoDepth);
		return d;
    }

	void HarmonyDecision(float freq, int root, int quality, int& harmony)
	{
//       float noteRep = (int)(log(freq/(C0))/log(semitone)+0.5);
       int noteRep = (log(freq/(C0))/log(semitone)) +0.5;
		
		int octave = 0;
		int currentNote = 0;
        //reduce pitch height to one octave
        while(noteRep>=12+root)
        {
			noteRep-=12;
            octave+=12;
        }
		for(; currentNote <= chordQualityTable[quality][15] && chordQualityTable[quality][currentNote]+root <= noteRep; currentNote++);

		switch(harmony)
		{
			case HighAndHigher:
				destinationChordNote = chordQualityTable[quality][currentNote]+root+octave;
				destinationChordNote2 = chordQualityTable[quality][currentNote+1]+root+octave;
				break;

			case LowAndLower:
				destinationChordNote = chordQualityTable[quality][currentNote-2]+root+octave;
				destinationChordNote2 = chordQualityTable[quality][currentNote-3]+root+octave;
				break;

			case HigherAndLower:
				destinationChordNote = chordQualityTable[quality][currentNote+1]+root+octave;
				destinationChordNote2 = chordQualityTable[quality][currentNote-3]+root+octave;
				break;

			case HighAndLow:
				destinationChordNote = chordQualityTable[quality][currentNote]+root+octave;
				destinationChordNote2 = chordQualityTable[quality][currentNote-2]+root+octave;
				break;
		}
			//THE BACKING VOCALS NEVER SING THE SAME NOTE AS THE VOCAL LINE
	}

	void processHarmony(float freq, float original, int root, int quality,float *shiftFactor, float *shiftFactor2, int& harmonyChoice, float& refHz, int& attackParam, int& attackParam2, float& vibRate, float& vibDepth, float sampleRate, int bufferSize)
	{
		referenceHzPar = refHz;		
		attack = attackParam;
		attack2 = attackParam2;
		HarmonyDecision(freq,root,quality,harmonyChoice);
		correctedHz = referenceHzPar * pow( 2.0, ((double)destinationChordNote - 69.0) / 12.0 );
		correctedHz2 = referenceHzPar * pow( 2.0, ((double)destinationChordNote2 - 69.0) / 12.0 );

		if (destinationChordNote != currPitch)
		{
			numElapsed = 0;
			currPitch = destinationChordNote;
		}

		if (attack > numElapsed)
		{
			double n = (correctedHz - freq) / attack * numElapsed;
			correctedHz = freq + n;
		}

		if (destinationChordNote2 != currPitch2)
		{
			numElapsed2 = 0;
			currPitch2 = destinationChordNote2;
		}

		if (attack2 > numElapsed2)
		{
			double n = (correctedHz2 - freq) / attack2 * numElapsed2;
			correctedHz2 = freq + n;
		}

		numElapsed++;		
		numElapsed2++;			
		
		*shiftFactor = original/correctedHz;
		*shiftFactor2 = original/correctedHz2;
		*shiftFactor += addVibrato(bufferSize,sampleRate,vibRate,vibDepth);
		*shiftFactor2 += addVibrato(bufferSize,sampleRate,vibRate,vibDepth);
	}

};



#endif