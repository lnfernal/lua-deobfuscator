#include "auxiliaries.hpp"

namespace deobf::vm_arch::aux {
    static const std::unordered_map<vm_arch::opcode, vm_arch::opcode> kst_optimized_bidict {
        { vm_arch::opcode::op_ne, vm_arch::opcode::op_eq },
        { vm_arch::opcode::op_ne1, vm_arch::opcode::op_eq1 },
        { vm_arch::opcode::op_ne2, vm_arch::opcode::op_eq2 },
        { vm_arch::opcode::op_ne3, vm_arch::opcode::op_eq3 },

        { vm_arch::opcode::op_ge, vm_arch::opcode::op_lt },
        { vm_arch::opcode::op_ge1, vm_arch::opcode::op_lt1 },
        { vm_arch::opcode::op_ge2, vm_arch::opcode::op_lt2 },
        { vm_arch::opcode::op_ge3, vm_arch::opcode::op_lt3 },

        { vm_arch::opcode::op_gt, vm_arch::opcode::op_le },
        { vm_arch::opcode::op_gt1, vm_arch::opcode::op_le1 },
        { vm_arch::opcode::op_gt2, vm_arch::opcode::op_le2 },
        { vm_arch::opcode::op_gt3, vm_arch::opcode::op_le3 },

        { vm_arch::opcode::op_test1, vm_arch::opcode::op_test },
        { vm_arch::opcode::op_testset1, vm_arch::opcode::op_testset },

        // bidrectional mapping
        { vm_arch::opcode::op_lt, vm_arch::opcode::op_ge },
        { vm_arch::opcode::op_lt1, vm_arch::opcode::op_ge1 },
        { vm_arch::opcode::op_lt2, vm_arch::opcode::op_ge2 },
        { vm_arch::opcode::op_lt3, vm_arch::opcode::op_ge3 },

        { vm_arch::opcode::op_le, vm_arch::opcode::op_gt },
        { vm_arch::opcode::op_le1, vm_arch::opcode::op_gt1 },
        { vm_arch::opcode::op_le2, vm_arch::opcode::op_gt2 },
        { vm_arch::opcode::op_le3, vm_arch::opcode::op_gt3 },

        { vm_arch::opcode::op_eq, vm_arch::opcode::op_ne },
        { vm_arch::opcode::op_eq1, vm_arch::opcode::op_ne1 },
        { vm_arch::opcode::op_eq2, vm_arch::opcode::op_ne2 },
        { vm_arch::opcode::op_eq3, vm_arch::opcode::op_ne3 },

        { vm_arch::opcode::op_test, vm_arch::opcode::op_test1 },
        { vm_arch::opcode::op_testset, vm_arch::opcode::op_testset1 },
    };


    static const std::unordered_map<vm_arch::opcode, vm_arch::opcode> normalized_opcode_map {
        { vm_arch::opcode::op_eq, vm_arch::opcode::op_eq },
        { vm_arch::opcode::op_eq1, vm_arch::opcode::op_eq },
        { vm_arch::opcode::op_eq2, vm_arch::opcode::op_eq },
        { vm_arch::opcode::op_eq3, vm_arch::opcode::op_eq },

        { vm_arch::opcode::op_ge, vm_arch::opcode::op_ge },
        { vm_arch::opcode::op_ge1, vm_arch::opcode::op_ge },
        { vm_arch::opcode::op_ge2, vm_arch::opcode::op_ge },
        { vm_arch::opcode::op_ge3, vm_arch::opcode::op_ge },

        { vm_arch::opcode::op_gt1, vm_arch::opcode::op_gt },
        { vm_arch::opcode::op_gt2, vm_arch::opcode::op_gt },
        { vm_arch::opcode::op_gt3, vm_arch::opcode::op_gt },

        { vm_arch::opcode::op_lt, vm_arch::opcode::op_lt },
        { vm_arch::opcode::op_lt1, vm_arch::opcode::op_lt },
        { vm_arch::opcode::op_lt2, vm_arch::opcode::op_lt },
        { vm_arch::opcode::op_lt3, vm_arch::opcode::op_lt },

        { vm_arch::opcode::op_le, vm_arch::opcode::op_le },
        { vm_arch::opcode::op_le1, vm_arch::opcode::op_le },
        { vm_arch::opcode::op_le2, vm_arch::opcode::op_le },
        { vm_arch::opcode::op_le3, vm_arch::opcode::op_le },

        { vm_arch::opcode::op_ne, vm_arch::opcode::op_ne },
        { vm_arch::opcode::op_ne1, vm_arch::opcode::op_ne },
        { vm_arch::opcode::op_ne2, vm_arch::opcode::op_ne },
        { vm_arch::opcode::op_ne3, vm_arch::opcode::op_ne },
    };

    vm_arch::opcode get_kst_optimized_logic_opcode(vm_arch::opcode original, bool is_kb, bool is_kc) {
        // or could mask op with is_ka AND is_kc, unfortunately this is a bad idea since we need those opcodes for other stuff
        switch (original) {
            case vm_arch::opcode::op_lt: {
                if (is_kb && is_kc) {
                    return vm_arch::opcode::op_lt3;
                }
                else if (is_kc) {
                    return vm_arch::opcode::op_lt2;
                }
                else if (is_kb) {
                    return vm_arch::opcode::op_lt1;
                }

                break;
            }
            case vm_arch::opcode::op_ge: {
                if (is_kb && is_kc) {
                    return vm_arch::opcode::op_ge3;
                }
                else if (is_kc) {
                    return vm_arch::opcode::op_ge2;
                }
                else if (is_kb) {
                    return vm_arch::opcode::op_ge1;
                }

                break;
            }

            case vm_arch::opcode::op_le: {
                if (is_kb && is_kc) {
                    return vm_arch::opcode::op_le3;
                }
                else if (is_kc) {
                    return vm_arch::opcode::op_le2;
                }
                else if (is_kb) {
                    return vm_arch::opcode::op_le1;
                }

                break;
            }
            case vm_arch::opcode::op_gt: {
                if (is_kb && is_kc) {
                    return vm_arch::opcode::op_gt3;
                }
                else if (is_kc) {
                    return vm_arch::opcode::op_gt2;
                }
                else if (is_kb) {
                    return vm_arch::opcode::op_gt1;
                }

                break;
            }

            case vm_arch::opcode::op_ne: {
                if (is_kb && is_kc) {
                    return vm_arch::opcode::op_ne3;
                }
                else if (is_kc) {
                    return vm_arch::opcode::op_ne2;
                }
                else if (is_kb) {
                    return vm_arch::opcode::op_ne1;
                }

                break;
            }
            case vm_arch::opcode::op_eq: {
                if (is_kb && is_kc) {
                    return vm_arch::opcode::op_eq3;
                }
                else if (is_kc) {
                    return vm_arch::opcode::op_eq2;
                }
                else if (is_kb) {
                    return vm_arch::opcode::op_eq1;
                }

                break;
            }
        }

        return original;
    }
    
    vm_arch::opcode get_inverse_kst_optimized_logic_opcode(vm_arch::opcode original) {
        if (auto result = kst_optimized_bidict.find(original); result != kst_optimized_bidict.cend())
            return result->second;

        return original;
    }

    bool is_kst_optimized_opcode(vm_arch::opcode original) {
        if (auto result = kst_optimized_bidict.find(original); result != kst_optimized_bidict.cend()) {
            return (result->second >= vm_arch::opcode::op_eq && result->second <= vm_arch::opcode::op_testset);
        }

        return false;
    }


    vm_arch::opcode get_normalized_opcode(vm_arch::opcode original) {
        if (auto result = normalized_opcode_map.find(original); result != normalized_opcode_map.cend()) {
            return result->second;
        }

        return original;
    }
}
