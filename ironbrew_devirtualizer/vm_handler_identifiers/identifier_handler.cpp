#include "identifier_handler.hpp"

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	handler_interface* basic_opcode_handler::set_next_handler(std::unique_ptr<handler_interface>&& handler) {
		next_handler = std::move(handler);
		return next_handler.get();
	}

	vm_arch::opcode basic_opcode_handler::handle(ir::statement::block* path) {
		vm_arch::opcode result{ };

		if (next_handler)
			result = next_handler->handle(path);

		return result;
	}
}