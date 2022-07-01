#ifndef __L22_AST_SWEEP_NODE_H__
#define __L22_AST_SWEEP_NODE_H__

#include <cdk/ast/basic_node.h>

namespace l22 {

  /**
   * Class for describing stop nodes.
   */
  class sweep_node: public cdk::basic_node {
    cdk::expression_node *_vector;
    cdk::expression_node *_function;
    cdk::expression_node *_low;
    cdk::expression_node *_high;
    

  public:
    inline sweep_node(int lineno, cdk::expression_node *vector, cdk::expression_node *function,
      cdk::expression_node *low, cdk::expression_node *high) :
        cdk::basic_node(lineno), _vector(vector), _function(function), _low(low), _high(high)  {
    }

    inline cdk::expression_node *vector() {
      return _vector;
    }
    inline cdk::expression_node *function() {
      return _function;
    }
    inline cdk::expression_node *low() {
      return _low;
    }
    inline cdk::expression_node *high() {
      return _high;
    }

  public:
    void accept(basic_ast_visitor *sp, int level) {
      sp->do_sweep_node(this, level);
    }

  };

} // l22

#endif
