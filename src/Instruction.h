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

#ifndef _INSTRUCTIONS_H__
#define _INSTRUCTIONS_H__


#include "StaticInit.h"
#include "TypedAtom.h"

namespace push {
    
class Instruction : public TypedAtom {
    const Operator op_; // const, so the compiler might be able to inline the call
    std::string name_;
    
    Type intype_;
    Type outtype_;
    
    bool static_typed_;
    
    public:
    Instruction(Operator op, std::string name) : op_(op), name_(name) {}
    Instruction(Operator op, std::string name, Type intype, Type outtype, bool static_ = true) : 
	op_(op), name_(name), intype_(intype), outtype_(outtype), static_typed_(static_) {}
    
    Operator get_op() const { return op_; }

    const Type& get_precondition() const   { return intype_; }
    const Type& get_postcondition() const  { return outtype_; }
    bool is_static() const	    { return static_typed_; }
    
    std::string to_string() const { return name_; }
   
    virtual bool can_run(const Env& env) const { return intype_.can_pop_from(env); }
    virtual unsigned operator()(Env& env) const { 
	if (can_run(env)) op_(env); return 1;} // currently ignores 'effort' 
    
    protected: 
    bool equal_to(const CodeBase& other) const { return op_ == static_cast<const Instruction&>(other).op_; }
};

inline
Code make_instruction(Operator op, std::string type, std::string name) {
    return static_initializer.register_pushfunc( Code(new Instruction(op, type + "." + name, Type(), Type(),true )) ); 
}

inline
Code make_instruction(Operator op, std::string type_name) {
    return static_initializer.register_pushfunc( Code(new Instruction(op, type_name, Type(), Type(), true))); 
}

inline
Code make_instruction(Operator op, std::string type, std::string name, Type in, Type out, bool static_=true) {
//    std::cout << "Instruction " << type << "." << name << " has type " << in << " -> " << out << std::endl;
    return static_initializer.register_pushfunc( Code(new Instruction(op, type + "." + name, in, out, static_)) ); 
}

inline
Code make_instruction(Operator op, std::string type_name, Type intype, Type outtype, bool static_=true) {
//    std::cout << "Instruction " << type_name << " has type " << intype << " -> " << outtype << std::endl;
    return static_initializer.register_pushfunc( Code(new Instruction(op, type_name, intype, outtype, static_))); 
}

inline
Code make_instruction(Operator op, const char* type_name, Type intype, Type outtype, bool static_=true) {
    return make_instruction(op, std::string(type_name), intype, outtype, static_);
}

} // namespace push
#endif
