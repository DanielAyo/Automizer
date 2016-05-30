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

#include "NNLSBase.h"
#include <cstdlib>
#include <fstream>
#include <cmath>
#include "chromamethods.h"
#include <algorithm>
#include <vector>
#include <iterator>

NNLSBase::NNLSBase(float inputSampleRate) :
    m_frameCount(0),
    m_logSpectrum(0),
    m_blockSize(2048),
    m_stepSize(1024),
    m_lengthOfNoteIndex(0),
    m_meanTunings(0),
    m_localTunings(0),
    m_whitening(0.4),// Spectral whitening: no whitening - 0; whitening - 1.
    m_preset(0.0),
    m_useNNLS(1.0), //use approximate transcription (NNLS)
	m_useHMM(1.0),
    m_localTuning(0.0),
    m_kernelValue(0),
    m_kernelFftIndex(0),
    m_kernelNoteIndex(0),
    m_dict(0),
    m_tuneLocal(0.0), //Tuning can be performed locally or on the whole extraction segment
    m_doNormalizeChroma(0),//How shall the chroma vector be normalized
    m_rollon(1.0), //Consider the cumulative energy spectrum (from low to high frequencies). All bins below the first bin whose cumulative energy exceeds the quantile [spectral roll on] x [total energy] will be set to 0. A value of 0 means that no bins will be changed.
    m_boostN(0.1),
	m_s(0.7), //Determines how individual notes in the note dictionary look: higher values mean more dominant higher harmonics.
	sinvalues(0),
	cosvalues(0)
{
	sampleRate = inputSampleRate;
    // make the *note* dictionary matrix
    m_dict = new float[nNote * 84];
    for (unsigned i = 0; i < nNote * 84; ++i) m_dict[i] = 0.0;    
}

NNLSBase::~NNLSBase()
{
    delete [] m_dict;
}



bool
NNLSBase::initialise(size_t channels, size_t stepSize, size_t blockSize)
{	
	
	dictionaryMatrix(m_dict, m_s);
	
	// make things for tuning estimation
	for (int iBPS = 0; iBPS < nBPS; ++iBPS) 
	{
        sinvalues.push_back(sin(2*M_PI*(iBPS*1.0/nBPS)));
        cosvalues.push_back(cos(2*M_PI*(iBPS*1.0/nBPS)));
    }
    
	
	// make hamming window of length 1/2 octave
	int hamwinlength = nBPS * 6 + 1;
    float hamwinsum = 0;
    for (int i = 0; i < hamwinlength; ++i) 
	{ 
        hw.push_back(0.54 - 0.46 * cos((2*M_PI*i)/(hamwinlength-1)));    
        hamwinsum += 0.54 - 0.46 * cos((2*M_PI*i)/(hamwinlength-1));
    }
    for (int i = 0; i < hamwinlength; ++i) 
		hw[i] = hw[i] / hamwinsum;
    
    // initialise the tuning
    for (int iBPS = 0; iBPS < nBPS; ++iBPS) 
	{
        m_meanTunings.push_back(0);
        m_localTunings.push_back(0);
    }
	
    m_blockSize = blockSize;
    m_stepSize = stepSize;
    m_frameCount = 0;
    int tempn = nNote * m_blockSize/2;
    float *tempkernel;

    tempkernel = new float[tempn];

    logFreqMatrix(sampleRate, m_blockSize, tempkernel);
    m_kernelValue.clear();
    m_kernelFftIndex.clear();
    m_kernelNoteIndex.clear();
    int countNonzero = 0;
    for (int iNote = 0; iNote < nNote; ++iNote) 
	{ // I don't know if this is wise: manually making a sparse matrix
        for (size_t iFFT = 0; iFFT < blockSize/2; ++iFFT) 
		{
            if (tempkernel[iFFT + blockSize/2 * iNote] > 0) 
			{
                m_kernelValue.push_back(tempkernel[iFFT + blockSize/2 * iNote]);

                if (tempkernel[iFFT + blockSize/2 * iNote] > 0) 
                    countNonzero++;
            
                m_kernelFftIndex.push_back(iFFT);
                m_kernelNoteIndex.push_back(iNote);				
            }
        }
    }
    delete [] tempkernel;

    return true;
}

void
NNLSBase::reset()
{
	
    // Clear buffers, reset stored values, etc
    m_frameCount = 0;
    m_logSpectrum.clear();
    for (int iBPS = 0; iBPS < nBPS; ++iBPS) 
	{
        m_meanTunings[iBPS] = 0;
        m_localTunings[iBPS] = 0;
    }
    m_localTuning.clear();
}

void
NNLSBase::baseProcess(float *magnitude)
{   
    m_frameCount++;   
	m_localTuning.clear();

    // note magnitude mapping using pre-calculated matrix
    float *nm  = new float[nNote]; // note magnitude
    for (int iNote = 0; iNote < nNote; iNote++) 
        nm[iNote] = 0; // initialise as 0

    int binCount = 0;
    for (std::vector<float>::iterator it = m_kernelValue.begin(); it != m_kernelValue.end(); ++it) 
	{
        nm[m_kernelNoteIndex[binCount]] += magnitude[m_kernelFftIndex[binCount]] * m_kernelValue[binCount];
        binCount++;	
    }

    float one_over_N = 1.0/m_frameCount;
    // update means of complex tuning variables
    for (int iBPS = 0; iBPS < nBPS; ++iBPS) 
		m_meanTunings[iBPS] *= float(m_frameCount-1)*one_over_N;
    
    for (int iTone = 0; iTone < round(nNote*0.62/nBPS)*nBPS+1; iTone = iTone + nBPS) 
	{
        for (int iBPS = 0; iBPS < nBPS; ++iBPS) 
			m_meanTunings[iBPS] += nm[iTone + iBPS]*one_over_N;

        float ratioOld = 0.997;

        for (int iBPS = 0; iBPS < nBPS; ++iBPS) 
		{
            m_localTunings[iBPS] *= ratioOld; 
            m_localTunings[iBPS] += nm[iTone + iBPS] * (1 - ratioOld);
        }
    }

    float localTuningImag = 0;
    float localTuningReal = 0;
    for (int iBPS = 0; iBPS < nBPS; ++iBPS) 
	{
        localTuningReal += m_localTunings[iBPS] * cosvalues[iBPS];
        localTuningImag += m_localTunings[iBPS] * sinvalues[iBPS];
    }
    
    float normalisedtuning = atan2(localTuningImag, localTuningReal)/(2*M_PI);
    m_localTuning.push_back(normalisedtuning);
    
    Feature f1; // logfreqspec
    for (int iNote = 0; iNote < nNote; iNote++) 
        f1.values.push_back(nm[iNote]);

    // deletes
    delete[] nm;
	m_logSpectrum.clear();
    m_logSpectrum.push_back(f1); // remember note magnitude
}

