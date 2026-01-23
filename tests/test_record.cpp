#include "../src/record.h"
#include <cassert>
#include <iostream>

void test_round_trip_basic() {
    std::vector<char> buf = serialize("hello", "world");
    Record out;
    bool ok = deserialize(buf.data(), buf.size(), out);
    assert(ok);
    assert(out.key == "hello");
    assert(out.value == "world");
    std::cout << "PASS: test_round_trip_basic\n";
}

void test_round_trip_empty_key() {
    std::vector<char> buf = serialize("", "value");
    Record out;
    bool ok = deserialize(buf.data(), buf.size(), out);
    assert(ok);
    assert(out.key == "");
    assert(out.value == "value");
    std::cout << "PASS: test_round_trip_empty_key\n";
}

void test_round_trip_empty_value() {
    std::vector<char> buf = serialize("key", "");
    Record out;
    bool ok = deserialize(buf.data(), buf.size(), out);
    assert(ok);
    assert(out.key == "key");
    assert(out.value == "");
    std::cout << "PASS: test_round_trip_empty_value\n";
}

void test_round_trip_both_empty() {
    std::vector<char> buf = serialize("", "");
    Record out;
    bool ok = deserialize(buf.data(), buf.size(), out);
    assert(ok);
    assert(out.key == "");
    assert(out.value == "");
    std::cout << "PASS: test_round_trip_both_empty\n";
}

void test_round_trip_large_data() {
    std::string key(1000, 'k');
    std::string value(5000, 'v');
    std::vector<char> buf = serialize(key, value);
    Record out;
    bool ok = deserialize(buf.data(), buf.size(), out);
    assert(ok);
    assert(out.key == key);
    assert(out.value == value);
    std::cout << "PASS: test_round_trip_large_data\n";
}

void test_deserialize_too_short() {
    char buf[3] = {0, 0, 0};
    Record out;
    bool ok = deserialize(buf, 3, out);
    assert(!ok);
    std::cout << "PASS: test_deserialize_too_short\n";
}

void test_deserialize_corrupt_checksum() {
    std::vector<char> buf = serialize("foo", "bar");
    // flip a byte in the checksum (last 4 bytes)
    buf[buf.size() - 1] ^= 0xFF;
    Record out;
    bool ok = deserialize(buf.data(), buf.size(), out);
    assert(!ok);
    std::cout << "PASS: test_deserialize_corrupt_checksum\n";
}

void test_deserialize_corrupt_payload() {
    std::vector<char> buf = serialize("foo", "bar");
    // corrupt a byte in the middle of the payload
    buf[6] ^= 0xFF;
    Record out;
    bool ok = deserialize(buf.data(), buf.size(), out);
    assert(!ok);
    std::cout << "PASS: test_deserialize_corrupt_payload\n";
}

void test_deserialize_wrong_total_len() {
    std::vector<char> buf = serialize("foo", "bar");
    // overwrite total_len with a wrong value
    buf[0] = 0;
    buf[1] = 0;
    buf[2] = 0;
    buf[3] = 99;
    Record out;
    bool ok = deserialize(buf.data(), buf.size(), out);
    assert(!ok);
    std::cout << "PASS: test_deserialize_wrong_total_len\n";
}

void test_deserialize_truncated() {
    std::vector<char> buf = serialize("hello", "world");
    // pass only half the buffer
    Record out;
    bool ok = deserialize(buf.data(), buf.size() / 2, out);
    assert(!ok);
    std::cout << "PASS: test_deserialize_truncated\n";
}

void test_checksum_deterministic() {
    const char data[] = "test data";
    uint32_t c1 = compute_checksum(data, sizeof(data) - 1);
    uint32_t c2 = compute_checksum(data, sizeof(data) - 1);
    assert(c1 == c2);
    std::cout << "PASS: test_checksum_deterministic\n";
}

void test_checksum_differs_on_change() {
    char data[] = "test data";
    uint32_t c1 = compute_checksum(data, sizeof(data) - 1);
    data[0] = 'T';
    uint32_t c2 = compute_checksum(data, sizeof(data) - 1);
    assert(c1 != c2);
    std::cout << "PASS: test_checksum_differs_on_change\n";
}

void test_serialize_size() {
    std::vector<char> buf = serialize("abc", "defgh");
    // expected: 4 (total_len) + 4 (key_len) + 3 (key) + 4 (val_len) + 5 (val) + 4 (checksum) = 24
    assert(buf.size() == 24);
    std::cout << "PASS: test_serialize_size\n";
}

int main() {
    test_round_trip_basic();
    test_round_trip_empty_key();
    test_round_trip_empty_value();
    test_round_trip_both_empty();
    test_round_trip_large_data();
    test_deserialize_too_short();
    test_deserialize_corrupt_checksum();
    test_deserialize_corrupt_payload();
    test_deserialize_wrong_total_len();
    test_deserialize_truncated();
    test_checksum_deterministic();
    test_checksum_differs_on_change();
    test_serialize_size();

    std::cout << "\nAll record tests passed!\n";
    return 0;
}
