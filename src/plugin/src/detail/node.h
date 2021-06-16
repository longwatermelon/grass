#pragma once
#include <string>
#include <memory>
#include <vector>


namespace plugin
{
    enum class NodeType
    {
        FUNCTION_CALL,
        VARIABLE,
        VARIABLE_DEFINITION,
        INT,
        STRING,
        COMPOUND,
        NOOP
    };

    struct Node
    {
        Node(NodeType type) : type(type) {}

        NodeType type;

        // function call
        std::string function_call_name;
        std::vector<std::unique_ptr<Node>> function_call_args; 

        // string
        std::string string_value;

        // int
        int int_value{ 0 };

        // compound
        std::vector<std::unique_ptr<Node>> compound_value;

        // variable definition
        std::string variable_definition_name;
        std::unique_ptr<Node> variable_definition_value;

        // variable
        std::string variable_name;
    }; 
}
