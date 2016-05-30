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
#ifndef _NNLS_CHROMA_
#define _NNLS_CHROMA_

#include <map>
#include "NNLSBase.h"
#define _USE_MATH_DEFINES // for C++
#include <cmath>
using namespace std;
#define M_PI  3.14159265358979323846

class NNLSChroma : public NNLSBase
{
public:
    NNLSChroma(float inputSampleRate);
    virtual ~NNLSChroma();
	
	vector<float> chroma;
	vector<float> basschroma;
    FeatureSet getRemainingFeatures(float *inputBuffer, float& whiten, int&, float&);
    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();
protected:
    mutable int m_outputLogSpec;
    mutable int m_outputTunedSpec;
    mutable int m_outputSemiSpec;
    mutable int m_outputChroma;
    mutable int m_outputBassChroma;
};
#endif
