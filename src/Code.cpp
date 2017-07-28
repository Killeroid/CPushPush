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

#include "Code.h"
#include "Literal.h"
#include "Env.h"
#include "CodeUtils.h"

#include <typeinfo>

using namespace std;

namespace push
{
    
CodeList::CodeList(Code a)  {
    _stack.resize(1);
    _stack[0] = a;
    _size = a->size() + 1;
    _binary_size = a->binary_size(); // or +1? don't know
}

unsigned CodeList::operator()(Env& env) const
{
    for (CodeArray::const_iterator it = _stack.begin(); it!=_stack.end(); ++it) 
	env.exec_stack.push_back(Exec(*it));
    return 1;
}

void CodeList::calc_sizes() {
    _size = 1;
    _binary_size = _stack.size() - 1; // internal nodes
    for (CodeArray::const_iterator it = _stack.begin(); it != _stack.end(); ++it) {
	_size += (*it)->size();
	_binary_size += (*it)->binary_size();
    }
}

std::string CodeList::to_string() const {
    std::string r = "(";
    for (int i = _stack.size()-1; i >= 0; --i) {
	r += _stack[i]->to_string() + (i==0? "": " ");
    }
    r += ")";
    return r;
}

istream& operator>>(istream& is, Code& code) {
    code = parse(is);
    return is;
}


bool CodeBase::operator==(const CodeBase& b) const
{
    const CodeBase& a = *this;
    if (&a == &b)		  return true;
    if (typeid(a) != typeid(b))   return false;
    //if (a.size() != b.size())	  return false;
    if (a.len() != b.len())	  return false;
   
    const CodeArray& astack = a.get_stack();
    const CodeArray& bstack = b.get_stack();
    
    for (unsigned i = 0; i < astack.size(); ++i) { 
	if (!(*astack[i] == *bstack[i]) ) return false; 
    }
    
    return a.equal_to(b);
}

bool smaller(Code a, Code b) {
    if (equal_to(a,b)) return false;

    //if (a->size() < b->size()) return true;
    //if (a->size() > b->size()) return false;
    if (a->len() < b->len()) return true;
    if (a->len() > b->len()) return false;

    // same size now check contents...
    const CodeArray& as = a->get_stack();
    const CodeArray& bs = b->get_stack();

    if (as.size() == 0) {
	return a.get() < b.get(); // pointer compare, atoms are unique (hmm, except constantcode...)
    }

    for (unsigned i = 0; i < as.size(); ++i) {
	Code suba = as[i];
	Code subb = bs[i];

	if (!equal_to(suba, subb)) {
	    return smaller(suba, subb);
	}
    }

    return false;
}

} // namespace push
