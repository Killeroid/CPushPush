/***************************************************************************
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

#ifndef _CODE_OPS_H__
#define _CODE_OPS_H__

#include "Env.h"
#include "CodeUtils.h"
#include "Literal.h"

namespace push {

inline
unsigned _cons(Env& env) {
    
    if (first<Code>(env)->size() + second<Code>(env)->size() >= env.parameters.max_points_in_program) return 1;
    
    Code a = pop<Code>(env);
    Code b = pop<Code>(env);
    push(env, cons(b,a));
    return 1;
}

inline
unsigned _list(Env& env) {
    if (1 + first<Code>(env)->size() + second<Code>(env)->size() >= env.parameters.max_points_in_program) return 1;
    
    Code a = pop<Code>(env);
    Code b = pop<Code>(env);
    push(env, list(b,a));
    return 1;
}

inline unsigned _length(Env& env) {
    Code a = pop<Code>(env);
    push<int>(env, a->len());
    return 1;
}

inline unsigned _size(Env& env) {
    Code a = pop<Code>(env);
    push<int>(env, a->size());
    return a->size();
}

/* Few Executable/quoting constructs */

inline unsigned _do(Env& env) {
    static Code code_pop = parse("CODE.POP");
    env.push_guarded(code_pop);
    env.push_guarded(top<Code>(env));
    return 1;
}

inline unsigned _do_star(Env& env) {
    env.push_guarded( pop<Code>(env) );
    return 1;
}

inline unsigned _quote(Env& env) {
    push<Code>(env, pop<Exec>(env).lock() );
    return 1;
}

inline unsigned _if(Env& env) {
    Code first = pop<Code>(env);
    Code second = pop<Code>(env);

    if (pop<bool>(env)) {
        env.push_guarded(second);
    } else {
        env.push_guarded(first);
    }
    return 1;
}

/* From */
template <class T>
inline unsigned _from_T(Env& env) { 
    T a = pop<T>(env);
    push<Code>(env, Code(new Literal<T>(a)));
    return 1;
}


inline unsigned _instructions(Env& env) { push(env, env.function_set); return 1; }

inline unsigned _append(Env& env) {
    if (1 + first<Code>(env)->size() + second<Code>(env)->size() >= env.parameters.max_points_in_program) return 1;
    
    Code first = pop<Code>(env);
    Code second = pop<Code>(env);

    CodeArray first_stack = first->get_stack();
    if (first_stack.empty()) first_stack.push_back(first);
    
    CodeArray second_stack = second->get_stack();
    if (second_stack.empty()) second_stack.push_back(second);
    
    second_stack.reserve(second_stack.size() + first_stack.size());
    std::copy(first_stack.begin(), first_stack.end(), std::back_inserter(second_stack) );

    push<Code>(env, CodeList::adopt(second_stack)); //Code(new CodeList(second_stack)));
    return first_stack.size();
}

inline unsigned _atom(Env& env) {
    if (pop<Code>(env)->get_stack().size()) {
        push<bool>(env, false);
    } else {
        push<bool>(env, true);
    }

    return 1;
}

inline unsigned _car(Env& env) {
    
    if (top<Code>(env)->get_stack().size()) 
        push<Code>(env, pop<Code>(env)->get_stack().back());
    return 1;
}

inline unsigned _cdr(Env& env) {

    if (top<Code>(env)->get_stack().size()) {
        CodeArray stack = pop<Code>(env)->get_stack();
        stack.pop_back();
        push<Code>(env, CodeList::adopt(stack)); //Code(new CodeList(stack)));
    }
	
    return 1;
}

inline unsigned _container(Env& env) {

    Code first = pop<Code>(env);
    Code second = pop<Code>(env);

    push(env, find_container(first, second));
    
    return first->size() * second->size();
}

inline unsigned _contains(Env& env) {

    Code first = pop<Code>(env);
    Code second = pop<Code>(env);

    push(env, contains(first, second));
    return first->size() * second->size();
}

inline unsigned _extract(Env& env) {
    //if (not has_elements<Code>(env,1)) return 1;
    //if (not has_elements<int>(env,1)) return 1;

    Code code = pop<Code>(env);
    int val = pop<int>(env);
    
    if (code->size() == 0) { // nil
        push(env, code);
        return 1;
    }

    val = std::abs(val) % code->size();

    push(env, extract(code, val));
    return code->size(); // Complexity is O(n) 
}

inline unsigned _insert(Env& env) {

    Code one    = first<Code>(env);
    Code other  = second<Code>(env);
    int val     = first<int>(env);
    
    /*if (one->size() == 0) { // nil
	pop<int>(env);
	pop<Code>(env);
	return 1;
    }*/

    val = std::abs(val) % one->size();
    
    Code result = insert(one, val, other);
    if (result->size() > env.parameters.max_points_in_program) return 1;
   
    pop<Code>(env);
    pop<Code>(env);
    pop<int>(env);
    
    push(env, result);
    return 1;
}

inline unsigned _member(Env& env) {

    Code first = pop<Code>(env);
    Code second = pop<Code>(env);

    push(env, member(first, second));

    return 1;
}

inline unsigned _noop(Env& env) { return 1; }

/* large parts of the stuff below should go to a sourc file */

unsigned _nth(Env& env); 
unsigned _nthcdr(Env& env);
unsigned _null(Env& env);
unsigned _position(Env& env);

} // namespace push

#endif
