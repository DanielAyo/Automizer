#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AutomizerAudioProcessorEditor::AutomizerAudioProcessorEditor (AutomizerAudioProcessor* ownerFilter)
    : AudioProcessorEditor (ownerFilter),
	outputGainLabel("", "Output Gain(dB):"), 
	voice1GainLabel("", "Voice 1 Gain(dB):"),
	voice2GainLabel("", "Voice 2 Gain(dB):"),
	shiftTypeLabel("", "Formant Preservation:"), 
	transposeLabel("", "Transpose:"),
	pan1Label("", "Voice 1 Pan(%):"), 
	pan2Label("", "Voice 2 Pan(%):"),
	keyLabel("", "Key:"),
	scaleLabel("", "Scale:"),
	autotuneLabel("","Autotune:"),
	attackLabel("","Attack(ms):"),
	refLabel("", "Reference(Hz):"),
	shiftLabel("", "Intelligent Harmony:"),
	rollOnLabel("", "Spectral Roll On(%):"),
	whitenLabel("", "Spectral Whitening(%):"),
	harmonyLabel("", "Harmony Type:"),
	tuneLabel("", "Chord Tuning"),
	harm1AttackLabel("", "Voice 1 Attack(ms):"),
	harm2AttackLabel("", "Voice 2 Attack(ms):"),
	vDepthLabel("", "Vibrato Depth(%):"),
	vRateLabel("", "Vibrato Rate(Hz):"),
	inputLabel("","INPUT 1 (L) VOX"),
	input2Label("","INPUT 2 (R) INS"),
	systemLabel("","SYSTEM"),
	outputLabel("","OUTPUT"),
	nameLabel("","THE AUTOMIZER by Daniel Oluyomi"),
	voice1Label("","VOICE 1"),
	voice2Label("","VOICE 2"),
	pitchLabel (String::empty),
	chordLabel(String::empty),
	transposePitchLabel(String::empty),
	centsLabel(String::empty), 
	MIDINoteLabel(String::empty),
	harm1Label(String::empty),
	harm2Label(String::empty),
	groupComponent (0),
	groupComponent2 (0),
	groupComponent3 (0),
	groupComponent4 (0)
{
   // Set up the sliders
    addAndMakeVisible (&outputGainSlider);
    outputGainSlider.setSliderStyle (Slider::LinearHorizontal);
	outputGainSlider.setColour (Slider::thumbColourId, Colours::white);

    outputGainSlider.addListener (this);
    outputGainSlider.setRange (-30, 6, 0.1);

    addAndMakeVisible (&voice1GainSlider);
    voice1GainSlider.setSliderStyle (Slider::Rotary);
    voice1GainSlider.addListener (this);
    voice1GainSlider.setRange (-30, 6, 0.1);
    voice1GainSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    voice1GainSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));
    
	addAndMakeVisible (&voice2GainSlider);
    voice2GainSlider.setSliderStyle (Slider::Rotary);
    voice2GainSlider.addListener (this);
    voice2GainSlider.setRange (-30, 6, 0.1);
    voice2GainSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    voice2GainSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));
    
    addAndMakeVisible (&transposeSlider);
    transposeSlider.setSliderStyle (Slider::Rotary);
    transposeSlider.addListener (this);
    transposeSlider.setRange (-12, 12, 0.1);
    transposeSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    transposeSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));
    
	addAndMakeVisible (&pan1Slider);
    pan1Slider.setSliderStyle (Slider::Rotary);
    pan1Slider.addListener (this);
    pan1Slider.setRange (0, 100, 1);
    pan1Slider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    pan1Slider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));
   
	addAndMakeVisible (&pan2Slider);
    pan2Slider.setSliderStyle (Slider::Rotary);
    pan2Slider.addListener (this);
    pan2Slider.setRange (0, 100, 1);
    pan2Slider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    pan2Slider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));
   
	addAndMakeVisible (&attackSlider);
    attackSlider.setSliderStyle (Slider::Rotary);
    attackSlider.addListener (this);
    attackSlider.setRange (0, 100, 1);
    attackSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    attackSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));

	addAndMakeVisible (&refSlider);
    refSlider.setSliderStyle (Slider::Rotary);
    refSlider.addListener (this);
    refSlider.setRange (430, 450, 1);
    refSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    refSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));

    addAndMakeVisible (&rollOnSlider);
	rollOnSlider.setSliderStyle (Slider::Rotary);
    rollOnSlider.addListener (this);
    rollOnSlider.setRange (0, 5, 1);
    rollOnSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    rollOnSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));

    addAndMakeVisible (&whitenSlider);
    whitenSlider.setSliderStyle (Slider::Rotary);
    whitenSlider.addListener (this);
    whitenSlider.setRange (0, 100, 1);
    whitenSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    whitenSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));

	addAndMakeVisible (&harm1AttackSlider);
    harm1AttackSlider.setSliderStyle (Slider::Rotary);
    harm1AttackSlider.addListener (this);
    harm1AttackSlider.setRange (0, 100, 1);
    harm1AttackSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    harm1AttackSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));

	addAndMakeVisible (&harm2AttackSlider);
    harm2AttackSlider.setSliderStyle (Slider::Rotary);
    harm2AttackSlider.addListener (this);
    harm2AttackSlider.setRange (0, 100, 1);
    harm2AttackSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    harm2AttackSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));
	
    addAndMakeVisible (&vDepthSlider);
    vDepthSlider.setSliderStyle (Slider::Rotary);
    vDepthSlider.addListener (this);
    vDepthSlider.setRange (0, 0.05, 0.01);
    vDepthSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    vDepthSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));	

    addAndMakeVisible (&vRateSlider);
    vRateSlider.setSliderStyle (Slider::Rotary);
    vRateSlider.addListener (this);
    vRateSlider.setRange (0.1, 10, 0.1);
    vRateSlider.setColour (Slider::rotarySliderFillColourId, Colour (0xfbffffff));
    vRateSlider.setColour (Slider::rotarySliderOutlineColourId, Colour (0xff3c3c54));	

    addAndMakeVisible(&harmonyComboBox);
    harmonyComboBox.setEditableText(false);
	harmonyComboBox.setJustificationType(Justification::left);
	harmonyComboBox.addItem("High and Higher",HarmonizerEngine::HighAndHigher);
	harmonyComboBox.addItem("Low and Lower",HarmonizerEngine::LowAndLower);
	harmonyComboBox.addItem("Higher and Lower",HarmonizerEngine::HigherAndLower);
	harmonyComboBox.addItem("High and Low",HarmonizerEngine::HighAndLow);
    harmonyComboBox.addListener (this);

    addAndMakeVisible(&shiftTypeComboBox);
    shiftTypeComboBox.setEditableText(false);
    shiftTypeComboBox.setJustificationType(Justification::left);
	shiftTypeComboBox.addItem("On", HarmonizerEngine::lent);
    shiftTypeComboBox.addItem("Off",HarmonizerEngine::delay);
    shiftTypeComboBox.addListener(this);

    addAndMakeVisible(&shiftComboBox);
    shiftComboBox.setEditableText(false);
    shiftComboBox.setJustificationType(Justification::left);
	shiftComboBox.addItem("On", HarmonizerEngine::on);
    shiftComboBox.addItem("Off",HarmonizerEngine::off);
    shiftComboBox.addListener(this);

    addAndMakeVisible(&keyComboBox);
    keyComboBox.setEditableText(false);
    keyComboBox.setJustificationType(Justification::left);
	keyComboBox.addItem("C", HarmonizerEngine::C);
	keyComboBox.addItem("C#/Db",HarmonizerEngine::CSharp);
	keyComboBox.addItem("D", HarmonizerEngine::D);
	keyComboBox.addItem("D#/Eb", HarmonizerEngine::DSharp);
	keyComboBox.addItem("E", HarmonizerEngine::E);
	keyComboBox.addItem("F",HarmonizerEngine::F);
	keyComboBox.addItem("F#/Gb", HarmonizerEngine::FSharp);
	keyComboBox.addItem("G", HarmonizerEngine::G);
	keyComboBox.addItem("G#/Ab", HarmonizerEngine::GSharp);
	keyComboBox.addItem("A",HarmonizerEngine::A);
	keyComboBox.addItem("A#/Bb", HarmonizerEngine::ASharp);
	keyComboBox.addItem("B", HarmonizerEngine::B);
    keyComboBox.addListener(this);

	addAndMakeVisible(&scaleComboBox);
    scaleComboBox.setEditableText(false);
    scaleComboBox.setJustificationType(Justification::left);
	scaleComboBox.addItem("Major", HarmonizerEngine::Major);
    scaleComboBox.addItem("Harmonic Minor",HarmonizerEngine::Minor);
	scaleComboBox.addItem("Pentatonic", HarmonizerEngine::Pentatonic);
	scaleComboBox.addItem("Chromatic",HarmonizerEngine::Chromatic);
    scaleComboBox.addListener(this);

    addAndMakeVisible(&autotuneComboBox);
    autotuneComboBox.setEditableText(false);
    autotuneComboBox.setJustificationType(Justification::left);
	autotuneComboBox.addItem("On", HarmonizerEngine::on);
    autotuneComboBox.addItem("Off",HarmonizerEngine::off);
    autotuneComboBox.addListener(this);

    addAndMakeVisible(&tuneComboBox);
    tuneComboBox.setEditableText(false);
    tuneComboBox.setJustificationType(Justification::left);
	tuneComboBox.addItem("Automatic", HarmonizerEngine::on);
    tuneComboBox.addItem("Fixed",HarmonizerEngine::off);
    tuneComboBox.addListener(this);

	addAndMakeVisible (groupComponent = new GroupComponent (String::empty,
                                                            String::empty));
	groupComponent->setColour (GroupComponent::outlineColourId, Colours::dimgrey);
    groupComponent->setColour (GroupComponent::textColourId, Colours::white);

	addAndMakeVisible (groupComponent2 = new GroupComponent (String::empty,
                                                            String::empty));
    groupComponent2->setColour (GroupComponent::outlineColourId, Colours::dimgrey);
    groupComponent2->setColour (GroupComponent::textColourId, Colours::white);

	addAndMakeVisible (groupComponent3 = new GroupComponent (String::empty,
                                                            String::empty));
    groupComponent3->setColour (GroupComponent::outlineColourId, Colours::dimgrey);
    groupComponent3->setColour (GroupComponent::textColourId, Colours::white);

	addAndMakeVisible (groupComponent4 = new GroupComponent (String::empty,
                                                            String::empty));
    groupComponent4->setColour (GroupComponent::outlineColourId, Colours::dimgrey);
    groupComponent4->setColour (GroupComponent::textColourId, Colours::white);


    outputGainLabel.attachToComponent(&outputGainSlider, false);
    outputGainLabel.setFont(Font (16.0f));
	outputGainLabel.setColour(Label::textColourId, Colours::white);
    
    voice1GainLabel.attachToComponent(&voice1GainSlider, false);
    voice1GainLabel.setFont(Font (16.0f));
	voice1GainLabel.setColour(Label::textColourId, Colours::white);

    voice2GainLabel.attachToComponent(&voice2GainSlider, false);
    voice2GainLabel.setFont(Font (16.0f));
	voice2GainLabel.setColour(Label::textColourId, Colours::white);

    transposeLabel.attachToComponent(&transposeSlider, false);
    transposeLabel.setFont(Font (16.0f));
	transposeLabel.setColour(Label::textColourId, Colours::white);
    
    pan1Label.attachToComponent(&pan1Slider, false);
    pan1Label.setFont(Font (16.0f));
	pan1Label.setColour(Label::textColourId, Colours::white);
	
	pan2Label.attachToComponent(&pan2Slider, false);
    pan2Label.setFont(Font (16.0f));
	pan2Label.setColour(Label::textColourId, Colours::white);

	keyLabel.attachToComponent(&keyComboBox, false);
    keyLabel.setFont(Font (16.0f));
	keyLabel.setColour(Label::textColourId, Colours::white);

	scaleLabel.attachToComponent(&scaleComboBox, false);
    scaleLabel.setFont(Font (16.0f));
	scaleLabel.setColour(Label::textColourId, Colours::white);

	attackLabel.attachToComponent(&attackSlider, false);
    attackLabel.setFont(Font (16.0f));
	attackLabel.setColour(Label::textColourId, Colours::white);

	refLabel.attachToComponent(&refSlider, false);
    refLabel.setFont(Font (16.0f));
	refLabel.setColour(Label::textColourId, Colours::white);

	rollOnLabel.attachToComponent(&rollOnSlider, false);
    rollOnLabel.setFont(Font (16.0f));
	rollOnLabel.setColour(Label::textColourId, Colours::white);

	whitenLabel.attachToComponent(&whitenSlider, false);
    whitenLabel.setFont(Font (16.0f));
	whitenLabel.setColour(Label::textColourId, Colours::white);

	harm1AttackLabel.attachToComponent(&harm1AttackSlider, false);
    harm1AttackLabel.setFont(Font (16.0f));
	harm1AttackLabel.setColour(Label::textColourId, Colours::white);

	harm2AttackLabel.attachToComponent(&harm2AttackSlider, false);
    harm2AttackLabel.setFont(Font (16.0f));
	harm2AttackLabel.setColour(Label::textColourId, Colours::white);

	vDepthLabel.attachToComponent(&vDepthSlider, false);
    vDepthLabel.setFont(Font (16.0f));
	vDepthLabel.setColour(Label::textColourId, Colours::white);

	vRateLabel.attachToComponent(&vRateSlider, false);
    vRateLabel.setFont(Font (16.0f));
	vRateLabel.setColour(Label::textColourId, Colours::white);

	harmonyLabel.attachToComponent(&harmonyComboBox, false);
    harmonyLabel.setFont(Font (16.0f));
	harmonyLabel.setColour(Label::textColourId, Colours::white);
	
	shiftTypeLabel.attachToComponent(&shiftTypeComboBox, false);
    shiftTypeLabel.setFont(Font (16.0f));
	shiftTypeLabel.setColour(Label::textColourId, Colours::white);

	shiftLabel.attachToComponent(&shiftComboBox, false);
    shiftLabel.setFont(Font (16.0f));
	shiftLabel.setColour(Label::textColourId, Colours::white);

	autotuneLabel.attachToComponent(&autotuneComboBox, false);
	autotuneLabel.setColour (Label::textColourId, Colours::white);
    autotuneLabel.setFont(Font (16.0f));

	tuneLabel.attachToComponent(&tuneComboBox, false);
    tuneLabel.setFont(Font (16.0f));
	tuneLabel.setColour(Label::textColourId, Colours::white);

    addAndMakeVisible (&inputLabel);
	inputLabel.setFont(Font (24.0f));
    inputLabel.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&input2Label);
	input2Label.setFont(Font (24.0f));
    input2Label.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&voice1Label);
	voice1Label.setFont(Font (18.0f));
    voice2Label.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&voice2Label);
	voice2Label.setFont(Font (18.0f));
    voice2Label.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&outputLabel);
	outputLabel.setFont(Font (24.0f));
    outputLabel.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&systemLabel);
	systemLabel.setFont(Font (24.0f));
    systemLabel.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&nameLabel);
	nameLabel.setFont(Font (16.0f));
    nameLabel.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&pitchLabel);
  	pitchLabel.setFont(Font (16.0f));
    pitchLabel.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&chordLabel);
  	chordLabel.setFont(Font (18.0f));
	chordLabel.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&harm1Label);
  	harm1Label.setFont(Font (18.0f));
    harm1Label.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&harm2Label);
  	harm2Label.setFont(Font (18.0f));
    harm2Label.setColour (Label::textColourId, Colours::white);
	
    addAndMakeVisible (&transposePitchLabel);
  	transposePitchLabel.setFont(Font (16.0f));
    transposePitchLabel.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&centsLabel);
  	centsLabel.setFont(Font (16.0f));
    centsLabel.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&MIDINoteLabel);
  	MIDINoteLabel.setFont(Font (18.0f));
    MIDINoteLabel.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&voice1Label);
  	voice1Label.setFont(Font (20.0f));
    voice1Label.setColour (Label::textColourId, Colours::white);

    addAndMakeVisible (&voice2Label);
  	voice2Label.setFont(Font (20.0f));
    voice2Label.setColour (Label::textColourId, Colours::white);


    // add the triangular resizer component for the bottom-right of the UI
    addAndMakeVisible(resizer = new ResizableCornerComponent (this, &resizeLimits));
    resizeLimits.setSizeLimits(800, 600, 800, 600);

	// set our component's initial size to be the last one that was stored in the filter's settings
    setSize(ownerFilter->lastUIWidth,
            ownerFilter->lastUIHeight);
    
    startTimer(100);
}

AutomizerAudioProcessorEditor::~AutomizerAudioProcessorEditor()
{
	deleteAndZero (groupComponent);
	deleteAndZero (groupComponent2);
	deleteAndZero (groupComponent3);
	deleteAndZero (groupComponent4);
}

//==============================================================================
void AutomizerAudioProcessorEditor::paint (Graphics& g)
{
   g.fillAll (Colour (0xff05122b));
	
}
void AutomizerAudioProcessorEditor::resized()
{
    //GUI Layout			
	inputLabel.setBounds (25, 560, 150,25);//x,y,width,height
	input2Label.setBounds (225,560, 150,25);
	systemLabel.setBounds (455,560, 150,25);
	outputLabel.setBounds (655,560, 150,25);
	nameLabel.setBounds (300,5,600,25);

	autotuneComboBox.setBounds(30,65,150,30);
    transposeSlider.setBounds (30, 125, 150, 40);
	refSlider.setBounds(30,185,150,40);
	keyComboBox.setBounds(30,245,150, 30);
	scaleComboBox.setBounds(30,315,150,30);
	attackSlider.setBounds(30,385,150,40);
	MIDINoteLabel.setBounds (25, 445, 400, 40);
	centsLabel.setBounds (25, 505, 150, 40);

	rollOnSlider.setBounds(230,65,150,40);
	whitenSlider.setBounds(230,145,150,40);
	tuneComboBox.setBounds(230,225,150,30);
	chordLabel.setBounds (215, 285, 200, 40);

	shiftComboBox.setBounds(430, 65, 150, 30);
	harmonyComboBox.setBounds(430,135,150,30);
	shiftTypeComboBox.setBounds(430, 225, 150, 30);
	vDepthSlider.setBounds(430,295,150,40); 
	vRateSlider.setBounds(430,375,150,40);

	
	harm1AttackSlider.setBounds(630,65,150,40);
    voice1GainSlider.setBounds(630, 125, 150, 40);
	pan1Slider.setBounds(630, 185, 150, 40);
	harm1Label.setBounds(630,225,150,25);	

	harm2AttackSlider.setBounds(630,285,150,40);    	
    voice2GainSlider.setBounds(630, 345, 150, 40);
	pan2Slider.setBounds(630, 405, 150, 40);  
	harm2Label.setBounds(630,445,400,25);
	outputGainSlider.setBounds (630, 500, 150, 40);	
	groupComponent->setBounds (0, 30, 199, 530);
	groupComponent2->setBounds (200, 30, 199, 530);
	groupComponent3->setBounds (400, 30, 199, 530);
	groupComponent4->setBounds (600, 30, 199, 530);

    resizer->setBounds(getWidth() - 16, getHeight() - 16, 16, 16);
    
    getProcessor()->lastUIWidth = getWidth();
    getProcessor()->lastUIHeight = getHeight();
}
// This timer periodically checks whether any of the filter's parameters have changed...
void AutomizerAudioProcessorEditor::timerCallback()
{
    AutomizerAudioProcessor* ourProcessor = getProcessor();
    
    outputGainSlider.setValue(ourProcessor->engine->outputGaindB, dontSendNotification);
    voice1GainSlider.setValue(ourProcessor->engine->voice1GaindB, dontSendNotification);
    voice2GainSlider.setValue(ourProcessor->engine->voice2GaindB, dontSendNotification);
    transposeSlider.setValue(ourProcessor->engine->semitoneTranspose, dontSendNotification);
    pan1Slider.setValue(ourProcessor->engine->panLPercent, dontSendNotification);
   	pan2Slider.setValue(ourProcessor->engine->panRPercent, dontSendNotification);
	keyComboBox.setSelectedId(ourProcessor->engine->keyGUI, dontSendNotification);
	scaleComboBox.setSelectedId(ourProcessor->engine->scaleGUI, dontSendNotification);
	attackSlider.setValue(ourProcessor->engine->attackTimeMilliseconds, dontSendNotification);
	refSlider.setValue(ourProcessor->engine->referenceHzPar, dontSendNotification);
	rollOnSlider.setValue(ourProcessor->engine->rollOnPar, dontSendNotification);
	whitenSlider.setValue(ourProcessor->engine->whiteningPercent, dontSendNotification);
	harmonyComboBox.setSelectedId(ourProcessor->engine->harmonyPar, dontSendNotification);
	
	shiftTypeComboBox.setSelectedId(ourProcessor->engine->shiftTypePar,dontSendNotification);
	shiftComboBox.setSelectedId(ourProcessor->engine->shiftPar,dontSendNotification);
	autotuneComboBox.setSelectedId(ourProcessor->engine->autotunePar,dontSendNotification);
	tuneComboBox.setSelectedId(ourProcessor->engine->tunePar,dontSendNotification);
	harm1AttackSlider.setValue(ourProcessor->engine->harm1AttackTimeMilliseconds, dontSendNotification);
	harm2AttackSlider.setValue(ourProcessor->engine->harm2AttackTimeMilliseconds, dontSendNotification);
	vRateSlider.setValue(ourProcessor->engine->vRate, dontSendNotification);
	vDepthSlider.setValue(ourProcessor->engine->vDepth, dontSendNotification);
	
	
	displayPitchInfo();
	displayChordInfo();

}

// This is our Slider::Listener callback, when the user drags a slider.
void AutomizerAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    // It's vital to use setParameterNotifyingHost to change any parameters that are automatable
    // by the host, rather than just modifying them directly, otherwise the host won't know
    // that they've changed.
    
    if (slider == &outputGainSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kOutputGain,
                                                   (float)outputGainSlider.getValue());
    }

    else if (slider == &voice1GainSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kVoice1Gain,
                                                   (float)voice1GainSlider.getValue());
    }
    else if (slider == &voice2GainSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kVoice2Gain,
                                                   (float)voice2GainSlider.getValue());
    }
    else if (slider == &transposeSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kTranspose,
                                                   (float)transposeSlider.getValue());
    }
    else if (slider == &pan1Slider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kPan1,
                                                   (float)pan1Slider.getValue());
    }

	 else if (slider == &pan2Slider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kPan2,
	                                              (float)pan2Slider.getValue());
	}
	 else if (slider == &attackSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kAttack,
	                                              (float)attackSlider.getValue());
	}
	 else if (slider == &refSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kRef,
	                                              (float)refSlider.getValue());
	}

	 else if (slider == &rollOnSlider)
    {
		getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kRollOn,
	                                              (float)rollOnSlider.getValue());
	}
	 else if (slider == &whitenSlider)
    {
		getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kWhite,
	                                              (float)whitenSlider.getValue());
	}


	 else if (slider == &harm1AttackSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kHarm1Attack,
	                                              (float)harm1AttackSlider.getValue());
	}
	 else if (slider == &harm2AttackSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kHarm2Attack,
	                                              (float)harm2AttackSlider.getValue());
	}
	 else if (slider == &vDepthSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kVibratoDepth,
	                                              (float)vDepthSlider.getValue());
	}
	 else if (slider == &vRateSlider)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kVibratoRate,
	                                              (float)vRateSlider.getValue());
	}
}

void AutomizerAudioProcessorEditor::comboBoxChanged (ComboBox *comboBox)
{
    if(comboBox == &shiftTypeComboBox)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kPitchShiftType,
                                                   (float)shiftTypeComboBox.getSelectedId());
   

        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kTranspose,
                                                   (float)transposeSlider.getValue());	
	
	}
	

	else if (comboBox == &harmonyComboBox)
    {
		getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kHarmony,
	                                              (float)harmonyComboBox.getSelectedId());
	}

	else if(comboBox == &autotuneComboBox)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kAutotune,
                                                   (float)autotuneComboBox.getSelectedId());
    }
	else if(comboBox == &autotuneComboBox)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kAutotune,
                                                   (float)autotuneComboBox.getSelectedId());
    }

	else if(comboBox == &tuneComboBox)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kTune,
                                                   (float)tuneComboBox.getSelectedId());
    }

	 else if (comboBox == &keyComboBox)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kKey,
	                                              (float)keyComboBox.getSelectedId());
	}

	 else if (comboBox == &scaleComboBox)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kScale,
	                                              (float)scaleComboBox.getSelectedId());
	}

	 else if (comboBox == &shiftComboBox)
    {
        getProcessor()->setParameterNotifyingHost (AutomizerAudioProcessor::kShift,
	                                              (float)shiftComboBox.getSelectedId());
	}
}

void AutomizerAudioProcessorEditor::displayPitchInfo()
{
	 String displayPitchText;
	 String displayTransposeText;
	 String displayMIDIText;
	 String displayCentsText;
	 String displayHarm1Text;
	 String displayHarm2Text;
     displayPitchText.preallocateBytes (128);
	 displayTransposeText.preallocateBytes(128);
	 displayMIDIText.preallocateBytes(128);
	 displayCentsText.preallocateBytes(128);
	 displayHarm1Text.preallocateBytes(128);
	 displayHarm2Text.preallocateBytes(128);

	 AutomizerAudioProcessor* ourProcessor = getProcessor();
	 displayPitchText << "Frequency Estimate: " << ourProcessor->engine->detectedPitch << " Hz";
	 displayTransposeText << "Transpose: " << ourProcessor->engine->transposePitch << " Hz";
	 displayMIDIText << "Note Estimate: " << ourProcessor->engine->pitchValue;
	 displayHarm1Text << "Voice 1 Note: " << ourProcessor->engine->harm1Value;
	 displayHarm2Text << "Voice 2 Note: " << ourProcessor->engine->harm2Value;
	 displayCentsText << "Offset: " << ourProcessor->engine->cents << " cents";
	 
	 pitchLabel.setText (displayPitchText, dontSendNotification);
	 transposePitchLabel.setText (displayTransposeText, dontSendNotification);
	 centsLabel.setText(displayCentsText, dontSendNotification);
	 MIDINoteLabel.setText(displayMIDIText,dontSendNotification);
	 harm1Label.setText(displayHarm1Text, dontSendNotification);
	 harm2Label.setText(displayHarm2Text, dontSendNotification);

}
void AutomizerAudioProcessorEditor::displayChordInfo()
{
	 String displayChordText;
     displayChordText.preallocateBytes (128);
	 AutomizerAudioProcessor* ourProcessor = getProcessor();
	 displayChordText << "Chord Estimate: " << ourProcessor->engine->chordStr;
	 chordLabel.setText (displayChordText, dontSendNotification);
}