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

#ifndef _CODE__H__
#define _CODE__H__

#include <string>
#include <vector>
#include <map>
#include <set>

//#include <boost/shared_ptr.hpp>
#include "shared_ptr.h"

namespace push {

/*
 * program ::= instruction | literal | ( program* )
 *
 */

class Env;
class CodeBase;

//typedef boost::shared_ptr<CodeBase> Code;
typedef push::detail::shared_ptr<CodeBase> Code;
typedef push::detail::weak_ptr<CodeBase>   Exec;
typedef std::vector<Code> CodeArray;
typedef unsigned (*Operator)(Env&);

typedef std::map<std::string, push::Code> String2CodeMap;
typedef std::set<push::Code> CodeSet;

/* Globals (Check StaticInit) */
extern const String2CodeMap& str2code_map;
extern const Code& instructions;
extern const CodeSet& erc_set;
extern const CodeSet& exec_set; // DO, DO*, IF
extern const Code& nil;

typedef Code (*ParseHook)(std::string);
extern const std::vector<ParseHook>& parse_extension_hooks;

class CodeBase
{
    private:
    friend class CodeList; // the only one allowed to touch the stack
    CodeArray _stack;
    public:
    
    CodeBase() {}
    CodeBase(const CodeArray& stack) : _stack(stack) {}
    const CodeArray& get_stack() const { return _stack; }
    
    virtual ~CodeBase() {}
  
    bool operator==(const CodeBase& other) const;
    
    /* virtual functions */
    virtual unsigned operator()(Env& env) const = 0;
    
    virtual unsigned binary_size() const = 0; // Size of the thing if we look at it as a binary (cons-pair) tree
    virtual unsigned size() const  = 0;	
    virtual unsigned len()  const = 0;
    
    virtual std::string to_string() const = 0;
    
    protected:
    virtual bool equal_to(const CodeBase& other) const { return true; };
};

class CodeAtom : public CodeBase 
{
    public:
	unsigned binary_size() const { return 1; }
	unsigned size() const   { return 1; }
	unsigned len() const    { return 1; }
};

class CodeList : public CodeBase
{
    unsigned _size;
    unsigned _binary_size; 

    void calc_sizes();
    
    public:
    
    CodeList() : _size(-1u), _binary_size(-1u) { calc_sizes(); }
    CodeList(const CodeArray& stack) : CodeBase(stack), _size(-1u), _binary_size(-1u) { calc_sizes(); }
    CodeList(Code a);
    
    // implementation of virtuals
    unsigned binary_size() const { return _binary_size; }
    unsigned size() const { return _size; }
    unsigned len()  const { return _stack.size();  }
    
    std::string to_string() const;
    unsigned operator()(Env& env) const;
    
    // no equal_to overload, equality checked in parent
    
    // special 'constructor' that will destroy the argument (for efficiency reasons)
    static Code adopt(CodeArray& vec) { // beware, destructs vec! 
	CodeList* lst = new CodeList;
	lst->_stack.swap(vec); // destructs vec
	lst->calc_sizes();
	return Code(lst);
    }
};

inline std::string str(Code code) { if (code.get()==0) return "no code"; return code->to_string(); }
inline std::ostream& operator<<(std::ostream& os, Code code) { os << str(code); return os; }

/* Assumes an expression is found on the line itself */
std::istream& operator>>(std::istream& is, Code& code);

// defined in Code.cpp
inline bool equal_to(Code a, Code b, int& effort) { bool res =*a==*b; effort = std::max(a->size(), b->size()); return res; }
inline bool equal_to(Code a, Code b) { return *a == *b; }
bool smaller(Code a, Code b);

/* Code.cpp */
Code protect(Code code);


} // namespace push

#endif
