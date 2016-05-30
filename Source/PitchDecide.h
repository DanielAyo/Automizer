#ifndef PITCHDECIDE
#define PITCHDECIDE

#include <math.h>

#define semitone 1.059463094359	// 12th root of 2
#define C0 6.875*semitone*semitone*semitone //First note of MIDI representation C

const int scaleTable[4][14]=
{ 0,  2,  4,  5,  7,  9, 11, 12,  0,  0,  0,  0,  0,  7+1,
  0,  2,  3,  5,  7,  8, 11, 12,  0,  0,  0,  0,  0,  7+1,
  0,  2,  4,  7,  9,  12, 0,  0,  0,  0,  0,  0,  0,  5+1,
  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 12+1};

class PitchDecide
{
public:
	short midiNote;
	float sampleRate;
	float currPitch;
	float referenceHzPar;
	float shiftFactor;
	int destinationNote;
	int numElapsed;
	int attack;
	double AttackTimeMilliseconds;
	
	float correctedHz;
	int keyPar;
	int scalePar;
	float attackMill;
	int key;
	int scale;

	PitchDecide()
	{
		midiNote = 0;
		currPitch = 0;
		numElapsed = 0;
		attack = 0;
		AttackTimeMilliseconds = 0.0;
		referenceHzPar = 440.f;
	}

	~PitchDecide()
	{}

	float processDecision(float detectedPitch, float *refHz, int *keyParam, int *scaleParam, int *attackParam)
	{
		referenceHzPar = *refHz;
		keyPar = *keyParam;
		scalePar = *scaleParam;
		attack = *attackParam;
		midiNote = pitchToMIDI(detectedPitch);
		correctedHz = referenceHzPar * pow( 2.0, ((double)midiNote - 69.0) / 12.0 );
		if (midiNote != currPitch)
		{
			numElapsed = 0;
			currPitch = midiNote;
		}
		if (attack > numElapsed)
		{
			double n = (correctedHz - detectedPitch) / attack * numElapsed;
			correctedHz = detectedPitch + n;
		}
		numElapsed++;
		shiftFactor = detectedPitch/correctedHz;

		if (shiftFactor > 2.0)
			shiftFactor = 2.0f;
		if (shiftFactor < 0.5)
		    shiftFactor = 0.5f;
		return shiftFactor;
	}

	int pitchToMIDI(float freq) // Function modified from http://code.google.com/p/voice2midi/source/browse/trunk/source/voice2midi/voice2midi.cpp
		//[Accessed 26th August 2013]
	{
        key = keyPar;
        scale = scalePar;

        float fnote= log(freq/(C0))/log(semitone);
        int octave=0;

        //reduce pitch height to one octave
        while(fnote>12+key)
        {
			fnote-=12;
            octave+=12;
        }
        int i;

        //find the first note in the scale table bigger than the current note
        for(i=0; i < scaleTable[scale][13] && scaleTable[scale][i]+key < fnote ;i++);

        if(i==0)
		{
			destinationNote = scaleTable[scale][i]+key+octave;
        	return destinationNote;
		}
        if(i==scaleTable[scale][13])
		{
			destinationNote = scaleTable[scale][i-1]+key+octave;
        	return destinationNote;
		}
        // decide whether we are shifting up or down
        if(scaleTable[scale][i]-fnote > fnote - scaleTable[scale][i-1])
		{
			destinationNote = scaleTable[scale][i-1]+key+octave;
            return scaleTable[scale][i-1]+key+octave;
		}
        else
		{
			destinationNote = scaleTable[scale][i]+key+octave;
        	return destinationNote;
		}
	} 
};


#endif