/*
This class has been used from https://github.com/sammyer/Chordroid/blob/master/src/com/chordroid/FFT.java 
[Accessed 25th August 2013]

*/




#ifndef MYERFFT
#define MYERFFT

// constant declarations
#define PI 3.14159265358979
#define LOG2 0.6931471805599453
#include <vector>

class MyerFFT
{
	public:
		MyerFFT(int dataSize, float sampleRate);
		void fft(double *x, double *y);
		std::vector<float> windowed_fft(float *buffer,int bufferStartPos,int bufferSize);
		virtual ~MyerFFT();
		double *sinx;
		double *cosx;
		
	private:
		int n;
		int m;
		double *window;
};

#endif 
