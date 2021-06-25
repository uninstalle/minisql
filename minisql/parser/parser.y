%{
    #include <iostream>
    extern int yylex(void);
    void yyerror(const char* s);
%}

%code requires {
    #include "instruction.hh"
    extern void flex_flush_buffer();
}


%union{
    std::string* string_literal;
    int32_t* integer_literal;
    float* float_literal;
    Minisql::Instruction::Field* field;
    Minisql::Instruction::FieldList* field_list;
    Minisql::Instruction::Condition* condition;
    Minisql::Instruction::ConditionList* condition_list;
    Minisql::Instruction::ValueList* value_list;

    Minisql::Instruction::Instruction* base_inst;
    Minisql::Instruction::CreateTable* create_table_inst;
    Minisql::Instruction::DropTable* drop_table_inst;
    Minisql::Instruction::CreateIndex* create_index_inst;
    Minisql::Instruction::DropIndex* drop_index_inst;
    Minisql::Instruction::Select* select_inst;
    Minisql::Instruction::Insert* insert_inst;
    Minisql::Instruction::Delete* delete_inst;
    Minisql::Instruction::Quit* quit_inst;
    Minisql::Instruction::Execfile* execfile_inst;
}


%token
KW_CREATE
KW_TABLE
KW_DROP
KW_INDEX
KW_ON
KW_SELECT
KW_FROM
KW_WHERE
KW_INSERT
KW_INTO
KW_VALUES
KW_DELETE
KW_QUIT
KW_EXECFILE
KW_UNIQUE
KW_PRIMARY
KW_KEY
KW_AND
KW_INT
KW_FLOAT
KW_CHAR
OP_LP
OP_RP
OP_SEMI
OP_COMMA
OP_WILDCARD
OP_GE
OP_GT
OP_LE
OP_LT
OP_EQ
OP_NE

%token <string_literal> NAME
%token <string_literal> CHAR
%token <float_literal> FLOAT
%token <integer_literal> INTEGER

%type <string_literal> literal
%type <string_literal> type
%type <string_literal> attribute
%type <field> field_decl
%type <field_list> field_decl_list
%type <string_literal> primary_key_define
%type <condition> condition
%type <condition_list> conditions
%type <string_literal> op
%type <value_list> values

%type <base_inst> query
%type <create_table_inst> create_table
%type <drop_table_inst> drop_table
%type <create_index_inst> create_index
%type <drop_index_inst> drop_index
%type <select_inst> select
%type <insert_inst> insert
%type <delete_inst> delete
%type <quit_inst> quit
%type <execfile_inst> execfile


%%

queries:
    queries query {
        Minisql::InstructionQueue::enqueue($2);
        if (Minisql::isJIT)
            YYACCEPT;
    }
    | %empty
    ;

query:
    create_table {
        $$ = $1;
    }
    | drop_table {
        $$ = $1;
    }
    | create_index {
        $$ = $1;
    }
    | drop_index{
        $$ = $1;
    }
    | select {
        $$ = $1;
    }
    | insert {
        $$ = $1;
    }
    | delete {
        $$ = $1;
    }
    | quit {
        $$ = $1;
    }
    | execfile {
        $$ = $1;
    }
    ;

create_table:
    KW_CREATE KW_TABLE NAME OP_LP field_decl_list OP_RP OP_SEMI {
        $$ = new Minisql::Instruction::CreateTable($3,$5);
    }
    ;

field_decl_list:
    field_decl_list OP_COMMA primary_key_define {
        $$ = $1;
        $$->findPrimaryKey($3);
    }
    | field_decl_list OP_COMMA field_decl {
        $$ = $1;
        $$->fieldList.push_back(std::move(*$3));
        delete $3;
    }
    | field_decl {
        $$ = new Minisql::Instruction::FieldList();
        $$->fieldList.push_back(std::move(*$1));
        delete $1;
    }
    ;

field_decl:
    NAME type attribute {
        $$ = new Minisql::Instruction::Field($1,$2,$3);
    }
    | NAME type {
        $$ = new Minisql::Instruction::Field($1,$2);
    }
    ;

type:
    KW_INT {
        $$ = new std::string("int");
    }
    | KW_FLOAT {
        $$ = new std::string("float");
    }
    | KW_CHAR OP_LP INTEGER OP_RP {
        $$ = new std::string("char" + std::to_string(std::move(*$3)));
        delete $3;
    }
    ;

attribute:
    KW_UNIQUE {
        $$ = new std::string("unique");
    }
    ;

primary_key_define:
    KW_PRIMARY KW_KEY OP_LP NAME OP_RP {
        $$ = $4;
    }

drop_table:
    KW_DROP KW_TABLE NAME OP_SEMI {
        $$ = new Minisql::Instruction::DropTable($3);
    }
    ;

create_index:
    KW_CREATE KW_INDEX NAME KW_ON NAME OP_LP NAME OP_RP OP_SEMI {
        $$ = new Minisql::Instruction::CreateIndex($3,$5,$7);
    }
    ;

drop_index:
    KW_DROP KW_INDEX NAME OP_SEMI {
        $$ = new Minisql::Instruction::DropIndex($3);
    }
    ;

select:
    KW_SELECT OP_WILDCARD KW_FROM NAME OP_SEMI {
        $$ = new Minisql::Instruction::Select($4);
    }
    | KW_SELECT OP_WILDCARD KW_FROM NAME KW_WHERE conditions OP_SEMI {
        $$ = new Minisql::Instruction::Select($4,$6);
    }
    ;

conditions:
    conditions KW_AND condition {
        $$ = $1;
        $$->conditions.push_back(std::move(*$3));
        delete $3;
    }
    | condition {
        $$ = new Minisql::Instruction::ConditionList();
        $$->conditions.push_back(std::move(*$1));
        delete $1;
    }
    ;

condition:
    NAME op literal {
        $$ = new Minisql::Instruction::Condition($1,$2,$3);
    }
    ;

op:
    OP_EQ {
        $$ = new std::string("=");
    }
    | OP_NE {
        $$ = new std::string("!=");
    }
    | OP_GE {
        $$ = new std::string(">=");
    }
    | OP_GT {
        $$ = new std::string(">");
    }
    | OP_LE {
        $$ = new std::string("<=");
    }
    | OP_LT {
        $$ = new std::string("<");
    }
    ;

literal:
    INTEGER {
        $$ = new std::string(std::to_string(std::move(*$1)));
        delete $1;
    }
    | FLOAT {
        $$ = new std::string(std::to_string(std::move(*$1)));
        delete $1;
    }
    | CHAR {
        $$ = $1;
    }
    ;

insert:
    KW_INSERT KW_INTO NAME KW_VALUES OP_LP values OP_RP OP_SEMI {
        $$ = new Minisql::Instruction::Insert($3,$6);
    }
    ;

values:
    values OP_COMMA literal {
        $$ = $1;
        $$->values.push_back(std::move(*$3));
        delete $3;
    }
    | literal {
        $$ = new Minisql::Instruction::ValueList();
        $$->values.push_back(std::move(*$1));
        delete $1;
    }
    ;

delete:
    KW_DELETE KW_FROM NAME KW_WHERE conditions OP_SEMI {
        $$ = new Minisql::Instruction::Delete($3,$5);
    }
    | KW_DELETE KW_FROM NAME OP_SEMI {
        $$ = new Minisql::Instruction::Delete($3);
    }
    ;

quit:
    KW_QUIT OP_SEMI {
        $$ = new Minisql::Instruction::Quit();
    }
    ;

execfile:
    KW_EXECFILE CHAR OP_SEMI {
        $$ = new Minisql::Instruction::Execfile($2);
    }
    ;

%%

void yyerror(const char* s)
{
    std::cerr << std::endl << s << std::endl;
}