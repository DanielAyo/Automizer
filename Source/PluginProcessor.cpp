/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AutomizerAudioProcessor::AutomizerAudioProcessor()
{
	//initialisations
	if(this->getSampleRate() > 0)
		sampleRate = (float)this->getSampleRate();
	else
		sampleRate = 44100.f;
	setLatencySamples(pitchBufferSize);
	//minimum grpahical positions
    lastUIWidth = 800;
    lastUIHeight = 600;

	this->engine = new HarmonizerEngine(sampleRate);
}

AutomizerAudioProcessor::~AutomizerAudioProcessor()
{
	delete engine;
}

//==============================================================================
const String AutomizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int AutomizerAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float AutomizerAudioProcessor::getParameter (int index)
{
   // This method will be called by the host, probably on the audio thread, so
    // it's absolutely time-critical. Don't use critical sections or anything
    // UI-related, or anything at all that may block in any way!
    switch (index)
    {
        case kOutputGain:			return engine->outputGainPar;
		case kVoice1Gain:			return engine->voice1GainPar;
		case kVoice2Gain:			return engine->voice2GainPar;
        case kPitchShiftType:		return engine->shiftTypePar;
        case kTranspose:			return engine->transposePar; 
		case kPan1:					return engine->panLPar;
		case kPan2:					return engine->panRPar;
		case kKey:					return engine->keyPar;
		case kScale:				return engine->scalePar;
		case kAutotune:				return engine->autotunePar;
		case kAttack:				return engine->attackPar;
		case kRef:					return engine->referenceHzPar;
		case kShift:				return engine->shiftPar;
		case kRollOn:				return engine->rollOnPar;
		case kWhite:				return engine->whiteningPar;
		case kHarmony:				return engine->harmonyPar;

		case kHarm1Attack:			return engine->harm1AttackPar;
		case kHarm2Attack:			return engine->harm2AttackPar;
		case kVibratoDepth:			return engine->vDepth;
		case kVibratoRate:			return engine->vRate;
		case kTune:					return engine->tunePar;
		default:					return 0.0f;
    }
}

void AutomizerAudioProcessor::setParameter (int index, float newValue)
{
	switch (index)
    {
        case kOutputGain:			
			engine->outputGaindB = newValue;
			engine->cookGain();
			break;
		case kVoice1Gain:			
			engine->voice1GaindB = newValue;
			engine->cookGain();
			break;
		case kVoice2Gain:			
			engine->voice2GaindB = newValue;
			engine->cookGain();
			break;
        case kPitchShiftType:		
			engine->shiftTypePar = newValue;  
			break;
        case kTranspose:			
			engine->semitoneTranspose = newValue; 
			engine->cookMusic();
			break;
		case kPan1:
			engine->panLPercent = newValue;
			engine->cookPan();
			break;
		case kPan2:				
			engine->panRPercent = newValue;
			engine->cookPan();
			break;
		case kKey:
			engine->keyGUI = newValue;
			engine->cookMusic();
			break;
		case kScale:
			engine->scaleGUI = newValue;
			engine->cookMusic();
			break;
		case kAutotune:
			engine->autotunePar = newValue;
			break;
		case kAttack:
			engine->attackTimeMilliseconds = newValue;
			engine->cookMusic();
			break;
		case kRef:
			engine->referenceHzPar = newValue;
			break;
		case kShift:
			engine->shiftPar = newValue;
			break;
		case kRollOn:				
			engine->rollOnPar  = newValue;
			break;
		case kWhite:				
			engine->whiteningPercent = newValue;
			engine->cookPan();
			break;
		case kHarmony:				
			engine->harmonyPar = newValue;
			break;
		case kHarm1Attack:			
			engine->harm1AttackTimeMilliseconds = newValue;
			engine->cookMusic();
			break;
		case kHarm2Attack:			
			engine->harm2AttackTimeMilliseconds = newValue;
			engine->cookMusic();
			break;
		case kVibratoDepth:
			engine->vDepth = newValue;
			break;
		case kVibratoRate:
			engine->vRate = newValue;
			break;
		case kTune:
			engine->tunePar = newValue;
			break;
        default:					
			break;
    }
}

const String AutomizerAudioProcessor::getParameterName (int index)
{
     switch (index)
    {
        case kOutputGain:		return "OutputGain";
		case kVoice1Gain:		return "Voice 1 Gain";
		case kVoice2Gain:		return "Voice 2 Gain";
        case kPitchShiftType:	return "PitchShiftType";
        case kTranspose:		return "Transpose";
		case kPan1:				return "Voice 1 Pan";
		case kPan2:				return "Voice 2 Pan";
		case kKey:				return "Key";
		case kScale:			return "Scale";
		case kAutotune:			return "Autotune";
		case kAttack:			return "Attack";
		case kRef:				return "Reference";
		case kShift:			return "TransposeOnOff";
		case kRollOn:			return "rollOnPar";
		case kWhite:			return "whiteningPar";
		case kHarmony:			return "harmonyPar";
		case kHarm1Attack:		return "Harmony1Attack";
		case kHarm2Attack:		return "Harmony2Attack";
		case kVibratoDepth:		return "VibratoDepth";
		case kVibratoRate:		return "VibratoRate";
		case kTune:				return "Tuning";
        default:				break;
    }
    
    return String::empty;
}

const String AutomizerAudioProcessor::getParameterText (int index)
{
    return String (getParameter (index), 2);
}

const String AutomizerAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String AutomizerAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool AutomizerAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool AutomizerAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
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

bool AutomizerAudioProcessor::silenceInProducesSilenceOut() const
{
    return true;
}

double AutomizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AutomizerAudioProcessor::getNumPrograms()
{
    return 0;
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
    return String::empty;
}

void AutomizerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AutomizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

	engine->prepare();
}

void AutomizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
	engine->reset();
}

void AutomizerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	// Change sample rate if required
	if (sampleRate != this->getSampleRate())
	{
		delete engine;
		sampleRate = (float)this->getSampleRate();
		this->engine = new HarmonizerEngine(sampleRate);
		engine->prepare();
	}
    // Helpful information about this block of samples:
    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?
  
	float *samplesL = buffer.getWritePointer(0, 0);
	float *samplesR = buffer.getWritePointer(1, 0);

	if (numInputChannels == 2)
	{
		while (--numSamples >= 0)
			engine->process(samplesL++,samplesR++);
	
	}
	if (numInputChannels == 1)
	{	
		while (--numSamples >= 0)
		{
			engine->process(samplesL, samplesL);
			++samplesL;
		}
	}
    // In case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
}

//==============================================================================
bool AutomizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AutomizerAudioProcessor::createEditor()
{
    return new AutomizerAudioProcessorEditor (this);
}

//==============================================================================
void AutomizerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
   // Create an outer XML element..
    XmlElement xml("AYOPLUGINSETTINGS");
    
    // add some attributes to it..
    xml.setAttribute("uiWidth", lastUIWidth);
    xml.setAttribute("uiHeight", lastUIHeight);

    xml.setAttribute("OutputGain", engine->outputGainPar);
	xml.setAttribute("Voice1Gain", engine->voice1GainPar);
	xml.setAttribute("Voice2Gain", engine->voice2GainPar);
    xml.setAttribute("PitchShiftType", engine->shiftTypePar);
    xml.setAttribute("Transpose", engine->transposePar);
	xml.setAttribute("PanVoice1", engine->panLPar);
	xml.setAttribute("PanVoice2", engine->panRPar);
	xml.setAttribute("Key", engine->keyPar);
	xml.setAttribute("Scale", engine->scalePar);
	xml.setAttribute("Autotune", engine->autotunePar);
	xml.setAttribute("Attack", engine->attackPar);
	xml.setAttribute("Reference", engine->referenceHzPar);
	xml.setAttribute("TransposeOnOff", engine->shiftPar);
	xml.setAttribute("RollOnOff", engine->rollOnPar);
	xml.setAttribute("SpectralWhitening", engine->whiteningPar);
	xml.setAttribute("SpectralShape", engine->harmonyPar);

	xml.setAttribute("Harm1Attack", engine->harm1AttackPar);
	xml.setAttribute("Harm2Attack", engine->harm2AttackPar);
	
	xml.setAttribute("VibratoDepth", engine->vDepth);
	xml.setAttribute("VibratoRate", engine->vRate);
	xml.setAttribute("Tuning", engine->tunePar);
    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary(xml, destData);

}

void AutomizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    // This getXmlFromBinary() helper function retrieves our XML from the binary blob..
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    
    if(xmlState != 0)
    {
        // make sure that it's actually our type of XML object..
        if(xmlState->hasTagName("AYOPLUGINSETTINGS"))
        {
            // ok, now pull out our parameters..
            lastUIWidth  = xmlState->getIntAttribute("uiWidth", lastUIWidth);
            lastUIHeight = xmlState->getIntAttribute("uiHeight", lastUIHeight);
            
            engine->outputGainPar   = (float)xmlState->getDoubleAttribute("OutputGain", engine->outputGainPar);
			engine->voice1GainPar   = (float)xmlState->getDoubleAttribute("Voice1Gain", engine->voice1GainPar);
			engine->voice2GainPar   = (float)xmlState->getDoubleAttribute("Voice2Gain", engine->voice2GainPar);
            engine->shiftTypePar    = (float)xmlState->getDoubleAttribute("PitchShiftType", engine->shiftTypePar);
            engine->transposePar    = (float)xmlState->getDoubleAttribute("Transpose", engine->transposePar);
			engine->panLPar         = (float)xmlState->getDoubleAttribute("PanVoice1",engine->panLPar);
			engine->panRPar         = (float)xmlState->getDoubleAttribute("PanVoice2", engine->panRPar);
			engine->keyPar          = (float)xmlState->getDoubleAttribute("Key", engine->keyPar);
			engine->scalePar        = (float)xmlState->getDoubleAttribute("Scale", engine->scalePar);
			engine->autotunePar     = (float)xmlState->getDoubleAttribute("Autotune", engine->autotunePar);
			engine->attackPar       = (float)xmlState->getDoubleAttribute("Attack", engine->attackPar);
			engine->referenceHzPar  = (float)xmlState->getDoubleAttribute("Reference", engine->referenceHzPar);	
			engine->shiftPar	    = (float)xmlState->getDoubleAttribute("TransposeOnOff", engine->shiftPar);	
			engine->rollOnPar		= (float)xmlState->getDoubleAttribute("RollOnOff", engine->rollOnPar);	
			engine->whiteningPar	= (float)xmlState->getDoubleAttribute("SpectralWhitening", engine->whiteningPar);	
			engine->harmonyPar		= (float)xmlState->getDoubleAttribute("SpectralShape", engine->harmonyPar);	
			engine->harm1AttackPar  = (float)xmlState->getDoubleAttribute("Harm1Attack", engine->harm1AttackPar);	
			engine->harm2AttackPar  = (float)xmlState->getDoubleAttribute("Harm2Attack", engine->harm2AttackPar);	
			engine->vDepth  = (float)xmlState->getDoubleAttribute("VibratoDepth", engine->vDepth);	
			engine->vRate  = (float)xmlState->getDoubleAttribute("Harm2Attack", engine->vRate);	
			engine->tunePar =  (float)xmlState->getDoubleAttribute("Tuning", engine->tunePar);	
		}
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutomizerAudioProcessor();
}
