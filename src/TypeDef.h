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

#ifndef _TYPE_DEF__H__
#define _TYPE_DEF__H__

#include "Word.h"
#include "Code.h"
#include "Type.h"

#include "Type.h"

namespace push {


template <typename T>  inline size_t get_stack_num() { /* typename_t_has_no_stack(); */ }

enum {
    EXEC_STACK = 0, 
    INTEGER_STACK = 1,
    CODE_STACK = 2,
    BOOL_STACK = 3,
    FLOAT_STACK = 4,
    NAME_STACK = 5,
};

const int LAST_STANDARD_STACK = NAME_STACK;

extern Type nullType;
extern Type execType;
extern Type integerType;
extern Type codeType;
extern Type boolType;
extern Type floatType;
extern Type nameType;
extern Type typeType;

extern Type unknownType;

    
template <> inline size_t get_stack_num<Exec>()		{ return EXEC_STACK; }
template <> inline size_t get_stack_num<int>()		{ return INTEGER_STACK; }
template <> inline size_t get_stack_num<Code>()		{ return CODE_STACK; }
template <> inline size_t get_stack_num<bool>()		{ return BOOL_STACK; }
template <> inline size_t get_stack_num<double>()	{ return FLOAT_STACK; }
template <> inline size_t get_stack_num<name_t>()	{ return NAME_STACK; }


template <typename T> inline
Type make_type() {
    int n = get_stack_num<T>();
    Type type(n+1, n);
    return type;
}

template <typename T, typename U> inline
Type make_type() {
    return make_type<T>() + make_type<U>();
}

template <typename T1, typename T2, typename T3> inline
Type make_type() {
    return make_type<T1>() + make_type<T2,T3>();
}

template <typename T1, typename T2, typename T3, typename T4>  inline
Type make_type() {
    return make_type<T1,T2>() + make_type<T3,T4>();
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>  inline
Type make_type() {
    return make_type<T1,T2>() + make_type<T3,T4,T5>();
}

template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>  inline
Type make_type() {
    return make_type<T1,T2,T3>() + make_type<T4,T5,T6>();
}

template <typename T> inline
const Type& get_type() {
    static Type static_type = make_type<T>();
    return static_type;
}

} // namespace push
#endif
