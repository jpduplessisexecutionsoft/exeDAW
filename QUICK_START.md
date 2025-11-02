# exeDAW Quick Start Guide

## Getting Started

### Launching the Application
1. Build the solution (Release or Debug configuration)
2. Run `exeDAW.exe`
3. The DAW window will open with transport controls and an empty channel list

### Basic Workflow

#### Step 1: Create Tracks
1. Click **"+ Add Track"** button
2. A new track will appear in the channel list (e.g., "Track 1")
3. Repeat to create multiple tracks

#### Step 2: Load Audio Files
1. Select a track from the channel list by clicking on it
2. Click **"Load Audio"** button
3. Audio will be loaded to that channel (marked with `[LOADED]` indicator)

#### Step 3: Adjust Track Settings
- **Volume**: Use the track properties (not visible in UI yet - future enhancement)
- **Mute/Solo**: Can be set programmatically
- **Pan**: Can be set programmatically

#### Step 4: Configure Transport Settings
- **Tempo (BPM)**: Edit the tempo field (default: 120)
- **Loop**: Check the "Loop" checkbox to enable loop playback
- **Time Signature**: 4/4 (can be changed programmatically)

#### Step 5: Playback Control
- Click **? Play** to start playback
- Click **? Pause** to pause
- Click **? Stop** to stop and reset
- Click **? Record** to enable recording mode

#### Step 6: Waveform Visualization
- Select a track from the channel list
- The waveform will display in the top area
- The grid helps with time reference

## Programmatic Usage

### Creating and Managing Channels

```cpp
// Get the sequencer
auto sequencer = daw->GetSequencer();

// Create a new channel
auto channel = sequencer->CreateChannel("My Track", SequencerChannel::AUDIO);

// Get channel ID
uint32_t channelId = channel->GetChannelId();

// Load audio file
sequencer->LoadAudioToChannel(channelId, "path/to/audio.wav");

// Set channel properties
channel->SetVolume(0.8f);
channel->SetPan(0.0f);  // Center
channel->SetMuted(false);
channel->SetSolo(false);
```

### Transport Control

```cpp
auto transport = daw->GetTransport();

// Control playback
transport->Play();
transport->Pause();
transport->Stop();
transport->Record();

// Check state
if (transport->IsPlaying())
{
    // Do something
}

// Adjust tempo
transport->SetTempo(140.0f);

// Configure loop
transport->SetLoopEnabled(true);
transport->SetLoopRange(0, 44100 * 10);  // 10 seconds at 44.1kHz
```

### Audio Player Control

```cpp
auto player = channel->GetAudioPlayer();

// Playback control
player->Play();
player->Pause();
player->Stop();

// Position
player->SetPosition(0);  // Go to start
uint32_t currentPos = player->GetPosition();

// Volume
player->SetVolume(0.9f);

// Duration
uint32_t durationSamples = player->GetDuration();

// Get audio data
const auto& audioData = player->GetAudioData();
```

### Waveform Visualization

```cpp
auto visualizer = daw->GetVisualizer();

// Set audio to visualize
visualizer->SetAudioData(player->GetAudioData());

// Generate waveform points
auto points = visualizer->GenerateWaveformPoints(800, 300);

// Audio analysis
float peak = visualizer->GetPeakAmplitude();
float rms = visualizer->GetRMSAmplitude();

// Zoom
visualizer->SetZoomLevel(2.0f);

// Display mode
visualizer->SetDisplayMode(true);  // Stereo
```

## Keyboard Shortcuts (GUI)
(To be implemented in future versions)

| Shortcut | Action |
|----------|--------|
| `Space` | Play/Pause |
| `Enter` | Record |
| `Delete` | Delete selected track |
| `Ctrl+T` | Add new track |
| `Ctrl+O` | Load audio file |
| `Ctrl+Z` | Undo |

## Troubleshooting

### Application Won't Start
- Ensure Visual Studio runtime libraries are installed
- Check that comctl32.dll is available on the system

### No Waveform Appears
- Make sure you've loaded audio to a channel
- Select the channel in the channel list
- The waveform display will refresh

### Playback Issues
- Verify audio file path is correct
- Ensure the audio file format is supported
- Check that the channel has an audio player assigned

### Channel Not Appearing
- Click "Add Track" and verify in the list
- Check that the track count increases (GetTrackCount())

## Tips & Tricks

1. **Multiple Tracks**: Create and manage multiple independent tracks simultaneously
2. **Volume Mixing**: Each track has independent volume control
3. **Panning**: Pan tracks left/right for stereo placement
4. **Solo**: Solo individual tracks for focused editing
5. **Mute**: Mute tracks without deleting them
6. **Loop Playback**: Enable loop to repeat a section indefinitely
7. **Real-time Visualization**: Waveform updates as you switch channels

## Performance Notes

- The application can handle dozens of channels efficiently
- Waveform visualization is optimized for real-time display
- Audio players can be created and destroyed dynamically
- Memory is automatically managed with smart pointers

## Next Steps

1. Experiment with multiple tracks and audio files
2. Try different tempo and time signature settings
3. Use the visualizer to understand audio content
4. Integrate real audio file support (WAV, MP3, FLAC)
5. Add effects processing to tracks
6. Implement audio output to speakers
7. Create project save/load functionality

## Support & Documentation

- See `DAW_ARCHITECTURE.md` for detailed architecture documentation
- Check individual header files for complete API documentation
- Review source files for implementation details

Enjoy creating music with exeDAW!
