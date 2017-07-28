/***************************************************************************
 *   Copyright (C) 2004 by Maarten Keijzer                                 *
 *   mkeijzer@xs4all.nl                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "Instruction.h"
#include "Env.h"

#include "CodeOps.h"
#include "Literal.h"
#include "RandOps.h"

namespace push{

unsigned code_do_range(Env& env) {
    
    int n = pop<int>(env);
    int i = pop<int>(env);
    
    Code code = pop<Code>(env);

    int direction = 1;
    if (i > n) direction = -1;

    push(env, i);
    if (i != n) {
    
	i += direction;
	    
	CodeArray vec(5);
	static Code quote   = parse("CODE.QUOTE");
	static Code DoRange = parse("CODE.DO*RANGE");
	
	vec[0] = DoRange;
	vec[1] = code;
	vec[2] = quote;
	vec[3] = Code(new Literal<int>(n));
	vec[4] = Code(new Literal<int>(i));

	env.push_guarded( CodeList::adopt(vec)); //Code(new CodeList(vec)));
	
	
    }
    env.push_guarded(code);
    
    return 1;
}

unsigned do_count(Env& env) {
    //if (not has_elements<int>(env,1)) return 1;
    //if (not has_elements<Code>(env,1)) return 1;

    int n = pop<int>(env);
    Code code = pop<Code>(env);

    if (n <= 0) { return 1; }
    
    CodeArray vec(5);
    static Code zero = Code(new Literal<int>(0));
    static Code quote = parse("CODE.QUOTE");
    static Code DoRange = parse("CODE.DO*RANGE");
    
    vec[0] = DoRange;
    vec[1] = quote;
    vec[2] = code;
    vec[3] = Code(new Literal<int>(n-1));
    vec[4] = zero;
    
    env.push_guarded(CodeList::adopt(vec)); //Code(new CodeList(vec)));
    return 1;
}


unsigned do_times(Env& env) {
    int n = pop<int>(env);
    Code code = pop<Code>(env);

    if (n <= 0) { return 1; }
    
    CodeArray vec(5);
    static Code zero = Code(new Literal<int>(0));
    static Code quote = parse("CODE.QUOTE");
    static Code int_pop = parse("INTEGER.POP");
    static Code DoRange = parse("CODE.DO*RANGE");
    
    vec[0] = DoRange;
    vec[1] = quote;
    vec[2] = cons(int_pop, code);
    vec[3] = Code(new Literal<int>(n-1));
    vec[4] = zero;
    
    env.push_guarded(CodeList::adopt(vec)); //Code(new CodeList(vec)));
    return 1;

}

void initCode() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    /* CODE */
    Type binaryCode = codeType + codeType;

    make_instruction(_cons, "CODE.CONS", binaryCode, codeType);
    make_instruction(_list, "CODE.LIST", binaryCode, codeType);
    make_instruction(_do, "CODE.DO", codeType, execType);
    make_instruction(_do_star, "CODE.DO*", codeType, execType);
    make_instruction(_quote, "CODE.QUOTE", execType, codeType);
    make_instruction(_if, "CODE.IF", boolType + binaryCode, execType);
    make_instruction(_length, "CODE.LENGTH", codeType, integerType);
    make_instruction(_size, "CODE.SIZE", codeType, integerType);

    make_instruction(_from_T<int>, "CODE.FROMINTEGER", integerType, codeType);
    make_instruction(_from_T<double>, "CODE.FROMFLOAT", floatType, codeType);
    make_instruction(_from_T<bool>, "CODE.FROMBOOLEAN", boolType, codeType);
    make_instruction(_from_T<name_t>, "CODE.FROMNAME", nameType, codeType);

    make_instruction(_instructions, "CODE.INSTRUCTIONS", nullType, codeType);
    make_instruction(_append, "CODE.APPEND", binaryCode, codeType);
    make_instruction(_atom, "CODE.ATOM", codeType, boolType);
    make_instruction(_car, "CODE.CAR", codeType, codeType);
    make_instruction(_cdr, "CODE.CDR", codeType, codeType);
    make_instruction(_container, "CODE.CONTAINER", binaryCode, codeType);
    make_instruction(_extract, "CODE.EXTRACT", codeType+integerType, codeType);
    make_instruction(_insert, "CODE.INSERT", binaryCode+integerType, codeType);
    make_instruction(_member, "CODE.MEMBER", binaryCode, boolType);
    make_instruction(_noop, "CODE.NOOP", nullType, nullType);
    make_instruction(_nth, "CODE.NTH", integerType + codeType, codeType);
    make_instruction(_nthcdr, "CODE.NTHCDR", integerType + codeType, codeType);
    make_instruction(_null, "CODE.NULL", codeType, boolType);
    make_instruction(_position, "CODE.POSITION", binaryCode, integerType);

    make_instruction(rand_code, "CODE.RAND", nullType, codeType);
    make_instruction(rand_code, "CODE.ERC", nullType, codeType);
    
    make_instruction(code_do_range, "CODE.DO*RANGE", integerType+integerType+codeType, execType+execType, false);
    make_instruction(do_count, "CODE.DO*COUNT", integerType+codeType, execType, false);
    make_instruction(do_times, "CODE.DO*TIMES", integerType+codeType, execType, false);
}


} // namespace push


