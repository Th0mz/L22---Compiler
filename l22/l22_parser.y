%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <algorithm>
#include <memory>
#include <cstring>
#include <cdk/compiler.h>
#include <cdk/types/types.h>
#include ".auto/all_nodes.h"
#define LINE                         compiler->scanner()->lineno()
#define yylex()                      compiler->scanner()->scan()
#define yyerror(compiler, s)         compiler->scanner()->error(s)
//-- don't change *any* of these --- END!
%}

%parse-param {std::shared_ptr<cdk::compiler> compiler}

%union {
  //--- don't change *any* of these: if you do, you'll break the compiler.
  YYSTYPE() : type(cdk::primitive_type::create(0, cdk::TYPE_VOID)) {}
  ~YYSTYPE() {}
  YYSTYPE(const YYSTYPE &other) { *this = other; }
  YYSTYPE& operator=(const YYSTYPE &other) { type = other.type; return *this; }

  std::shared_ptr<cdk::basic_type> type;        /* expression type */
  //-- don't change *any* of these --- END!

  int                   i;	/* integer value */
  double                d;
  std::string          *s;	/* symbol name or string literal */
  
  cdk::basic_node      *node;	/* node pointer */
  cdk::sequence_node   *sequence;
  cdk::expression_node *expression; /* expression nodes */
  cdk::lvalue_node     *lvalue;

  l22::block_node       *block_node;
  std::vector<std::shared_ptr<cdk::basic_type>> *types;
};

%token tINT_TYPE tDOUBLE_TYPE tTEXT_TYPE tVOID_TYPE 
%token tFOREIGN tUSE tPUBLIC tPRIVATE tVAR tTYPE_DECL
%token tAND tOR tNOT tGE tLE tEQ tNE
%token tIF tTHEN tELIF tELSE tWHILE tDO tSTOP tAGAIN
%token tWRITE tWRITELN tRETURN
%token tBEGIN tEND tSIZEOF
%token tINPUT
%token tUNARY
%token tSWEEP tWITH tCHANGE

%token <i> tINTEGER
%token <s> tIDENTIFIER tSTRING
%token <d> tDOUBLE
%token tNULL

%type <block_node> block  
%type <node> declaration variable instruction elif program block_instruction
%type <sequence> declarations variables 
%type <sequence> instructions exprs
%type <expression> expr block_expr function_def
%type <s> string
%type <type> data_type func_type
%type <types> data_types
%type <lvalue> lval

%{
// Precedences
%}

%nonassoc tIF tTHEN
%nonassoc tELSE tELIF
%nonassoc tWHILE

%right '='
%left tOR
%left tAND
%nonassoc tNOT
%left '<' '>' tGE tLE tEQ tNE
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY
%nonassoc '(' '['


%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%

file : /* Empty */            { compiler->ast(new cdk::sequence_node(LINE)); }
     | declarations program   { compiler->ast(new cdk::sequence_node(LINE, new l22::program_node(LINE, $2), $1)); } 
     | declarations           { compiler->ast($1); }
     | program                { compiler->ast(new l22::program_node(LINE, $1)); }
     ;


program   : tBEGIN block tEND      { $$ = $2; }        
	     ;


declaration : variable ';'                                   { $$ = $1; }
            | data_type tIDENTIFIER '=' expr ';'             { $$ = new l22::variable_declaration_node(LINE, tPRIVATE,  $1, *$2, $4); delete $2; }
            | data_type tIDENTIFIER '=' block_expr             { $$ = new l22::variable_declaration_node(LINE, tPRIVATE,  $1, *$2, $4); delete $2; }
            | tVAR tIDENTIFIER '=' expr ';'                  { $$ = new l22::variable_declaration_node(LINE, tPRIVATE,  cdk::primitive_type::create(4, cdk::typename_type::TYPE_UNSPEC), *$2, $4); delete $2; }
            | tVAR tIDENTIFIER '=' block_expr                  { $$ = new l22::variable_declaration_node(LINE, tPRIVATE,  cdk::primitive_type::create(4, cdk::typename_type::TYPE_UNSPEC), *$2, $4); delete $2; }
            /* -------------- USE -------------- */
            | tUSE data_type tIDENTIFIER ';'                 { $$ = new l22::variable_declaration_node(LINE, tUSE,  $2, *$3, nullptr); delete $3; }
            
            /* ------------- PUBLIC ------------ */
            | tPUBLIC data_type tIDENTIFIER ';'              { $$ = new l22::variable_declaration_node(LINE, tPUBLIC,  $2, *$3, nullptr); delete $3; }
            | tPUBLIC data_type tIDENTIFIER '=' expr ';'     { $$ = new l22::variable_declaration_node(LINE, tPUBLIC,  $2, *$3, $5); delete $3; }
            | tPUBLIC data_type tIDENTIFIER '=' block_expr     { $$ = new l22::variable_declaration_node(LINE, tPUBLIC,  $2, *$3, $5); delete $3; }
            | tPUBLIC tVAR tIDENTIFIER '=' expr ';'          { $$ = new l22::variable_declaration_node(LINE, tPUBLIC,  cdk::primitive_type::create(4, cdk::typename_type::TYPE_UNSPEC), *$3, $5); delete $3; }
            | tPUBLIC tVAR tIDENTIFIER '=' block_expr          { $$ = new l22::variable_declaration_node(LINE, tPUBLIC,  cdk::primitive_type::create(4, cdk::typename_type::TYPE_UNSPEC), *$3, $5); delete $3; }
            | tPUBLIC tIDENTIFIER '=' expr ';'              { $$ = new l22::variable_declaration_node(LINE, tPUBLIC,  cdk::primitive_type::create(4, cdk::typename_type::TYPE_UNSPEC), *$2, $4); delete $2;}
            | tPUBLIC tIDENTIFIER '=' block_expr          { $$ = new l22::variable_declaration_node(LINE, tPUBLIC,  cdk::primitive_type::create(4, cdk::typename_type::TYPE_UNSPEC), *$2, $4); delete $2;}
            /* ------------- FOREIGN ------------- */
            | tFOREIGN func_type tIDENTIFIER ';'             { $$ = new l22::variable_declaration_node(LINE, tFOREIGN,  $2, *$3, nullptr); delete $3; }
            /* TODO : é possivel fazer atribuições ? */
            // | tFOREIGN func_type tIDENTIFIER '=' expr    { $$ = new l22::variable_declaration_node(LINE, tFOREIGN,  $2, *$3, $5); delete $3; }
            // | tFOREIGN tVAR tIDENTIFIER '=' expr         { $$ = new l22::variable_declaration_node(LINE, tFOREIGN,  cdk::primitive_type::create(4, cdk::typename_type::TYPE_UNSPEC), *$3, $5); delete $3; }
            ;


declarations : declarations declaration                     { $$ = new cdk::sequence_node(LINE, $2, $1); }
             | declaration                                  { $$ = new cdk::sequence_node(LINE, $1); }
             ;


variable : data_type tIDENTIFIER        { $$ = new l22::variable_declaration_node(LINE, tPRIVATE, $1, *$2, nullptr); delete $2; }
         ;


variables : variables ',' variable      { $$ = new cdk::sequence_node(LINE, $3, $1); }
          | variable                    { $$ = new cdk::sequence_node(LINE, $1); }
          ;

func_type : data_type '<' '>'             { $$ = cdk::functional_type::create($1); }
          | data_type '<' data_types '>'  { $$ = cdk::functional_type::create(*$3, $1); }
          ;
            
data_type : tINT_TYPE                     { $$ = cdk::primitive_type::create(4, cdk::typename_type::TYPE_INT); }
          | tDOUBLE_TYPE                  { $$ = cdk::primitive_type::create(8, cdk::typename_type::TYPE_DOUBLE); }
          | tTEXT_TYPE                    { $$ = cdk::primitive_type::create(4, cdk::typename_type::TYPE_STRING); }
          | func_type	                 { $$ = $1; }
          | tVOID_TYPE                    { $$ = cdk::primitive_type::create(4, cdk::typename_type::TYPE_VOID); }
          | '[' data_type ']'             { $$ = cdk::reference_type::create(4, $2); }
          ;   


data_types : data_types ',' data_type   { $$ = $1; $$->push_back($3); }
           | data_type                  { $$ = new std::vector<std::shared_ptr<cdk::basic_type>>(); $$->push_back($1); }
           ;
          

block     : '{' declarations instructions '}'         { $$ = new l22::block_node(LINE, $2, $3); }
          | '{' instructions '}'                      { $$ = new l22::block_node(LINE, new cdk::sequence_node(LINE), $2); }
     	| '{' declarations '}'                      { $$ = new l22::block_node(LINE, $2, new cdk::sequence_node(LINE)); }
          | '{' '}'                                   { $$ = new l22::block_node(LINE, new cdk::sequence_node(LINE), new cdk::sequence_node(LINE)); }
          ;


instructions : instruction                            { $$ = new cdk::sequence_node(LINE, $1); }
             | block_instruction                      { $$ = new cdk::sequence_node(LINE, $1); }
             | instruction ';' instructions           { std::reverse($3->nodes().begin(), $3->nodes().end()); $$ = new cdk::sequence_node(LINE, $1, $3); std::reverse($$->nodes().begin(), $$->nodes().end()); }
             | block_instruction instructions         { std::reverse($2->nodes().begin(), $2->nodes().end()); $$ = new cdk::sequence_node(LINE, $1, $2); std::reverse($$->nodes().begin(), $$->nodes().end()); }
             ;

block_instruction : block_expr                         { $$ = new l22::evaluation_node(LINE, $1); }
                  | tIF '(' expr ')' tTHEN block       { $$ = new l22::if_node(LINE, $3, $6); }
                  | tIF '(' expr ')' tTHEN block elif  { $$ = new l22::if_else_node(LINE, $3, $6, $7); }
                  | tWHILE '(' expr ')' tDO block      { $$ = new l22::while_node(LINE, $3, $6); }
                  | tRETURN block_expr                 { $$ = new l22::return_node(LINE, $2); }                         
                  ;


instruction    : expr                                                      { $$ = new l22::evaluation_node(LINE, $1); }
               | tWRITE exprs                                              { $$ = new l22::print_node(LINE, $2, false); }
               | tWRITELN exprs                                            { $$ = new l22::print_node(LINE, $2, true); }
               | tAGAIN                                                    { $$ = new l22::again_node(LINE); }
               | tSTOP                                                     { $$ = new l22::stop_node(LINE); }
               | tRETURN expr                                              { $$ = new l22::return_node(LINE, $2); }
               | tRETURN                                                   { $$ = new l22::return_node(LINE); }
               | tSWEEP expr tTYPE_DECL expr tWITH expr '.' '.' expr       { $$ = new l22::sweep_node(LINE, $2, $4, $6, $9); }
               | tWITH expr tCHANGE expr tTYPE_DECL expr '.' '.' expr      { $$ = new l22::with_node(LINE, $2, $4, $6, $9); }
               ;

elif : tELSE block                                 { $$ = $2; }
     | tELIF '(' expr ')' tTHEN block              { $$ = new l22::if_node(LINE, $3, $6); }
     | tELIF '(' expr ')' tTHEN block elif         { $$ = new l22::if_else_node(LINE, $3, $6, $7); }

exprs : exprs ',' expr                  { $$ = new cdk::sequence_node(LINE, $3, $1); }
      | expr                            { $$ = new cdk::sequence_node(LINE, $1); } 
      | exprs ',' block_expr            { $$ = new cdk::sequence_node(LINE, $3, $1); }
      | block_expr                      { $$ = new cdk::sequence_node(LINE, $1); }
      ;

string : tSTRING                        { $$ = $1; }
       | string tSTRING                 { $$ = new std::string(*$1 + *$2); delete $1; delete $2; }
       ;

expr : tINTEGER                         { $$ = new cdk::integer_node(LINE, $1); }
     | tDOUBLE                          { $$ = new cdk::double_node(LINE, $1); }
     | string                           { $$ = new cdk::string_node(LINE, $1); }
     | tNULL                            { $$ = new l22::nullptr_node(LINE); }
     | '+' expr %prec tUNARY            { $$ = new l22::identity_node(LINE, $2); }
     | '-' expr %prec tUNARY            { $$ = new cdk::neg_node(LINE, $2); }
     | expr '+' expr	               { $$ = new cdk::add_node(LINE, $1, $3); }
     | expr '-' expr	               { $$ = new cdk::sub_node(LINE, $1, $3); }
     | expr '*' expr	               { $$ = new cdk::mul_node(LINE, $1, $3); }
     | expr '%' expr	               { $$ = new cdk::mod_node(LINE, $1, $3); }
     | expr '/' expr	               { $$ = new cdk::div_node(LINE, $1, $3); }
     | expr '<' expr	               { $$ = new cdk::lt_node(LINE, $1, $3); }
     | expr '>' expr	               { $$ = new cdk::gt_node(LINE, $1, $3); }
     | expr tGE expr	               { $$ = new cdk::ge_node(LINE, $1, $3); }
     | expr tLE expr                    { $$ = new cdk::le_node(LINE, $1, $3); }
     | expr tNE expr	               { $$ = new cdk::ne_node(LINE, $1, $3); }
     | expr tEQ expr	               { $$ = new cdk::eq_node(LINE, $1, $3); }
     | expr tOR expr                    { $$ = new cdk::or_node(LINE, $1, $3); }
     | expr tAND expr                   { $$ = new cdk::and_node(LINE, $1, $3); }
     | tNOT expr                        { $$ = new cdk::not_node(LINE, $2); }
     | '(' expr ')'                     { $$ = $2; }
     | '[' expr ']'                     { $$ = new l22::stack_alloc_node(LINE, $2);}
     | lval '?' %prec tUNARY            { $$ = new l22::address_of_node(LINE, $1); }  
     | lval                             { $$ = new cdk::rvalue_node(LINE, $1); }
     | lval '=' expr                    { $$ = new cdk::assignment_node(LINE, $1, $3); }
     | tSIZEOF '(' expr ')'             { $$ = new l22::sizeof_node(LINE, $3); }
     | tINPUT                           { $$ = new l22::read_node(LINE); }
     
     /* ---------------- FUNÇÕES ---------------- */	
     | '(' function_def ')' '(' exprs ')'       { $$ = new l22::function_call_node(LINE, $2, $5); }
     | '(' function_def ')' '(' ')'             { $$ = new l22::function_call_node(LINE, $2, new cdk::sequence_node(LINE)); }
     | lval '(' exprs ')'                       { $$ = new l22::function_call_node(LINE, new cdk::rvalue_node(LINE, $1), $3); }
     | lval '(' ')'                             { $$ = new l22::function_call_node(LINE, new cdk::rvalue_node(LINE, $1), new cdk::sequence_node(LINE)); }
     | '@' '(' exprs ')'                        { $$ = new l22::function_call_node(LINE, nullptr, $3); }
     | '@' '(' ')'                              { $$ = new l22::function_call_node(LINE, nullptr, new cdk::sequence_node(LINE)); }
     ;


            
function_def : '(' ')' tTYPE_DECL data_type ':' block            { $$ = new l22::function_definition_node(LINE, new cdk::sequence_node(LINE), $4, $6); }
             | '(' variables ')' tTYPE_DECL data_type ':' block  { $$ = new l22::function_definition_node(LINE, $2, $5, $7); /* TODO : add functional type */}
             ;

block_expr : function_def           { $$ = $1; }
           | lval '=' block_expr    { $$ = new cdk::assignment_node(LINE, $1, $3); }
           ;

lval : tIDENTIFIER                 { $$ = new cdk::variable_node(LINE, $1); }
     | expr '[' expr ']'           { $$ = new l22::index_node(LINE, $1, $3); } 
     ;

%%
