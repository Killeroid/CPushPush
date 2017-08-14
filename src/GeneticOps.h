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

#ifndef _GENETIC_OPS_H__
#define _GENETIC_OPS_H__

#include "CodeUtils.h"
#include "RandOps.h"

namespace push {

inline Code subtree_xover(Code mom, Code dad) {
    
    int r1 = rng.random(mom->size());
    int r2 = rng.random(dad->size());

    return insert(mom, r1, extract(dad, r2));
}
    
inline unsigned subtree_xover(Env& env) {
    //if (not has_elements<Code>(env,2)) return 1;
    
    Code dad = pop<Code>(env);
    Code mom = pop<Code>(env);
    
    Code child = subtree_xover(mom, dad);
    // if this pushes over the size limit, don't do it

    if(child->size() >= (unsigned) env.parameters.max_points_in_program) {
        push(env, mom);
    } else {
        push(env, child);
    }

    return 1;
}

inline unsigned subtree_binary_xover(Env& env) {
    //if (not has_elements<Code>(env,2)) return 1;
    
    Code dad = pop<Code>(env);
    Code mom = pop<Code>(env);
    
    int r1 = rng.random(mom->binary_size());
    int r2 = rng.random(dad->binary_size());

    Code child = insert_binary(mom, r1, extract_binary(dad, r2));

    if(child->size() > (unsigned) env.parameters.max_points_in_program) {
        push(env, mom);
    } else {
        push(env, child);
    }

    return 1;
}

inline unsigned subtree_mutation(Env& env) {
    //if (not has_elements<Code>(env,1)) return 1;
    
    Code code = pop<Code>(env);

    int r1 = rng.random(code->size());
    //int subSize = extract(code, r1)->size();
    
    rand_code(env);
    
    Code newcode = pop<Code>(env);
    newcode = insert(code,r1,newcode); 

    if(newcode->size() >= (unsigned) env.parameters.max_points_in_program) {
        push(env, code);
    } else {
        push(env, newcode);
    }
    
    return 1;
}

inline unsigned subtree_fair_mutation(Env& env) {
	Code code = pop<Code>(env);

	int r1 = rng.random(code->size());
	int subSize = extract(code, r1)->size();

	// maybe in the future we'll add a parameter for this?
	// int range = (size * env.parameters.fair_mutation_range);

	int range = (int)(subSize * .3);
	if(range < 1) range = 1;

	int offset = rng.random(2 * range) - range;

	// save the old max points 

	int max_points = env.parameters.max_points_in_random_expression;

	env.parameters.max_points_in_random_expression = subSize + offset;

    if(env.parameters.max_points_in_random_expression < 1) {
		env.parameters.max_points_in_random_expression = 1;
    }

	env.clear_stacks();

	rand_code(env);
    
	Code newcode = pop<Code>(env);

	// if this pushes us over the size limit, don't do it

	int totalNewSize = newcode->size() + code->size() - subSize;

	if(totalNewSize > env.parameters.max_points_in_program) {
    	push(env, code);
	} else {
    	push(env, insert(code, r1, newcode)); 
	}

	// restore the old max points

	env.parameters.max_points_in_random_expression = max_points;

	return 1;
}

inline unsigned deletion_mutation(Env& env) {
    //if (not has_elements<Code>(env,1)) return 1;

    Code code = pop<Code>(env);

    int r = rng.random(code->size());

    push(env, insert(code, r, nil)); // TODO remove code instead of replacing with nil
    //push(env, remove(code, r));
    return 1;    
}

// This deletion_mutation_no_empty_list function is a replacement
// of the standard deletion_mutation that deletes a subtree instead
// of replacing it with ().  I think this might be the preferred 
// behavior in general, but I've left the original function in place
// in order to preserve the original behavior.

inline unsigned deletion_mutation_no_empty_list(Env& env) {
    Code code = pop<Code>(env);

    int r = rng.random(code->size());

    push(env, remove(code, r));

    return 1;    
}

} // namespace push
#endif
