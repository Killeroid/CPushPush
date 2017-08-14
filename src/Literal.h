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

#ifndef _CONSTANT_CODE_H__
#define _CONSTANT_CODE_H__


#include "TypedAtom.h"
#include "Word.h"
#include "Env.h"
#include <sstream>

namespace push {

template <class T>
    inline bool does_equal(const T& a, const T& b) { return a == b; }
    
template <class T>
class Literal : public TypedAtom 
{
    T value;
    public:
    
    const Type& get_precondition() const { return nullType; }
    const Type& get_postcondition() const { return get_type<T>(); }
    
    Literal(T val) : value(val) {}
    unsigned operator()(Env& env) const { push<T>(env, value); return 1; }

    std::string to_string() const {
        std::ostringstream os;
        os.precision(17);
        os.setf(std::ios_base::showpoint);
        os << value;
        return os.str();
    }
    
    T get() const { return value; }
     
    bool equal_to(const CodeBase& other) const { 
        return does_equal(value, static_cast<const Literal<T>& >(other).value);
    }
};

// specialization for bool
template <>
inline std::string Literal<bool>::to_string() const { return value?"TRUE":"FALSE"; }

// specialization for name_t
template <>
inline unsigned Literal<name_t>::operator()(Env& env) const { 
    if (env.quote_name_flag) {
        env.quote_name_flag = false;
        push<name_t>(env, value);
    }
    else {
        env.push_guarded( get_code(value) );
    }
    
    return 1;
}

template <>
inline std::string Literal<name_t>::to_string() const { return print(value); }

/* Packs a single type in a piece of code */
template <class T> 
inline Code pack(Env& env) {
    return Code( new Literal<T>( pop<T>(env) ) );
}

/* Specialization for Code, just return it */
template <>
inline Code pack<Code>(Env& env) {
    return pop<Code>(env);
}

/* Specialization for Exec, return the code */
template <>
inline Code pack<Exec>(Env& env) {
    return pop<Exec>(env).lock();
}

/* new untyped version of Set, called DEFINE: NAME.DEFINE, INTEGER.DEFINE, etc.*/
template <class T>
inline unsigned name_define(Env& env) {
    static Type type = make_type<T,name_t>(); // also takes care of <name_t,name_t>
    if (!type.can_pop_from(env)) return 1;
    
    name_t name = pop<name_t>(env);
    Code code = pack<T>(env);
    
    set_code(name, code);
    return 1;
}

extern unsigned edefine(Env& env);
template <>
inline unsigned name_define<Exec>(Env& env) { return edefine(env); }

/* Typed version */
Code pack(Env& env, const Type& type);


} // namespace push

#endif

