// SPDX-License-Identifier: MIT

// nitrosf2: convert the music of Nintendo DS games to MIDI, SoundFont 2 and SFZ.
//
//     nitrosf2 [options] <image.nds | archive.sdat> [...]
//
// Output goes beside each input. Windows accepts files dropped on the executable. macOS uses Nitrosf2.app.
//
// ntdlib's DS frame (ntdlib/sequence_export/ntr/archive_input.h) reads the inputs and the options. A cartridge image
// gives each sound archive (SDAT) that a file of its file system contains, each folder of Procyon Studio's songs and
// sound effects (DSE), each folder of Universomo's songs (.ums), and the sounds of Nintendo SPD's driver
// (sounddata.bin). The games' profiles (ntdlib/sequence_export/ntr/game_profiles.h) give what each game's code sets
// before a sequence starts.

#include "ntdlib/sequence_export/ntr/archive_input.h"
#include "ntdlib/sequence_export/ntr/game_profiles.h"

int main(int argc, char** argv)
{
    ntdlib::sequence_export::ntr::ArchiveConverter converter;
    converter.program = "nitrosf2";
    converter.version_line = "nitrosf2 " NITROSF2_VERSION;
    converter.description =
        "nitrosf2: convert the music of Nintendo DS games to MIDI, SoundFont 2 and SFZ.\n"
        "Give it a DS or DSi cartridge image (.nds) or a sound archive (.sdat).\n";
    ntdlib::sequence_export::ntr::UseGameProfiles(converter);

    return ntdlib::sequence_export::ntr::RunArchiveConverter(argc, argv, converter);
}
