#include "proto.hpp"

namespace deobf::vm_arch {
	// DEBUG IGNORE SHIT CODE!! not being used anywhere tho

	std::string proto::print_chunk() const {
		auto sz = 0ul;
		for (auto& instr : instructions) {
			switch (instr->type) {
				/*case instruction_type::a: {
					std::cout << '[' << sz << "] : " << vm_arch::opcode_map[instr->op] << ' ' << static_cast<long>(instr->a) << std::endl;

					break;
				}

				case instruction_type::ab: {
					std::cout << '[' << sz << "] : " << vm_arch::opcode_map[instr->op] << ' ' << static_cast<long>(instr->a) << ' ' << static_cast<long>(instr->b) << std::endl;

					break;
				}*/
			case instruction_type::abc: {
				std::cout << '[' << sz << "] : " << vm_arch::opcode_map[instr->op] << ' ' << static_cast<long>(instr->a) << ' ' << static_cast<long>(instr->b) << ' ' << static_cast<long>(instr->c) << std::endl;

				break;
			}
			case instruction_type::abx: {
				std::cout << '[' << sz << "] : " << vm_arch::opcode_map[instr->op] << ' ' << static_cast<long>(instr->a) << ' ' << static_cast<long>(instr->bx) << std::endl;

				break;
			}
			case instruction_type::asbx: {
				std::cout << '[' << sz << "] : " << vm_arch::opcode_map[instr->op] << ' ' << static_cast<long>(instr->a) << ' ' << static_cast<long>(instr->sbx) << std::endl;

				break;
			}
			case instruction_type::asbxc: {
				std::cout << '[' << sz << "] : " << vm_arch::opcode_map[instr->op] << ' ' << static_cast<long>(instr->a) << ' ' << static_cast<long>(instr->sbx) << ' ' << static_cast<long>(instr->c) << std::endl;

				break;
			}
			default:
				throw std::runtime_error("invalid instr type blah blah");
			}

			sz++;
		}

		return "";
	}
}