#pragma once

#include <aal/amta.h>
#include <exio/binary_reader.h>
#include <exio/error.h>
#include <exio/swap.h>
#include <exio/types.h>
#include <exio/util/magic_utils.h>
#include <nonstd/span.h>

namespace aal {

namespace bars {

constexpr auto Magic = exio::util::MakeMagic("BARS");

struct Header {
  std::array<char, 4> magic;
  u32 file_size;
  u16 bom;
  u16 version;
  u32 asset_count;
  EXIO_DEFINE_FIELDS(BarsHeader, magic, file_size, bom, version, asset_count);
};

struct Resource {
  u32 amta_offset;
  s32 asset_offset;
  EXIO_DEFINE_FIELDS(BarsResource, amta_offset, asset_offset);
};

}  // namespace bars

class AudioResources {
public:
  AudioResources(tcb::span<const u8> data) : m_reader{data, exio::Endianness::Big} {
    m_reader = {data, exio::ByteOrderMarkToEndianness(m_reader.Read<bars::Header>().value().bom)};
    const auto header = *m_reader.Read<bars::Header>();

    if (header.magic != bars::Magic) {
      throw exio::InvalidDataError("Invalid BARS magic");
    }
    if (header.version != 0x0101) {
      throw exio::InvalidDataError("Unknwon BARS version");
    }

    m_num_assets = header.asset_count;
    m_hash_offset = sizeof(bars::Header);
    m_data_offset = m_hash_offset + header.asset_count * sizeof(u32);
  }

  AudioMetadata GetResource(u16 index) {
    if (index > m_num_assets) {
      throw std::out_of_range("Index was outside the bounds of the array" + std::to_string(index));
    }

    const size_t resource_offset = m_data_offset + sizeof(bars::Resource) * index;
    const auto resource = *m_reader.Read<bars::Resource>(m_data_offset);
    return AudioMetadata(m_reader, resource.amta_offset);
  }

  std::optional<AudioMetadata> GetResource(std::string_view name) {
    if (m_num_assets == 0) {
      return std::nullopt;
    }

    const auto expected_hash = 0;  // exio::crc32(name);

    u32 a = 0;
    u32 b = m_num_assets - 1;
    while (a <= b) {
      const u32 m = (a + b) / 2;
      const auto hash = m_reader.Read<u32>(m_hash_offset + sizeof(u32) * m);
      if (expected_hash < hash) {
        b = m - 1;
      } else if (expected_hash > hash) {
        a = m + 1;
      } else {
        return GetResource(m);
      }
    }

    return std::nullopt;
  }

private:
  u16 m_num_assets;
  u16 m_hash_offset;
  u32 m_data_offset;
  mutable exio::BinaryReader m_reader;
};

}  // namespace aal