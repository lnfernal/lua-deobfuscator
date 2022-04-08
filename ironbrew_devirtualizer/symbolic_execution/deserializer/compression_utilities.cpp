#include "compression_utilities.hpp"

#include <string_view>

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
    // REWRITEETEITEITIE TODO
    const std::string compression_utilities::decompress_vm_string(std::string_view vm_string) { // shit code dealwit
        std::ostringstream result;
        std::istringstream compression_stream{ vm_string.data() };
        auto conversion_table = std::vector<std::string>{ };

        for (auto i = 0ul; i < 256ul; ++i)
            conversion_table.emplace_back(1, static_cast<char>(i));

        static const auto base36_map = std::string{ "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" };
        auto read_compressed_partial = [&compression_stream]() -> long {
            const auto read_offset = base36_map.find(static_cast<char>(compression_stream.get()));
            if (read_offset == -1)
                return -1;

            //std::unique_ptr<char[]> data_block{ new char[read_offset] }; // lazy to make_unique
            auto data_block = std::make_unique<char[]>(read_offset);
            compression_stream.read(data_block.get(), read_offset);

            return strtol(data_block.get(), nullptr, 36);
        };

        // ignore this shitty code ( took from ironbrew lua )
        {
            std::string current_character(1, static_cast<char>(read_compressed_partial()));
            for (std::string decompression_data; compression_stream.good(); result << current_character, current_character = decompression_data) {
                long current_writer = read_compressed_partial();
                if (current_writer < conversion_table.size())
                    decompression_data = conversion_table[current_writer];
                else
                    decompression_data = current_character + current_character.front(); // todo: .front() might result in UB?

                conversion_table.push_back(current_character + decompression_data.front());
            }
        }

        return result.str();
    }

    const std::string compression_utilities::unescape_vm_string(std::string_view vm_string) {
        std::stringstream unescaped;

        //unescaped.fill('0');

        std::string current_character;
        for (auto iterator = vm_string.cbegin() + 1; iterator != vm_string.cend(); ++iterator) {
            auto& character = (*iterator);
            if (character == '\\') {
                unescaped << static_cast<unsigned char>(std::stol(current_character));
                current_character.clear();
            }
            else {
                current_character += character;
            }
        }

        return unescaped.str();
    }
}
