/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 4.2.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "MediaPlayer.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MediaPlayer::MediaPlayer ()
{
    //[Constructor_pre] You can add your own custom stuff here..



    //[/Constructor_pre]

    addAndMakeVisible (openButton = new TextButton ("open"));
    openButton->setButtonText (TRANS("Open..."));
    openButton->addListener (this);

    addAndMakeVisible (playButton = new TextButton ("play"));
    playButton->setButtonText (TRANS("Play"));
    playButton->addListener (this);
    playButton->setColour (TextButton::buttonColourId, Colours::green);

    addAndMakeVisible (stopButton = new TextButton ("stop"));
    stopButton->setButtonText (TRANS("Stop"));
    stopButton->addListener (this);
    stopButton->setColour (TextButton::buttonColourId, Colours::red);

    addAndMakeVisible (settingsButton = new TextButton ("settings"));
    settingsButton->setButtonText (TRANS("Audio Settings..."));
    settingsButton->addListener (this);
    settingsButton->setColour (TextButton::buttonColourId, Colours::yellow);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..

    // * All'avvio disabilita i pulsanti start e stop, fino a quando non viene caricato un file
    playButton->setEnabled(false);
    stopButton->setEnabled(false);

    // * Inizializza il formatManager con i formati audio base
    formatManager.registerBasicFormats();

    sourcePlayer.setSource(&transportSource);

    deviceManager.addAudioCallback(&sourcePlayer);

    deviceManager.initialise(0,2, nullptr, true);

    deviceManager.addChangeListener(this);
    transportSource.addChangeListener(this);

    state = Stopped;
    //[/Constructor]
}

MediaPlayer::~MediaPlayer()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    openButton = nullptr;
    playButton = nullptr;
    stopButton = nullptr;
    settingsButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void MediaPlayer::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void MediaPlayer::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    openButton->setBounds (20, 8, getWidth() - 40, 24);
    playButton->setBounds (20, 40, getWidth() - 40, 24);
    stopButton->setBounds (20, 72, getWidth() - 40, 24);
    settingsButton->setBounds (20, 104, getWidth() - 40, 24);
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void MediaPlayer::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == openButton)
    {
        //[UserButtonCode_openButton] -- add your button handler code here..
        FileChooser chooser ("Select a wave file to play...",
                                File::nonexistent,
                                "*.wav"
                             );
        if (chooser.browseForFileToOpen()) {
            File file (chooser.getResult());

            readerSource = new AudioFormatReaderSource(formatManager.createReaderFor(file), true);

            transportSource.setSource(readerSource);

            playButton->setEnabled(true);
        }
        //[/UserButtonCode_openButton]
    }
    else if (buttonThatWasClicked == playButton)
    {
        //[UserButtonCode_playButton] -- add your button handler code here..
        if ((Stopped == state) || (Paused == state)) {
            changeState(Starting);
        } else if (Playing == state) {
            changeState(Pausing);
        }
        //[/UserButtonCode_playButton]
    }
    else if (buttonThatWasClicked == stopButton)
    {
        //[UserButtonCode_stopButton] -- add your button handler code here..
        if (Paused == state) {
            changeState(Stopped);
        } else {
            changeState(Stopping);
        }
        //[/UserButtonCode_stopButton]
    }
    else if (buttonThatWasClicked == settingsButton)
    {
        //[UserButtonCode_settingsButton] -- add your button handler code here..
        bool showMidiInputOption = false;
        bool showMidiOutputOption = false;
        bool showChannelAsStereoPairs = true;
        bool hideAdvancedOption = false;

        AudioDeviceSelectorComponent settings (deviceManager,
                                                0,
                                                0,
                                                1,
                                                2,
                                                showMidiInputOption,
                                                showMidiOutputOption,
                                                showChannelAsStereoPairs,
                                                hideAdvancedOption);
        settings.setSize(500,400);
        DialogWindow::showModalDialog(String ("Audio Settings"),
                                        &settings,
                                        TopLevelWindow::getTopLevelWindow(0),
                                        Colours::white,
                                        true
                                        );
        //[/UserButtonCode_settingsButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
void MediaPlayer::changeListenerCallback(ChangeBroadcaster* source) {
    if (&deviceManager == source) {
        AudioDeviceManager::AudioDeviceSetup setup;
        deviceManager.getAudioDeviceSetup(setup);

        if (setup.outputChannels.isZero()) {
            sourcePlayer.setSource(nullptr);
        } else {
            sourcePlayer.setSource(&transportSource);
        }

    } else if (&transportSource == source) {
        if (transportSource.isPlaying()) {
            changeState(Playing);
        } else {
            if ((Stopping == state) || (Playing == state))
                changeState(Stopped);
            else if (Pausing == state)
                changeState(Paused);
        }
    }

}

void MediaPlayer::changeState(TransportState newState) {
    if (state != newState) {
        state = newState;
        switch (state) {
            case Stopped:
                playButton->setButtonText("Play");
                stopButton->setButtonText("Stop");
                stopButton->setEnabled(false);
                transportSource.setPosition(0.0);
                break;
            case Starting:
                transportSource.start();
                break;
            case Playing:
                playButton->setButtonText("Pause");
                stopButton->setButtonText("Stop");
                stopButton->setEnabled(true);
                break;
            case Pausing:
                transportSource.stop();
                break;
            case Paused:
                playButton->setButtonText("Resume");
                stopButton->setButtonText("Return to Zero");
                break;
            case Stopping:
                transportSource.stop();
                break;
        }
    }
}
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MediaPlayer" componentName=""
                 parentClasses="public Component, public ChangeListener" constructorParams=""
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.330" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ffffffff"/>
  <TEXTBUTTON name="open" id="673c4579e26bef89" memberName="openButton" virtualName=""
              explicitFocusOrder="0" pos="20 8 40M 24" buttonText="Open..."
              connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="play" id="164efc8f147f5579" memberName="playButton" virtualName=""
              explicitFocusOrder="0" pos="20 40 40M 24" bgColOff="ff008000"
              buttonText="Play" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="stop" id="9d876e55fe71224e" memberName="stopButton" virtualName=""
              explicitFocusOrder="0" pos="20 72 40M 24" bgColOff="ffff0000"
              buttonText="Stop" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="settings" id="18625d795f9c8419" memberName="settingsButton"
              virtualName="" explicitFocusOrder="0" pos="20 104 40M 24" bgColOff="ffffff00"
              buttonText="Audio Settings..." connectedEdges="0" needsCallback="1"
              radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
