#include "aal/amta.h"

namespace aal {

AudioMetadata::AudioMetadata(exio::BinaryReader reader, size_t offset) {
  const auto data{reader.span()};
  m_reader = {data, exio::ByteOrderMarkToEndianness(reader.Read<amta::Header>(offset).value().bom)};

  const auto header = *m_reader.Read<amta::Header>(offset);
  if (header.magic != amta::Magic) {
    throw exio::InvalidDataError("Invalid AMTA magic");
  }
  if (header.version != 0x0400) {
    std::cout << (header.bom == 0xFEFF ? "BE" : "LE") << " | " << m_reader.Tell() << std::endl;
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
  name = reader.ReadString(name_offset + sizeof(u32), *reader.Read<u32>(name_offset));
}

}  // namespace aal