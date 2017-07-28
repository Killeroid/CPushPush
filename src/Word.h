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

#ifndef _WORD__H__
#define _WORD__H__

#include "Code.h"

namespace push {

extern void destroy(unsigned index);

class simple_name {
    unsigned index;
    
    simple_name& operator=(const simple_name&);
    simple_name(const simple_name&);
    
    public:
    simple_name(unsigned nm) : index(nm) {}
    ~simple_name() { destroy(index); }
    
    unsigned idx() const { return index; }
};

// define type of name
typedef push::detail::shared_ptr<simple_name> name_t;

// size of the global dictionary
extern unsigned dict_size(void);

// lookup a name and return a reference to it (used for parsing)
extern name_t lookup(std::string name);

// print the name 
extern std::string print(name_t name);
extern name_t rand_name();
extern name_t rand_bound_name();

// get the code the name is binded to 
extern Code get_code(name_t name);
// set this code
extern void set_code(name_t name, Code code);

// run a round of garbage collection
extern void collect_garbage();

} // namespace push
#endif
