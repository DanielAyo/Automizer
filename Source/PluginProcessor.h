#ifndef AUTOMIZER
#define AUTOMIZER

#include "../JuceLibraryCode/JuceHeader.h"
#include "HarmonizerEngine.h"

//==============================================================================
/**
*/
class AutomizerAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    AutomizerAudioProcessor();
    ~AutomizerAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;
    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
	// these are used to persist the UI's size - the values are stored along with the
    // filter's other parameters, and the UI component will update them when it gets
    // resized.
    int lastUIWidth, lastUIHeight;
	
	enum Parameters
    {
        kOutputGain = 0,
		kVoice1Gain,
		kVoice2Gain,
		kPitchShiftType,
		kTranspose,
		kPan1,
		kPan2,
		kKey,
		kScale,
		kAutotune,
		kAttack,
		kRef,
		kShift,
		kRollOn,
		kWhite,
		kHarmony,
		kHarm1Attack,
		kHarm2Attack,
		kVibratoDepth,
		kVibratoRate,
		kTune,
        kNumParameters
    };

	HarmonizerEngine* engine;
private:
	float sampleRate;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutomizerAudioProcessor)
};

#endif  
