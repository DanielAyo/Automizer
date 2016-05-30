/*
This class has been modified from https://github.com/sammyer/Chordroid/blob/master/src/com/chordroid/Viterbi.java
[Accessed 25th August 2013]

*/

#ifndef MYERVITERBI
#define MYERVITERBI

class Viterbi
{
	public:
		Viterbi();
		virtual ~Viterbi();
		int viterbi(float *bassChroma, float *midChroma);
		void reset();
	protected:
	private:
		float pathProbs[60];
		int pathChordLengths[60][2];
		
		float getTransProb(int chordLen, int prevChordLen);
		bool normalize(float *chroma,int n);
		void getRootProbs(float *chroma, float *rootProbs);
		float getMidChordProb(float *chroma, int chordTypeId, int noteNum);
		void getChordProbs(float *bassChroma, float *midChroma, float *chordProbs);
};

#endif 