#include "instruction.hpp"

namespace deobf::vm_arch {
	// TODO: couldv'e checked enum ranges instead with switch/cmp 
	static inline std::unordered_set<vm_arch::opcode> logic_operations{
		// FLAG A == 0
		vm_arch::opcode::op_eq,
		vm_arch::opcode::op_eq1,
		vm_arch::opcode::op_eq2,
		vm_arch::opcode::op_eq3,

		vm_arch::opcode::op_lt,
		vm_arch::opcode::op_lt1,
		vm_arch::opcode::op_lt2,
		vm_arch::opcode::op_lt3,

		vm_arch::opcode::op_le,
		vm_arch::opcode::op_le1,
		vm_arch::opcode::op_le2,
		vm_arch::opcode::op_le3,

		// SAME INSTRUCTION, FLAG A == 1
		vm_arch::opcode::op_ne,
		vm_arch::opcode::op_ne1,
		vm_arch::opcode::op_ne2,
		vm_arch::opcode::op_ne3,

		vm_arch::opcode::op_ge,
		vm_arch::opcode::op_ge1,
		vm_arch::opcode::op_ge2,
		vm_arch::opcode::op_ge3,

		vm_arch::opcode::op_gt,
		vm_arch::opcode::op_gt1,
		vm_arch::opcode::op_gt2,
		vm_arch::opcode::op_gt3
	};

	bool instruction::operator==(const instruction& other) const {
		return (op == op &&
			type == type &&
			a == a &&
			b == b &&
			c == c &&
			bx == bx &&
			sbx == sbx
			);
	}

	std::size_t instruction::get_target_pc(std::size_t pc_loc) const {
		if (op == vm_arch::opcode::op_tforloop || op == vm_arch::opcode::op_forloop || op == vm_arch::opcode::op_forprep || op == vm_arch::opcode::op_jmp) {
			return sbx; //pc_loc += sbx;
		}
		else if (logic_operations.count(op)) {
			return pc_loc + 1;
		}
		else if (op == vm_arch::opcode::op_test ||
			op == vm_arch::opcode::op_test1 ||
			op == vm_arch::opcode::op_testset ||
			op == vm_arch::opcode::op_testset1) {
			return pc_loc + 1;
		}
		else if (op == vm_arch::opcode::op_loadbool1) {
			return pc_loc + 1;
		}
	}

	bool instruction::is_unconditional_jump() const {
		if (logic_operations.count(op))
			return true;

		return (op == vm_arch::opcode::op_jmp ||
			op == vm_arch::opcode::op_test ||
			op == vm_arch::opcode::op_test1 ||
			op == vm_arch::opcode::op_testset ||
			op == vm_arch::opcode::op_testset1 ||
			op == vm_arch::opcode::op_forloop ||
			op == vm_arch::opcode::op_tforloop ||
			op == vm_arch::opcode::op_forprep ||
			op == vm_arch::opcode::op_loadbool1 // c != 0
			);
	}
	
	std::bitset<64> instruction::get_constant_targets() const {
		std::bitset<64> result{ };

		if (is_ka) {
			result |= a;
		}

		if (is_kb) {
			if (b > 0) {
				result |= b;
			}
			else {
				result |= bx;
			}
		}

		if (is_kc) {
			result |= c;
		}
		return result;
	}

	// debugging OBV not being used anywhere
	void instruction::print() const {

		switch (this->type) {
			/*case instruction_type::a: {
				std::cout << vm_arch::opcode_map[this->op] << ' ' << static_cast<long>(this->a) << std::endl;

				break;
			}*/

			/*case instruction_type::ab: {
				std::cout << vm_arch::opcode_map[this->op] << ' ' << static_cast<long>(this->a) << ' ' << static_cast<long>(this->b) << std::endl;

				break;
			}*/
		case instruction_type::abc: {
			std::cout << vm_arch::opcode_map[this->op] << ' ' << static_cast<long>(this->a) << ' ' << static_cast<long>(this->b) << ' ' << static_cast<long>(this->c) << std::endl;

			break;
		}
		case instruction_type::abx: {
			std::cout << vm_arch::opcode_map[this->op] << ' ' << static_cast<long>(this->a) << ' ' << static_cast<long>(this->bx) << std::endl;

			break;
		}
		case instruction_type::asbx: {
			std::cout << vm_arch::opcode_map[this->op] << ' ' << static_cast<long>(this->a) << ' ' << static_cast<long>(this->sbx) << std::endl;

			break;
		}
		case instruction_type::asbxc: {
			std::cout << vm_arch::opcode_map[this->op] << ' ' << static_cast<long>(this->a) << ' ' << static_cast<long>(this->sbx) << ' ' << static_cast<long>(this->c) << std::endl;

			break;
		}
		default:
			throw std::runtime_error("invalid instr type blah blah");
		}

	}
}