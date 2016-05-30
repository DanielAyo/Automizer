#ifndef __PLUGINEDITOR_H_51DE3417__
#define __PLUGINEDITOR_H_51DE3417__

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class AutomizerAudioProcessorEditor  :  public AudioProcessorEditor,
										public SliderListener,
										public Timer, 
										public ComboBox::Listener
{
public:
    AutomizerAudioProcessorEditor (AutomizerAudioProcessor* ownerFilter);
    ~AutomizerAudioProcessorEditor();

    //==============================================================================
    // This is just a standard Juce paint method...
    void paint (Graphics& g);
	void timerCallback();
    void resized();
    void sliderValueChanged (Slider*);
    void comboBoxChanged (ComboBox*);
	void displayPitchInfo();
	void displayChordInfo();
private:
    Label outputGainLabel, voice1GainLabel, voice2GainLabel, shiftTypeLabel, transposeLabel, pan1Label, pan2Label,voice1Label, voice2Label, nameLabel;
	Label keyLabel, scaleLabel, autotuneLabel, attackLabel, refLabel, shiftLabel, whitenLabel, rollOnLabel, harmonyLabel, inputLabel, outputLabel, systemLabel, input2Label;
    Slider outputGainSlider, voice1GainSlider, voice2GainSlider, transposeSlider,  pan1Slider,  pan2Slider;
	Slider attackSlider, refSlider, whitenSlider, rollOnSlider;
	ComboBox shiftTypeComboBox, autotuneComboBox, keyComboBox, scaleComboBox, shiftComboBox, harmonyComboBox, tuneComboBox;
	Label pitchLabel, chordLabel, transposePitchLabel, centsLabel, MIDINoteLabel, harm1Label, harm2Label;
    ScopedPointer<ResizableCornerComponent> resizer;
    ComponentBoundsConstrainer resizeLimits;
    Label harm1AttackLabel, harm2AttackLabel, vDepthLabel, vRateLabel, tuneLabel;
	Slider harm1AttackSlider, harm2AttackSlider, vDepthSlider, vRateSlider;


	GroupComponent* groupComponent;
	GroupComponent* groupComponent2;
	GroupComponent* groupComponent3;
	GroupComponent* groupComponent4;
    AutomizerAudioProcessor* getProcessor() const
    {
        return static_cast <AutomizerAudioProcessor*> (getAudioProcessor());
    }
};


#endif  // __PLUGINEDITOR_H_51DE3417__
