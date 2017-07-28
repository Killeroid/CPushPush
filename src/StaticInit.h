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

#ifndef _STATIC_INIT_H__
#define _STATIC_INIT_H__

#include <iostream>

#include "Code.h"

namespace push {

extern int init_push();
    
namespace detail {

extern Code* nil_ptr;
extern Code* instructions_ptr;
extern String2CodeMap* str2code_map_ptr;
extern CodeSet* erc_set_ptr;
extern CodeSet* exec_set_ptr;
extern std::vector<ParseHook>* parse_extension_hooks_ptr;

extern Env* default_root_env_ptr;

extern int init_count;

extern void init_types();

class StaticInit {
    public:

    StaticInit() {
	if (init_count++ == 0) {
	    nil_ptr		= new Code(new CodeList( CodeArray() ) );
	    instructions_ptr	= new Code(*nil_ptr);
	    str2code_map_ptr    = new String2CodeMap();
	    erc_set_ptr		= new CodeSet();
	    exec_set_ptr	= new CodeSet;
	    parse_extension_hooks_ptr = new std::vector<ParseHook>;
	    default_root_env_ptr = 0;	    
	    init_types();
	}
    }
   
    ~StaticInit() {
	if (--init_count == 0) {
	    delete nil_ptr;
	    delete instructions_ptr;
	    delete str2code_map_ptr;
	    delete erc_set_ptr;
	    delete exec_set_ptr;
	    delete parse_extension_hooks_ptr;
	}
    }
    
    /* Registers a function and makes it globally available through the 'instructions' Code */
    Code register_pushfunc(Code code); 

    /* Adds a Hook function that can be used to parse extra-pushlike syntax */
    int register_parse_hook(ParseHook);

    /* Set a root environment as a subclass of Env */
    int set_root_env(Env* env); 
    
};



}} // namespace detail / push

// the object that does the initialization
static push::detail::StaticInit static_initializer;
#endif

