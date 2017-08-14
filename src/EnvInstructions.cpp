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

#include "Instruction.h"
#include "Env.h"

#include "RNG.h"

namespace push {

/* CONFIGURATION SPECIALS (under the caption ENV). These will not change the 'current' environment,
 * but rather the 'next' environment. This way the current environment is untouched and will not 
 * be destroyed. 
 *
 * There's one exception, the random seed. Currently we only have a global random number generator (which is not
 * a part of an env), So set the seed for *both* envs. 
 * */
unsigned set_instructions(Env& env) {
    env.next().function_set = pop<Code>(env);
    return 1;
} 

template <class T>
void set_param(T& value, Env& env) {
    value = pop<T>(env);
}

unsigned set_mxrf(Env& env) { set_param(env.next().parameters.max_random_float, env); return 1; }
unsigned set_mnrf(Env& env) { set_param(env.next().parameters.min_random_float, env); return 1; }
unsigned set_mxri(Env& env) { set_param(env.next().parameters.max_random_integer, env); return 1; }
unsigned set_mnri(Env& env) { set_param(env.next().parameters.min_random_integer, env); return 1; }
unsigned set_epl(Env& env)  { set_param(env.next().parameters.evalpush_limit, env); return 1; }
unsigned set_mpire(Env& env){ set_param(env.next().parameters.max_points_in_random_expression, env); return 1; }
unsigned set_mpip(Env& env) { set_param(env.next().parameters.max_points_in_program, env); return 1; }
unsigned set_rs(Env & env) {
    set_param(env.next().parameters.random_seed, env); 
    rng.reseed(env.next().parameters.random_seed);
    env.parameters.random_seed = env.next().parameters.random_seed;
    return 1;
}

void initEnv() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    
    make_instruction(set_instructions, "ENV.INSTRUCTIONS", codeType, nullType);
    make_instruction(set_mxrf,  "ENV.MAX-RANDOM-FLOAT", floatType, nullType);
    make_instruction(set_mnrf,  "ENV.MIN-RANDOM-FLOAT", floatType, nullType);
    make_instruction(set_mxri,  "ENV.MAX-RANDOM-INTEGER", integerType, nullType);
    make_instruction(set_mnri,  "ENV.MIN-RANDOM-INTEGER", integerType, nullType);
    make_instruction(set_epl,   "ENV.EVALPUSH-LIMIT", integerType, nullType);
    make_instruction(set_mpire, "ENV.MAX-POINTS-IN-RANDOM-EXPRESSION", integerType, nullType);
    make_instruction(set_mpip,  "ENV.MAX-POINTS-IN-PROGRAM", integerType, nullType);
    make_instruction(set_rs,    "ENV.RANDOM-SEED", integerType, nullType);
}


} // namespace push

