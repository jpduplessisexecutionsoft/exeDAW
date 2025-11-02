# exeDAW API Reference

## Class Reference

### AudioPlayer

Manages individual audio playback on a channel.

#### Constructor
```cpp
AudioPlayer();
```

#### Methods

##### File Loading
```cpp
bool LoadAudioFile(const std::string& filePath);
```
- Loads an audio file into the player
- Returns: true if successful, false otherwise

##### Playback Control
```cpp
void Play();
void Pause();
void Stop();
void Resume();
```
- `Play()`: Start playback from current position
- `Pause()`: Pause playback without resetting position
- `Stop()`: Stop playback and reset to beginning
- `Resume()`: Resume from paused state

##### Position Control
```cpp
void SetPosition(uint32_t sampleIndex);
uint32_t GetPosition() const;
uint32_t GetDuration() const;
```
- `SetPosition()`: Set playback position in samples
- `GetPosition()`: Get current playback position in samples
- `GetDuration()`: Get total audio duration in samples

##### Volume Control
```cpp
void SetVolume(float volume);
float GetVolume() const;
```
- `SetVolume()`: Set volume (0.0 - 1.0, values clamped)
- `GetVolume()`: Get current volume level

##### State Queries
```cpp
const AudioData& GetAudioData() const;
PlaybackState GetState() const;
```
- `GetAudioData()`: Get audio sample data
- `GetState()`: Get current playback state (STOPPED, PLAYING, PAUSED, RECORDING)

#### PlaybackState Enum
```cpp
enum PlaybackState {
    STOPPED = 0,
    PLAYING = 1,
    PAUSED = 2,
  RECORDING = 3
};
```

---

### SequencerChannel

Represents a single track/channel in the sequencer.

#### Constructor
```cpp
SequencerChannel(uint32_t channelId, const std::string& name, 
       ChannelType type = AUDIO);
```

#### Methods

##### Channel Information
```cpp
uint32_t GetChannelId() const;
std::string GetChannelName() const;
void SetChannelName(const std::string& name);
ChannelType GetChannelType() const;
```

##### Audio Player Management (CRUD)
```cpp
bool AssignAudioPlayer(std::shared_ptr<AudioPlayer> player);
std::shared_ptr<AudioPlayer> GetAudioPlayer() const;
bool HasAudioPlayer() const;
void RemoveAudioPlayer();
```
- `AssignAudioPlayer()`: Assign (Create/Update) an audio player
- `GetAudioPlayer()`: Retrieve the assigned player (Read)
- `HasAudioPlayer()`: Check if player is assigned
- `RemoveAudioPlayer()`: Remove the player (Delete)

##### Channel Properties
```cpp
void SetVolume(float volume);
float GetVolume() const;

void SetPan(float pan);
float GetPan() const;

void SetMuted(bool muted);
bool IsMuted() const;

void SetSolo(bool solo);
bool IsSolo() const;
```
- `Volume`: Range 0.0 - 1.0, clamped automatically
- `Pan`: Range -1.0 (left) to 1.0 (right), clamped automatically
- `Muted`: Channel is silent when true
- `Solo`: Only this channel plays when true

#### ChannelType Enum
```cpp
enum ChannelType {
    AUDIO = 0,  // Audio track
    MIDI = 1,   // MIDI track
    BUS = 2     // Master/submix bus
};
```

---

### SequencerEngine

CRUD operations for managing all sequencer channels.

#### Constructor
```cpp
SequencerEngine();
```

#### Methods

##### CREATE Operations
```cpp
std::shared_ptr<SequencerChannel> CreateChannel(
    const std::string& name, 
    SequencerChannel::ChannelType type = SequencerChannel::AUDIO
);
```
- Creates a new channel with auto-incrementing ID
- Returns: shared_ptr to the created channel

##### READ Operations
```cpp
std::shared_ptr<SequencerChannel> GetChannel(uint32_t channelId) const;
std::shared_ptr<SequencerChannel> GetChannelByName(const std::string& name) const;
const std::vector<std::shared_ptr<SequencerChannel>>& GetAllChannels() const;
uint32_t GetChannelCount() const;
```
- `GetChannel()`: Get channel by ID
- `GetChannelByName()`: Get channel by name
- `GetAllChannels()`: Get all channels as vector
- `GetChannelCount()`: Get number of channels

##### UPDATE Operations
```cpp
bool UpdateChannel(uint32_t channelId, const std::string& newName);
```
- Updates channel name
- Returns: true if successful, false if channel not found

##### DELETE Operations
```cpp
bool DeleteChannel(uint32_t channelId);
void DeleteAllChannels();
```
- `DeleteChannel()`: Remove specific channel, returns success
- `DeleteAllChannels()`: Clear all channels

##### Audio Player Integration
```cpp
bool LoadAudioToChannel(uint32_t channelId, const std::string& filePath);
bool AssignPlayerToChannel(uint32_t channelId, 
     std::shared_ptr<AudioPlayer> player);
```
- `LoadAudioToChannel()`: Load audio file and assign to channel
- `AssignPlayerToChannel()`: Assign existing player to channel

##### Utility
```cpp
uint32_t GetChannelIdCounter() const;
```
- Returns the next available channel ID (useful for tracking)

---

### TransportControl

Cubase-style transport controls for the sequencer.

#### Constructor
```cpp
TransportControl();
```

#### Methods

##### Transport State Control
```cpp
void Play();
void Pause();
void Stop();
void Resume();
void Record();
```
- `Play()`: Start playback
- `Pause()`: Pause playback (maintains position)
- `Stop()`: Stop playback and reset to start
- `Resume()`: Resume from paused state
- `Record()`: Enable recording mode

##### Transport State Queries
```cpp
TransportState GetState() const;
bool IsPlaying() const;
bool IsPaused() const;
bool IsRecording() const;
bool IsStopped() const;
```

##### Playhead Position
```cpp
void SetPlayheadPosition(uint32_t sampleIndex);
uint32_t GetPlayheadPosition() const;
```

##### Tempo Control
```cpp
void SetTempo(float bpm);
float GetTempo() const;
```
- Range: BPM > 0.0
- Default: 120.0 BPM

##### Time Signature
```cpp
void SetTimeSignature(uint32_t numerator, uint32_t denominator);
void GetTimeSignature(uint32_t& numerator, uint32_t& denominator) const;
```
- Default: 4/4
- Must be positive integers

##### Loop Control
```cpp
void SetLoopEnabled(bool enabled);
bool IsLoopEnabled() const;

void SetLoopRange(uint32_t startSample, uint32_t endSample);
void GetLoopRange(uint32_t& startSample, uint32_t& endSample) const;
```

##### Sample Rate
```cpp
void SetSampleRate(uint32_t sampleRate);
uint32_t GetSampleRate() const;
```
- Default: 44100 Hz

##### Callbacks
```cpp
void SetPlaybackCallback(void (*callback)(TransportState state));
```
- Register callback function called when transport state changes

#### TransportState Enum
```cpp
enum TransportState {
    STOPPED = 0,
    PLAYING = 1,
    PAUSED = 2,
    RECORDING = 3
};
```

---

### WaveformVisualizer

Audio waveform visualization and analysis.

#### Constructor
```cpp
WaveformVisualizer();
```

#### Methods

##### Audio Data
```cpp
void SetAudioData(const AudioData& audioData);
void ClearAudioData();
```
- `SetAudioData()`: Set audio to visualize
- `ClearAudioData()`: Clear loaded audio

##### Waveform Generation
```cpp
std::vector<WaveformPoint> GenerateWaveformPoints(
    uint32_t pixelWidth, 
    uint32_t pixelHeight
);
```
- Generates points for rendering waveform
- Returns: vector of WaveformPoint structs with (x, min, max) amplitudes

##### Audio Analysis
```cpp
float GetPeakAmplitude() const;
float GetRMSAmplitude() const;
```
- `GetPeakAmplitude()`: Get maximum amplitude value
- `GetRMSAmplitude()`: Get RMS amplitude value

##### Display Parameters
```cpp
void SetZoomLevel(float zoomFactor);
float GetZoomLevel() const;

void SetDisplayMode(bool stereo);
bool GetDisplayMode() const;
```
- `SetZoomLevel()`: Set zoom factor (> 0.0)
- `SetDisplayMode()`: true for stereo, false for mono

#### WaveformPoint Struct
```cpp
struct WaveformPoint {
    float x;         // Horizontal position
    float minAmplitude;    // Minimum amplitude at this point
    float maxAmplitude;      // Maximum amplitude at this point
};
```

---

### DAWApplication

Main coordinator for the audio workstation.

#### Constructor
```cpp
DAWApplication();
```

#### Methods

##### Initialization
```cpp
bool Initialize();
```
- Initializes all subsystems
- Sets default transport settings

##### Component Access
```cpp
std::shared_ptr<SequencerEngine> GetSequencer();
std::shared_ptr<TransportControl> GetTransport();
std::shared_ptr<WaveformVisualizer> GetVisualizer();
```

##### Convenience Methods
```cpp
std::shared_ptr<SequencerChannel> AddTrack(const std::string& trackName);
bool LoadAudioFile(uint32_t channelId, const std::string& filePath);

void PlayAll();
void StopAll();
void PauseAll();

uint32_t GetTrackCount() const;
```
- `AddTrack()`: Create new audio track
- `LoadAudioFile()`: Load audio to specific channel
- `PlayAll()`: Play all tracks with audio
- `StopAll()`: Stop all tracks
- `PauseAll()`: Pause all tracks

---

### DAWWindow

Windows API-based GUI for the DAW.

#### Constructor
```cpp
DAWWindow();
```

#### Methods

##### Window Management
```cpp
bool Create(HINSTANCE hInstance, int nCmdShow);
void Destroy();

HWND GetHandle() const;
```

##### GUI Operations
```cpp
void Redraw();
LRESULT HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
```

#### Control IDs
```cpp
#define IDC_PLAY_BTN 1001    // Play button
#define IDC_PAUSE_BTN       1002    // Pause button
#define IDC_STOP_BTN        1003    // Stop button
#define IDC_RECORD_BTN      1004    // Record button
#define IDC_TEMPO_EDIT      1005    // Tempo input field
#define IDC_TEMPO_SPIN 1006    // Tempo spinner
#define IDC_ADD_TRACK_BTN   1007    // Add track button
#define IDC_CHANNEL_LIST 1008    // Channel list box
#define IDC_LOAD_AUDIO_BTN  1009    // Load audio button
#define IDC_LOOP_CHK   1010    // Loop checkbox
#define IDC_DELETE_TRACK_BTN 1011   // Delete track button
```

---

## AudioData Structure

```cpp
struct AudioData {
    std::vector<float> samples;     // Audio samples (PCM)
    uint32_t sampleRate;  // Sample rate in Hz (default: 44100)
    uint16_t channels;              // Number of channels (default: 2)
    uint16_t bitDepth;     // Bits per sample (default: 16)
    std::string filePath;  // Source file path
};
```

---

## Common Usage Patterns

### Pattern 1: Create and Play Audio

```cpp
auto daw = std::make_shared<DAWApplication>();
daw->Initialize();

auto channel = daw->AddTrack("Guitar");
daw->LoadAudioFile(channel->GetChannelId(), "guitar.wav");
daw->PlayAll();
```

### Pattern 2: Multi-Track Management

```cpp
auto sequencer = daw->GetSequencer();

// Create multiple tracks
auto drums = sequencer->CreateChannel("Drums");
auto bass = sequencer->CreateChannel("Bass");
auto vocals = sequencer->CreateChannel("Vocals");

// Load audio to each
sequencer->LoadAudioToChannel(drums->GetChannelId(), "drums.wav");
sequencer->LoadAudioToChannel(bass->GetChannelId(), "bass.wav");
sequencer->LoadAudioToChannel(vocals->GetChannelId(), "vocals.wav");

// Mix levels
drums->SetVolume(0.8f);
bass->SetVolume(0.7f);
vocals->SetVolume(0.9f);
```

### Pattern 3: Transport Configuration

```cpp
auto transport = daw->GetTransport();

transport->SetTempo(140.0f);
transport->SetTimeSignature(3, 4);
transport->SetSampleRate(48000);
transport->SetLoopEnabled(true);
transport->SetLoopRange(0, 48000 * 8);  // 8 seconds at 48kHz
```

### Pattern 4: CRUD Operations

```cpp
auto sequencer = daw->GetSequencer();

// CREATE
auto channel = sequencer->CreateChannel("New Track");
uint32_t id = channel->GetChannelId();

// READ
auto ch = sequencer->GetChannel(id);
auto all = sequencer->GetAllChannels();
uint32_t count = sequencer->GetChannelCount();

// UPDATE
sequencer->UpdateChannel(id, "Updated Track");
channel->SetVolume(0.75f);

// DELETE
sequencer->DeleteChannel(id);
```

---

## Thread Safety Notes

Current implementation is single-threaded. For multi-threaded audio:
- Add mutex locks around shared resources
- Use lock-free queues for audio processing
- Ensure callback functions are thread-safe
- Consider using std::atomic for state variables

---

## Memory Management

All objects use `std::shared_ptr` for automatic memory management:
- No manual `delete` required
- Automatic cleanup when last reference is released
- Safe for assignment and passing between functions

---

## Error Handling

Most methods return:
- `bool`: Success/failure indication
- Value return: Result or default if unavailable
- nullptr: If object not found

Check return values before using:
```cpp
if (auto player = channel->GetAudioPlayer())
{
 // Player exists, safe to use
    player->Play();
}
```
