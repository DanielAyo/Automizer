/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AutomizerAudioProcessor::AutomizerAudioProcessor()
{
	// add single side-chain bus for secondary input (instrument that will produce chord information)
	busArrangement.inputBuses.add(AudioProcessorBus("Sidechain In", AudioChannelSet::mono()));

	// To be compatible with all VST2 DAWs, it's best to pass through the sidechain
	if (isVST2())
		busArrangement.outputBuses.add(AudioProcessorBus("Sidechain Out", AudioChannelSet::mono()));

}

AutomizerAudioProcessor::~AutomizerAudioProcessor()
{
}

//==============================================================================
const String AutomizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AutomizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AutomizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

double AutomizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AutomizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AutomizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AutomizerAudioProcessor::setCurrentProgram (int index)
{
}

const String AutomizerAudioProcessor::getProgramName (int index)
{
    return String();
}

void AutomizerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AutomizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void AutomizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AutomizerAudioProcessor::setPreferredBusArrangement (bool isInput, int bus, const AudioChannelSet& preferredSet)
{
	// Reject any bus arrangements that are not compatible with your plugin

	const bool isMainBus = (bus == 0);
	const bool isSideChain = (bus == 1);
 
    const int numChannels = preferredSet.size();

	// do not allow disabling channels on main bus
	if (numChannels == 0 && isMainBus) return false;

	// VST2 does not natively support sidechains/aux buses.
	// But many DAWs treat the third input of a plug-in
	// as a sidechain. So limit the main bus to stereo!
	if (isVST2())
	{
		if (isMainBus && numChannels != 2) return false;

		// we only allow mono sidechains in VST-2
		if (isSideChain && numChannels != 1)
			return false;
	}

	// always have the same channel layout on both input and output on the main bus
	if (isMainBus && !AudioProcessor::setPreferredBusArrangement(!isInput, bus, preferredSet))
		return false;

   #if JucePlugin_IsMidiEffect
    if (numChannels != 0)
        return false;
   #elif JucePlugin_IsSynth
    if (isInput || (numChannels != 1 && numChannels != 2))
        return false;
   #else
    if (numChannels != 1 && numChannels != 2)
        return false;

    if (! AudioProcessor::setPreferredBusArrangement (! isInput, bus, preferredSet))
        return false;
   #endif

    return AudioProcessor::setPreferredBusArrangement (isInput, bus, preferredSet);
}
#endif

void AutomizerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool AutomizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AutomizerAudioProcessor::createEditor()
{
    return new AutomizerAudioProcessorEditor (*this);
}

//==============================================================================
void AutomizerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AutomizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutomizerAudioProcessor();
}
