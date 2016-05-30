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

Header file modified from http://isophonics.net/sites/isophonics.net/files/nnls-chroma-0.2.1.tgz [Accessed 26th August 2013]
*/

#ifndef _NNLS_BASE_
#define _NNLS_BASE_

#include <list>
#include <vector>
#include <map>
//using namespace std;


#define M_PI  3.14159265358979323846

class NNLSBase 
{
public:
    virtual ~NNLSBase();
	struct Feature
{
	std::vector<float> values;
	std::string label;
};
typedef std::vector<Feature> FeatureList;

typedef std::map<int, FeatureList> FeatureSet; // key is output no */
    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();
protected:
    NNLSBase(float inputSampleRate);
    void baseProcess(float *inputBuffers);
	// Calculates log2 of number.
double log2( double n )
{
    // log(n)/log(2) is log2.
    return log( n ) / logf( 2.0 );
}

double round(double number)
{
	return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}
    int m_frameCount;
    FeatureList m_logSpectrum;
	//vector<float> m_logSpectrum;
    size_t m_blockSize;
    size_t m_stepSize;
    int m_lengthOfNoteIndex;
    std::vector<float> m_meanTunings;
    std::vector<float> m_localTunings;
    float m_whitening;
    float m_preset;
	float m_useNNLS;
    float m_useHMM;
	//float nm[nNote];
    std::vector<float> m_localTuning;
    std::vector<float> m_kernelValue;
    std::vector<int> m_kernelFftIndex;
    std::vector<int> m_kernelNoteIndex;
    float *m_dict;
    bool m_tuneLocal;
    float m_doNormalizeChroma;
    float m_rollon;
    float m_boostN;
    float m_s;
    std::vector<float> hw;
    std::vector<float> sinvalues;
    std::vector<float> cosvalues;
	float sampleRate;
	friend class HarmonizerEngine;
};






#endif