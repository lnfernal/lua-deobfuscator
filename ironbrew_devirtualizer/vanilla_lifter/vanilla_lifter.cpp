#include "vanilla_lifter.hpp"

namespace deobf::ironbrew_devirtualizer::vanilla_lifter::proto_translator {
	std::unique_ptr<vm_arch::vanilla_proto> translate_chunk(vm_arch::proto* chunk, bool translate_protos) {
		auto new_proto = std::make_unique<vm_arch::vanilla_proto>();

		// todo renumber chunks? (linedefined, lastlinedfined)

		new_proto->is_vararg = chunk->is_vararg;
		new_proto->num_upvalues = chunk->nups;
		new_proto->num_parameters = chunk->num_params;
		new_proto->max_stack_size = chunk->max_stack_size; // todo resolve here on p->code or dynamic analysis?

		// translate embeded protos
		if (translate_protos)
			for (auto& proto : chunk->protos)
				new_proto->protos.emplace_back(translate_chunk(proto.get()));

		// convert line info
		new_proto->line_info.reserve(chunk->instructions.size());
		for (auto& instruction : chunk->instructions)
			new_proto->line_info.push_back(instruction->line_defined);

		// convert constants
		new_proto->constants.reserve(chunk->constants.size());
		for (auto& constant : chunk->constants) {
			std::cout << "KSS:" << constant->to_string() << std::endl;
			new_proto->constants.emplace_back(constant);
		}

		// convert code
		new_proto->code.reserve(chunk->instructions.size()); // we aren't creating any new instructions during transpilation
		for (auto pc = 0ul; pc < chunk->instructions.size(); ++pc) {
			// todo fix
			//if (chunk->instructions.at(pc).get()->op == vm_arch::opcode::op_invalid) // skip for now
			//	continue;
			chunk->instructions.at(pc)->print();

			auto new_instruction = instruction_translator::convert_instruction(chunk->instructions.at(pc).get(), pc);

			std::cout << static_cast<int>(new_instruction->opcode) << std::endl;
			new_proto->code.emplace_back(std::move(new_instruction));
		}

		return std::move(new_proto);
	}
}