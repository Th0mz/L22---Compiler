  #ifndef __L22_AST_NULL_PTR_NODE_H__
#define __L22_AST_NULL_PTR__NODE_H__

#include <cdk/ast/typed_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/types/basic_type.h>

namespace l22 {

  /**
   * Class for describing null pointer nodes.
   */
  class nullptr_node: public cdk::expression_node {
  public:
    inline nullptr_node(int lineno) :
        cdk::expression_node(lineno) {
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_nullptr_node(this, level);
    }

  };

} // l22

#endif