#pragma once

#include <exio/binary_reader.h>
#include <exio/error.h>
#include <exio/swap.h>
#include <exio/types.h>
#include <exio/util/magic_utils.h>
#include <nonstd/span.h>

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
  AudioMetadata(exio::BinaryReader reader, size_t offset) {
    const auto data{reader.span()};
    m_reader = {data, exio::ByteOrderMarkToEndianness(reader.Read<amta::Header>().value().bom)};

    const auto header = *m_reader.Read<amta::Header>();
    if (header.magic != amta::Magic) {
      throw exio::InvalidDataError("Invalid AMTA magic");
    }
    if (header.version != 0x0400) {
      throw exio::InvalidDataError("Only AMTA version 4 is supported");
    }

    m_data = *reader.Read<amta::Data>(offset + header.data_offset);
    if (m_data.magic != amta::DataMagic) {
      throw exio::InvalidDataError("Invalid audio metadata DATA magic");
    }

    m_marker = *reader.Read<amta::Marker>(offset + header.mark_offset);
    if (m_marker.magic != amta::MarkerMagic) {
      throw exio::InvalidDataError("Invalid audio metadata MARK magic");
    }

    m_ext = *reader.Read<amta::Ext>(offset + header.ext_offset);
    if (m_ext.magic != amta::ExtMagic) {
      throw exio::InvalidDataError("Invalid audio metadata EXT_ magic");
    }

    m_string_table = *reader.Read<amta::StringTable>(offset + header.string_table_offset);
    if (m_string_table.magic != amta::StringTableMagic) {
      throw exio::InvalidDataError("Invalid audio metadata STRG magic");
    }

    const auto name_offset =
        offset + header.string_table_offset + sizeof(amta::StringTable) + m_data.name_offset;
    u32 len = *reader.Read<u32>(name_offset);
    name = reader.ReadString(name_offset + sizeof(u32), len);
  }

  std::string_view name;

private:
  amta::Data m_data;
  amta::Marker m_marker;
  amta::Ext m_ext;
  amta::StringTable m_string_table;
  mutable exio::BinaryReader m_reader;
};

}  // namespace aal