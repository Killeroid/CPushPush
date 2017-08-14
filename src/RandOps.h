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

#ifndef _RAND_OPS_H__
#define _RAND_OPS_H__

#include "RNG.h"
#include "Env.h"
#include "CodeUtils.h"

namespace push {

inline unsigned rand_bool(Env& env) {
    push<bool>(env, rng.flip(0.5));
    return 1;
}

inline unsigned rand_int(Env& env) {
    int mn = env.parameters.min_random_integer;
    int mx = env.parameters.max_random_integer;

    int erc = rng.random(mx-mn)+mn;

    push(env,erc);
    
    return 1;
}

inline unsigned rand_float(Env& env) {
    double mn = env.parameters.min_random_float;
    double mx = env.parameters.max_random_float;

    double erc = rng.uniform() * (mx-mn) + mn;
    
    push(env, erc);

    return 1;
}

inline unsigned rand_code(Env& env) {
   
    // make sure random code is generated in a 'fresh' environment

    Env* fresh = env.clone();
    
    Code code = random_code(*fresh, env.parameters.max_points_in_random_expression);
    push(env,code);
    
    delete fresh;

    return code->size();
}

} // namespace push
#endif
