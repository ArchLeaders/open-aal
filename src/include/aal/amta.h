#pragma once

#include <exio/binary_reader.h>
#include <exio/error.h>
#include <exio/swap.h>
#include <exio/types.h>
#include <exio/util/magic_utils.h>
#include <nonstd/span.h>

#include <iostream>

namespace aal {

namespace amta {

constexpr auto Magic = exio::util::MakeMagic("AMTA");
constexpr auto DataMagic = exio::util::MakeMagic("DATA");
constexpr auto MarkerMagic = exio::util::MakeMagic("MARK");
constexpr auto ExtMagic = exio::util::MakeMagic("EXT_");
constexpr auto StringTableMagic = exio::util::MakeMagic("STRG");

struct Header {
  std::array<char, 4> magic;
  u16 bom;
  u16 version;
  u32 file_size;
  u32 data_offset;
  u32 mark_offset;
  u32 ext_offset;
  u32 string_table_offset;
  EXIO_DEFINE_FIELDS(Header, magic, bom, version, file_size, data_offset, mark_offset, ext_offset,
                     string_table_offset);
};

struct Data {
  enum Type : u8 { Wave, Stream };

  struct StreamTrack {
    u32 channel_count;
    f32 volume;
    EXIO_DEFINE_FIELDS(StreamTrack, channel_count, volume);
  };

  std::array<char, 4> magic;
  u32 section_size;
  u32 name_offset;
  u32 sample_count;
  Type type;
  u8 wave_channels;
  u8 used_stream_tracks;
  u8 flags;
  u32 duration;
  u32 sample_rate;
  u32 loop_start_sample;
  u32 loop_end_sample;
  f32 loudness;
  std::array<StreamTrack, 8> stream_tracks;
  f32 amplitude_peak;
  EXIO_DEFINE_FIELDS(Data, magic, section_size, name_offset, sample_count, type, wave_channels,
                     used_stream_tracks, flags, duration, sample_rate, loop_start_sample,
                     loop_end_sample, loudness, stream_tracks, amplitude_peak);
};

struct Marker {
  struct MarkerEntry {
    u32 id;
    u32 name_offset;
    u32 start_pos;
    u32 length;
    EXIO_DEFINE_FIELDS(MarkerEntry, id, name_offset, start_pos, length);
  };

  std::array<char, 4> magic;
  u32 section_size;
  u32 num_entries;
  EXIO_DEFINE_FIELDS(Marker, magic, section_size, num_entries);
};

struct Ext {
  struct ExtEntry {
    u32 unknown[2];
    EXIO_DEFINE_FIELDS(ExtEntry, unknown);
  };

  std::array<char, 4> magic;
  u32 section_size;
  u32 num_entries;
  EXIO_DEFINE_FIELDS(Ext, magic, section_size, num_entries);
};

struct StringTable {
  std::array<char, 4> magic;
  EXIO_DEFINE_FIELDS(StringTable, magic);
};

}  // namespace amta

class AudioMetadata {
public:
  AudioMetadata(exio::BinaryReader reader, size_t offset);
  std::string name;

private:
  amta::Data m_data;
  amta::Marker m_marker;
  amta::Ext m_ext;
  amta::StringTable m_string_table;
  mutable exio::BinaryReader m_reader;
};

}  // namespace aal