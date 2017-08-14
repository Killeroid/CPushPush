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

#ifndef _CODE_UTILS_H__
#define _CODE_UTILS_H__

#include "Code.h"
#include "TypeDef.h"

namespace push {

class Env;
class Type;
    
inline Code cons(Code a, Code b) 
{
    CodeArray stack = b->get_stack();
    
    if (stack.size()) {
        stack.push_back(a);
    } else {
        stack.resize(2);
        stack[0] = b;
        stack[1] = a;
    }

    return CodeList::adopt(stack); //Code( new CodeList(stack) );
}

inline Code list(Code a, Code b)
{
    CodeArray stack(2);
    stack[0] = b;
    stack[1] = a;
    return CodeList::adopt(stack); //Code( new CodeList(stack) );
}

inline Code car(Code a) {
    const CodeArray& stack = a->get_stack();
    if (stack.size() == 0) return a;
    return stack.back();
}

inline Code cdr(Code a) {
    CodeArray stack = a->get_stack();
    if (stack.size() == 0) return nil;
    stack.pop_back();
    return CodeList::adopt(stack); //Code( new CodeList(stack));
}

class Env;
extern Code random_code(Env& env, unsigned maxpoints);
extern Code find_container(Code tree, Code subtree);
extern bool contains(Code code, Code subtree);

extern Code extract(Code code, unsigned val);
extern Code insert(Code code, unsigned val, Code subcode); 
extern Code remove(Code code, unsigned val); 
extern Code extract_binary(Code code, unsigned val);
extern Code insert_binary(Code code, unsigned val, Code subcode); 

extern bool member(Code code, Code query);

extern Code flatten(Code code);
extern Code pack(Env& env, const Type& type);

extern Code parse(std::istream& is);
extern Code parse(std::string);

extern void all_points(Code code, CodeArray &array);
extern int discrepancy(Code code1, Code code2);

extern Code swap_sublist_position(Code code, int p1, int p2);
extern Code make_terminal(Env& env);

extern Code compile(Code code, const Type& initial_type = nullType, const Type& desired_type = nullType);
extern Code clean(Code code, const Type& initial_type = nullType, const Type& desired_type = nullType);

class DiversityPool {
	public:
		DiversityPool(int t);
		int getSize();
		void addIndividual(Code c);

		void clear() { _species.clear(); }
		void setTolerance(int t) { _tolerance = t; }

		std::vector< std::vector< Code > > _species;
		int _tolerance;
};

} // namespace push
#endif

