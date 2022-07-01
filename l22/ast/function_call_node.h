#ifndef __L22_AST_FUNCTION_CALL_NODE_H__
#define __L22_AST_FUNCTION_CALL_NODE_H__

#include <string>
#include <cdk/ast/basic_node.h>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/nil_node.h>
#include <cdk/ast/expression_node.h>

namespace l22 {

    /**
     * Class for describing function call nodes.
     */
    class function_call_node : public cdk::expression_node {
        cdk::expression_node *_pointer;
        cdk::sequence_node *_arguments;

    public:
        function_call_node(int lineno, cdk::expression_node *pointer) : 
          cdk::expression_node(lineno), _pointer(pointer), _arguments(new cdk::sequence_node(lineno)) {
        }

        function_call_node(int lineno, cdk::expression_node *pointer, cdk::sequence_node *arguments) : 
          cdk::expression_node(lineno), _pointer(pointer), _arguments(arguments) {
        }

    public:
        cdk::expression_node* pointer () {
            return _pointer;
        }

        cdk::sequence_node* arguments() {
            return _arguments;
        }

    public:
        void accept(basic_ast_visitor *sp, int level) {
            sp->do_function_call_node(this, level);
        }
    };
} // l22

#endif

