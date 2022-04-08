#pragma once

#include <variant>
#include <unordered_map>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "statement.hpp"
#include "node.hpp"

/*
exp
    : 'nil' | 'false' | 'true'
    | number
    | string
    | '...' (parlist useless)
    | functiondef
    | prefixexp
    | tableconstructor
    | <assoc=right> exp operatorPower exp
    | operatorUnary exp
    | exp operatorMulDivMod exp
    | exp operatorAddSub exp
    | <assoc=right> exp operatorStrcat exp
    | exp operatorComparison exp
    | exp operatorAnd exp
    | exp operatorOr exp
    | exp operatorBitwise exp
*/

namespace deobf::ast::ir::expression {
    using statement_t = typename statement::statement;
    
    struct expression : public statement_t {
        virtual ~expression() = default;
    };

    // on visitor pattern method, we enumerate each element of the vector and visit it.
    using expression_list_t = std::vector<std::shared_ptr<expression>>;

    struct name_and_args final : public expression { // name:?(body)
        std::string name;
        expression_list_t body;

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        explicit name_and_args(const std::string& name, expression_list_t body) :
            name(name),
            body(std::move(body))
        { }

        void accept(abstract_visitor_pattern* visitor) override;
    };

    using name_and_args_t = std::vector<std::shared_ptr<name_and_args>>;

    struct variable_suffix final : public expression {
        /*
        varSuffix
            : nameAndArgs* ('[' exp ']' | '.' NAME)
            ;
        */
        name_and_args_t name_and_args{ };

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        std::shared_ptr<expression> name; // um

        explicit variable_suffix(std::shared_ptr<expression> name, name_and_args_t name_and_args) :
            name(std::move(name)),
            name_and_args(std::move(name_and_args))
        { }

        void accept(abstract_visitor_pattern* visitor) override;
    };

    struct variable final : public expression {
        std::shared_ptr<expression> name;
        std::vector<std::shared_ptr<variable_suffix>> suffixes;

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        explicit variable(std::shared_ptr<expression> name, std::vector<std::shared_ptr<variable_suffix>> suffixes) :
            name(std::move(name)),
            suffixes(std::move(suffixes))
        { }

        void accept(abstract_visitor_pattern* visitor) override;
    };

    using variable_list_t = std::vector<std::shared_ptr<expression>>;  // variables are expressions aswell, but for more context make variable_list_t

    struct literal_base : public expression { // null object.
        std::string to_string() const override = 0;

        [[nodiscard]] bool equals(const node* other_node) const override = 0;

        void accept(abstract_visitor_pattern* visitor) override = 0;
    };

    struct nil_literal final : public literal_base {
        std::string to_string() const final override {
            return "nil";
        }

        [[nodiscard]] bool equals(const node* other_node) const final override {
            return dynamic_cast<const nil_literal*>(other_node) != nullptr;
        }

        void accept(abstract_visitor_pattern* visitor) final override;
    };

    struct numeral_literal final : public literal_base {
        double value; // lua numbers are double (IEEE754, info for decoding: sign=most signifcant bit, mantissa=52bits)

        std::string to_string() const final override {
            std::stringstream stream;

            stream << value; // todo deal with FPEs?

            return stream.str(); // better than retarded to_string
        }

        [[nodiscard]] bool equals(const node* other_node) const final override;

        explicit numeral_literal(const std::string& value) : value(std::stod(value)) { };
        explicit numeral_literal(double value) : value(value) { };

        void accept(abstract_visitor_pattern* visitor) final override;
    };

    struct array_index_literal final : public literal_base { // used to indicate array part indices (go till MAX_UINT)
        std::size_t value;

        std::string to_string() const final override {
            std::stringstream stream;

            stream << value; // todo deal with FPEs?

            return stream.str(); // better than retarded to_string
        }

        [[nodiscard]] bool equals(const node* other_node) const final override {
            return false;
        }

        explicit array_index_literal(std::size_t value) : value(value) { };

        void accept(abstract_visitor_pattern* visitor) final override;
    };

    struct boolean_literal final : public literal_base {
        bool value;

        std::string to_string() const final override {
            return value ? "true" : "false";
        }

        [[nodiscard]] bool equals(const node* other_node) const final override;

        explicit boolean_literal(bool value) : value(value) { };

        void accept(abstract_visitor_pattern* visitor) final override;
    };

    struct string_literal final : public literal_base {
        std::string value;

        std::string to_string() const final override {
            return value;
        }

        [[nodiscard]] bool equals(const node* other_node) const final override;

        explicit string_literal(const std::string& value) : value(value) { }

        void accept(abstract_visitor_pattern* visitor) final override;
    };

    using name_list_t = std::vector<std::shared_ptr<string_literal>>; // we don't need to handle varargs, since antlr counts them as terminal symbols so we can push into our name list.

    struct binary_expression final : public expression {
        enum class operation_t { // todo luau operations ?
            // not a operation
            none,

            // basic math operations
            add, // +
            sub, // -
            div, // /
            mul, // *
            mod, // %
            pow, // ^ ( not xor )

            // string manip operations
            concat, // ..

            // comparison operations
            gt, // >
            lt, // <
            ge, // >=
            le, // <=
            eq, // ==
            neq, // ~=

            // logical operations
            and,
            or ,
        } operation;

        static const std::unordered_map<operation_t, std::string> operation_map;

        std::shared_ptr<expression> left, right; // bin LHS, RHS

        [[nodiscard]] auto is_communicative_op() const noexcept {
            return operation == operation_t::add || operation == operation_t::mul;
        }

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override {
            if (auto result = dynamic_cast<const binary_expression*>(other_node))
                return std::tie(operation, left, right) == std::tie(result->operation, result->left, result->right);

            return false;
        }

        std::vector<std::shared_ptr<node>> get_children() const override;

        // polish notation constructor
        explicit binary_expression(operation_t operation, std::shared_ptr<expression> left, std::shared_ptr<expression> right) : operation(operation),
            left(std::move(left)),
            right(std::move(right))
        { }

        void accept(abstract_visitor_pattern* visitor) override;
    };

    struct unary_expression final : public expression {
        enum class operation_t { // using unary_expr = std::variant<std::monostate, not, len, minus>
            none, // not a operation
            not,
            len,
            minus,
        } operation;

        static const std::unordered_map<operation_t, std::string> operation_map;

        std::shared_ptr<expression> body;

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        explicit unary_expression(operation_t operation, std::shared_ptr<expression> body) :
            operation(operation),
            body(std::move(body))
        { }

        void accept(abstract_visitor_pattern* visitor) override;
    };

    struct table final : public expression { // called table constructor by lua's PIL
        std::vector<std::pair<std::shared_ptr<expression>, std::shared_ptr<expression>>> entries{ }; // key also can't be a nil_literal, todo safety?
        std::size_t array_size;

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        explicit table(std::vector<std::pair<std::shared_ptr<expression>, std::shared_ptr<expression>>> entries, std::size_t array_size) :
            entries(std::move(entries)),
            array_size(array_size)
        { };

        void accept(abstract_visitor_pattern* visitor) override;
    };

    struct function_call final : public expression {
        std::optional<std::shared_ptr<expression>> name; // or expression
        name_and_args_t arguments{ };

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        explicit function_call(name_and_args_t arguments) :
            arguments(std::move(arguments))
        { };

        explicit function_call(std::shared_ptr<expression> name, name_and_args_t arguments) :
            name(std::move(name)),
            arguments(std::move(arguments))
        { };

        void accept(abstract_visitor_pattern* visitor) override;
    };

    struct function final : public expression_t { // a function prototype. (in statements because defining a function can be both)
        std::optional<std::string> function_name; // optional since a function could be anonymous on a variable list, we can use the has_value method.
        
        //std::string static_symbol;

        ir::expression::name_list_t parameters{ };
        std::shared_ptr<ir::statement::block> body;

        std::string to_string() const override;

        [[nodiscard]] bool equals(const node* other_node) const override;

        std::vector<std::shared_ptr<node>> get_children() const override;

        enum class function_type {
            anonymous_t, // passed on varlist for example, in roblox : signal:Connect(function(...) end)
            variable_t, // local x = function() end
            local_t,
            global_t,
        } type;

        explicit function(function_type type, ir::expression::name_list_t parameters, std::shared_ptr<ir::statement::block> body, const std::string& name) :
            type(type),
            parameters(std::move(parameters)),
            body(std::move(body)),
            function_name(name)
        { };

        explicit function(function_type type, ir::expression::name_list_t parameters, std::shared_ptr<ir::statement::block> body) :
            type(type),
            parameters(std::move(parameters)),
            body(std::move(body))
        { };

        void accept(abstract_visitor_pattern* visitor) override;
    };
}