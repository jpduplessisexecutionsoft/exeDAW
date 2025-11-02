# exeDAW - Professional Digital Audio Workstation

A C++14-based Digital Audio Workstation with reusable audio player objects, full CRUD channel management, multi-track sequencing, real-time waveform visualization, and Cubase-style transport controls.

## ?? Features

### Audio Player System
- ? **Unlimited Audio Players**: Create as many players as needed for different channels
- ? **Play/Pause/Stop Controls**: Standard playback controls
- ? **Volume Control**: Per-player volume adjustment (0.0 - 1.0)
- ? **Position Tracking**: Precise playback position and duration queries
- ? **Multiple Playback States**: STOPPED, PLAYING, PAUSED, RECORDING

### Sequencer Engine
- ? **CRUD Operations**: Full Create, Read, Update, Delete functionality for channels
- ? **Multi-Track Support**: Manage multiple sequencer channels simultaneously
- ? **Channel Types**: AUDIO, MIDI, BUS channel support
- ? **Dynamic Channel Management**: Add/remove channels at runtime
- ? **Channel Properties**: Volume, pan, mute, solo per channel

### Transport Controls (Cubase-Style)
- ? **Playback Control**: Play, Pause, Stop, Resume, Record buttons
- ? **Tempo Adjustment**: Adjustable BPM (beats per minute)
- ? **Time Signature**: Configurable time signature (4/4 default)
- ? **Loop Control**: Enable/disable looping with custom loop points
- ? **Playhead Management**: Track and control playback position
- ? **Sample Rate Control**: Configurable sample rate (44.1kHz default)

### Waveform Visualization
- ? **Real-Time Waveform Display**: Visual representation of audio samples
- ? **Peak Detection**: Identify maximum amplitude values
- ? **RMS Calculation**: Root Mean Square amplitude analysis
- ? **Zoom Control**: Scalable waveform view
- ? **Mono/Stereo Display**: Support for both mono and stereo visualization
- ? **Grid Overlay**: Reference grid for time measurement

### Windows GUI
- ? **Transport Controls**: Intuitive play, pause, stop, record buttons
- ? **Channel List**: Visual list of all sequencer channels with status
- ? **Track Management**: Add, delete, and manage tracks
- ? **Waveform Display**: Real-time waveform rendering
- ? **Tempo Control**: Live tempo adjustment with spinner
- ? **Loop Controls**: Enable/disable and configure looping

## ?? Project Structure

```
exeDAW/
??? AudioPlayer.h/cpp       # Individual audio playback
??? SequencerChannel.h/cpp         # Track/channel representation
??? SequencerEngine.h/cpp          # CRUD channel management
??? TransportControl.h/cpp    # Cubase-style transport controls
??? WaveformVisualizer.h/cpp       # Waveform visualization & analysis
??? DAWApplication.h/cpp           # Main DAW coordinator
??? DAWWindow.h/cpp        # Windows GUI implementation
??? exeDAW.cpp            # Application entry point
??? exeDAW.h          # Main header
??? framework.h          # System headers
??? DAW_ARCHITECTURE.md  # Architecture documentation
??? QUICK_START.md     # Quick start guide
??? API_REFERENCE.md     # Complete API documentation
??? README_DAW.md # This file
```

## ?? Quick Start

### Building the Project
1. Open `exeDAW.vcxproj` in Visual Studio
2. Set configuration to Debug or Release
3. Build solution (Ctrl+Shift+B)
4. Run executable (F5 or Ctrl+F5)

### Basic Usage

```cpp
// Initialize DAW
auto daw = std::make_shared<DAWApplication>();
daw->Initialize();

// Create tracks
auto track1 = daw->AddTrack("Drums");
auto track2 = daw->AddTrack("Bass");

// Load audio
daw->LoadAudioFile(track1->GetChannelId(), "drums.wav");
daw->LoadAudioFile(track2->GetChannelId(), "bass.wav");

// Configure transport
auto transport = daw->GetTransport();
transport->SetTempo(128.0f);
transport->SetLoopEnabled(true);

// Playback
daw->PlayAll();
```

## ?? GUI Controls

### Transport Controls
| Button | Function |
|--------|----------|
| ? Play | Start playback from current position |
| ? Pause | Pause playback without resetting |
| ? Stop | Stop and reset to beginning |
| ? Record | Enable recording mode |

### Additional Controls
- **Tempo (BPM)**: Editable field for playback tempo
- **Loop**: Checkbox to enable/disable looping
- **+ Add Track**: Create new sequencer channel
- **Load Audio**: Load audio file to selected channel
- **Delete Track**: Remove selected channel

### Waveform Display
- Top section shows real-time waveform visualization
- Grid overlay for time reference
- Displays min/max amplitude waveform

### Channel List
Shows all channels with status indicators:
- Channel ID and name
- `[LOADED]` - Audio file present
- `[M]` - Channel muted
- `[S]` - Channel soloed

## ?? Documentation

### Architecture Documentation
See `DAW_ARCHITECTURE.md` for:
- Complete component architecture
- Class responsibilities and relationships
- CRUD operation workflows
- Code examples

### Quick Start Guide
See `QUICK_START.md` for:
- Step-by-step usage guide
- Common workflows
- Troubleshooting
- Tips and tricks

### API Reference
See `API_REFERENCE.md` for:
- Complete API documentation
- Method signatures
- Parameter descriptions
- Usage patterns

## ?? Architecture

### Core Components

1. **AudioPlayer**: Manages individual audio playback
   - Load audio files
   - Play/pause/stop controls
   - Volume and position management
   - Playback state tracking

2. **SequencerChannel**: Represents a sequencer track
   - Owns an AudioPlayer instance
 - Channel properties (volume, pan, mute, solo)
   - Channel identification and naming

3. **SequencerEngine**: Manages all channels (CRUD)
   - Create channels
   - Read channel information
   - Update channel properties
   - Delete channels
   - Audio player assignment

4. **TransportControl**: Cubase-style transport
   - Playback state management
   - Tempo and time signature
   - Loop control
   - Sample rate configuration
   - State change callbacks

5. **WaveformVisualizer**: Audio analysis and visualization
   - Waveform point generation
   - Peak amplitude detection
   - RMS calculation
   - Zoom and display modes

6. **DAWApplication**: Main coordinator
   - Integrates all components
   - Convenience methods for common operations
   - Multi-track playback control

7. **DAWWindow**: Windows GUI implementation
   - Transport control UI
   - Channel management UI
   - Waveform display
   - Real-time event handling

## ?? CRUD Operations

### Audio Player CRUD (per Channel)

```cpp
// CREATE: Assign player to channel
auto player = std::make_shared<AudioPlayer>();
channel->AssignAudioPlayer(player);

// READ: Get assigned player
auto current = channel->GetAudioPlayer();

// UPDATE: Assign different player
auto newPlayer = std::make_shared<AudioPlayer>();
channel->AssignAudioPlayer(newPlayer);

// DELETE: Remove player
channel->RemoveAudioPlayer();
```

### Channel CRUD (Sequencer)

```cpp
// CREATE: New channel
auto channel = sequencer->CreateChannel("Track Name");

// READ: Get channels
auto ch = sequencer->GetChannel(id);
auto all = sequencer->GetAllChannels();

// UPDATE: Modify channel
sequencer->UpdateChannel(id, "New Name");

// DELETE: Remove channel
sequencer->DeleteChannel(id);
```

## ?? Memory Management

- Uses `std::shared_ptr` for automatic memory management
- No manual memory allocation/deallocation required
- Objects automatically cleaned up when references removed
- Safe for assignment and parameter passing

## ??? Build Requirements

- **C++ Standard**: C++14 or later
- **Compiler**: Visual Studio 2015 or later
- **Platform**: Windows 7 or later
- **Libraries**: Common Controls (comctl32.lib)
- **Runtime**: Visual C++ Runtime libraries

## ?? Performance Characteristics

- **Channels**: Can handle dozens to hundreds of channels
- **Memory**: Efficient use with smart pointers
- **Waveform Display**: Optimized real-time rendering
- **Audio Loading**: Simplified for demo (production would use audio libraries)
- **Thread Safety**: Currently single-threaded (can be extended)

## ?? Use Cases

1. **Music Production**: Multi-track recording and mixing
2. **Audio Editing**: Non-destructive audio track management
3. **Audio Sequencing**: Timeline-based track organization
4. **Sound Design**: Layer multiple audio sources
5. **Music Composition**: Arrange and mix multiple instruments

## ?? Future Enhancements

- [ ] Real audio file format support (WAV, MP3, FLAC, OGG)
- [ ] Audio effects (EQ, reverb, compression, delay)
- [ ] MIDI support and editing
- [ ] Real-time audio engine integration (ASIO/PortAudio)
- [ ] Parameter automation
- [ ] Cue points and markers
- [ ] Undo/Redo system
- [ ] Project save/load
- [ ] VST/AU plugin support
- [ ] Advanced visualization (spectrum analyzer, oscilloscope)
- [ ] Multi-threaded audio processing
- [ ] Professional mixing tools
- [ ] Virtual instruments
- [ ] Recording to file

## ?? Notes

- Current implementation includes a simplified sine wave generator for audio simulation
- GUI is scalable and can be extended with additional features
- All classes follow C++14 standards
- Comprehensive documentation included
- Well-commented source code

## ?? Documentation Files

1. **README_DAW.md** - This file, overview and quick reference
2. **DAW_ARCHITECTURE.md** - Detailed architecture and design
3. **QUICK_START.md** - Step-by-step usage guide
4. **API_REFERENCE.md** - Complete API documentation

## ?? Contributing

This is a demonstration audio workstation. Feel free to:
- Extend with additional features
- Integrate real audio library support
- Add more visualization options
- Implement additional DSP features
- Optimize for performance

## ?? License

This project is part of the exeDAW suite by Execution Software.

## ?? Getting Started

1. Clone or extract the repository
2. Open `exeDAW.vcxproj` in Visual Studio
3. Build the solution
4. Run the application
5. Start creating audio tracks!

---

**Happy music production!** ??

For detailed information on specific features, see the documentation files in the project root.
