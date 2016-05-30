/*
  NNLS-Chroma / Chordino

  Audio feature extraction plugins for chromagram and chord
  estimation.

  Centre for Digital Music, Queen Mary University of London.
  This file copyright 2008-2010 Matthias Mauch and QMUL.
    
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of the
  License, or (at your option) any later version.  See the file
  COPYING included with this distribution for more information.



 Cpp file modified from http://isophonics.net/sites/isophonics.net/files/nnls-chroma-0.2.1.tgz [Accessed 26th August 2013]
*/
#include "NNLSChroma.h"

#include "chromamethods.h"

#include <cstdlib>
#include <fstream>
#include <cmath>

#include <cstring>
#include <algorithm>

NNLSChroma::NNLSChroma(float inputSampleRate) : 
	NNLSBase(inputSampleRate)
{}


NNLSChroma::~NNLSChroma()
{}


bool
NNLSChroma::initialise(size_t channels, size_t stepSize, size_t blockSize)
{

    unsigned short index = 0;
	m_outputLogSpec = index++;
    m_outputTunedSpec = index++;
    m_outputSemiSpec = index++;
	m_outputChroma = index++;
    m_outputBassChroma = index++;

    if (!NNLSBase::initialise(channels, stepSize, blockSize)) 
        return false;
	
    return true;
}

void
NNLSChroma::reset()
{
    NNLSBase::reset();
}

NNLSChroma::FeatureSet
NNLSChroma::getRemainingFeatures(float *inputBuffer, float& whiten, int& tune, float& refHz)
{
	NNLSBase::baseProcess(inputBuffer);
    FeatureSet fs;
	fs[m_outputLogSpec].clear();
    fs[m_outputLogSpec].push_back(m_logSpectrum[m_logSpectrum.size()-1]);
	float normalisedtuning;
   
    FeatureSet fsOut;
    if (m_logSpectrum.size() == 0) 
		return fsOut;
    
    /**  Calculate Tuning
         calculate tuning from (using the angle of the complex number defined by the 
         cumulative mean real and imag values)*/
    
    float meanTuningImag = 0;
    float meanTuningReal = 0;

	if(tune==1)
	{
		for (int iBPS = 0; iBPS < nBPS; ++iBPS) 
		{
			meanTuningReal += m_meanTunings[iBPS] * cosvalues[iBPS];
			meanTuningImag += m_meanTunings[iBPS] * sinvalues[iBPS];
		}
	}

	float cumulativetuning = 440 * pow(2,atan2(meanTuningImag, meanTuningReal)/(24*M_PI));
	normalisedtuning = atan2(meanTuningImag, meanTuningReal)/(2*M_PI);

    int intShift = floor(normalisedtuning * 3);
    float floatShift = normalisedtuning * 3 - intShift; // floatShift is a really bad name for this
		    
	
    /** Tune Log-Frequency Spectrogram
        calculate a tuned log-frequency spectrogram (f2): use the tuning estimated above (kinda f0) to 
        perform linear interpolation on the existing log-frequency spectrogram (kinda f1).*/
    
    float tempValue = 0;
    float dbThreshold = 0; // relative to the background spectrum
    float thresh = pow(10,dbThreshold/20);
    int count = 0;
	//===
	FeatureList tunedSpec;
    int nFrame = m_logSpectrum.size();
	//===
	for (FeatureList::iterator i = m_logSpectrum.begin(); i != m_logSpectrum.end(); ++i) 
	{
	    Feature currentLogSpectrum = *i;
        Feature currentTunedSpec; // tuned log-frequency spectrum
     
        currentTunedSpec.values.push_back(0.0); currentTunedSpec.values.push_back(0.0); // set lower edge to zero
		
        if (m_tuneLocal) 
		{
            intShift = floor(m_localTuning[count] * 3);
            floatShift = m_localTuning[count] * 3 - intShift; // floatShift is a really bad name for this
        }
	        
        for (unsigned k = 2; k < currentLogSpectrum.values.size() - 3; ++k) 
		{ // interpolate all inner bins
            tempValue = currentLogSpectrum.values[k + intShift] * (1-floatShift) + currentLogSpectrum.values[k+intShift+1] * floatShift;
            currentTunedSpec.values.push_back(tempValue);
        }
		        
        currentTunedSpec.values.push_back(0.0); currentTunedSpec.values.push_back(0.0); currentTunedSpec.values.push_back(0.0); // upper edge

        vector<float> runningmean = SpecialConvolution(currentTunedSpec.values,hw);
        vector<float> runningstd;
        for (int i = 0; i < nNote; i++) // first step: squared values into vector (variance)
            runningstd.push_back((currentTunedSpec.values[i] - runningmean[i]) * (currentTunedSpec.values[i] - runningmean[i]));
        
        runningstd = SpecialConvolution(runningstd,hw); // second step convolve
        for (int i = 0; i < nNote; i++) 
		{ 
            runningstd[i] = sqrt(runningstd[i]); // square root to finally have running std
            if (runningstd[i] > 0) 
			{
				currentTunedSpec.values[i] = (currentTunedSpec.values[i] - runningmean[i]) > 0 ?
                    (currentTunedSpec.values[i] - runningmean[i]) / pow(runningstd[i],whiten) : 0;
            }
        }
        fsOut[m_outputTunedSpec].push_back(currentTunedSpec);
        tunedSpec.push_back(currentTunedSpec);
        count++;
    }
    /** Semitone spectrum and chromagrams
        Semitone-spaced log-frequency spectrum derived from the tuned log-freq spectrum above. the spectrum
        is inferred using a non-negative least squares algorithm.
        2 different kinds of chromagram are calculated, "treble", "bass".
    */
    count = 0;

    for (FeatureList::iterator it = tunedSpec.begin(); it != tunedSpec.end(); ++it) 
	{
        Feature currentTunedSpec = *it; // logfreq spectrum
		float b[nNote];
        bool some_b_greater_zero = false;
        float sumb = 0;

        for (int i = 0; i < nNote; i++) 
		{
            b[i] = currentTunedSpec.values[i];
            sumb += b[i];
            if (b[i] > 0) 
                some_b_greater_zero = true;
        }
        // here's where the non-negative least squares algorithm calculates the note activation x
	
        chroma = vector<float>(12, 0);
        basschroma = vector<float>(12, 0);
        float currval;
        int iSemitone = 0;

		if (some_b_greater_zero) 
		{
			float x[84+1000];
            for (int i = 1; i < 1084; ++i) x[i] = 1.0;
            vector<int> signifIndex;
            int index=0;
            sumb /= 84.0;
            for (int iNote = nBPS/2 + 2; iNote < nNote - nBPS/2; iNote += nBPS) 
			{
				float currval = 0;
                for (int iBPS = -nBPS/2; iBPS < nBPS/2+1; ++iBPS) 
					currval += b[iNote + iBPS]; 

                if (currval > 0) signifIndex.push_back(index);
                    index++;
            }
            float rnorm;
            float w[84+1000];
            float zz[84+1000];
            int indx[84+1000];
            int mode;
            int dictsize = nNote*signifIndex.size();
            float *curr_dict = new float[dictsize];
            for (int iNote = 0; iNote < (int)signifIndex.size(); ++iNote) 
			{
				for (int iBin = 0; iBin < nNote; iBin++) 
                    curr_dict[iNote * nNote + iBin] = 1.0 * m_dict[signifIndex[iNote] * nNote + iBin];
            }
            
			nnls(curr_dict, nNote, nNote, signifIndex.size(), b, x, &rnorm, w, zz, indx, &mode);
            delete [] curr_dict;
            
			for (int iNote = 0; iNote < (int)signifIndex.size(); ++iNote) 
			{
				chroma[signifIndex[iNote] % 12] += x[iNote] * treblewindow[signifIndex[iNote]];
                basschroma[signifIndex[iNote] % 12] += x[iNote] * basswindow[signifIndex[iNote]];
            }
        }	
	}
    return fsOut;     	
 }

