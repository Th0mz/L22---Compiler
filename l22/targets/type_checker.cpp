#include <string>
#include "targets/type_checker.h"
#include ".auto/all_nodes.h"  // automatically generated
#include <cdk/types/primitive_type.h>


#define ASSERT_UNSPEC { if (node->type() != nullptr && !node->is_typed(cdk::TYPE_UNSPEC)) return; }

//---------------------------------------------------------------------------

void l22::type_checker::do_sequence_node(cdk::sequence_node *const node, int lvl) {
    for (size_t i = 0; i < node->size(); i++)
        node->node(i)->accept(this, lvl);
}

//---------------------------------------------------------------------------
void l22::type_checker::do_with_node(l22::with_node *const node, int lvl) {
    
    node->function()->accept(this, lvl);
    if (!node->function()->is_typed(cdk::TYPE_FUNCTIONAL)) {
        throw std::string("function passed is not valid");
    }

    node->vector()->accept(this, lvl);
    if (!node->vector()->is_typed(cdk::TYPE_POINTER)) {
        throw std::string("vector passed is not valid");
    }

    node->low()->accept(this, lvl);
    node->high()->accept(this, lvl);

}

void l22::type_checker::do_sweep_node(l22::sweep_node * const node, int lvl) {

    node->vector()->accept(this, lvl + 2);
    if (!node->vector()->is_typed(cdk::TYPE_POINTER)) {
        throw std::string("pointer on sweep must be a type pointer");
    }

    node->function()->accept(this, lvl + 2);
    if (!node->function()->is_typed(cdk::TYPE_FUNCTIONAL)) {
        throw std::string("function must be a function");
    }

    // check if the return type is the same as the pointed element
    auto functional_type = cdk::functional_type::cast(node->function()->type());
    auto output_type = functional_type->output()->component(0);
    auto input_type = functional_type->input()->component(0);
    auto referenced_type = cdk::reference_type::cast(node->vector()->type())->referenced();

    if (output_type->name() != referenced_type->name()) {
        throw std::string("output type and referenced type must be the same");
    }

    if (input_type->name() != referenced_type->name()) {
        throw std::string("input type and referenced type must be the same");
    }

    node->low()->accept(this, lvl + 2);
    node->high()->accept(this, lvl + 2);
    if (!(node->low()->is_typed(cdk::TYPE_INT) && node->high()->is_typed(cdk::TYPE_INT))) {
        throw std::string("low and high values must be integers");
    }

}

void l22::type_checker::do_nil_node(cdk::nil_node *const node, int lvl) {
    // EMPTY
}
void l22::type_checker::do_data_node(cdk::data_node *const node, int lvl) {
    // EMPTY
}
void l22::type_checker::do_block_node(l22::block_node *const node, int lvl) {
    // EMPTY
}

//---------------------------------------------------------------------------

void l22::type_checker::do_integer_node(cdk::integer_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void l22::type_checker::do_string_node(cdk::string_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
}

void l22::type_checker::do_double_node(cdk::double_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
}

void l22::type_checker::do_nullptr_node(l22::nullptr_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->type(cdk::reference_type::create(4,cdk::primitive_type::create(0,cdk::TYPE_UNSPEC)));
}

//---------------------------------------------------------------------------

void l22::type_checker::do_not_node(cdk::not_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->argument()->accept(this, lvl + 2);
    if (node->argument()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        node->argument()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else {
        throw std::string("Wrong type in unary logical expression.");
    }
}

void l22::type_checker::do_and_node(cdk::and_node *const node, int lvl) {
    processBinaryExpressionInt(node, lvl);
}
void l22::type_checker::do_or_node(cdk::or_node *const node, int lvl) {
    processBinaryExpressionInt(node, lvl);
}

//---------------------------------------------------------------------------

void l22::type_checker::do_return_node(l22::return_node *const node, int lvl) {
    // TODO
    if (!node->retval()) {
        throw std::string("there is no return expresstion");
    }

    node->retval()->accept(this, lvl + 2);
    // TODO : check if function calling return has the same return
    // value as the retval expression 
}

//---------------------------------------------------------------------------

void l22::type_checker::do_index_node(l22::index_node *const node, int lvl) {

    std::shared_ptr< cdk::basic_type > referenced_type;
    if (node->base()) {
        node->base()->accept(this, lvl + 2);
        if (!node->base()->is_typed(cdk::TYPE_POINTER)) {
            throw std::string("pointer expression expected in index left-value");
        }

        // REVIEW : a referencia não pode ser TYPE_FUNCTIONAL???
        // ou a base é que não pode ser typefuntional
        referenced_type = cdk::reference_type::cast(node->base()->type())->referenced(); 
        if (referenced_type->name() == cdk::TYPE_FUNCTIONAL) {
            throw std::string("the pointer cant be referencing a function");
        } 

    } else {
        throw std::string("return pointer expression expected in left-value index.");
    }

    if(!node->index()->is_typed(cdk::TYPE_INT)) {
        throw std::string("integer expression expected in left-value index");
    }

    node->type(referenced_type);
}

//---------------------------------------------------------------------------

void l22::type_checker::do_variable_declaration_node(l22::variable_declaration_node *const node, int lvl) {
    
    if (node->initializer() != nullptr) {
        node->initializer()->accept(this, lvl + 2);

        if (node->is_typed(cdk::TYPE_INT)) {
            
            if (node->initializer()->is_typed(cdk::TYPE_UNSPEC)) { 
                node->initializer()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
            }
            if (!node->initializer()->is_typed(cdk::TYPE_INT)) {
                throw std::string("wrong type for initializer (integer expected).");
            }
        
        } else if (node->is_typed(cdk::TYPE_DOUBLE)) {      
           
            if (node->initializer()->is_typed(cdk::TYPE_UNSPEC)) { 
                node->initializer()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
            }
            if (!node->initializer()->is_typed(cdk::TYPE_INT) && !node->initializer()->is_typed(cdk::TYPE_DOUBLE)) {
                throw std::string("wrong type for initializer (integer or double expected).");
            }
        
        } else if (node->is_typed(cdk::TYPE_STRING)) {
            if (!node->initializer()->is_typed(cdk::TYPE_STRING)) {
                throw std::string("wrong type for initializer (string expected).");
            }
        
        } else if (node->is_typed(cdk::TYPE_FUNCTIONAL)) {

            if (node->initializer()->is_typed(cdk::TYPE_FUNCTIONAL)) {
                // REVIEW : compareFunctionalType
                //if (! compareFunctionalType(cdk::functional_type::create(4, node->type()), cdk::functional_type::create(4, node->initializer()->type()))) {
                //    throw std::string("wrong type for initializer (function expected).");
                //}
            } else {
                throw std::string("wrong type for initializer (function expected).");
            }

        } else if (node->is_typed(cdk::TYPE_POINTER)) {
            if (node->initializer()->is_typed(cdk::TYPE_POINTER)) {
                // REVIEW : todo comparePointer
                //if (!comparePonterType(cdk::reference_type::create(4, node->type()), cdk::reference_type::create(4, node->initializer()->type()))) {
                //    throw std::string("initializer pointed value must be the same as the declarated type");
                //}
            } else {
                auto initializer = (cdk::literal_node<int>*)node->initializer();
                if (initializer == nullptr || initializer->value() != 0) {
                    throw std::string("wrong type for initializer (pointer expected).");
                }
            }

        } else if (node->is_typed(cdk::TYPE_UNSPEC)) {
            // type is var => node type is the type of the initializer
            node->type(node->initializer()->type());

        } else {
            throw std::string("unknown type for initializer.");
        }
    }

    const std::string &id = node->identifier();
    auto symbol = l22::make_symbol(node->qualifier(), node->type(), id, (bool) node->initializer());
    if (_symtab.insert(id, symbol)) {
        _parent->set_new_symbol(symbol);
    } else {
        throw std::string("cant redeclare variables, variable with id : " + id + " is already declared");
    }
  }

//---------------------------------------------------------------------------

void l22::type_checker::do_neg_node(cdk::neg_node *const node, int lvl) {
    processUnaryExpression(node, lvl);
}

void l22::type_checker::do_identity_node(l22::identity_node *const node, int lvl) {
    processUnaryExpression(node, lvl);
}

//---------------------------------------------------------------------------

void l22::type_checker::do_stop_node(l22::stop_node *const node, int lvl) {
    // EMPTY 
}

void l22::type_checker::do_again_node(l22::again_node *const node, int lvl) {
    // EMPTY
}

//---------------------------------------------------------------------------

void l22::type_checker::do_stack_alloc_node(l22::stack_alloc_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->argument()->accept(this, lvl + 2);
    if (!node->argument()->is_typed(cdk::TYPE_INT)) {
        throw std::string("integer expression expected in allocation expression.");
    }

    auto myType = cdk::reference_type::create(4,cdk::primitive_type::create(0,cdk::TYPE_UNSPEC));
    node->type(myType);
}

void l22::type_checker::do_address_of_node(l22::address_of_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->argument()->accept(this, lvl + 2);
    node->type(cdk::reference_type::create(4, node->argument()->type()));
}

//---------------------------------------------------------------------------

void l22::type_checker::do_function_definition_node(l22::function_definition_node *const node, int lvl) {
    // TODO
    node->arguments()->accept(this, lvl + 2);

    std::vector < std::shared_ptr < cdk::basic_type >> input_types;
    size_t num_arguments = node->arguments()->size();
    for (size_t i = 0; i < num_arguments; i++) {
        input_types.push_back(node->argument(i)->type());
    }

    node->type(cdk::functional_type::create(input_types, node->returnval()));

    //node->type(cdk::primitive_type::create(4,))
    // não chamarar o type checker nos argumentos?
    //node->arguments()->accept(this, lvl + 2);

}

void l22::type_checker::do_function_call_node(l22::function_call_node *const node, int lvl) {
    ASSERT_UNSPEC;

    // TODO
    if (node->pointer() == nullptr) {
        // it is a recursive call
        // REVIEW : recursive calls cant appear outside function
        return;
    }

    // REVIEW : não verificar se é type_pointer mas sim type_functional certo?
    node->pointer()->accept(this, lvl + 2);
    if (!node->pointer()->is_typed(cdk::TYPE_FUNCTIONAL)) {
        throw std::string("functions can only be called by pointer reference");
    }

    if (!node->arguments()) {
        throw std::string("non initialized arguments");
    }
    node->arguments()->accept(this, lvl + 2);
    
    auto funcType = cdk::functional_type::cast(node->pointer()->type());
    node->type(funcType->output(0));
    // o tipo deste nó é o valor de retorno da função

    // verificar se o tipo da função apontado é igual ao tipo do ponteiro ???

    // verificar se a função foi declarada (usando a tabela de simbolos)
    // ns se é necessário pois já possuimos um ponteiro para a mesma
    // => existe se tudo correu bem

    // isto vai tudo depender de como se faz a definição de função
    
}

//---------------------------------------------------------------------------

void l22::type_checker::do_sizeof_node(l22::sizeof_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->expression()->accept(this, lvl + 2);
    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//===========================================================================
//---------------------------------------------------------------------------

void l22::type_checker::processUnaryExpression(cdk::unary_operation_node *const node, int lvl) {
    
    node->argument()->accept(this, lvl + 2);

    if (node->argument()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if (node->argument()->is_typed(cdk::TYPE_DOUBLE)) {
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if (node->argument()->is_typed(cdk::TYPE_UNSPEC)) {
        node->argument()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else {
        throw std::string("wrong type in argument of unary expression.");
    }
}

void l22::type_checker::processBinaryExpressionInt(cdk::binary_operation_node *const node, int lvl) {
    
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    if (!node->left()->is_typed(cdk::TYPE_INT)) {
        throw std::string("left side of the expresstion must be an integer");
    }

    node->right()->accept(this, lvl + 2);
    if (!node->right()->is_typed(cdk::TYPE_INT)) {
        throw std::string("right side of the expresstion must be an integer");
    }

    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

void l22::type_checker::processBinaryExpressionIntDbl(cdk::binary_operation_node *const node, int lvl) {
    
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if ((node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) ||
               (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) ||
               (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))) {
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if ((node->left()->is_typed(cdk::TYPE_UNSPEC) || node->right()->is_typed(cdk::TYPE_UNSPEC))) {
        if (node->left()->is_typed(cdk::TYPE_UNSPEC)) {
            node->left()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));   
        }
        if (node->right()->is_typed(cdk::TYPE_UNSPEC)) {
            node->right()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));   
        }

        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else {
        throw std::string("wrong types in binary expression, only accept int and double");
    }
}

void l22::type_checker::processBinaryExpressionIntDblPtr(cdk::binary_operation_node *const node, int lvl) {

    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);

    if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_INT)) {
        node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
    } else if ((node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_INT)) ||
               (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_DOUBLE)) ||
               (node->left()->is_typed(cdk::TYPE_DOUBLE) && node->right()->is_typed(cdk::TYPE_DOUBLE))) {
        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
    } else if (node->left()->is_typed(cdk::TYPE_POINTER) && node->right()->is_typed(cdk::TYPE_INT)) {
        node->type(node->left()->type());
    
    } else if (node->left()->is_typed(cdk::TYPE_INT) && node->right()->is_typed(cdk::TYPE_POINTER)) {
        node->type(node->right()->type());
    } else if (((node->left()->is_typed(cdk::TYPE_UNSPEC) || node->right()->is_typed(cdk::TYPE_UNSPEC))) &&
               ((!node->left()->is_typed(cdk::TYPE_POINTER) && !node->right()->is_typed(cdk::TYPE_POINTER)))) {
        
        if (node->left()->is_typed(cdk::TYPE_UNSPEC)) {
            node->left()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));   
        }
        if (node->right()->is_typed(cdk::TYPE_UNSPEC)) {
            node->right()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));   
        }

        node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));

    } else {
        throw std::string("wrong types in binary expression, only accept int and double");
    }
}

void l22::type_checker::processBinaryExpressionComp (cdk::binary_operation_node *const node, int lvl) {
    ASSERT_UNSPEC;
    node->left()->accept(this, lvl + 2);
    node->right()->accept(this, lvl + 2);
    if (node->left()->type() != node->right()->type()) {
        throw std::string("same type expected on both sides of equality operator");
    }

    node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
}

//===========================================================================
//---------------------------------------------------------------------------

void l22::type_checker::do_add_node(cdk::add_node *const node, int lvl) {
    processBinaryExpressionIntDblPtr(node, lvl);
}
void l22::type_checker::do_sub_node(cdk::sub_node *const node, int lvl) {
    processBinaryExpressionIntDblPtr(node, lvl);
}
void l22::type_checker::do_mul_node(cdk::mul_node *const node, int lvl) {
    processBinaryExpressionIntDbl(node, lvl);
}
void l22::type_checker::do_div_node(cdk::div_node *const node, int lvl) {
    processBinaryExpressionIntDbl(node, lvl);
}
void l22::type_checker::do_mod_node(cdk::mod_node *const node, int lvl) {
    processBinaryExpressionInt(node, lvl);
}
void l22::type_checker::do_lt_node(cdk::lt_node *const node, int lvl) {
    processBinaryExpressionInt(node, lvl);
}
void l22::type_checker::do_le_node(cdk::le_node *const node, int lvl) {
    processBinaryExpressionInt(node, lvl);
}
void l22::type_checker::do_ge_node(cdk::ge_node *const node, int lvl) {
    processBinaryExpressionInt(node, lvl);
}
void l22::type_checker::do_gt_node(cdk::gt_node *const node, int lvl) {
    processBinaryExpressionInt(node, lvl);
}
void l22::type_checker::do_ne_node(cdk::ne_node *const node, int lvl) {
    processBinaryExpressionComp(node, lvl);
}
void l22::type_checker::do_eq_node(cdk::eq_node *const node, int lvl) {
    processBinaryExpressionComp(node, lvl);
}

//---------------------------------------------------------------------------

void l22::type_checker::do_variable_node(cdk::variable_node *const node, int lvl) {
    ASSERT_UNSPEC;
    const std::string &id = node->name();
    std::shared_ptr<l22::symbol> symbol = _symtab.find(id);

    if (symbol != nullptr) {
        node->type(symbol->type());
    } else {
        throw id;
    }
}

void l22::type_checker::do_rvalue_node(cdk::rvalue_node *const node, int lvl) {
    ASSERT_UNSPEC;
    try {
        node->lvalue()->accept(this, lvl);
        node->type(node->lvalue()->type());
    } catch (const std::string &id) {
        throw std::string("undeclared variable '" + id + "'");
    }
}

void l22::type_checker::do_assignment_node(cdk::assignment_node *const node, int lvl) {
    ASSERT_UNSPEC;

    try {
        // check if left value is in _symtab
        node->lvalue()->accept(this, lvl + 2);
    } catch (const std::string &id) {
        throw "undeclared variable '" + id + "'";
    }

    node->rvalue()->accept(this, lvl + 2);

    if (node->lvalue()->is_typed(cdk::TYPE_INT)) {
        if (node->rvalue()->is_typed(cdk::TYPE_INT)) {
            node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        } else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
            node->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
            node->rvalue()->type(cdk::primitive_type::create(4, cdk::TYPE_INT));
        } else {
            throw std::string("assigned non int value to int lval");
        }
    
    } else if (node->lvalue()->is_typed(cdk::TYPE_DOUBLE)) {      
        if ((node->rvalue()->is_typed(cdk::TYPE_DOUBLE)) || 
            (node->rvalue()->is_typed(cdk::TYPE_INT))) {
            node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
        } else if (node->rvalue()->is_typed(cdk::TYPE_UNSPEC)) {
            node->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
            node->rvalue()->type(cdk::primitive_type::create(8, cdk::TYPE_DOUBLE));
        } else {
            throw std::string("assigned non double/int value to double lval");
        }
    
    } else if (node->lvalue()->is_typed(cdk::TYPE_STRING)) {
        if (node->rvalue()->is_typed(cdk::TYPE_STRING)) {
            node->type(cdk::primitive_type::create(4, cdk::TYPE_STRING));
        } else {
            throw std::string("assigned non string value to string lval");
        }
    
    } else if (node->lvalue()->is_typed(cdk::TYPE_FUNCTIONAL)) {
        if (node->rvalue()->is_typed(cdk::TYPE_FUNCTIONAL)) {
            // REVIEW : compareFunctionalType
            //if (compareFunctionalType()) {
            //
            //} else {
            //    throw std::string("");
            //}
            node->type(cdk::primitive_type::create(4, cdk::TYPE_FUNCTIONAL));
        } else {
            throw std::string("assigned non functional type or functional type with wrong input and output types to lval");
        }

    } else if (node->lvalue()->is_typed(cdk::TYPE_POINTER)) {
        
        if (node->rvalue()->is_typed(cdk::TYPE_POINTER)) {
            // REVIEW : todo comparePOinter
            //if (comparePonterType(cdk::reference_type::create(4, node->rvalue()->type()) , cdk::reference_type::create(4, node->lvalue()->type()))) {
                node->rvalue()->type(node->lvalue()->type());
                node->type(node->lvalue()->type());
            //} else {
            //    throw std::string("pointers must point to the same type and with the same reference level");
            //}

        } else {
            throw std::string("assigned non pointer value to pointer lval");
        }
    } else {
        throw std::string("unknown typ)e for initializer.");
    }

}

//---------------------------------------------------------------------------

void l22::type_checker::do_program_node(l22::program_node *const node, int lvl) {
    // EMPTY
}

void l22::type_checker::do_evaluation_node(l22::evaluation_node *const node, int lvl) {
  	node->argument()->accept(this, lvl + 2);
}

void l22::type_checker::do_print_node(l22::print_node *const node, int lvl) {
	node->argument()->accept(this, lvl + 2);
}

void l22::type_checker::do_read_node(l22::read_node *const node, int lvl) {
    node->type(cdk::primitive_type::create(0, cdk::TYPE_UNSPEC));
}

//---------------------------------------------------------------------------

void l22::type_checker::do_while_node(l22::while_node *const node, int lvl) {
    node->condition()->accept(this, lvl + 4);
    if (!node->condition()->is_typed(cdk::TYPE_INT)) {
        throw std::string("expected integer condition");
    }
}

//---------------------------------------------------------------------------

void l22::type_checker::do_if_node(l22::if_node *const node, int lvl) {
    node->condition()->accept(this, lvl + 4);
    if (!node->condition()->is_typed(cdk::TYPE_INT)) {
        throw std::string("expected integer condition");
    }

}

void l22::type_checker::do_if_else_node(l22::if_else_node *const node, int lvl) {
    node->condition()->accept(this, lvl + 4);
    if (!node->condition()->is_typed(cdk::TYPE_INT)) {
        throw std::string("expected integer condition");
    }
}

bool l22::type_checker::comparePonterType (std::shared_ptr<cdk::reference_type> pointer1, std::shared_ptr<cdk::reference_type> pointer2) {
    int level1 = 1;
    int level2 = 1;

    auto reference1 = pointer1->referenced();
    auto reference2 = pointer2->referenced();

    // REVIEW : infinity loop need fix
    std::cerr << reference1->name() << std::endl;
    std::cerr << reference2->name() << std::endl;

    std::cerr << (cdk::reference_type::create(4, reference1)->referenced())->name() << std::endl;
    std::cerr << (cdk::reference_type::create(4, reference2)->referenced())->name() << std::endl;


    while (reference1->name() == cdk::TYPE_POINTER) {
        level1++;
        
        auto reference = cdk::reference_type::create(4, reference1)->referenced(); 
        reference1 = reference;

    }

    while (reference2->name() == cdk::TYPE_POINTER) {
        level2++;

        auto reference = cdk::reference_type::create(4, reference2)->referenced(); 
        reference2 = reference;
    }

    // rValue is unspecified
    if (reference2->name() == cdk::TYPE_UNSPEC) {
        return true;
    }

    if (level1 != level2 || reference1->name() != reference2->name()) {
        return false;
    }

    return true;
}

bool l22::type_checker::compareFunctionalType (std::shared_ptr<cdk::functional_type> func1, std::shared_ptr<cdk::functional_type> func2) {
    
    size_t input_length = func1->input_length();
    size_t output_length = func1->output_length();

    if (input_length != func2->input_length() || output_length != func2->output_length()) {
        return false;
    }

    for (size_t i = 0; i < input_length; i++) {
        if (func1->input()->component(i)->name() != func2->input()->component(i)->name()) {
            return false;
        }

        if (func1->input()->component(i)->name() == cdk::TYPE_POINTER) {
            if (!comparePonterType(cdk::reference_type::create(4, func1->input()->component(i)), cdk::reference_type::create(4, func2->input()->component(i)))) {
                return false;
            }
        }
    }

    for (size_t i = 0; i < output_length; i++) {
        if (func1->output()->component(i)->name() != func2->output()->component(i)->name()) {
            return false;
        }

        if (func1->output()->component(i)->name() == cdk::TYPE_POINTER) {
            if (!comparePonterType(cdk::reference_type::create(4, func1->output()->component(i)), cdk::reference_type::create(4, func2->output()->component(i)))) {
                return false;
            }
        }
    }

    
    return true;
}

