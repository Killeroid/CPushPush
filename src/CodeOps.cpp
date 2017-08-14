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

#include "CodeOps.h"
using namespace std;
namespace push {
    
unsigned _nth(Env& env) {
    //if (not has_elements<Code>(env,1)) return 1;
    //if (not has_elements<int>(env,1)) return 1;

    Code first = pop<Code>(env);
    int val = pop<int>(env);

    if (first->get_stack().size() == 0) { // nil or atom
        push(env, first);
        return 1;
    }
    
    const CodeArray& stack = first->get_stack();
    val = std::abs(val) % stack.size();
    
    push(env, stack[stack.size()-val-1]);
    
    return first->len();
}

unsigned _nthcdr(Env& env) {
    //if (not has_elements<Code>(env,1)) return 1;
    //if (not has_elements<int>(env,1)) return 1;

    Code first = pop<Code>(env);
    int val = pop<int>(env);

    if (first->get_stack().size() == 0) { // nil or atom
	push(env, first);
	return 1;
    }
    
    CodeArray stack = first->get_stack();
    val = std::abs(val) % stack.size();

    while(--val >= 0) stack.pop_back();
    
    push(env, Code(new CodeList(stack)));
    
    return first->len();
}

unsigned _null(Env& env) {
    //if (not has_elements<Code>(env,1)) return 1;
    Code first = pop<Code>(env);

    push(env, equal_to(nil, first));
    return 1;
}

unsigned _position(Env& env) {
    //if (not has_elements<Code>(env,2)) return 1;
   
    Code first = pop<Code>(env);
    Code second = pop<Code>(env);

    if (first->get_stack().size() == 0) {
	if (equal_to(first,second))
	    push(env, 0);
	else
	    push(env,-1);

	return 1;
    }

    const CodeArray& stack = first->get_stack();
    for (int i = stack.size()-1;i >= 0; --i) {
	if (equal_to(stack[i], second)) {
	    push<int>(env, stack.size()-i-1);
	    return stack.size() * second->len();
	}
    }

    push<int>(env, -1);
    return stack.size() * second->len();
}

}
