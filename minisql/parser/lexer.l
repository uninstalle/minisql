%{
    #include <iostream>
    #include "Parser.hh"
    #include "instruction.hh"
    int lineno = 1;
    int colno = 1;
%}

%%

create {
    colno += yyleng;
    return KW_CREATE;
}

table {
    colno += yyleng;
    return KW_TABLE;
}

drop {
    colno += yyleng;
    return KW_DROP;
}

index {
    colno += yyleng;
    return KW_INDEX;
}

on {
    colno += yyleng;
    return KW_ON;
}

select {
    colno += yyleng;
    return KW_SELECT;
}

from {
    colno += yyleng;
    return KW_FROM;
}

where {
    colno += yyleng;
    return KW_WHERE;
}

insert {
    colno += yyleng;
    return KW_INSERT;
}

into {
    colno += yyleng;
    return KW_INTO;
}

values {
    colno += yyleng;
    return KW_VALUES;
}

delete {
    colno += yyleng;
    return KW_DELETE;
}

quit {
    colno += yyleng;
    return KW_QUIT;
}

execfile {
    colno += yyleng;
    return KW_EXECFILE;
}

unique {
    colno += yyleng;
    return KW_UNIQUE;
}

primary {
    colno += yyleng;
    return KW_PRIMARY;
}

key {
    colno += yyleng;
    return KW_KEY;
}

and {
    colno += yyleng;
    return KW_AND;
}

int {
    colno += yyleng;
    return KW_INT;
}

float {
    colno += yyleng;
    return KW_FLOAT;
}

char {
    colno += yyleng;
    return KW_CHAR;
}

"(" {
    colno += yyleng;
    return OP_LP;
}

")" {
    colno += yyleng;
    return OP_RP;
}

";" {
    colno += yyleng;
    return OP_SEMI;
}

"," {
    colno += yyleng;
    return OP_COMMA;
}

"*" {
    colno += yyleng;
    return OP_WILDCARD;
}

">=" {
    colno += yyleng;
    return OP_GE;
}

">" {
    colno += yyleng;
    return OP_GT;
}

"<=" {
    colno += yyleng;
    return OP_LE;
}

"<" {
    colno += yyleng;
    return OP_LT;
}

"=" {
    colno += yyleng;
    return OP_EQ;
}

"<>"|"!=" {
    colno += yyleng;
    return OP_NE;
}

[a-zA-Z][_a-zA-Z0-9]* {
    colno += yyleng;
    yylval.string_literal = new std::string(yytext);
    return NAME;
}

-?[0-9]+("."[0-9]+|e-?[0-9]+) {
    colno += yyleng;
    yylval.float_literal = new float(std::stof(yytext));
    return FLOAT;
}

-?[0-9]+ {
    colno += yyleng;
    yylval.integer_literal = new int (std::stoi(yytext));
    return INTEGER;
}

'[^\']{1,255}' {
    colno += yyleng;
    yylval.string_literal = new std::string(yytext + 1,yytext + yyleng - 1);
    return CHAR;
}


[ \t]+ {
    colno++;
}

\n\r? {
    lineno++;
    colno = 1;
}

. {
    std::cerr << "Unrecognized symbol: \'" << yytext << "\' at line " << lineno << " col " << colno << std::endl;
}

%%

int yywrap()
{
    return 1;
}

void flex_flush_buffer()
{
    yyrestart(yyin);
}