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

#include "chromamethods.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <boost/tokenizer.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/lexical_cast.hpp>
//#include "chorddict.cpp"

using namespace std;
using namespace boost;

// Calculates log2 of number.
double log2( double n )
{
    // log(n)/log(2) is log2.
    return log( n ) / logf( 2.0 );
}

void dictionaryMatrix(float* dm, float s_param) 
{
    int binspersemitone = nBPS;
    int minoctave = 0; // this must be 0
    int maxoctave = 7; // this must be 7
    
    // pitch-spaced frequency vector
    int minMIDI = 21 + minoctave * 12 - 1; // this includes one additional semitone!
    int maxMIDI = 21 + maxoctave * 12; // this includes one additional semitone!
    vector<float> cq_f;
    float oob = 1.0/binspersemitone; // one over binspersemitone
	 // 0.083333 is approx 1/12
    for (int i = minMIDI; i < maxMIDI; ++i) 
	{
        for (int k = 0; k < binspersemitone; ++k)	 
            cq_f.push_back(440 * pow(2.0,0.083333333333 * (i+oob*k-69)));
    }
    cq_f.push_back(440 * pow(2.0,0.083333 * (maxMIDI-69)));

    float curr_f;
    float floatbin;
    float curr_amp;

    // now for every combination calculate the matrix element
    for (int iOut = 0; iOut < 12 * (maxoctave - minoctave); ++iOut) 
	{
        for (int iHarm = 1; iHarm <= 20; ++iHarm) 
		{
            curr_f = 440 * pow(2,(minMIDI-69+iOut)*1.0/12) * iHarm;
            floatbin = ((iOut + 1) * binspersemitone + 1) + binspersemitone * 12 * log2(iHarm);
            curr_amp = pow(s_param,float(iHarm-1));
            for (int iNote = 0; iNote < nNote; ++iNote) 
			{
                if (abs(iNote+1.0-floatbin)<2) 
                    dm[iNote  + nNote * iOut] += cospuls(iNote+1.0, floatbin, binspersemitone + 0.0) * curr_amp;
            }
        }
    }
}

float cospuls(float x, float centre, float width) 
{
    float recipwidth = 1.0/width;
    if (abs(x - centre) <= 0.5 * width) 
        return cos((x-centre)*2*M_PI*recipwidth)*.5+.5;

    return 0.0;
}

bool logFreqMatrix(int fs, int blocksize, float *outmatrix) 
{
	
    int binspersemitone = nBPS; 
    int minoctave = 0; // this must be 0
    int maxoctave = 7; // this must be 7
    int oversampling = 80;
	
    // linear frequency vector
    vector<float> fft_f;
    for (int i = 0; i < blocksize/2; ++i) 
        fft_f.push_back(i * (fs * 1.0 / blocksize));
   
    float fft_width = fs * 2.0 / blocksize;
	
    // linear oversampled frequency vector
    vector<float> oversampled_f;
    for (int i = 0; i < oversampling * blocksize/2; ++i) 
        oversampled_f.push_back(i * ((fs * 1.0 / blocksize) / oversampling));
	
    // pitch-spaced frequency vector
    int minMIDI = 21 + minoctave * 12 - 1; // this includes one additional semitone!
    int maxMIDI = 21 + maxoctave * 12; // this includes one additional semitone!
    vector<float> cq_f;
    float oob = 1.0/binspersemitone; // one over binspersemitone
	for (int i = minMIDI; i < maxMIDI; ++i) 
	{
        for (int k = 0; k < binspersemitone; ++k)	 
            cq_f.push_back(440 * pow(2.0,0.083333333333 * (i+oob*k-69)));
    }
    cq_f.push_back(440 * pow(2.0,0.083333 * (maxMIDI-69)));

    int nFFT = fft_f.size();
	
    vector<float> fft_activation;
    for (int iOS = 0; iOS < 2 * oversampling; ++iOS) 
	{
        float cosp = cospuls(oversampled_f[iOS],fft_f[1],fft_width);
        fft_activation.push_back(cosp);
    }
	
    float cq_activation;
    for (int iFFT = 1; iFFT < nFFT; ++iFFT) 
	{
        // find frequency stretch where the oversampled vector can be non-zero (i.e. in a window of width fft_width around the current frequency)
        int curr_start = oversampling * iFFT - oversampling;
        int curr_end = oversampling * iFFT + oversampling; // don't know if I should add "+1" here
        for (unsigned iCQ = 0; iCQ < cq_f.size(); ++iCQ) 
		{
            outmatrix[iFFT + nFFT * iCQ] = 0;
            if (cq_f[iCQ] * pow(2.0, 0.084) + fft_width > fft_f[iFFT] && cq_f[iCQ] * pow(2.0, -0.084 * 2) - fft_width < fft_f[iFFT]) 
			{ // within a generous neighbourhood
                for (int iOS = curr_start; iOS < curr_end; ++iOS) 
				{
                    cq_activation = pitchCospuls(oversampled_f[iOS],cq_f[iCQ],binspersemitone*12);
                    outmatrix[iFFT + nFFT * iCQ] += cq_activation * fft_activation[iOS-curr_start];
                }				
            }
        }
    }
    return true;	
}

float pitchCospuls(float x, float centre, int binsperoctave) 
{
    float warpedf = -binsperoctave * (log2(centre) - log2(x));
    float out = cospuls(warpedf, 0.0, 2.0);
    // now scale to correct for note density
    float c = log(2.0)/binsperoctave;
    if (x > 0) 
        out = out / (c * x);
	else 
        out = 0;
    return out;
}

/** Special Convolution
    special convolution is as long as the convolvee, i.e. the first argument. in the valid core part of the 
    convolution it contains the usual convolution values, but the pads at the beginning (ending) have the same values
    as the first (last) valid convolution bin.
**/

vector<float> SpecialConvolution(vector<float> convolvee, vector<float> kernel)
{
    float s;
    int m, n;
    int lenConvolvee = convolvee.size();
    int lenKernel = kernel.size();

    vector<float> Z(nNote,0);
    assert(lenKernel % 2 != 0); // no exception handling !!!
    
    for (n = lenKernel - 1; n < lenConvolvee; n++) 
	{
    	s=0.0;

    	for (m = 0; m < lenKernel; m++) 
            s += convolvee[n-m] * kernel[m];

        Z[n -lenKernel/2] = s;
    }
    
    // fill upper and lower pads
    for (n = 0; n < lenKernel/2; n++) 
		Z[n] = Z[lenKernel/2];    
    for (n = lenConvolvee; n < lenConvolvee +lenKernel/2; n++) 
		Z[n - lenKernel/2] = Z[lenConvolvee - lenKernel/2 -  1];

    return Z;
}