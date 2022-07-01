%option c++ prefix="l22_scanner_" outfile="l22_scanner.cpp"
%option stack noyywrap yylineno 8bit debug
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "l22_parser.tab.h"

static bool restart_functions = false;
static int lLevelsToPop = 0, lIndex = 0;
static std::vector<int> indents;   // known indentation levels
static std::vector<int> deferred;  // deferred tokens

static std::ostringstream strlit;

inline bool valid(int indent) {
    lLevelsToPop = lIndex = -1;
    bool indentOk = false;
    for (size_t ix = 0; ix < indents.size(); ix++) {
        if (indent == indents[ix]) {
            lLevelsToPop = indents.size() - ix - 1;
            lIndex = ix;
            indentOk = true;
        }
    }
    return indentOk;
}

// don't change this
#define yyerror LexerError
%}

SPACE           [ \t]

%x X_STRING X_COMMENT_ONE X_COMMENT_MULTI X_BACKSLASH X_NEWLINE
%%

 /* ---------------------------------------------------------------------- */
 /* -                           IDENTATION                               - */
 /* ---------------------------------------------------------------------- */
 /*                                                                        */

  {
        //yydebug=1; set_debug(1);
        if (restart_functions) {
          indents.pop_back();
          if (indents.size() == 0) {
            restart_functions = false;
          }
          return '}';
        } else {
          if (lLevelsToPop > 0) {
            lLevelsToPop--;
            indents.pop_back();
            return '}';
          }
        }
        
  }

\n                        yy_push_state(X_NEWLINE); /* delay decision */

<X_NEWLINE>^{SPACE}+      {
                            yy_pop_state();
  
                            if (valid(yyleng)) {
                              if (lLevelsToPop == 0) {
                                return ';';
                              } else {
                                if (lLevelsToPop > 0) {
                                  lLevelsToPop--;
                                  indents.pop_back();
                                  return '}';
                                }
                              }
                            } else if (indents.size() == 0) {
                                indents.push_back(yyleng);
                                return '{';
                            } else if (yyleng > indents.back()) {
                              indents.push_back(yyleng);
                              return '{';
                            } else {
                              std::cerr << "bad indent at line " << yylineno;
                            }
                          }

<X_NEWLINE>\n             ; // ignore successive newline chars
<X_NEWLINE>.              {
                            yyless(0);
                            yy_pop_state();
                            if (indents.size() > 0) {
                              indents.pop_back();
                              if (indents.size() > 0) { 
                                restart_functions = true; 
                              }
                              return '}';
                            } else {
                              return ';'; // top-level declarations
                            }
                          }

 /* ---------------------------------------------------------------------- */
 /* -                              TYPES                                 - */
 /* ---------------------------------------------------------------------- */
 /* int double text void                                                   */
 
"int"             return tINT_TYPE;
"double"          return tDOUBLE_TYPE;
"text"            return tTEXT_TYPE;
"void"            return tVOID_TYPE;
[,<>\[\]]         return *yytext;
 
 /* ---------------------------------------------------------------------- */
 /* -                           DECLARATIONS                             - */
 /* ---------------------------------------------------------------------- */
 /* foreign use public var                                                 */
 
"foreign"         return tFOREIGN;
"use"             return tUSE;
"public"          return tPUBLIC;
"var"             return tVAR;
"->"              return tTYPE_DECL;
 
 /* ---------------------------------------------------------------------- */
 /* -                            OPERATORS                               - */
 /* ---------------------------------------------------------------------- */
 /* and or not () [] + - ? * / % < > <= >= == != =                         */
 /* (Only representing as keywords symbol with >1 chars.)                  */
 
"and"               return tAND;
"or"                return tOR;
"not"               return tNOT;
">="                return tGE; 
"<="                return tLE; 
"=="                return tEQ; 
"!="                return tNE; 
[-()\[\]+?*/%<>=:]  return *yytext;

 /* ---------------------------------------------------------------------- */
 /* -                           INSTRUCTIONS                             - */
 /* ---------------------------------------------------------------------- */
 /* if then: elif else: while do: stop again write writeln return          */

[.]               return *yytext;
"sweep"           return tSWEEP;
"change"          return tCHANGE;
"with"            return tWITH;
"if"              return tIF;
"elif"            return tELIF;
"while"           return tWHILE; 
"stop"            return tSTOP; 
"again"           return tAGAIN; 
"write"           return tWRITE; 
"writeln"         return tWRITELN; 
"return"          return tRETURN;

"then:"           return tTHEN;
"do:"             return tDO;
"else:"           return tELSE;

 
 /* ---------------------------------------------------------------------- */
 /* -                          EXPRESSIONS                               - */
 /* ---------------------------------------------------------------------- */
 /* input null                                                             */
 
"input"           return tINPUT;
"null"            return tNULL;
 
 /* ---------------------------------------------------------------------- */
 /* -                            OTHERS                                  - */
 /* ---------------------------------------------------------------------- */
 /* begin end sizeof                                                       */
 
"begin"           return tBEGIN;
"end"             return tEND;
"sizeof"          return tSIZEOF;
[ ]+"..."[ ]*\n   ;
 
 /* ---------------------------------------------------------------------- */
 /* -                           COMMENTS                                 - */
 /* ---------------------------------------------------------------------- */
 /* ; (* *)                                                                */
 
";"                    yy_push_state(X_COMMENT_ONE);
<X_COMMENT_ONE>\n      yy_pop_state(); yy_push_state(X_NEWLINE);
<X_COMMENT_ONE>.       ;
 
"(*"                   yy_push_state(X_COMMENT_MULTI);
<X_COMMENT_MULTI>"*)"  yy_pop_state();
<X_COMMENT_MULTI>"(*"  yy_push_state(X_COMMENT_MULTI);
<X_COMMENT_MULTI>.|\n  ; 
 
 /* ---------------------------------------------------------------------- */
 /* -                           INTEGER                                  - */
 /* ---------------------------------------------------------------------- */
 
 /* Base 10 */
0|[1-9][0-9]*     { try {
                      yylval.i = std::stoi(yytext, nullptr, 10);
                      return tINTEGER;
                    } catch (std::out_of_range &e) { yyerror("Integer out of range."); }
                  }
  
 /* Base 7 (starts with 0, followed by one or more digits ranging from 0-6) */
0[0-6]+           { try {
                      yylval.i = std::stoi(yytext, nullptr, 7);
                      return tINTEGER;
                    } catch (std::out_of_range &e) { yyerror("Integer out of range."); }
                  }

0[0-9]+           yyerror("Invalid base 7 number");
 
 
 /* ---------------------------------------------------------------------- */
 /* -                           DOUBLE                                   - */
 /* ---------------------------------------------------------------------- */
 
[0-9]+"."[0-9]+       { try { 
                          yylval.d = std::stod(yytext, NULL); 
                          return tDOUBLE; 
                        } catch (std::out_of_range& e) { yyerror("Double out of range."); }
                      }
 
[0-9]+"."             { try { 
                          yylval.d = std::stod(yytext, NULL); 
                          return tDOUBLE; 
                        } catch (std::out_of_range& e) { yyerror("Double out of range."); }
                      }
 
"."[0-9]+             { try { 
                          yylval.d = std::stod(yytext, NULL); 
                          return tDOUBLE; 
                        } catch (std::out_of_range& e) { yyerror("Double out of range."); }
                      }
 
[0-9]+("."[0-9]+)?[Ee][-+]?[0-9]+   { try { 
                                        yylval.d = std::stod(yytext, NULL); 
                                        return tDOUBLE; 
                                      } catch (std::out_of_range& e) { yyerror("Double out of range."); }
                                    }
 
 /* ---------------------------------------------------------------------- */
 /* -                            TEXT                                    - */
 /* ---------------------------------------------------------------------- */

"\""                      yy_push_state(X_STRING);
<X_STRING>"\""            {
                            yylval.s = new std::string(strlit.str());
                            strlit.str("");
                            yy_pop_state();
                            return tSTRING;
                          }

<X_STRING>\\                   yy_push_state(X_BACKSLASH); 
<X_STRING>[ ]+"..."[ ]*\n\t*   ;
<X_STRING>\n                   yyerror("Invalid character in string."); /* Invalid characters, 0x0A */
<X_STRING>\0                   yyerror("Invalid character in string."); /* Invalid characters, 0x00 */
<X_STRING>.                    strlit << *yytext;

<X_BACKSLASH>t                 strlit << '\t'; yy_pop_state();
<X_BACKSLASH>n                 strlit << '\n'; yy_pop_state();
<X_BACKSLASH>r                 strlit << '\r'; yy_pop_state();
<X_BACKSLASH>\\                strlit << '\\'; yy_pop_state();
<X_BACKSLASH>"\""              strlit << '\"'; yy_pop_state();

<X_BACKSLASH>[0-6]{1,3}  {
                            unsigned long ul = strtoul(yytext, NULL, 7); 
                            if (ul > 255) {
                              yyerror("Invalid character in string.");
                            } else {
                              strlit << (char)(unsigned char) ul;
                            }
                            yy_pop_state();
                          }

<X_BACKSLASH>.            strlit << *yytext; yy_pop_state();
 

 /* ---------------------------------------------------------------------- */
 /* -                         IDENTIFIERS                                - */
 /* ---------------------------------------------------------------------- */
 
[A-Za-z][A-Za-z0-9_]*  yylval.s = new std::string(yytext); return tIDENTIFIER;


.                         ; /* ignore all unmatched characters */
%%