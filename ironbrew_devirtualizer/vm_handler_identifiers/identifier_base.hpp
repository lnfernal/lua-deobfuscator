#pragma once
#include "./ast/optimization_passes/base_optimizer.hpp"
#include "./ast/ir/abstract_visitor_pattern.hpp"
#include "./ast/ir/node.hpp"

#include "identifier_common.hpp"
#include "identifier_handler.hpp"

/*
	opcode regexes (not being used here, for lazy people) :
	   {ARTIMETIC_INSTRUCTION, { R"(\b(L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\s=\s([L_\d\[\]\s]+(?=([+*\/\-%])))\3\s(L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])[\r\n;])", "$1=artimetic_instr_handler($2,\'$3\',$4)" }},
	   {LEN_INSTRUCTION, { R"((L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\s*?=\s*?#(L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])[\r\n;])", "$1=len_handler($2)"}},
	   {CONCAT_INSTRUCTION, { R"(\bfor\s*?L_\d+_forvar\d+\s*?=\s*?L_\d+_\s*?(?:\+\s*?1)?,\s*?(?:L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\s*?do\s+(L_\d+_)\s*?=\s*?\1\s\.{2}\s*?L_\d+_\[L_\d+_forvar\d+\][;]?\s+end[;\r\n])", "" }},
	   {GETTABLE_INSTRUCTION, { R"(\b(L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\s*?=\s*?(L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\[(L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\][\r\n;])", "$1=gettable_handler($2,$3)" }},
	   {SETTABLE_INSTRUCTION, { R"(\b(L_\d+_\[L_\d+_\[L_\d+_(?:\[L_\d+_\])?\]\])\[(L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\]\s*?=\s*?(L_\d+_\[L_\d+_(?:\[L_\d+_\])?\]))", "" }},
	   {CALL_INSTRUCTION, { R"((L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\((?:(L_\d+_\[L_\d+_\s?\+\s?1\])?|(L_\d+_\(L_\d+_,\s*?L_\d+_\s*?\+\s*?1,\s*?(?:L_\d+_\[L_\d+_(?:\[L_\d+_\])?\]))?)\))", "todo" }},
	   {COMPARE_INSTRUCTION, { R"(\bif\s*?\((L_\d+_\[L_\d+_\[L_\d+_(?:\[L_\d+_\])?\]\])\s*?(~=|==|>|<|>=|<=)\s*?(L_\d+_\[L_\d+_\[L_\d+_(?:\[L_\d+_\])?\]\])\)\s*?then\s+L_\d+_\s*?=\s*?L_\d+_\[L_\d+_\][\r\n;]\s*?end)", "" }},
	   {IF_STMT_INSTRUCTION, { R"(\bif\s*?\((L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\)\s*?then\s+L_\d+_\s*?=\s*?L_\d+_\[L_\d+_\][;\r\n]\s*?end)", "" }},
	   {NOT_STMT_INSTRUCTION, { R"(\bif\s*?\(not\((L_\d+_\[L_\d+_(?:\[L_\d+_\])?\])\)\)\s*?then\s+L_\d+_\s*?=\s*?L_\d+_\[L_\d+_\][;\r\n]\s*?end)", "" }}
*/

namespace deobf::ironbrew_devirtualizer::vm_handler_identifiers {
	using namespace ast;

	struct opcode_identifier_client final {
		std::unique_ptr<basic_opcode_handler> handler;
		std::map<ir::statement::block*, vm_arch::opcode> memoized_paths;

		explicit opcode_identifier_client();// sets up the chain

		vm_arch::opcode handle(ir::statement::block* path);
	};
}