#pragma once

#include <variant>
#include <string>
#include <memory>
#include <functional>
#include <map>

#include "instruction.hpp"


namespace deobf::vm_arch::aux {
	vm_arch::opcode get_kst_optimized_logic_opcode(vm_arch::opcode original, bool is_kb, bool is_kc);
	bool is_kst_optimized_opcode(vm_arch::opcode original);
	vm_arch::opcode get_inverse_kst_optimized_logic_opcode(vm_arch::opcode original);
	vm_arch::opcode get_normalized_opcode(vm_arch::opcode original);
}