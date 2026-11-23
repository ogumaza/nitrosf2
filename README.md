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
  --dse-model earlier|later
                 convert DSE songs and sound effects on the model of Explorers
                 of Time (earlier) or Explorers of Sky (later) where no model
                 covers the game's driver build; the result only approximates
                 the game. Packs compressed other than with LZ77 or in GFSA
                 archives aren't found, and give no warning
  --list         list sequences without converting
  --quiet        print errors only
  --version      show the version
```

nitrosf2 reads DS and DSi cartridge images (`.nds`) and loose sound archives (`.sdat`). The default output folder is
`<input name>_nitrosf2`, beside the input.

An image can contain more than one sound archive, and an archive can sit in any file of the image's file system,
whatever the file is called. Each archive gets a folder of its own in the image's output folder. So does each folder of
songs and sound effects of Procyon Studio's driver, and each folder of Universomo's songs (see below). The image's ARM7
program gives the build of the DS's sound driver that plays the archive. A loose archive plays on the most common
build.

Each sequence gets these files:

- `<name>.mid`: the sequence as a standard MIDI file.
- `<name>.sf2`: a SoundFont that contains the sequence's instruments.
- `sfz/<name>/`: an SFZ file for each MIDI track.
- `sfz/samples/`: the WAV samples that all of the folder's SFZ files share.

## Supported games

Nearly every DS game plays its music through the sound archives (SDAT) and the sound driver of Nintendo's SDK.
nitrosf2 converts every sequence of such an archive, the sequences of its sequence archives included.

nitrosf2 also converts the songs and sound effects of Digital Sound Elements (DSE), Procyon Studio's sound driver.
These games use builds of it that nitrosf2 models:

- Pokémon Mystery Dungeon: Explorers of Time, Explorers of Darkness and Explorers of Sky
- Professor Layton and the Diabolical Box, Professor Layton and the Unwound Future, and Professor Layton and the Last
  Specter, with its London Life
- Soma Bringer (Japan)
- Luminous Arc and Luminous Arc 2
- Luminous Arc 3: Eyes (Japan)
- Inazuma Eleven, Inazuma Eleven 2: Kyoui no Shinryakusha Fire, and Inazuma Eleven 3: Sekai e no Chousen!! Bomber
  and The Ogre (Japan)
- Fushigi no Dungeon: Fuurai no Shiren DS 2: Sabaku no Majou, Fuurai no Shiren 4: Kami no Hitomi to Akuma no Heso,
  and Fuurai no Shiren 5: Fortune Tower to Unmei no Dice (Japan)
- Ni no Kuni: Shikkoku no Madoushi (Japan)

DSE's driver steps its songs on a timer that the game sets. Most games set 10 ms. The main game of Professor Layton
and the Last Specter steps once a video frame, every 16.715 ms, and its London Life every 10 ms. nitrosf2 plays each
song at its game's rate.

DSE's driver also drops the fraction of each channel's timer period, so its high notes play sharp of their keys. A
sequence's SoundFont tunes each key that the sequence plays to the driver's pitch, and the SFZ files and the banks tune
every key. A bent note that sounds alone on its MIDI channel gets the pitch bend at which the driver plays it. A note
that the channel's bend would play more than 5 cents off moves to a channel of its own while one is free. Notes that
share a channel under one bend can still play a few cents off.

Ni no Kuni contains six sequenced pieces that the game never plays: no code in it loads a song. Its music is streamed
recordings, which nitrosf2 doesn't convert. The six pieces convert on the driver of Inazuma Eleven 3, whose code the
game shares.

A sound effect is named after its file and its slot, such as `se_sys_005`. Apart from the Explorers games, most of
these games keep their sound effects in a layout of bank that nitrosf2 doesn't read yet. nitrosf2 then converts their
songs only, and gives a warning for each such bank.

Where two DSE banks of one folder share an id, the SFZ files name their samples after the banks' files. The samples of
the two banks then stay apart in `sfz/samples/`.

Other games use other builds of DSE's driver, and nitrosf2 has no model of those yet. It finds their songs and sound
effects, in the file system or inside the games' packs, but converts them only when you ask for it with `--dse-model`.
They then play on the model of one of the Explorers games, and the result only approximates what the game plays. Without
the option, `--list` doesn't show them, and a warning gives their count. nitrosf2 doesn't find songs in packs
compressed other than with the SDK's LZ77 or in Level-5's GFSA archives. Such an image gives no warning about them.

nitrosf2 converts the songs of Universomo's tracker driver too. Beat City uses it. Its 147 songs go in the folder
`universomo`, each named after its song file, such as `au12`. Songs 81 to 146 are the music, and the rest are sound
effects. The game's table of sounds says which songs loop.

Super Princess Peach plays its sounds on Nintendo SPD's driver, and nitrosf2 converts them too. They go in the folder
`spd`, each named after its block and its index in the block, such as `01_000`. Block 0 contains the sounds that every
scene of the game loads, and each other block contains one scene's sounds. Some sound effects of block 0 play the
instruments of whichever scene is loaded. nitrosf2 can't convert those alone yet. It lists each one as not converted
and goes on with the rest.

A few sounds get no file. Eight of them only start another sound, which then plays in their place, and the report
names each as an alias of that sound. The game's code can also set a flag that sends two pieces, 02_007 and 03_002, on
to sections that they don't reach otherwise. Those pieces convert twice: as they start, and with the flag set
(`02_007_FLAG1`, `03_002_FLAG1`). Their six sections get no file of their own, and the report names each.

A game that plays its music through another driver, or only as streamed recordings, has nothing that nitrosf2 can
convert yet. nitrosf2 then says that it found no sequences.

### Games' profiles

A game's code often changes what a sequence plays. It sets a variable that the sequence reads, gives some tracks other
channels, starts the console's reverb or sets a track's volume. nitrosf2 contains a profile of what the code of these
games does there:

| Game | Version | What the profile sets |
|---|---|---|
| New Super Mario Bros. | Every region | The red coins' rising notes, one track for each coin |
| New Super Mario Bros. | USA | The faster music once a level's time runs low or a player leads in Mario vs. Luigi, and the World 8 map with every track |
| Mario Kart DS | USA | A program for each driver, the pans of four sound effects, and each course's music on the final lap: faster and a semitone up |
| Pokémon Diamond and Pearl | Every region | The reverb and the channels of the title and the ending, and the TV's music at the volume that the player's room sets |
| Pokémon Platinum | Every region | The reverb and the channels of the title and the ending, the TV's music at the volume that the player's room sets, and the faster end of a round in the Wi-Fi Plaza's game |
| Pokémon HeartGold and SoulSilver | Every region | The channels of the title and the ending, and the faster end of a round in the Wi-Fi Plaza's game |
| Pokémon Mystery Dungeon: Blue Rescue Team | USA | The reverb, and the channels of the songs' tracks |
| Pokémon Black, White, Black 2 and White 2 | USA | The tracks that play in each area of the field, and the last 20 seconds of a round of Poké Transfer's minigame |
| Pokémon Mystery Dungeon: Explorers of Time, Explorers of Darkness and Explorers of Sky | USA | The level at which the game starts a piece |
| Luminous Arc | USA | The level at which the game starts its music |
| Fushigi no Dungeon: Fuurai no Shiren DS 2 | Japan | The level at which the game starts a song |
| Fuurai no Shiren 4 and Fuurai no Shiren 5 | Japan | The level at which the game starts a song, read from the game's code. In Shiren 5, also the change of level that some scenes make as a song starts |
| Inazuma Eleven, Inazuma Eleven 2: Kyoui no Shinryakusha Fire, and Inazuma Eleven 3: Sekai e no Chousen!! Bomber and The Ogre | Japan (Inazuma Eleven and Inazuma Eleven 2 in revision 1) | The level at which the game starts its songs, read from the game's code |
| The Legend of Zelda: Spirit Tracks | USA and Europe | The mix of each scene: the train's speeds, a chase's distance, a height in one scene, the phases of other scenes, the intros of three pieces |
| The Legend of Zelda: Phantom Hourglass | USA | The sea music of a ship at rest, and the fog with and without the Ghost Ship |

Where a game plays a sequence in more than one way, each way gets files of its own. Their names add a suffix for the way
to the sequence's name, such as `SEQ_18_SLOW` or `BGM_WORLD8_FULL`. A sequence of a game without a profile plays as its
archive gives it.

Some games start a song below full level, and the conversion then plays it at the level of most of the scenes that
start it. A song that several scenes start at a second level converts a second time, into files named for that level
out of 256, such as `bgm0086_LEVEL256`. The Sky Jukebox of Explorers of Sky plays every piece at full level.

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

The instruments play the console's PCM8, PCM16 and IMA-ADPCM waves at the rates that the driver sets, and the sound
unit's square waves and noise. The driver drops the fraction of each channel's timer period, so a high note plays sharp
of its key, by a few cents in most music and by more near the top of the range. A sequence's SoundFont tunes each key
that the sequence plays to match. The SFZ files tune every key. A note that bends on a part of its own bends to the
pitch that the channel's timer gives. A preset contains a program's envelope and the track's LFO. Velocity, CC7 and CC11
act squared in SoundFont and SFZ, and the performer writes values that give the driver's level through that law. CC11
also contains the volumes that the archive and the game give a sequence and its tracks. CC1 sets the LFO's depth.

The DS pans linearly, and a sound panned to one side loses level at the centre. CC10 sets each pan's position for the
sin/cos law of MIDI RP-036, and CC11 and the zones set the level that the DS's pan gives. Where the track's LFO moves
the pan of a note that plays alone on its part, with no other note in its release, CC10 and CC11 follow the pan
frame by frame, and CC1 stays at 0.

A sequence that loops plays its main loop once by default, with loop markers for players that loop on them. A sound
that holds a note for ever ends 10 s after it settles.

A sequence that the game's sound archive gives a volume of 0 converts silent, because the game plays it silent too.
The report says so for each such sequence.

### Universomo's songs

Universomo's driver is a tracker: each track of a song plays on one of the sound unit's channels, a row at a time, and
the game runs it once a frame (59.83 times a second). nitrosf2 plays each song on a model of the driver, tick by tick,
and records the sound unit's commands. Each track becomes a MIDI track. A note starts where the driver sets up its
channel and ends at the track's next note, or where a sample that plays once ends. The driver's slides, tone
portamento, vibrato and arpeggios become pitch bends, and its volume slides CC11. The level is linear in the channel's
volume, in the steps that the sound unit plays, and CC11 sets it through the squared law. Each sample of the bank
that a song plays is a preset over every key, with its loop. Even channels pan to 74 of 127 and odd ones to 54, as the
driver sets them.

The game's code plays every song at the same volume and changes nothing in a song from one stage to another. A song
that loops goes back to its start. The vibrato's phase carries over from one pass to the next, so in a few songs
each pass bends a little differently. The loop markers then repeat one pass, and the report counts the approximation.

### Nintendo SPD's sounds

The driver runs on the DS's second processor once a frame, about 128 times a second. nitrosf2 plays each sound on a
model of the whole driver, frame by frame, and records what the driver writes to the sound unit: each of the unit's 16
channels becomes a MIDI track, and each start of a channel a note. A MIDI tick is one of the driver's frames. The
channel's timer becomes the key and a pitch bend, so the driver's envelopes, vibrato, sweeps and fades come out as CC11
and pitch bends at the levels and pitches that the game plays. Each sample is a preset over every key, with its loop.

A sound can start other sounds as it starts or ends, and they play in the same file. A sound that loops plays from
the start of its longest loop, and the loop markers repeat that loop. Where the driver moves a note by a frame from
one pass to the next, the markers repeat one pass, and the report counts the approximation.

### SoundFont players

SoundFont players apply 0.4 of a zone's initialAttenuation. The files take that into account: the generator contains 2.5
times the attenuation that the specification calls for. A player that follows the specification plays such zones too
quiet.

On the DS, a sound panned fully to one side plays 3 dB louder than at the centre, and a SoundFont zone can't play above
its sample's level. Every SoundFont therefore plays 3 dB under the game, whatever its zones' pans. The files of a game
keep the balance that the game gives them. The SFZ files play at the game's level.

The specification gives the default CC10 modulator an amount of 1000. Every player found plays 500. Each global zone
therefore contains a CC10 modulator of amount 500 in place of the default one.

A preset that MIDI channel 10 plays also has a copy in bank 128, where players look for drums.

### SFZ

SFZ players ignore MIDI channels, and each file contains one instrument. Load the MIDI file in a DAW and assign an SFZ
player to each track with its matching file. The player tested is sfizz 1.2.3. The DS's channels don't interpolate,
so the regions set `sample_quality=0`.

## Differences from the game

- The driver's envelopes don't have the shapes of SoundFont's. The attack, decay and release take the times that fit
  the driver's curves best.
- The console's reverb is left to the player. Where a game starts it, CC91 sets its level.
- The surround mode of the SDK's output effect is left out. The files play in plain stereo.
- Super Princess Peach's driver makes an echo by recording its output and playing the recording back. MIDI and
  SoundFont can't express that. Each part sends CC91 at the echo's level to the player's reverb instead. The report
  counts the sounds that play it.
- Super Princess Peach's code changes the pitch and the tempo of its music with Peach's moods. The files play each
  piece as it starts, before any of that.
- The LFO is a sine on the DS and a triangle in SoundFont. SoundFont has no pan LFO. On a part that plays one note at
  a time, CC10 follows it. A pan LFO on notes that overlap on one part is only in the SFZ files.
- Fades and other changes that the game's code makes during play stay out, unless a profile lists them.
- A player that loops on the loop markers cuts the notes that sound across a song's loop end. The game lets them
  ring on.
- SoundFont players interpolate between a sample's points, and the DS doesn't. A sample with a lot of high
  frequencies, such as a drum, therefore plays a little quieter and duller in a SoundFont player. Beat City's music
  plays close to the game's level. The SFZ files set `sample_quality=0` and play without interpolation.

## Building

Requires C++20 and CMake 3.20 or later. nitrosf2 supports macOS on Apple Silicon and Windows x64 with clang-cl.

- **Windows:** install Visual Studio 2022 with "Desktop development with C++", including CMake. Add the "C++ Clang tools
  for Windows" component. Use Developer PowerShell or Developer Command Prompt.
- **macOS:** install Xcode command line tools (`xcode-select --install`) and CMake (`brew install cmake` or the
  installer from cmake.org).

nitrosf2 builds on three libraries: seqlib, xsflib and ntdlib. The repository contains them as Git submodules in
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
`build/Nitrosf2.app`. The app contains a copy of the program. You can move the app elsewhere.

C++ formatting uses clang-format with the repository's `.clang-format`.

## Tests

`ctest` checks that the program reports its version and refuses a variable out of range. The conversion and the games'
profiles are in ntdlib. ntdlib's tests cover them with small archives and profiles built by hand, and they require no
game data.

## Licence

nitrosf2 is under the [MIT](LICENSE) licence. The libraries it builds on, seqlib, xsflib and ntdlib, are under the MIT
licence too. `THIRD-PARTY-NOTICES.md` gives their licence texts. Game data is not covered by this licence.
