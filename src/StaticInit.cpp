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

#include <algorithm>
#include "StaticInit.h"
#include "Env.h"

namespace push {
    namespace detail {
	
	
	Code* nil_ptr;
	Code* instructions_ptr;
	String2CodeMap* str2code_map_ptr;
	CodeSet* erc_set_ptr;
	CodeSet* exec_set_ptr;
	int init_count;
	
	std::vector<ParseHook>* parse_extension_hooks_ptr;
	
	Env* default_root_env_ptr;
    }
    
Type nullType;
Type unknownType;

Type execType; //(EXEC_STACK+1,EXEC_STACK,1);
Type integerType; //(INTEGER_STACK+1,INTEGER_STACK,1);
Type codeType; //(CODE_STACK+1, CODE_STACK,1);
Type boolType; //(BOOL_STACK+1, BOOL_STACK,1);
Type floatType; //(FLOAT_STACK+1, FLOAT_STACK,1);
Type nameType;//(NAME_STACK+1, NAME_STACK,1);
    

    // globals
    const Code&		nil = *detail::nil_ptr;
    const Code&		instructions = *detail::instructions_ptr;
    const String2CodeMap&	str2code_map = *detail::str2code_map_ptr;
    const CodeSet&		erc_set = *detail::erc_set_ptr;
    const CodeSet&		exec_set = *detail::exec_set_ptr;
    const std::vector<ParseHook>& parse_extension_hooks = *detail::parse_extension_hooks_ptr;
   

    namespace detail {
    
    void init_types() {
	execType = Type(EXEC_STACK+1,EXEC_STACK,1);
	integerType = Type(INTEGER_STACK+1,INTEGER_STACK,1);
	codeType = Type(CODE_STACK+1, CODE_STACK,1);
	boolType = Type(BOOL_STACK+1, BOOL_STACK,1);
	floatType = Type(FLOAT_STACK+1, FLOAT_STACK,1); 
	nameType = Type(NAME_STACK+1, NAME_STACK,1);
    }

    typedef std::vector<std::string> Names;
    Names function_names;
	
    Code StaticInit::register_pushfunc(Code code) {
	CodeArray instruction_table = (*instructions_ptr)->get_stack();
	
	std::string name = str(code);
	
	int location = std::upper_bound(function_names.begin(), function_names.end(), name) - function_names.begin();
	instruction_table.insert(instruction_table.begin() + location, code);
	function_names.insert(function_names.begin() + location, name);
	
	//instruction_table.push_back(code);
	
	(*instructions_ptr) = Code(new CodeList(instruction_table));
	
	//std::cout << "Registered: " << name << std::endl;
	
	(*str2code_map_ptr)[name] = code;
	
	// if 'ends in ERC' it's part of the erc set
	unsigned i = name.size()-3;
	if (name.size() >=3 and i>=0 and name.substr(i,3) == "ERC" or name == "NAME.RANDBOUNDNAME")
	{   
	    (*erc_set_ptr).insert(code);
	}

	if (name == "CODE.DO" or name == "CODE.IF" or name == "CODE.DO*") {
	    (*exec_set_ptr).insert(code);
	}
	
	return code;
    } 
    
    int StaticInit::register_parse_hook(ParseHook hook) {
	parse_extension_hooks_ptr->push_back(hook);
	return 0;
    }
    
    int StaticInit::set_root_env(Env* env) {
	if (default_root_env_ptr != 0) {
	    // Two competing subclassed environments detected, issue error and abort
	    std::cerr << "Error: two subclassed environments want to be the root env" << std::endl;
	    exit(1);
	}
	default_root_env_ptr = env;
	return 1;
    }

    } // namespace detail

// static initialization

extern void initGenerics();
extern void initCode();
extern void initEnv();
extern void initExec();
extern void initInt(); 
extern void initFloat();
extern void initBool();
extern void initName();

int init_push() {
   
    // run garbage collector at exit
    atexit(collect_garbage);

    static bool initialized = false;
    if (initialized) return 1;
    initialized = true;
    
    detail::init_types();
    
    /* Initialize all instructions */
    initGenerics();
    initCode();
    initEnv(); 
    initExec();
    initInt();
    initFloat();
    initBool();
    initName();
    return 0;
}

static int bootstrap = init_push(); // will initialize library for dynamic libraries

}

