# nitrosf2

nitrosf2 converts the music of Nintendo DS games to MIDI, SoundFont 2 and SFZ. Each sequence gets a MIDI file and a
SoundFont containing its instruments. SFZ output is written beside them.

Drop cartridge images or sound archives onto `nitrosf2.exe` on Windows or `Nitrosf2.app` on macOS. Output goes in a
folder beside each one. From a terminal:

```
nitrosf2 [options] <archive.sdat | image.nds> [...]

  --output DIR   write to DIR (one input only)
  --only REGEX   convert matching sequence names (for example --only BGM)
  --loops N      repeat the main loop N times (default 1)
                 MIDI loop markers are included for players that support them
  --seed N       the driver's initial random state (default 0x12345678)
  --var N=V      set variable N (0-15 player, 16-31 global) to V before each
                 sequence starts; repeat for more variables
  --list         list sequences without converting
  --quiet        print errors only
  --version      show the version
```

nitrosf2 reads DS and DSi cartridge images (`.nds`) and loose sound archives (`.sdat`). The default output folder is
`<input name>_nitrosf2`, beside the input.

An image can hold more than one sound archive, and an archive can sit in any file of the image's file system, whatever
the file is called. Each archive gets a folder of its own in the image's output folder. So does each folder of songs and
sound effects of Procyon Studio's driver, and each folder of Universomo's songs (see below). The image's ARM7 program
tells nitrosf2 which build of the DS's sound driver plays the archive. A loose archive plays on the most common build.

Each sequence gets these files:

- `<name>.mid`: the sequence as a standard MIDI file.
- `<name>.sf2`: a SoundFont that holds the sequence's instruments.
- `sfz/<name>/`: an SFZ file for each MIDI track.
- `sfz/samples/`: the WAV samples that all of the folder's SFZ files share.

## Supported games

Nearly every DS game plays its music through the sound archives (SDAT) and the sound driver of Nintendo's SDK.
nitrosf2 converts every sequence of such an archive, the sequences of its sequence archives included.

nitrosf2 also converts the songs and sound effects of Digital Sound Elements (DSE), Procyon Studio's sound driver.
Pokémon Mystery Dungeon: Explorers of Time, Explorers of Darkness and Explorers of Sky use it. A sound effect is named
after its file and its slot, such as `se_sys_005`.

nitrosf2 converts the songs of Universomo's tracker driver too. Beat City uses it. Its 147 songs go in the folder
`universomo`, each named after its song file, such as `au81`. Songs 81 to 146 are the music, and the rest are sound
effects. Song 105 is the title screen's music and is named `au105_title_screen`.
The game's table of sounds says which songs loop.

A game that plays its music through another driver, or only as streamed recordings, has nothing that nitrosf2 can
convert yet. nitrosf2 then says that it found no sequences.

### Games' profiles

A game's code often changes what a sequence plays. It sets a variable that the sequence reads, gives some tracks other
channels, starts the console's reverb or sets a track's volume. nitrosf2 holds a profile of what the code of these
games does there:

| Game | Version | What the profile sets |
|---|---|---|
| New Super Mario Bros. | Every region | The red coins' rising notes, one track for each coin |
| Mario Kart DS | USA | A program for each driver, and the pans of four sound effects |
| Pokémon Diamond and Pearl | Every region | The reverb and the channels of the title and the ending |
| Pokémon Platinum | Every region | The reverb and the channels of the title and the ending |
| Pokémon HeartGold and SoulSilver | Every region | The channels of the title and the ending |
| Pokémon Mystery Dungeon: Blue Rescue Team | USA | The reverb, and the channels of the songs' tracks |
| Pokémon Black, White, Black 2 and White 2 | USA | The tracks that play in each area of the field |

Where a game plays a sequence in more than one way, each way gets a track of its own. The track's name says which way
it is. A sequence of a game without a profile plays as its archive gives it.

## Drag and drop

- **Windows:** drop one or more inputs onto `nitrosf2.exe`. The console shows the results and waits for Enter before
  closing.
- **macOS:** drop inputs onto `Nitrosf2.app`, or open it to choose files. The app shows the results and can open the
  output folders. The app wraps the command-line program because Finder cannot drop files onto a bare executable. The
  [build](#building) includes the app.

## The conversion

A sequence on the DS is a program as much as a list of notes. nitrosf2 plays each sequence on a model of the DS's
sound driver, frame by frame, and records what the driver plays. The driver has 16 channels. When a sequence asks for
more, the driver drops notes or takes a channel from a note that's playing. The model does the same, so the files drop
the notes that the console drops.

The instruments play the console's PCM8, PCM16 and IMA-ADPCM waves at the rates that the driver sets, and the sound unit's square
waves and noise. A preset holds a program's envelope and the track's LFO. Velocity, CC7 and CC11 act squared in
SoundFont and SFZ, and the performer writes values that give the driver's level through that law. CC11 also holds the
volumes that the archive and the game give a sequence and its tracks. CC1 holds the LFO's depth.

The DS pans linearly, and a sound panned to one side loses level at the centre. CC10 holds each pan's position for the
sin/cos law of MIDI RP-036, and CC11 and the zones hold the level that the DS's pan gives.

A sequence that loops plays its main loop once by default, with loop markers for players that loop on them. A sound
that holds a note for ever ends 10 s after it settles.

### Universomo's songs

Universomo's driver is a tracker: each track of a song plays on one of the sound unit's channels, a row at a time, and
the game runs it once a frame (59.83 times a second). nitrosf2 plays each song on a model of the driver, tick by tick,
and records the sound unit's commands. Each track becomes a MIDI track. A note starts where the driver sets up its
channel and ends at the track's next note, or where a sample that plays once ends. The driver's slides, tone
portamento, vibrato and arpeggios become pitch bends, and its volume slides CC11. The level is linear in the channel's
volume, in the steps that the sound unit plays, and CC11 holds it through the squared law. Each sample of the bank
that a song plays is a preset over every key, with its loop. Even channels pan to 74 of 127 and odd ones to 54, as the
driver sets them.

The game's code plays every song at the same volume and changes nothing in a song from one stage to another. A song
that loops goes back to its start. The vibrato's phase carries over from one pass to the next, so in a few songs
each pass bends a little differently. The loop markers then repeat one pass, and the report counts the approximation.

### SoundFont

SoundFont players apply 0.4 of a zone's initialAttenuation. The files take that into account: the generator holds 2.5
times the attenuation that the specification calls for. A player that follows the specification plays such zones too
quiet.

The specification gives the default CC10 modulator an amount of 1000. Every player found plays 500. Each global zone
therefore holds a CC10 modulator of amount 500 in place of the default one.

A preset that MIDI channel 10 plays also has a copy in bank 128, where players look for drums.

### SFZ

SFZ players ignore MIDI channels, and each file holds one instrument. Load the MIDI file in a DAW and assign an SFZ
player to each track with its matching file. The player tested is sfizz 1.2.3. The DS's channels don't interpolate,
so the regions set `sample_quality=0`.

## Differences from the game

- The driver's envelopes don't have the shapes of SoundFont's. The attack, decay and release take the times that fit
  the driver's curves best.
- The console's reverb is left to the player. Where a game starts it, CC91 holds its level.
- The surround mode of the SDK's output effect is left out. The files play in plain stereo.
- The LFO is a sine on the DS and a triangle in SoundFont. A pan LFO is only in the SFZ files.
- Fades and other changes that the game's code makes during play stay out, unless a profile lists them.
- A player that loops on the loop markers cuts the notes that sound across a song's loop end. The game lets them
  ring on.
- SoundFont players interpolate between a sample's points, and the DS doesn't. A sample with a lot of high
  frequencies, such as a drum, therefore plays a little quieter and duller in a SoundFont player. Beat City's music
  plays close to the game's level. A drum sample can play up to about 2 dB quieter in FluidSynth 2.6.2. The SFZ files
  set `sample_quality=0` and play without interpolation.

## Building

Requires C++20 and CMake 3.20 or later. nitrosf2 supports macOS on Apple Silicon and Windows x64 with clang-cl.

- **Windows:** install Visual Studio 2022 with "Desktop development with C++", including CMake. Add the "C++ Clang tools
  for Windows" component. Use Developer PowerShell or Developer Command Prompt.
- **macOS:** install Xcode command line tools (`xcode-select --install`) and CMake (`brew install cmake` or the
  installer from cmake.org).

nitrosf2 builds on three libraries: seqlib, xsflib and ntdlib. The repository holds them as Git submodules in
`externals/`. Clone it with its submodules:

```sh
git clone --recursive <repository URL>
```

In a clone made without `--recursive`, run `git submodule update --init` first.

From the repository root:

```sh
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release
```

On Windows, the first command is `cmake -S . -B build -A x64 -T ClangCL`. It selects clang-cl and 64-bit output.

The executable is `build/nitrosf2`, or `build/Release/nitrosf2.exe` with Visual Studio. macOS builds also produce
`build/Nitrosf2.app`. The app holds a copy of the program. You can move the app elsewhere.

C++ formatting uses clang-format with the repository's `.clang-format`.

## Tests

`ctest` checks that the program reports its version and refuses a variable out of range. The conversion and the games'
profiles are in ntdlib. ntdlib's tests cover them with small archives and profiles built by hand, and they require no
game data.

## Licence

nitrosf2 is under the [MIT](LICENSE) licence. The libraries it builds on, seqlib, xsflib and ntdlib, are under the MIT
licence too. `THIRD-PARTY-NOTICES.md` gives their licence texts. Game data is not covered by this licence.
