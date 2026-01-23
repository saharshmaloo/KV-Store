#include "record.h"

void write_int32(std::vector<char>& buf, uint32_t x) {
    buf.push_back((x >> 24) & 0xFF);
    buf.push_back((x >> 16) & 0xFF);
    buf.push_back((x >> 8) & 0xFF);
    buf.push_back(x & 0xFF);
}

uint32_t read_int32(const char* data) {
    return (uint32_t(uint8_t(data[0])) << 24) |
           (uint32_t(uint8_t(data[1])) << 16) |
           (uint32_t(uint8_t(data[2])) << 8)  |
           uint32_t(uint8_t(data[3]));
}

// Serialize a key-value pair into raw bytes
std::vector<char> serialize(const std::string& key, const std::string& value) {
    std::vector<char> buf;
    buf.reserve(16 + key.size() + value.size());

    // total len
    write_int32(buf, (12 + key.size() + value.size()));

    // key len + key
    write_int32(buf, key.size());
    buf.insert(buf.end(), key.begin(), key.end());

    // value len + value
    write_int32(buf, value.size());
    buf.insert(buf.end(), value.begin(), value.end());

    // checksum
    write_int32(buf, compute_checksum(buf.data() + 4, buf.size() - 4));
    return buf;
}

// Deserialize raw bytes back into a WALRecord
bool deserialize(const char* data, size_t len, Record& out) {
    if (len < 4) {
        return false;
    }

    uint32_t total_len = read_int32(data);
    if (len - 4 != total_len) {
        return false;
    }

    uint32_t key_len = read_int32(data + 4);
    if (total_len < 12 || key_len > total_len - 12) {
        return false;
    }

    uint32_t value_len = read_int32(data + 8 + key_len);
    if (total_len < (12 + key_len) || value_len > total_len - 12 - key_len) {
        return false;
    }

    uint32_t checksum = read_int32(data + 12 + key_len + value_len);

    if (compute_checksum(data + 4, len - 8) != checksum) {
        return false;
    }

    out.key = std::string(data + 8, key_len);
    out.value = std::string(data + 12 + key_len, value_len);
    return true;
}

// CRC32 Hash
uint32_t compute_checksum(const char* data, size_t len) {
    static uint32_t table[256];
    static bool table_initialized = false;

    if (!table_initialized) {
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i;
            for (int j = 0; j < 8; ++j) {
                c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
            }
            table[i] = c;
        }
        table_initialized = true;
    }

    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i) {
        crc = table[(crc ^ static_cast<uint8_t>(data[i])) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFFu;
}