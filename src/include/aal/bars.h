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
  EXIO_DEFINE_FIELDS(Header, magic, file_size, bom, version, asset_count);
};

struct Resource {
  u32 amta_offset;
  s32 asset_offset;
  EXIO_DEFINE_FIELDS(Resource, amta_offset, asset_offset);
};

}  // namespace bars

class AudioResources {
public:
  AudioResources(tcb::span<const u8> data);
  AudioMetadata GetResource(u16 index);
  std::optional<AudioMetadata> GetResource(std::string_view name);

private:
  u16 m_num_assets;
  u16 m_hash_offset;
  u32 m_data_offset;
  mutable exio::BinaryReader m_reader;
};

}  // namespace aal