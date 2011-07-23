/* Include C libraries */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* This file describes the statement and expression parser. The parser
 * converts the token stream into a series of statements to be compiled
 * into machine code later. 
 *
 * Unlike some languages such as Lisp or Ruby, C differentiates between 
 * statements and expressions, so the parser has two separate data structures
 * for the two types of syntax elements. Note that since some expressions also
 * double as full-fledged statements (such as var++ and x = 5), there is also
 * a generic statement which stores an inner expression.
 *
 * This parser uses a modified Shunning-Yard algorithm for parsing infix 
 * expressions. It uses a look-behind token to resolve some semantic
 * conflicts, such as whether the asterisk * means multiplication or
 * dereferencing. (x + *y has a different meaning completely from x * y.)
 *
 * In addition to doing parsing, the parser creates and maintains a type table
 * which stores all the known primitive types and any user defined types. The
 * types stored in this type table can later be used for compiling to assembly.
 */

/* Initialize all aspects of the parser */
void init_parser(){
    init_type_table();
}

/* Delete all parser resources */
void del_parser(){
    del_type_table();
}

