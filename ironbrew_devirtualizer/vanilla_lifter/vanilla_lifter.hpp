#pragma once

#include "instruction_translator.hpp"
#include "vm_arch/proto.hpp"
#include "vm_arch/vanilla_proto.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter::proto_translator {
	std::unique_ptr<vm_arch::vanilla_proto> translate_chunk(vm_arch::proto* chunk, bool = false);
}