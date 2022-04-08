#pragma once
#include "vm_arch/control_flow_graph.hpp"
#include "vm_arch/basic_block.hpp"
#include "vm_arch/proto.hpp"
#include "decryption_block_transformer.hpp"

#include <queue>
#include "../static_chunk_analysis.hpp"

namespace deobf::ironbrew_devirtualizer::static_chunk_analysis {
	using namespace vm_arch;

	struct static_chunk_analysis;

	// todo interact with static_chunk_analysis as API for constant moving/elimination (managment).
	namespace constant_decryption {
		struct constant_decryption_recongizer final {
			static_chunk_analysis& analyzer;

			std::queue<decryption_block_transformer> block_transformers;

			static inline std::string decrypt_string(const std::string& parameter, const std::string& round_xor_key);
			void run();

			explicit constant_decryption_recongizer(static_chunk_analysis& analyzer) :
				analyzer(analyzer)
			{ };
		};
	}
}