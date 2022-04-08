#pragma once

#include <string_view>
#include <cstdlib>
#include <unordered_set>
#include <iostream>
#include <unordered_map>
#include <algorithm>

#include <sstream>

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
	// todo class singleton with static functions instead?
	namespace compression_utilities {
		const std::string decompress_vm_string(std::string_view vm_string); // uses some cringe impelemenation LZW compression
		const std::string unescape_vm_string(std::string_view vm_string);
	}
}