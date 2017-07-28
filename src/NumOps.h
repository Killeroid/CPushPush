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

#ifndef _NUM_OPS_H__
#define _NUM_OPS_H__

#include "Env.h"

namespace push
{ // plus, minus, multiplies, divides

    
    template <class T>
    inline unsigned plus(Env& env) {
	//if (not has_elements<T>(env,2)) return 1;
	T first = pop<T>(env);
	get_stack<T>(env).back() += first;
	return 1;
    }

    template <class T>
    inline unsigned minus(Env& env) {
	//if (not has_elements<T>(env,2)) return 1;
	T first = pop<T>(env);
	get_stack<T>(env).back() -= first;
	return 1;
    }

    template <class T>
    inline unsigned multiplies(Env& env) {
	//if (not has_elements<T>(env,2)) return 1;
	T first = pop<T>(env);
	get_stack<T>(env).back() *= first;
	return 1;
    }

    template <class T>
    inline unsigned divides(Env& env) {
	//if (not has_elements<T>(env,2)) return 1;
	if (get_stack<T>(env).back() == T()) { env.error(); return 1; }
	T first = pop<T>(env);
	get_stack<T>(env).back() /= first;
	return 1;
    }
    
    template <class From, class To>
    inline unsigned push_cast(Env& env)  {
	//if (not has_elements<From>(env,1)) return 1;
	push<To>(env, static_cast<To>( pop<From>(env) ));
	return 1;
    }
    
    inline unsigned int2bool(Env& env)  { return push_cast<int,bool>(env); }
    inline unsigned int2float(Env& env) { return push_cast<int,double>(env); }
    
    inline unsigned bool2int(Env& env)   { return push_cast<bool,int>(env);    }
    inline unsigned bool2float(Env& env) { return push_cast<bool,double>(env); }
    
    inline unsigned float2int(Env& env)  { return push_cast<double, int>(env); }
    inline unsigned float2bool(Env& env) { return push_cast<double,bool>(env); }


    inline unsigned _fmod(Env& env) {
	//if (not has_elements<double>(env,2)) return 1;
	if (get_stack<double>(env).back() == 0.0) { env.error(); return 1; }

	double first = pop<double>(env);
	double second = pop<double>(env);
	push<double>(env, fmod(second,first));
	return 1;
    }
   
    inline unsigned int_mod(Env& env) {
	//if (not has_elements<int>(env,2)) return 1;
	if (get_stack<int>(env).back() == 0) {env.error(); return 1;}
	
	int first = pop<int>(env);
	get_stack<int>(env).back() %= first;
	return 1;
    }
   
    template <class T>
	inline unsigned smaller(Env& env) {
	    //if (not has_elements<T>(env,2)) return 1;
	    T first = pop<T>(env);
	    T second = pop<T>(env);

	    push<bool>(env, second < first);
	    return 1;
	}
    
    template <class T>
	inline unsigned greater(Env& env) {
	    //if (not has_elements<T>(env,2)) return 1;
	    T first = pop<T>(env);
	    T second = pop<T>(env);

	    push<bool>(env, second > first);
	    return 1;
	}
    
    template <class T>
	inline unsigned _max(Env& env) {
	    //if (not has_elements<T>(env,2)) return 1;
	    T first = pop<T>(env);
	    T second = pop<T>(env);

	    push<T>(env, std::max(first,second));
	    return 1;
	}
    
    template <class T>
	inline unsigned _min(Env& env) {
	    //if (not has_elements<T>(env,2)) return 1;
	    T first = pop<T>(env);
	    T second = pop<T>(env);

	    push<T>(env, std::min(first,second));
	    return 1;
	}

    inline unsigned _cos(Env& env) {
	//if (not has_elements<double>(env,1)) return 1;
	std::vector<double>& stack = get_stack<double>(env);
	stack.back() = cos(stack.back());
	return 1;
    }
    inline unsigned _sin(Env& env) {
	//if (not has_elements<double>(env,1)) return 1;
	std::vector<double>& stack = get_stack<double>(env);
	stack.back() = sin(stack.back());
	return 1;
    }
    inline unsigned _tan(Env& env) {
	//if (not has_elements<double>(env,1)) return 1;
	std::vector<double>& stack = get_stack<double>(env);
	stack.back() = tan(stack.back()); // hmm, can go wrong, math error
	return 1;
    }
    
    
}

#endif
