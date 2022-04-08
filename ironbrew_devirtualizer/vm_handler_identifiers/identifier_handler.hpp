#pragma once
#include "./ast/optimization_passes/base_optimizer.hpp"
#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"

#include "../../vm_arch/opcode.hpp"

// the following implementation uses the chain-of-responsibility design pattern to handle virtual opcodes and priority might be changed at runtime when we update to make it depend on stuff

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	// basic handler interface
	struct handler_interface {
	public: // todo: operator= instead of set_next_handler?
		virtual handler_interface* set_next_handler(std::unique_ptr<handler_interface>&& handler) = 0; // forward function
		virtual vm_arch::opcode handle(ir::statement::block* path) = 0;

		virtual ~handler_interface() noexcept = default;
	};


	// abstract handlers
	struct basic_opcode_handler : public handler_interface {
		handler_interface* set_next_handler(std::unique_ptr<handler_interface>&& handler) override;

		explicit basic_opcode_handler() :
			next_handler{ nullptr }
		{ }

		vm_arch::opcode handle(ir::statement::block* path) override;
	private:
		std::unique_ptr<handler_interface> next_handler;
	};
}