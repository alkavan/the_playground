#include <memory>
#include <cstdint>
#include <print>
#include <array>

// Temporary fallback until GCC implements std::is_within_lifetime
template<typename T>
bool is_within_lifetime(const T* p) {
    return p != nullptr;
}

struct PacketHeader {
    uint32_t magic;
    uint16_t version;
    uint16_t payload_size;
    uint32_t checksum;
};

struct DataPayload {
    float temperature;
    float humidity;
};

uint32_t crc32(const uint8_t* data, const size_t length)
{
    static constexpr std::array<uint32_t, 256> table = []() {
        std::array<uint32_t, 256> t{};
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t crc = i;
            for (int j = 0; j < 8; ++j) {
                crc = (crc >> 1) ^ (0xEDB88320u * (crc & 1));
            }
            t[i] = crc;
        }
        return t;
    }();

    uint32_t crc = 0xFFFFFFFFu;

    for (size_t i = 0; i < length; ++i) {
        crc = table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8);
    }

    return ~crc;  // final XOR-out value
}

int main() {
    alignas(8) uint8_t raw_packet[128] = {
        //>> Header data
        0xEF, 0xBE, 0xAD, 0xDE,   // magic = 0xDEADBEEF
        0x01, 0x00,               // version
        0x10, 0x00,               // payload_size
        0xF0, 0xB9, 0x68, 0x04,   // checksum = 0x468B9F0

        //>> Payload data
        0x00, 0x00, 0x48, 0x42,  // temperature = 50.0f
        0x00, 0x00, 0x48, 0x42,  // humidity = 50.0f
    };

    // Read payload data header
    auto* header = std::start_lifetime_as<PacketHeader>(raw_packet);

    if ( ! is_within_lifetime(header)) {
        std::println("Failed to start lifetime for header");
        return 1;
    }

    std::println("-- HEADER");
    std::println("Magic: 0x{:X}", header->magic);
    std::println("Version: {}", header->version);
    std::println("Payload size: {}", header->payload_size);
    std::println("Checksum: 0x{:08X}", header->checksum);

    if (header->magic != 0xDEADBEEF) {
        std::println("Invalid magic number!");
        return 1;
    }

    if (uint32_t crc32_sum = crc32(raw_packet + 12, 8); header->checksum != crc32_sum) {
        std::println("Wrong data checksum: 0x{:08X}", crc32_sum);
        return 1;
    }

    // Read example sensor data
    auto* data = std::start_lifetime_as<DataPayload>(raw_packet + sizeof(PacketHeader));

    if ( ! is_within_lifetime(data)) {
        std::println("Failed to start lifetime for sensor data");
        return 1;
    }
    std::println("\n-- PAYLOAD DATA");
    std::println("Temperature: {}°C", data->temperature);
    std::println("Humidity: {}%", data->humidity);

    return 0;
}
