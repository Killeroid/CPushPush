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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <string>
//#include <dlfcn.h>

#include "CodeUtils.h"
#include "Env.h"
#include "Instruction.h"

using namespace std;
using namespace push;
    
unsigned steps = 1000000;

int main(int argc, char* argv[])
{
    init_push();

    bool step = false;
    if (argc > 1 && string("step") == argv[1]) {
        step = true;
        if (argc > 2) {
            
            steps = atoi(argv[2]);
        }
    }
 
    bool trace = false;
    if (argc > 1 && string("trace") == argv[1]) {
        trace = true;
    }
    
    // to load extension libraries
    //if (argc > 1 && string("ext") == string(argv[1]))
//	dlopen("./libpush-ext.so", RTLD_NOW);
    
    Code code;
    Env env;
    
    try {
        code = parse(std::cin);
    } catch (exception& e) {
        std::cout << print_config(env);
        exit(0);
    }
    env = env.next();

    vector<Type> env_trace;
    vector<Code> instructs;
    
    push_call(env, code);
    unsigned r = 0;
    
    cout << env;
    env.print_exec_stack(cout);
    cout << "\n\n";
    
    while( not env.done() && r < steps) {
        if (step) {
            r+= env.go(1);
            cout << env;
            env.print_exec_stack(cout);
            //cout << env.make_type() << endl;
            //cout << env.guard_size() << endl;
            cout << "\nEffort " << r; 
            cout << "\n\n";
        } else {
            if (trace) {
                r += env.go_trace(steps, env_trace, &instructs, true);
            } else {
                r += env.go(steps);
            }
        }
    }
    
    if (r >= steps) {
        cout << "Bailed out after " << r << " execution steps" << endl;
    }
    
    if (trace) {
        for (unsigned i = 0; i < env_trace.size(); ++i) {
            cout << i << ' ' << env_trace[i] << "\t\t" << instructs[i] << '\n';
        }
    }
    
    cout << env;
    
}


