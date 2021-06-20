/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_PARSER_HH_INCLUDED
# define YY_YY_PARSER_PARSER_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 7 "./parser/parser.y"

    #include "instruction.hh"

#line 52 "./parser/parser.hh"

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    KW_CREATE = 258,
    KW_TABLE = 259,
    KW_DROP = 260,
    KW_INDEX = 261,
    KW_ON = 262,
    KW_SELECT = 263,
    KW_FROM = 264,
    KW_WHERE = 265,
    KW_INSERT = 266,
    KW_INTO = 267,
    KW_VALUES = 268,
    KW_DELETE = 269,
    KW_QUIT = 270,
    KW_EXECFILE = 271,
    KW_UNIQUE = 272,
    KW_PRIMARY = 273,
    KW_KEY = 274,
    KW_AND = 275,
    KW_INT = 276,
    KW_FLOAT = 277,
    KW_CHAR = 278,
    OP_LP = 279,
    OP_RP = 280,
    OP_SEMI = 281,
    OP_COMMA = 282,
    OP_WILDCARD = 283,
    OP_GE = 284,
    OP_GT = 285,
    OP_LE = 286,
    OP_LT = 287,
    OP_EQ = 288,
    OP_NE = 289,
    NAME = 290,
    CHAR = 291,
    FLOAT = 292,
    INTEGER = 293
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 12 "./parser/parser.y"

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

#line 124 "./parser/parser.hh"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_PARSER_PARSER_HH_INCLUDED  */
