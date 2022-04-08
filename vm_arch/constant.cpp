#include "constant.hpp"

namespace deobf::vm_arch {
	std::string constant::to_string() const {
		std::string result;

		switch (get_constant_type()) {
		case constant_type::number: {
			result = std::to_string(std::get<double>(value));
			break;
		}
		case constant_type::string: {
			result = std::get<std::string>(value);
			break;
		}
		case constant_type::boolean: {
			result = std::to_string(std::get<bool>(value));
			break;
		}
		default:
			std::cout << "nil\n";
			break;
		}

		return result;
	}
}