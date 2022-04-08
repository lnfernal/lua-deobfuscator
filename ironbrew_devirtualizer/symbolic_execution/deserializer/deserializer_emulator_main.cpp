#include "deserializer_emulator_main.hpp"
#include "vm_arch/proto.hpp"
#include "deserializer_context.hpp"

namespace deobf::ironbrew_devirtualizer::symbolic_execution::deserializer {
	std::unique_ptr<vm_arch::proto> deserializer_emulator_main::deserialize() {
        auto proto = std::make_unique<vm_arch::proto>();

        for (const auto current_order : deserializer_ctx->chunk_order) {
            switch (current_order) {
                case deserializer_enums::process_order::constants: {
                    const auto kst_max_size = deserializer_helper_object->get_bits<std::int32_t>();
                    for (auto i = 0l; i < kst_max_size; ++i) {
                        const auto randomized_order_type = deserializer_helper_object->get_bits<std::int8_t>();
                        if (const auto& kst_type = deserializer_ctx->constant_order_mapping.find(randomized_order_type); kst_type != deserializer_ctx->constant_order_mapping.cend()) {
                            switch (kst_type->second) {
                                case deserializer_enums::constant_type::string: {
                                    auto x = deserializer_helper_object->get_string();
                                    std::cout << "result:" << x << std::endl;
                                    proto->constants.push_back(std::make_unique<vm_arch::constant>(x));
                                    break;
                                }
                                case deserializer_enums::constant_type::number: {
                                    proto->constants.push_back(std::make_unique<vm_arch::constant>(deserializer_helper_object->get_float()));
                                    break;
                                }
                                case deserializer_enums::constant_type::boolean: {
                                    proto->constants.push_back(std::make_unique<vm_arch::constant>(static_cast<bool>(deserializer_helper_object->get_bits<std::int8_t>())));
                                    break;
                                }
                                default:
                                    throw std::runtime_error("unknown constant type? [deserializer]");
                            }
                        }
                        else {
                            proto->constants.push_back(std::make_unique<vm_arch::constant>(std::nullptr_t{ }));
                        }
                    }

                    break;
                }
                case deserializer_enums::process_order::instructions: {
                    // process instructions (not metamorphic)
                    const auto instruction_max_size = deserializer_helper_object->get_bits<std::int32_t>();

                    for (auto i = 0l; i < instruction_max_size; ++i) {
                        const auto instruction_descriptor = deserializer_helper_object->get_bits<std::int8_t>();
                        if (deserializer_helper_object->get_bits(instruction_descriptor, 1, 1) == 0) {
                            const auto instruction_bitfield_1 = deserializer_helper_object->get_bits(instruction_descriptor, 2, 3);
                            const auto instruction_bitfield_2 = deserializer_helper_object->get_bits(instruction_descriptor, 4, 6);

                            // straight pass = UB idiot
                            const auto virtual_opcode = deserializer_helper_object->get_bits<std::int16_t>();
                            const auto new_instruction_a = deserializer_helper_object->get_bits<std::int16_t>();

                            auto new_instruction = std::make_unique<vm_arch::instruction>(virtual_opcode, new_instruction_a);

                            const auto instruction_type = static_cast<enum vm_arch::instruction_type>(instruction_bitfield_1);
                            new_instruction->type = instruction_type;

                            switch (instruction_type) {
                                case vm_arch::instruction_type::abc: {
                                    new_instruction->b = deserializer_helper_object->get_bits<std::int16_t>();
                                    new_instruction->c = deserializer_helper_object->get_bits<std::int16_t>();
                                    break;
                                }
                                case vm_arch::instruction_type::abx: {
                                    new_instruction->bx = deserializer_helper_object->get_bits<std::int32_t>();
                                    break;
                                }
                                case vm_arch::instruction_type::asbx: {
                                    new_instruction->sbx = deserializer_helper_object->get_bits<std::int32_t>() - std::numeric_limits<unsigned short>::max() - 1;
                                    break;
                                }
                                case vm_arch::instruction_type::asbxc: {
                                    new_instruction->sbx = deserializer_helper_object->get_bits<std::int32_t>() - std::numeric_limits<unsigned short>::max() - 1;
                                    new_instruction->c = deserializer_helper_object->get_bits<std::int16_t>();
                                    break;
                                }
                            }
                            
                            const auto is_ka = deserializer_helper_object->get_bits(instruction_bitfield_2, 1); // on mutated ops such eq/lt/le
                            const auto is_kb = deserializer_helper_object->get_bits(instruction_bitfield_2, 2);
                            const auto is_kc = deserializer_helper_object->get_bits(instruction_bitfield_2, 3);

                            new_instruction->is_ka = is_ka;
                            new_instruction->is_kb = is_kb;
                            new_instruction->is_kc = is_kc;

                            proto->instructions.push_back(std::move(new_instruction));

                            //std::cout << new_instruction->virtual_opcode << " " << new_instruction->a << " " << new_instruction->b << " " << new_instruction->c << std::endl;
                        }
                    }
                    break;
                }
                case deserializer_enums::process_order::lineinfo: { // todo fix
                    //const auto lineinfo_max_size = deserializer_helper_object->get_bits<std::int32_t>();
                    //for (auto i = 0l; i < lineinfo_max_size; ++i) {
                    //    proto->instructions.at(i)->line_defined = deserializer_helper_object->get_bits<std::int32_t>(); //proto->lineinfo.push_back(deserializer_helper_object->get_bits<std::int32_t>());
                    //}

                    const auto lineinfo_max_size = deserializer_helper_object->get_bits<std::int32_t>();
                    for (auto i = 0ul; i < lineinfo_max_size; ++i) {
                        const auto line_defined = deserializer_helper_object->get_bits<std::int32_t>();

                        proto->lineinfo.push_back(line_defined);
                    }

                    break;
                }
                case deserializer_enums::process_order::protos: {
                    const auto proto_max_size = deserializer_helper_object->get_bits<std::int32_t>();
                    for (auto i = 0l; i < proto_max_size; ++i) {
                        proto->protos.emplace_back(deserialize());
                    }

                    break;
                }
                case deserializer_enums::process_order::parameters: {
                    proto->num_params = deserializer_helper_object->get_bits<std::int8_t>();
                    break;
                }
            }
        }

        return std::move(proto);
	}
}
