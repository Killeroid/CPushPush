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

#include "Env.h"
#include "Code.h"
#include "CodeUtils.h"
#include "Literal.h"
#include "Instruction.h"
#include "StaticInit.h"

#include <sstream>
#include <unistd.h>
#include <ios>
#include <limits>

using namespace std;

namespace push {

void internal_error( std::string s ) {
    abort();
}

/* The engine */
int Env::go(int n) 
{
    int effort = 0;

    // If the next environment still has work to do, continue that
    if (has_next() && not next().done()) {
	effort += next().go(n);
    }
    
    // The basic pop-exec cycle
    while ( (not exec_stack.empty()) && (effort < n) ) {
	Exec top = exec_stack.back();  
	exec_stack.pop_back();
	
	//std::cerr << "Exec " << top.lock() << std::endl;
	
	/* // Debugging code
	if (top->len() == 1) {
	    const Instruction* ins = dynamic_cast<const Instruction*>( top.get() );    
	    if (ins) { 
		
		std::cerr << make_type() << ' ';
		std::cerr << top.lock() << ' ' << ins->get_precondition() << std::endl;
		
	    }
	}
	else {
	    std::cerr << make_type() << " effort " << effort << " Len " << top->len() << std::endl;
	}*/
	
	effort += std::max(1u, (*top)(*this)); // execute instruction, every step takes at least one 
    }
    
    if (exec_stack.empty()) { guard.clear(); } // collect garbage
    
    return effort;
}

/* The engine */
int Env::go_yield(int n) 
{
    int effort = 0;

    // If the next environment still has work to do, continue that
    if (has_next() && not next().done()) {
	effort += next().go(n);
    }
    
    // The basic pop-exec cycle
    while ( (not exec_stack.empty()) && (++effort <= n) ) {
	Exec top = exec_stack.back();  
	exec_stack.pop_back();
	
	
	// yield on error
	if (top->len() == 1) {
	    const Instruction* ins = dynamic_cast<const Instruction*>( top.get() );    

	    if (ins && ins->can_run(*this)) { // check if it's a noop for reasons of stack contents
		break;
	    }
	}

	//effort += 
	(*top)(*this); // execute instruction
    }
    
    if (exec_stack.empty()) { guard.clear(); } // collect garbage
    
    return effort;
}

int Env::go_trace(int n, std::vector<Type>& trace, std::vector<Code>* ins_ptr, bool yield) 
{
    int effort = 0;

    // If the next environment still has work to do, continue that
    if (has_next() && not next().done()) {
	effort += next().go_trace(n, trace);
    }
    
    // The basic pop-exec cycle
    while ( (not exec_stack.empty()) && (++effort <= n) ) {
	Exec top = exec_stack.back();  
	exec_stack.pop_back();
	
	bool instruction_ran = false;
	if (top->len() == 1) {
	    const Instruction* ins = dynamic_cast<const Instruction*>( top.get() );    

	    if (ins && ins->can_run(*this)) { // check if it's a noop for reasons of stack contents
		instruction_ran = true;
	    }
	}
	
	(*top)(*this); // execute instruction
	
	if (instruction_ran) {
	    trace.push_back( make_type());
	    if (ins_ptr) {
		ins_ptr->push_back(top.lock());
	    }
	}
	
	// yield on error
	if (yield) {
		if (top->len() == 1) {
		const Instruction* ins = dynamic_cast<const Instruction*>( top.get() );    

		if (ins && ins->can_run(*this)) { // check if it's a noop for reasons of stack contents
		    break;
		}
	    }
	}
    }
    
    if (exec_stack.empty()) { guard.clear(); } // collect garbage
    
    return effort;
}

Env& Env::next() {
    if (next_env == 0) next_env = clone(); 
    return *next_env;
}

void Env::print_exec_stack(std::ostream& os)
{
    for (std::vector<Exec>::reverse_iterator it = exec_stack.rbegin(); it != exec_stack.rend(); ++it) {
	os << it->lock() << ' ';
    }
}

Type Env::make_type() const {
    Type a;
    a += Type(EXEC_STACK + 1,EXEC_STACK, exec_stack.size());
    a += Type(INTEGER_STACK + 1,INTEGER_STACK, int_stack.size());
    a += Type(CODE_STACK + 1,CODE_STACK, code_stack.size());
    a += Type(BOOL_STACK + 1,BOOL_STACK, bool_stack.size());
    a += Type(FLOAT_STACK + 1,FLOAT_STACK, double_stack.size());
    a += Type(NAME_STACK + 1,NAME_STACK, name_stack.size());
    
    return a;
}

// return the 'root environment' of the proper type

static Env& RootEnv() { 
    if (detail::default_root_env_ptr == 0) 
	detail::default_root_env_ptr = new Env();
    return *(detail::default_root_env_ptr); 
}

//exec_t Env::raw_pop_exec() { check_water(); const exec_t back = exec_stack.back(); exec_stack.pop_back(); return back; }
//void   Env::raw_push_exec(exec_t code) { check_water(); exec_stack.push_back(code); } // generally unsafe!!!

const Parameters global_parameters = { 
    1.0, -1.0, // max-min float
    10,-10,    // max-min int
    1000,      // eval_limit
    0.0001,    // new_erc_name
    50, 100,   // max points (init/total)
    time(0)*getpid()    // seed
};

std::string print(const Env& env) {
    std::ostringstream os;
    os.setf(std::ios_base::showpoint);
    
    os << "(\n";
    
    os << "\t(";
    for (unsigned i = 0; i < env.int_stack.size(); ++i)
	os << env.int_stack[i] << ' ';
    os << ")\n";

    os << "\t(";
    for (unsigned i = 0; i < env.code_stack.size(); ++i)
	os << env.code_stack[i] << ' ';
    os << ")\n";
    
    os << "\t(";
    for (unsigned i = 0; i < env.bool_stack.size(); ++i)
	os << (env.bool_stack[i]? "TRUE ":"FALSE ");
    os << ")\n";
    
    os << "\t(";
    for (unsigned i = 0; i < env.double_stack.size(); ++i)
	os << env.double_stack[i] << ' ';
    os << ")\n";

    os << "\t(";
    for (unsigned i = 0; i < env.name_stack.size(); ++i)
	os << print(env.name_stack[i]) << ' ';
    os << ")\n";
    
    
    os << ")\n";

    return os.str();
}

std::string print_config(const Env& env) {
    std::ostringstream os;
    
    os << "#PUSH CONFIGURATION\n";
    os << "(\n";
    os << "\tCODE.QUOTE\n\t(\n";

    const CodeArray& ins = env.function_set->get_stack();
    
    for (unsigned i = 0; i < ins.size(); ++i)
	os << "\t\t" << ins[i] << "\n";
    os << "\t)\n\tENV.INSTRUCTIONS\n\n";   
    
    os.precision(17);
    os.setf(std::ios_base::showpoint);
    os << "\t" << env.parameters.max_random_float << "\tENV.MAX-RANDOM-FLOAT\n";
    os << "\t" << env.parameters.min_random_float << "\tENV.MIN-RANDOM-FLOAT\n";
    os << "\t" << env.parameters.max_random_integer << "\tENV.MAX-RANDOM-INTEGER\n";
    os << "\t" << env.parameters.min_random_integer << "\tENV.MIN-RANDOM-INTEGER\n";
    os << "\t" << env.parameters.evalpush_limit    << "\tENV.EVALPUSH-LIMIT\n";
    os << "\t" << env.parameters.max_points_in_random_expression << "\tENV.MAX-POINTS-IN-RANDOM-EXPRESSION\n";
    os << "\t" << env.parameters.max_points_in_program << "\tENV.MAX-POINTS-IN-PROGRAM\n";
    os << "\t" << env.parameters.random_seed << "\tENV.RANDOM-SEED\n";
    
    os << ") \n#END OF PUSH3 CONFIGURATION";
    return os.str();
}

void Env::configure(Code code) {
    next(); // make sure a next environment is already there with proper parameters
    int oldlimit = parameters.max_points_in_program;
    parameters.max_points_in_program = std::numeric_limits<int>::max();

    push_guarded(code);
    while (go(100000)) {}
    
    parameters.max_points_in_program = oldlimit;
}

Code Env::pop_stack_from_id(int id) {
    
    Code result;
    
    static Code quote = parse("CODE.QUOTE");
    
    switch (id) {
	case EXEC_STACK: 
	    result = exec_stack.back().lock(); 
	    exec_stack.pop_back(); 
	    break;
	case INTEGER_STACK: 
	    result = Code(new Literal<int>(int_stack.back()));
	    int_stack.pop_back();
	    break;
	case CODE_STACK:
	    result = list(quote, code_stack.back());
	    code_stack.pop_back();
	    break;
	case BOOL_STACK:
	    result = Code(new Literal<bool>(bool_stack.back()));
	    bool_stack.pop_back();
	    break;
	case FLOAT_STACK:
	    result = Code(new Literal<double>(double_stack.back()));
	    double_stack.pop_back();
	    break;
	case NAME_STACK:
	    result = Code(new Literal<name_t>(name_stack.back()));
	    name_stack.pop_back();
	    break;
	//default:
	    // error
		    
    }
    
    return result;
}

}

