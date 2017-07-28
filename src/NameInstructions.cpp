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

#include "Literal.h"

namespace push {

unsigned name_quote(Env& env) {
    env.quote_name_flag = true;
    return 1;
}

unsigned name_code(Env& env) {
    //if (!has_elements<name_t>(env,1)) return 1;
    name_t name = pop<name_t>(env);
    Code code = get_code(name);
    push(env,code);
    return 1;
}


unsigned name_do_star(Env& env) {
    name_t name = pop<name_t>(env);
    Code code = get_code(name);
    env.push_guarded(code);
    return 1;
}

unsigned name_do(Env& env) {
    name_t name = top<name_t>(env);
    Code code = get_code(name);
    env.push_guarded(code);
    return 1;
}

static
unsigned lambda(Env& env) {
    // TODO find a way of avoiding overflowing the name table with all these names

    name_t name = rand_name();
    push(env, name);
    Exec exec = pop<Exec>(env);
    Code code = pop<Exec>(env).lock();
    set_code( top<name_t>(env), code);
    
    push(env, exec);
    
    return 1;
}

unsigned rand_name_ins(Env& env)   { push(env, rand_name()); return 1; }
unsigned rand_bound_name_ins(Env& env) { push(env, rand_bound_name()); return 1; }

void initName() {
    
    make_instruction(name_quote, "NAME.QUOTE", nullType, nullType, false);
    make_instruction(name_code,"CODE.DEFINITION", nameType, codeType);
    make_instruction(rand_name_ins, "NAME.RAND", nullType, nameType);
    make_instruction(rand_name_ins, "NAME.ERC", nullType, nameType);
    make_instruction(rand_bound_name_ins, "NAME.RANDBOUNDNAME", nullType, nameType);
   
    //make_instruction(name_do_star, "NAME.DO*", nameType, execType);
    //make_instruction(name_do, "NAME.DO", nameType, execType + nameType);
    //make_instruction(lambda, "NAME.LAMBDA", execType+execType, execType + nameType);
}


} // namespace push


