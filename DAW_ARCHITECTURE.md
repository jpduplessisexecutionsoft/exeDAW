# exeDAW - Digital Audio Workstation

A professional-grade DAW (Digital Audio Workstation) built in C++14 with Windows API GUI, featuring an audio player object system that can be used indefinitely across multiple channels with full CRUD operations and Cubase-style transport controls.

## Architecture Overview

### Core Components

#### 1. **AudioPlayer** (`AudioPlayer.h`/`.cpp`)
- Manages individual audio playback on a channel
- Can be instantiated unlimited times for different channels
- Features:
  - Play/Pause/Stop/Resume control
  - Volume control (0.0 - 1.0)
  - Playback position tracking
  - Audio data access
  - Multiple playback states (STOPPED, PLAYING, PAUSED, RECORDING)

```cpp
auto player = std::make_shared<AudioPlayer>();
player->LoadAudioFile("audio.wav");
player->Play();
float volume = 0.8f;
player->SetVolume(volume);
```

#### 2. **SequencerChannel** (`SequencerChannel.h`/`.cpp`)
- Represents a single track/channel in the sequencer
- CRUD operations for audio player assignment:
  - **Create**: Automatically assigned when channel is created
  - **Read**: GetAudioPlayer()
  - **Update**: AssignAudioPlayer()
  - **Delete**: RemoveAudioPlayer()
- Channel properties:
  - Volume and Pan controls
  - Mute and Solo functionality
  - Channel type (AUDIO, MIDI, BUS)
  - Channel naming

```cpp
auto channel = std::make_shared<SequencerChannel>(1, "Guitar");
channel->AssignAudioPlayer(player);
channel->SetVolume(0.8f);
channel->SetPan(-0.5f);  // Pan left
channel->SetMuted(false);
```

#### 3. **SequencerEngine** (`SequencerEngine.h`/`.cpp`)
- CRUD operations for managing all channels
- Channel Management:
  - **CREATE**: CreateChannel()
  - **READ**: GetChannel(), GetChannelByName(), GetAllChannels()
  - **UPDATE**: UpdateChannel()
  - **DELETE**: DeleteChannel(), DeleteAllChannels()
- Audio player integration
- Channel indexing and tracking

```cpp
auto engine = std::make_shared<SequencerEngine>();

// CREATE
auto channel = engine->CreateChannel("Bass Track");

// READ
auto ch = engine->GetChannel(1);
auto allChannels = engine->GetAllChannels();

// UPDATE
engine->UpdateChannel(1, "New Bass Track");

// DELETE
engine->DeleteChannel(1);
```

#### 4. **TransportControl** (`TransportControl.h`/`.cpp`)
- Cubase-style transport controls
- Transport States: STOPPED, PLAYING, PAUSED, RECORDING
- Playback Control:
- Play(), Stop(), Pause(), Resume(), Record()
  - Playhead position management
- Tempo Control:
  - Set/Get BPM (120 BPM default)
- Time Signature:
  - 4/4 default, customizable
- Loop Control:
  - Enable/Disable looping
  - Set loop range
- Sample Rate management (44100 Hz default)
- Callback support for transport state changes

```cpp
auto transport = std::make_shared<TransportControl>();
transport->SetTempo(140.0f);
transport->SetTimeSignature(4, 4);
transport->SetLoopEnabled(true);
transport->Play();
```

#### 5. **WaveformVisualizer** (`WaveformVisualizer.h`/`.cpp`)
- Audio waveform visualization and analysis
- Waveform generation for display
- Audio statistics:
  - Peak amplitude detection
  - RMS (Root Mean Square) calculation
- Display modes:
  - Mono/Stereo visualization
  - Zoom levels
  - Point-based rendering for scalable displays

```cpp
auto visualizer = std::make_shared<WaveformVisualizer>();
visualizer->SetAudioData(audioData);
auto points = visualizer->GenerateWaveformPoints(800, 300);  // 800x300 pixels
float peak = visualizer->GetPeakAmplitude();
visualizer->SetZoomLevel(2.0f);  // 2x zoom
```

#### 6. **DAWApplication** (`DAWApplication.h`/`.cpp`)
- Main coordinator for the audio workstation
- Integrates all components:
- SequencerEngine
  - TransportControl
  - WaveformVisualizer
- Convenience methods:
  - AddTrack()
  - LoadAudioFile()
  - PlayAll(), StopAll(), PauseAll()
  - GetTrackCount()

```cpp
auto daw = std::make_shared<DAWApplication>();
daw->Initialize();

auto track = daw->AddTrack("Lead Synth");
daw->LoadAudioFile(track->GetChannelId(), "synth.wav");
daw->PlayAll();
```

#### 7. **DAWWindow** (`DAWWindow.h`/`.cpp`)
- Windows API-based GUI
- UI Features:
  - Transport controls (Play, Pause, Stop, Record buttons)
  - Tempo display and adjustment
  - Time signature settings
  - Loop control checkbox
  - Channel/track list with status indicators
  - Add/Delete track functionality
  - Load audio file functionality
  - Real-time waveform display
  - Mute (M) and Solo (S) indicators

## Workflow: CRUD Operations Example

### Create New Channel
```cpp
auto sequencer = daw->GetSequencer();
auto channel = sequencer->CreateChannel("Vocal Track", SequencerChannel::AUDIO);
uint32_t channelId = channel->GetChannelId();
```

### Read Channel Information
```cpp
auto channel = sequencer->GetChannel(1);
std::string name = channel->GetChannelName();
bool hasAudio = channel->HasAudioPlayer();
float volume = channel->GetVolume();
```

### Update Channel
```cpp
sequencer->UpdateChannel(1, "Updated Vocal Track");
channel->SetVolume(0.9f);
channel->SetPan(0.3f);  // Slightly to the right
```

### Delete Channel
```cpp
sequencer->DeleteChannel(1);
```

### Assign Audio Player (CRUD on AudioPlayer)
```cpp
// Create and assign
auto player = std::make_shared<AudioPlayer>();
player->LoadAudioFile("vocal.wav");
channel->AssignAudioPlayer(player);

// Read
auto currentPlayer = channel->GetAudioPlayer();

// Update
auto newPlayer = std::make_shared<AudioPlayer>();
newPlayer->LoadAudioFile("new_vocal.wav");
channel->AssignAudioPlayer(newPlayer);

// Delete
channel->RemoveAudioPlayer();
```

## GUI Features

### Transport Controls (Bottom Section)
- **? Play**: Start playback of all channels
- **? Pause**: Pause playback
- **? Stop**: Stop playback and reset playhead
- **? Record**: Enable recording mode
- **Loop**: Enable/disable loop playback
- **Tempo (BPM)**: Adjust playback tempo (editable field)

### Sequencer Channel List
Shows all channels with status indicators:
- Channel ID and Name
- `[LOADED]` - Audio file is loaded
- `[M]` - Channel is muted
- `[S]` - Channel is soloed

### Waveform Display Area
- Top section showing audio waveform visualization
- Grid overlay for reference
- Real-time waveform rendering of selected channel
- Min/Max amplitude display

### Channel Management Buttons
- **+ Add Track**: Create a new sequencer channel
- **Load Audio**: Load audio file to selected channel
- **Delete Track**: Remove selected channel

## Build and Compile

The project uses C++14 and requires:
- Visual Studio 2015 or later
- Windows 7 or later
- Common Controls library (comctl32.lib)

### Project Files
- `AudioPlayer.h/cpp`
- `SequencerChannel.h/cpp`
- `SequencerEngine.h/cpp`
- `TransportControl.h/cpp`
- `WaveformVisualizer.h/cpp`
- `DAWApplication.h/cpp`
- `DAWWindow.h/cpp`
- `exeDAW.cpp` (main entry point)

All files compile successfully and are ready for use.

## Example Usage

```cpp
// Initialize DAW
auto daw = std::make_shared<DAWApplication>();
daw->Initialize();

// Create tracks
auto track1 = daw->AddTrack("Drums");
auto track2 = daw->AddTrack("Bass");
auto track3 = daw->AddTrack("Guitar");

// Load audio
uint32_t drumId = track1->GetChannelId();
uint32_t bassId = track2->GetChannelId();
uint32_t guitId = track3->GetChannelId();

daw->LoadAudioFile(drumId, "drums.wav");
daw->LoadAudioFile(bassId, "bass.wav");
daw->LoadAudioFile(guitId, "guitar.wav");

// Set properties
track1->SetVolume(0.8f);
track2->SetVolume(0.7f);
track3->SetVolume(0.9f);

// Configure transport
daw->GetTransport()->SetTempo(128.0f);
daw->GetTransport()->SetLoopEnabled(true);

// Playback
daw->PlayAll();
```

## Future Enhancements

1. **Audio Formats**: Support for WAV, MP3, FLAC, OGG
2. **Effects**: Equalizer, Reverb, Compression, Delay
3. **MIDI Support**: Full MIDI input/output capability
4. **Audio Engine**: Real-time audio processing with PortAudio or ASIO
5. **Automation**: Track parameter automation
6. **Markers**: Cue points and sections
7. **Undo/Redo**: Full history system
8. **File Operations**: Save/Load project files
9. **Plugins**: VST/AU plugin support
10. **Advanced Visualization**: Spectrum analyzer, oscilloscope

## Notes

- The audio loading in AudioPlayer is currently a simplified sine wave generator for demonstration
- In production, integrate with audio libraries like JUCE, PortAudio, or FMOD for real file loading
- The GUI is scalable and can be extended with more sophisticated visualization
- All classes use smart pointers (std::shared_ptr) for memory safety
- Thread-safe operations can be added for real-time audio processing
