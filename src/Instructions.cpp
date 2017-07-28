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
#include "StaticLibMacros.h"
#include "CodeUtils.h"
#include "Literal.h"
#include "RNG.h"

#define FULLGENERIC(name, func, static_)\
    cons(\
	    make_instruction(func<Exec>, "EXEC", #name, func##_in<Exec>(), func##_out<Exec>(), static_),\
    cons(\
	    make_instruction(func<int>, "INTEGER", #name, func##_in<int>(), func##_out<int>(), static_),\
    cons(\
	    make_instruction(func<double>, "FLOAT", #name, func##_in<double>(), func##_out<double>(), static_),\
    cons(\
	    make_instruction(func<bool>, "BOOLEAN", #name, func##_in<bool>(), func##_out<bool>(), static_),\
    cons(\
	    make_instruction(func<name_t>, "NAME", #name, func##_in<name_t>(), func##_out<name_t>(), static_),\
	    make_instruction(func<Code>, "CODE", #name, func##_in<Code>(), func##_out<Code>(), static_)\
	)))))
    
#define GENERIC(name,func) FULLGENERIC(name,func,true);
 
// Initialization of global Random Number Generator
RNG rng(push::global_parameters.random_seed);

// definition of the full push language
namespace push
{

    
template <typename T> Type yankdup_in() { return make_type<T,int>(); }
template <typename T> Type yankdup_out() { return make_type<T>(); }
    

template <typename T> Type dup_in()  { return make_type<T>(); }
template <typename T> Type dup_out() { return make_type<T,T>(); }


template <typename T> Type equals_in() { return make_type<T,T>(); }
template <typename T> Type equals_out() { return boolType; }


template <typename T> Type swap_in() { return make_type<T,T>(); }
template <typename T> Type swap_out() { return make_type<T,T>(); }


template <typename T> Type protected_pop_in() { return make_type<T>(); }
template <typename T> Type protected_pop_out() { return nullType; }


template <typename T> Type name_define_in() { return make_type<T,name_t>(); }
template <typename T> Type name_define_out() { return nullType; }


//template <typename T> Type name_get_in()  { return nameType; }
//template <typename T> Type name_get_out() { return unknownType; }

//Code ggt = FULLGENERIC(GET, name_get, false); //dynamic

template <typename T> Type shove_in()  { return make_type<T,int>(); }
template <typename T> Type shove_out() { return make_type<T>(); }


template <typename T> Type stackdepth_in()  { return nullType; }
template <typename T> Type stackdepth_out() { return integerType; } 


template <typename T> Type yank_in()  { return make_type<T, int>(); }
template <typename T> Type yank_out() { return make_type<T>(); }


template <typename T> Type rot_in()  { return make_type<T,T,T>(); }
template <typename T> Type rot_out() { return make_type<T,T,T>(); } 


template <typename T> Type flush_in()  { return nullType; }
template <typename T> Type flush_out() { return nullType; } 

template <class T>
unsigned flush(Env& env) { get_stack<T>(env).clear(); return 1; }

void initGenerics() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    
    GENERIC(YANKDUP,yankdup);
    GENERIC(DUP,dup);
    GENERIC(=,equals);
    GENERIC(SWAP,swap);
    GENERIC(POP,protected_pop);
    GENERIC(DEFINE, name_define);
    GENERIC(SHOVE, shove);
    GENERIC(STACKDEPTH, stackdepth);
    GENERIC(YANK, yank);
    GENERIC(ROT,rot);
    GENERIC(FLUSH,flush);
}

} // namespace push


