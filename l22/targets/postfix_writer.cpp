#include <string>
#include <sstream>
#include "targets/type_checker.h"
#include "targets/postfix_writer.h"
#include ".auto/all_nodes.h"  // all_nodes.h is automatically generated

//---------------------------------------------------------------------------
void l22::postfix_writer::do_with_node(l22::with_node * const node, int lvl) {
}

void l22::postfix_writer::do_sweep_node(l22::sweep_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int cond = ++_lbl;
  int end = ++_lbl;
  

  node->low()->accept(this, lvl);
  _pf.DUP32();

  //while (<low> < <high>) {
  _pf.LABEL(mklbl(cond));
  node->high()->accept(this, lvl);
  _pf.LT();
  _pf.JZ(mklbl(end));

  _pf.DUP32();

  // vector[i] = function(vector[i]) 
  node->vector()->accept(this, lvl);
  _pf.INT(2);
  _pf.SHTL();
  _pf.ADD();

  _pf.DUP32();
  _pf.LDINT();
  _pf.CALL("funcName");

  _pf.STINT();
  
  // <low> = <low> + 1
  _pf.INT(1);
  _pf.ADD();

  _pf.JMP(mklbl(cond));
  _pf.LABEL(mklbl(end));


}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_nil_node(cdk::nil_node * const node, int lvl) {
  // EMPTY
}
void l22::postfix_writer::do_data_node(cdk::data_node * const node, int lvl) {
  // EMPTY
}
void l22::postfix_writer::do_double_node(cdk::double_node * const node, int lvl) {
  if (_global == 0) {
    // global variable declaration
    _pf.SDOUBLE(node->value());
    if (_debug) { std::cout << "SDOUBLE" << "value" << std::endl; }
  } else {
    _pf.DOUBLE(node->value());
    if (_debug) { std::cout << "DOUBLE" << "value" << std::endl; }
  }
}
void l22::postfix_writer::do_not_node(cdk::not_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  node->argument()->accept(this, lvl + 2);
  _pf.INT(0);
  if (_debug) { std::cout << "INT" << "0" << std::endl; }
  _pf.EQ();
  if (_debug) { std::cout << "EQ" << std::endl; }
}
void l22::postfix_writer::do_and_node(cdk::and_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this,lvl+2);
  _pf.DUP32();
  if (_debug) { std::cout << "DUP32" << std::endl; }
  _pf.JZ(mklbl(lbl));
  if (_debug) { std::cout << "JZ" << mklbl(lbl) << std::endl; }
  node->right()->accept(this,lvl+2);
  _pf.AND();
  if (_debug) { std::cout << "AND" << std::endl; }
  _pf.ALIGN();
  if (_debug) { std::cout << "ALIGN" << std::endl; }
  _pf.LABEL(mklbl(lbl));
  if (_debug) { std::cout << "LABEL" << mklbl(lbl) << std::endl; }
}
void l22::postfix_writer::do_or_node(cdk::or_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl = ++_lbl;
  node->left()->accept(this, lvl+2);
  _pf.DUP32();
  if (_debug) { std::cout << "DUP32" << std::endl; }
  _pf.JNZ(mklbl(lbl));
  node->right()->accept(this,lvl + 2);
  _pf.OR();
    if (_debug) { std::cout << "OR" << std::endl; }

  _pf.ALIGN();
    if (_debug) { std::cout << "ALIGN" << std::endl; }
  _pf.LABEL(mklbl(lbl));
    if (_debug) { std::cout << "LABEL" << mklbl(lbl) << std::endl; }
}
void l22::postfix_writer::do_address_of_node(l22::address_of_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl + 2);
}
void l22::postfix_writer::do_nullptr_node(l22::nullptr_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  if (_global == 0) {
    // global variable
    _pf.SINT(0);
    if (_debug) { std::cout << "SINT" << "0" << std::endl; }
  } else {
    // local variable
    _pf.INT(0);
    if (_debug) {
    std::cout << "INT" << "0" << std::endl;
  }
  }
}
void l22::postfix_writer::do_variable_declaration_node(l22::variable_declaration_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // REVIEW : do functions have symbols associated with them???
  auto id = node->identifier();
  int offset = 0, typesize = node->type()->size();

  // calculate variable offset for local variables 
  if (_global != 0) {
    offset = _func_offset.top();
    _func_offset.pop();

    offset -= typesize;
    _func_offset.push(offset);
  }

  auto symbol = new_symbol();
  if (symbol) {
    symbol->set_offset(offset);
    reset_new_symbol();
  }

  if (_global == 0) {
    // global declarations
    // TODO : function declaration
    if (node->initializer() == nullptr) {
      // TODO : if it is a function cant do this i think
      _pf.BSS();
      if (_debug) { std::cout << "BSS" << std::endl; }
      _pf.ALIGN();
      if (_debug) { std::cout << "ALIGN" << std::endl; }
      _pf.LABEL(id);
        if (_debug) {
          std::cout << "LABEL" << id << std::endl;
        }
      _pf.SALLOC(typesize);
      if (_debug) { std::cout << "SALLOC" << typesize << std::endl; }
    } else {
      if (node->is_typed(cdk::TYPE_DOUBLE)) {
        _pf.DATA();
        if (_debug) { std::cout << "DATA" << std::endl; }
        _pf.ALIGN();
          if (_debug) {
    std::cout << "ALIGN" << std::endl; 
  }
        _pf.LABEL(id);
          if (_debug) {
            std::cout << "LABEL" << id << std::endl;
          }

        if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
          node->initializer()->accept(this, lvl);

        } else if (node->initializer()->is_typed(cdk::TYPE_INT)) {
          cdk::integer_node *dclini = dynamic_cast<cdk::integer_node*>(node->initializer());
          cdk::double_node ddi(dclini->lineno(), dclini->value());
          ddi.accept(this, lvl);
        } else {
          std::cerr << node->lineno() << ": '" << id << "' has bad initializer for real value\n";
        }


      } else if (node->is_typed(cdk::TYPE_INT) || node->is_typed(cdk::TYPE_POINTER) || node->is_typed(cdk::TYPE_STRING)) {
        // TODO : global declaration of string not working
        _pf.DATA();
        if (_debug) { std::cout << "DATA" << std::endl; }
        _pf.ALIGN();
        if (_debug) { std::cout << "ALIGN" << std::endl; }
        _pf.LABEL(id);
        if (_debug) { std::cout << "LABEL" << id << std::endl; }
        node->initializer()->accept(this, lvl);
      } else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {
        node->initializer()->accept(this, lvl);

        if (_lastFunction < 0) {
          std::cerr << "there is no function defined" << std::endl;
        } else {
          _pf.DATA();
          if (_debug) { std::cout << "DATA" << std::endl; }
          _pf.ALIGN();
            if (_debug) { std::cout << "ALIGN" << std::endl; }
          _pf.GLOBAL(id, _pf.OBJ());
            if (_debug) { std::cout << "GLOBAL" << id << "OBJ" << std::endl; }
          _pf.LABEL(id);
          if (_debug) { std::cout << "LABEL" << id << std::endl; }
          _pf.SADDR(mklbl(_lastFunction));
          if (_debug) { std::cout << "SADDR" << mklbl(_lastFunction) << std::endl; } 
        }
      }
    }
  } else {
    // local declarations
    // TODO : local variable declarations of functions
    if (node->initializer()->is_typed(cdk::TYPE_INT) || node->initializer()->is_typed(cdk::TYPE_STRING) ||
        node->initializer()->is_typed(cdk::TYPE_POINTER)) {
          node->initializer()->accept(this, lvl);
          _pf.LOCAL(offset);
          if (_debug) { std::cout << "LOCAL" << offset << std::endl; }

          _pf.STINT();
          if (_debug) { std::cout << "STINT" << std::endl; }
    } else if (node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
          node->initializer()->accept(this, lvl);
          _pf.LOCAL(offset);
          if (_debug) { std::cout << "LOCAL" << offset << std::endl; }
          _pf.STDOUBLE();
          if (_debug) { std::cout << "STDOUBLE" << std::endl; }
    }
  }



}
void l22::postfix_writer::do_block_node(l22::block_node * const node, int lvl) {
  _symtab.push();
  _global++;

  if (node->declarations()) {
    node->declarations()->accept(this, lvl + 2);
  }
  
  if (node->instructions()) {
    node->instructions()->accept(this, lvl + 2);
  }

  _global--;
  _symtab.pop();
}
void l22::postfix_writer::do_stop_node(l22::stop_node * const node, int lvl) {

  if (_while_end.size() != 0) {
    _pf.JMP(mklbl(_while_end.top()));
    if (_debug) {
      std::cout << "JMP" << mklbl(_while_end.top()) << std::endl;
    }
  } else {
    std::cerr << node->lineno() << "stop operation outside while loop" <<std::endl;
  }
}
void l22::postfix_writer::do_again_node(l22::again_node * const node, int lvl) {
  if (_while_cond.size() != 0) {
    _pf.JMP(mklbl(_while_cond.top()));
        if (_debug) { std::cout << "JMP" << mklbl(_while_cond.top()) << std::endl; }
  } else {
    std::cerr << node->lineno() << "again operation outside while loop" <<std::endl;
  }
}
void l22::postfix_writer::do_return_node(l22::return_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  // function is non void type
  if (_func_isVoid.top() == 0) {
    if (node->retval()->is_typed(cdk::TYPE_INT) || node->retval()->is_typed(cdk::TYPE_STRING) ||
        node->retval()->is_typed(cdk::TYPE_DOUBLE) || node->retval()->is_typed(cdk::TYPE_FUNCTIONAL) ) {
          _pf.STFVAL32();
          if (_debug) { std::cout << "STFVAL32" << std::endl; }
      } else if (node->retval()->is_typed(cdk::TYPE_DOUBLE)) {
          // TODO : if function type is TYPE_DOUBLE but the expression is type in
          // it must return the int converted to a double 
          _pf.STFVAL64();
          if (_debug) { std::cout << "STFVAL64" << std::endl; }
      }
  }

  _pf.LEAVE();
  if (_debug) { std::cout << "LEAVE" << std::endl; }
  _pf.RET();
  if (_debug) { std::cout << "RET" << std::endl; }

}
void l22::postfix_writer::do_index_node(l22::index_node * const node, int lvl) {
  // REVIEW : inside function different???
  ASSERT_SAFE_EXPRESSIONS;

  node->base()->accept(this, lvl);
  node->index()->accept(this, lvl);

  if(node->is_typed(cdk::TYPE_DOUBLE)) {
    _pf.INT(3);
    if (_debug) { std::cout << "INT" << "3" << std::endl; }
  }
  else {
    _pf.INT(2);
    if (_debug) { std::cout << "INT" << "3" << std::endl; }
  }

  _pf.SHTL();
  if (_debug) { std::cout << "SHTL" << std::endl; }
  _pf.ADD(); // add pointer and index
  if (_debug) { std::cout << "ADD" << std::endl; }

}
void l22::postfix_writer::do_sizeof_node(l22::sizeof_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.INT(node->expression()->type()->size());
  if (_debug) { std::cout << "INT" << node->expression()->type()->size() << std::endl; }
}
void l22::postfix_writer::do_stack_alloc_node(l22::stack_alloc_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS

  node->argument()->accept(this, lvl);

  auto type = cdk::reference_type::cast(node->type());
  if(type->referenced()->name() == cdk::TYPE_DOUBLE) {
    _pf.INT(3);
  }
  else {
    _pf.INT(2);
  }
  
  _pf.SHTL(); 
  _pf.ALLOC();
  _pf.SP();
}
void l22::postfix_writer::do_function_definition_node(l22::function_definition_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int funcName = ++_lbl;


  _symtab.push();
  _function.push(funcName);

  _pf.TEXT(mklbl(funcName));
  if (_debug) { std::cout << "TEXT" << mklbl(funcName) << std::endl; }
  _pf.ALIGN();
    if (_debug) {
    std::cout << "ALIGN" << std::endl; 
  }
  _pf.LABEL(mklbl(funcName));
    if (_debug) {
    std::cout << "LABEL" << mklbl(funcName) << std::endl;
  }

  // hardcoded frame size
  _pf.ENTER(2048);
  if (_debug) {
    std::cout << "ENTER" << "2048" << std::endl;
  }
  
  if (node->returnval()->name() == cdk::TYPE_VOID) {_func_isVoid.push(1);} 
  else { _func_isVoid.push(0); }

  _func_offset.push(0);

  if (node->block()) {
    node->block()->accept(this, lvl + 4);
  }

  _func_isVoid.pop();
  _func_offset.pop();

  _lastFunction = funcName;
  _function.pop();
  _symtab.pop();

}
void l22::postfix_writer::do_function_call_node(l22::function_call_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  int argsSize = 0;

  if (node->arguments()) {
    for (int ax = node->arguments()->size(); ax > 0; ax--) {
      auto arg = dynamic_cast<cdk::expression_node*>(node->arguments()->node(ax - 1));
      arg->accept(this, lvl + 2);
      argsSize += arg->type()->size();
    }
  }

  node->pointer()->accept(this, lvl + 2);

  // do function call
  _pf.CALL("f");

  if(argsSize != 0){
    _pf.TRASH(argsSize);
  }

}


void l22::postfix_writer::do_identity_node(l22::identity_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
}



//---------------------------------------------------------------------------

void l22::postfix_writer::do_sequence_node(cdk::sequence_node * const node, int lvl) {
  for (size_t i = 0; i < node->size(); i++) {
    node->node(i)->accept(this, lvl);
  }
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_integer_node(cdk::integer_node * const node, int lvl) {

  if (_global == 0) {
    // global variable declaration
    _pf.SINT(node->value()); 
    if (_debug) { std::cout << "SINT" << node->value() << std::endl; }

  } else {
    // local literal
    _pf.INT(node->value()); 
    if (_debug) { std::cout << "INT" << node->value() << std::endl; }
  }
}

void l22::postfix_writer::do_string_node(cdk::string_node * const node, int lvl) {
  int lbl1;

  /* generate the string */
  _pf.RODATA(); // strings are DATA readonly
  _pf.ALIGN(); // make sure we are 
    if (_debug) {
    std::cout << "ALIGN" << std::endl; 
  }
  _pf.LABEL(mklbl(lbl1 = ++_lbl)); // give the string a name
      if (_debug) {
    std::cout << "LABEL" << mklbl(lbl1) << std::endl;
  }
  
  _pf.SSTRING(node->value()); // output string characters

  if (_global == 0) {
    // global variable declaration
    _pf.DATA(); // return to the TEXT segment
    if (_debug) { std::cout << "DATA" << std::endl; }
    _pf.SADDR(mklbl(lbl1)); // the string to be printed
    if (_debug) { std::cout << "SADDR" << mklbl(lbl1) << std::endl; } 
  } else {
    // local literal
    _pf.TEXT(); // return to the TEXT segment
    if (_debug) {
      std::cout << "TEXT" << std::endl; 
    }
    _pf.ADDR(mklbl(lbl1)); // the string to be printed
    if (_debug) {
      std::cout << "ADDR" << mklbl(lbl1) << std::endl; 
    }
  }
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_neg_node(cdk::neg_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  _pf.NEG(); // 2-complement
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_add_node(cdk::add_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.ADD();
  if (_debug) { std::cout << "ADD" << std::endl; }
}
void l22::postfix_writer::do_sub_node(cdk::sub_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.SUB();
  if (_debug) { std::cout << "SUB" << std::endl; }
}
void l22::postfix_writer::do_mul_node(cdk::mul_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MUL();
  if (_debug) { std::cout << "MUL" << std::endl; }
}
void l22::postfix_writer::do_div_node(cdk::div_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.DIV();
  if (_debug) { std::cout << "DIV" << std::endl; }
}
void l22::postfix_writer::do_mod_node(cdk::mod_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.MOD();
  if (_debug) { std::cout << "MOD" << std::endl; }
}
void l22::postfix_writer::do_lt_node(cdk::lt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LT();
  if (_debug) { std::cout << "LT" << std::endl; }
}
void l22::postfix_writer::do_le_node(cdk::le_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.LE();
  if (_debug) { std::cout << "LE" << std::endl; }
}
void l22::postfix_writer::do_ge_node(cdk::ge_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GE();
  if (_debug) { std::cout << "GE" << std::endl; }
}
void l22::postfix_writer::do_gt_node(cdk::gt_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.GT();
  if (_debug) { std::cout << "GT" << std::endl; }
}
void l22::postfix_writer::do_ne_node(cdk::ne_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.NE();
  if (_debug) { std::cout << "NE" << std::endl; }
}
void l22::postfix_writer::do_eq_node(cdk::eq_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->left()->accept(this, lvl);
  node->right()->accept(this, lvl);
  _pf.EQ();
  if (_debug) { std::cout << "EQ" << std::endl; }
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_variable_node(cdk::variable_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  auto var = _symtab.find(node->name());
  if(_global != 0 && var->offset() != 0) {
    _pf.LOCAL(var->offset());
    if (_debug) { std::cout << "LOCAL" << var->offset() << std::endl; }
  }
  else {
    _pf.ADDR(var->name());
    if (_debug) {
      std::cout << "ADDR" << var->name() << std::endl; 
    }
  }
}

void l22::postfix_writer::do_rvalue_node(cdk::rvalue_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->lvalue()->accept(this, lvl);
  _pf.LDINT(); // depends on type size
  if (_debug) { std::cout << "LDINT" << std::endl; }

}

void l22::postfix_writer::do_assignment_node(cdk::assignment_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  
  node->rvalue()->accept(this, lvl + 2);
  if (node->type()->name() == cdk::TYPE_DOUBLE) {
    // convert rvalue from int to double (auto conversion)
    if (node->rvalue()->type()->name() == cdk::TYPE_INT) _pf.I2D();
    _pf.DUP64();
    if (_debug) {
        std::cout << "DUP64" << std::endl; 
    }
  } else {
    // TODO : does this cover function assignment??
    _pf.DUP32();
    if (_debug) {
        std::cout << "DUP32" << std::endl; 
    }
  }

  node->lvalue()->accept(this, lvl);
  if (node->type()->name() == cdk::TYPE_DOUBLE) {
    _pf.STDOUBLE();
    if (_debug) { std::cout << "STDOUBLE" << std::endl; }
    
  } else {
    _pf.STINT();
    if (_debug) { std::cout << "STINT" << std::endl; }

  }

}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_program_node(l22::program_node * const node, int lvl) {
  // Note that Simple doesn't have functions. Thus, it doesn't need
  // a function node. However, it must start in the main function.
  // The ProgramNode (representing the whole program) doubles as a
  // main function node.

  // generate the main function (RTS mandates that its name be "_main")
  _pf.TEXT();
  if (_debug) {
    std::cout << "TEXT" << std::endl; 
  }
  _pf.ALIGN();
    if (_debug) {
    std::cout << "ALIGN" << std::endl; 
  }
  _pf.GLOBAL("_main", _pf.FUNC());
  if (_debug) {
    std::cout << "GLOBAL" << "_main" << "FUNC" << std::endl;
  }
  _pf.LABEL("_main");
  if (_debug) {
    std::cout << "LABEL" << "_main" << std::endl;
  }
  // hardcoded frame size
  _pf.ENTER(2048);  
  if (_debug) {
    std::cout << "ENTER" << "2048" << std::endl;
  }

  _func_offset.push(0);
  _func_isVoid.push(0);
  
  node->statements()->accept(this, lvl);
  
  _func_isVoid.pop();
  _func_offset.pop();

  // end the main function
  // REVEIW : not always return 0
  _pf.INT(0);
  if (_debug) { std::cout << "INT" << "0" << std::endl; }
  _pf.STFVAL32();
  if (_debug) { std::cout << "STFVAL32" << std::endl; }
  _pf.LEAVE();
  if (_debug) { std::cout << "LEAVE" << std::endl; }
  _pf.RET();
  if (_debug) { std::cout << "RET" << std::endl; }


  // import needed extern libraries
  for (std::string lib : _functions_to_declare) {
    _pf.EXTERN(lib);
    if (_debug) { std::cout << "EXTERN" << lib << std::endl; }

  }
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_evaluation_node(l22::evaluation_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  node->argument()->accept(this, lvl); // determine the value
  if (node->argument()->is_typed(cdk::TYPE_INT)) {
    _pf.TRASH(4); // delete the evaluated value
      if (_debug) { std::cout << "TRASH" << "4" << std::endl; }
    
  } else if (node->argument()->is_typed(cdk::TYPE_STRING)) {
    _pf.TRASH(4); // delete the evaluated value's address
      if (_debug) { std::cout << "TRASH" << "4" << std::endl; }

  } else {
    std::cerr << "ERROR: CANNOT HAPPEN!" << std::endl;
    exit(1);
  }
}

void l22::postfix_writer::do_print_node(l22::print_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;

  size_t arguments_size = node->argument()->size();
  for (size_t ix = 0; ix < arguments_size; ix++) {
    auto argument = dynamic_cast<cdk::expression_node*>(node->argument()->node(ix));

    std::shared_ptr<cdk::basic_type> type = argument->type();
    argument->accept(this, lvl); // expression to print
    
    if (type->name() == cdk::TYPE_INT) {
      _functions_to_declare.insert("printi");
      _pf.CALL("printi");
      if (_debug) { std::cout << "CALL" << "printi" << std::endl; }

      _pf.TRASH(4); // trash int
      if (_debug) { std::cout << "TRASH" << "4" << std::endl; }


    } else if (type->name() == cdk::TYPE_DOUBLE) {
      _functions_to_declare.insert("printd");
      _pf.CALL("printd");
      if (_debug) { std::cout << "CALL" << "printd" << std::endl; }

      _pf.TRASH(8); // trash double
      if (_debug) { std::cout << "TRASH" << "8" << std::endl; }


    } else if (type->name() == cdk::TYPE_STRING) {
      _functions_to_declare.insert("prints");
      _pf.CALL("prints");
      if (_debug) { std::cout << "CALL" << "prints" << std::endl; }

      _pf.TRASH(4); // trash char pointer
      if (_debug) { std::cout << "TRASH" << "4" << std::endl; }

      
    } else {
      std::cerr << "non printable type" << std::endl;
      return;
    }

  }

  if (node->newline()) {
    _functions_to_declare.insert("println");
    _pf.CALL("println");
    if (_debug) { std::cout << "CALL" << "println" << std::endl; }

  }
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_read_node(l22::read_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  _pf.CALL("readi");
  if (_debug) { std::cout << "CALL" << "readi" << std::endl; }

  _pf.LDFVAL32();
  //node->argument()->accept(this, lvl);
  _pf.STINT();
  if (_debug) { std::cout << "STINT" << std::endl; }

}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_while_node(l22::while_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int cond = ++_lbl;
  int end = ++_lbl;

  _while_cond.push(cond);
  _while_end.push(end);

  _pf.LABEL(mklbl(cond));
    if (_debug) {
    std::cout << "LABEL" << mklbl(cond) << std::endl;
  }
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(end));
    if (_debug) {
      std::cout << "JZ" << mklbl(end) << std::endl; 
  }
  node->block()->accept(this, lvl + 2);
  _pf.JMP(mklbl(cond));
          if (_debug) {
      std::cout << "JMP" << mklbl(cond) << std::endl;
    }


  _pf.LABEL(mklbl(end));
    if (_debug) {
    std::cout << "LABEL" << mklbl(end) << std::endl;
  }
  _while_cond.pop();
  _while_end.pop();
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_if_node(l22::if_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
    if (_debug) {
      std::cout << "JZ" << mklbl(lbl1) << std::endl; 
  }
  node->block()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1));
      if (_debug) {
    std::cout << "LABEL" << mklbl(lbl1) << std::endl;
  }
}

//---------------------------------------------------------------------------

void l22::postfix_writer::do_if_else_node(l22::if_else_node * const node, int lvl) {
  ASSERT_SAFE_EXPRESSIONS;
  int lbl1, lbl2;
  node->condition()->accept(this, lvl);
  _pf.JZ(mklbl(lbl1 = ++_lbl));
    if (_debug) {
      std::cout << "JZ" << mklbl(lbl1) << std::endl; 
  }
  node->thenblock()->accept(this, lvl + 2);
  _pf.JMP(mklbl(lbl2 = ++_lbl));
  if (_debug) {
    std::cout << "JMP" << mklbl(lbl2) << std::endl;
  }
  _pf.LABEL(mklbl(lbl1));
  if (_debug) {
    std::cout << "LABEL" << mklbl(lbl1) << std::endl;
  }
  node->elseblock()->accept(this, lvl + 2);
  _pf.LABEL(mklbl(lbl1 = lbl2));
    if (_debug) {
    std::cout << "LABEL" << mklbl(lbl2) << std::endl;
  }
}