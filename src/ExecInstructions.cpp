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

using namespace std;

namespace push {

void exec_define(Env& env, name_t name, Code code, bool set_writable = true) {
    set_code(name, code);
    
    // make sure 'lower envs' will use the same name/code binding
    if (env.has_next()) {
	exec_define(env.next(), name, code, set_writable);
    }
}
    
unsigned edefine(Env& env) {
    exec_define(env, pop<name_t>(env), pop<Exec>(env).lock(), true);
    return 1;
}

unsigned edefine2(Env& env) {
    name_t name = pop<name_t>(env);
    exec_define(env, name, pop<Exec>(env).lock(), false);
    return 1;
}

unsigned s(Env& env) {
    Exec x = pop<Exec>(env);
    Exec y = pop<Exec>(env); 
    Exec z = pop<Exec>(env);
   
    if (z->size() + y->size() + 1 >= env.parameters.max_points_in_program) {
	push(env, z);
	push(env, y);
	push(env, x);
	return 1;
    }
    
    env.push_guarded( list( y.lock() ,z.lock() ) );
    push(env, z);
    push(env, x);
    return 1;
}

unsigned k(Env& env) {
    Exec x = pop<Exec>(env);
    pop<Exec>(env);
    push(env,x);
    return 1;
}

unsigned y(Env& env) {
    Exec x = pop<Exec>(env);
    static Code ycode = parse("EXEC.Y");
    
    if (2 + x->size() >= env.parameters.max_points_in_program) {
	push(env, x); // too big
	return 1;
    }
    
    env.push_guarded(list(ycode, x.lock()));
    push(env, x);
    return 1;
}

unsigned exec_if(Env& env) {
	bool val = pop<bool>(env);
	Exec a = pop<Exec>(env);
	Exec b = pop<Exec>(env);
	if (val) {
	    push(env,a);
	} else {
	    push(env,b);	
	}
	return 1;
}

Code first_unbound_name(Code c, Env& env) {
    Literal<name_t>* name = dynamic_cast<Literal<name_t>* >(c.get());

    if (name != 0) {
	Code code = get_code( name->get() );
	
	// create default code, how it will be return if there's no binding (TODO: create function default_code_for_name)
	static Code name_quote = parse("NAME.QUOTE");
	CodeArray vec(2);
	vec[0] = c;
	vec[1] = name_quote;

	CodeList def(vec);

	if (def == *code) {
	    return c; // found it
	}
    }
    
    const CodeArray& vec = c->get_stack();

    for (int i = vec.size()-1; i >= 0; --i) {
	Code res = first_unbound_name(vec[i], env);
	if (res != nil) {
	    return res;
	}
    }

    return nil;
}

Code replace_all(Code obj, Code name, Code to_insert) {
    if (*obj == *name) {
	return to_insert;
    }

    CodeArray vec = obj->get_stack();

    if (vec.size()) {
	for (unsigned i = 0; i < vec.size(); ++i) {
	    vec[i] = replace_all(vec[i], name, to_insert);
	}

	return Code(new CodeList(vec));
    }
    // no list
    return obj;
}

class DoRangeClass : public CodeList {
    public:
    DoRangeClass(const CodeArray& vec) : CodeList(vec) { assert(vec.size()==4); }
    
    unsigned operator()(Env& env) const {

	CodeArray vec = get_stack();
	
	int i = static_cast<Literal<int>*>( vec[3].get() )->get();
	int n = static_cast<Literal<int>*>( vec[2].get() )->get();
	
	int direction = 1;
	if (i > n) direction = -1;
	push(env, i);
	
	Exec code = Exec(vec[0]);
	
	if (i != n) {
	    vec[3] = Code(new Literal<int>(i+direction));
	    Code ranger = CodeList::adopt(vec); 
	    env.push_guarded(ranger);
	}	
	
	push(env, code);
	return 1;
    }
};


unsigned do_range(Env& env) {
    
    // if (!can_pop<1,2>(env)) return 1;
    
    int n = pop<int>(env);
    int i = pop<int>(env);
    
    static Code DoRange = parse("EXEC.DO*RANGE");
    Exec code = pop<Exec>(env);
    CodeArray vec(4);
    
    vec[0] = code.lock();
    vec[1] = DoRange;
    vec[2] = Code(new Literal<int>(n));
    vec[3] = Code(new Literal<int>(i));

    Code result = Code( new DoRangeClass(vec)); 
    env.push_guarded(result);
    return 1;
}

static
unsigned do_count(Env& env) {
    //if (not has_elements<int>(env,1)) return 1;
    //if (not has_elements<Code>(env,1)) return 1;

    int n = pop<int>(env);
    Exec code = pop<Exec>(env);

    if (n < 0) { return 1; }
    
    static Code DoRange = parse("EXEC.DO*RANGE");
    CodeArray vec(4);
    static Code zero = Code(new Literal<int>(0));
    
    vec[0] = code.lock();
    vec[1] = DoRange;
    vec[2] = Code(new Literal<int>(n-1));
    vec[3] = zero;
    
    Code result = Code( new DoRangeClass(vec));
    env.push_guarded(result);
    return 1;
}


static
unsigned do_times(Env& env) {
    int n = pop<int>(env);
    Exec code = pop<Exec>(env);

    if (n <= 0) { return 1;}
    
    CodeArray vec(4);
    static Code zero = Code(new Literal<int>(0));
    static Code int_pop = parse("INTEGER.POP");
    static Code DoRange = parse("EXEC.DO*RANGE");
    
    vec[0] = cons(int_pop, code.lock());
    vec[1] = DoRange;
    vec[2] = Code(new Literal<int>(n-1));
    vec[3] = zero;
    
    Code result = Code( new DoRangeClass(vec));
    env.push_guarded(result);
    return 1;
}

void initExec() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    make_instruction(k,"EXEC.K", execType + execType, execType);
    make_instruction(s,"EXEC.S", execType+execType+execType, execType + execType + execType);
    
    // EXEC.DEFINE is defined in Instructions.cpp (name_set)
    //make_instruction(edefine,"EXEC.DEFINE", execType+nameType, nullType);
    
    make_instruction(edefine2,":", execType+nameType, nullType);
    make_instruction(y,"EXEC.Y", execType, execType+execType);
    make_instruction(exec_if, "EXEC.IF", execType+execType+boolType, execType);
    make_instruction(do_range,"EXEC.DO*RANGE", integerType+integerType+execType, execType, false);
    make_instruction(do_count, "EXEC.DO*COUNT", integerType+execType, execType, false);
    make_instruction(do_times, "EXEC.DO*TIMES", integerType+execType, execType, false);
    
    
    //static Code exec_lambda = make_instruction(lambda, "EXEC.LAMBDA", execType+execType, execType, false);
}


} // namespace push

