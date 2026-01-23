// record.h
#pragma once
#include <string>
#include <cstdint>
#include <vector>

struct Record {
    std::string key;
    std::string value;
};

// Serialize a key-value pair into raw bytes
std::vector<char> serialize(const std::string& key, const std::string& value);

// Deserialize raw bytes back into a WALRecord
bool deserialize(const char* data, size_t len, Record& out);

uint32_t compute_checksum(const char* data, size_t len);