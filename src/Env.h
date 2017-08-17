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

#ifndef _ENV_H__
#define _ENV_H__

#include <cassert>
#include <vector>
#include <valarray>
#include <iostream>
#include <list>

#include "Code.h"
#include "Word.h"
#include "Type.h"
#include "TypeDef.h"

#include "CodeUtils.h" // for cdr

namespace push {

class PushLib;
    
struct Parameters {	
    double max_random_float;// = 1.0;
    double min_random_float;// = -1.0;
    int max_random_integer;// = 10
    int min_random_integer;// = -10
    int evalpush_limit;// = 1000
    double new_erc_name_probability;// = 0.001
    int max_points_in_random_expression;// = 50
    int max_points_in_program;// = 100
    long random_seed;// = time.time()
};

extern const Parameters global_parameters;

class Env {
    public:
        /* Gets the default environment, can be a subclass of Env (see StaticInit:set_root_env()) */
        static Env& getRootEnv();

        /* instructions */
        Code function_set;
        Parameters parameters;
	
    private:
        std::list<Code> guard; // guard, keep Code alive while executing

    public:
        void push_guarded(const Code& code) {
            guard.push_back(code); 
            exec_stack.push_back(Exec(code)); 
        }
            
        void print_exec_stack(std::ostream& os);
        unsigned exec_stack_size() const { return exec_stack.size(); }
        unsigned guard_size() const { return guard.size(); }
        
        /* Stacks */
        std::vector<Exec>	  exec_stack;
        std::vector<int>          int_stack;
        std::vector<Code>         code_stack;
        std::vector<bool>         bool_stack;
        std::vector<double>       double_stack;
        std::vector<name_t>	  name_stack;
        
        /* Support for names */
        bool quote_name_flag;
        
        /* embedded environment */
        Env* next_env;
        Env& next();
        bool has_next() const { return next_env != 0; }

        
        name_t insert_string(std::string name) {
            return lookup(name);
        }

        Env(unsigned _reserve=1000) : function_set(instructions), parameters(global_parameters)
        {
            reserve(_reserve);
            next_env=0;
            quote_name_flag = false;
            clear();
        }
        
        virtual ~Env(){ delete next_env; }
        
        Env(const Env& oth) : next_env(0) {
            operator=(oth);
        }
        
        Env& operator=(const Env& oth) {
            
            function_set = oth.function_set;
            parameters = oth.parameters;
            exec_stack = oth.exec_stack;
            guard = oth.guard;
            int_stack = oth.int_stack;
            code_stack = oth.code_stack;
            bool_stack = oth.bool_stack;
            double_stack = oth.double_stack;
            name_stack = oth.name_stack;
        
            quote_name_flag = oth.quote_name_flag;

            delete next_env;
            next_env = 0;
            if (oth.next_env) {
                next_env = oth.next_env->clone();
            }
            
            return *this;
        }
        
        virtual Env* clone() const {
            Env* newenv = new Env(*this);
            return newenv;
        }
        
        void clear() {
            clear_stacks();
            quote_name_flag = false;
        }
        
        void configure(Code code);
        
        virtual void reserve(unsigned _reserve)
        {
            exec_stack.reserve(_reserve);

            int_stack.reserve(_reserve);
            code_stack.reserve(_reserve);
            bool_stack.reserve(_reserve);
            double_stack.reserve(_reserve);
            name_stack.reserve(_reserve);
        }
        
        virtual void clear_stacks() {
            
            exec_stack.clear();
            guard.clear();
            
            int_stack.clear();
            code_stack.clear();
            bool_stack.clear();
            double_stack.clear();
            name_stack.clear();
        }

        /* Needed for type checking of preconditions */
        virtual unsigned get_stack_size(int which) const {
            switch (which) {
                case EXEC_STACK: return exec_stack.size();
                case INTEGER_STACK: return int_stack.size();
                case CODE_STACK: return code_stack.size();
                case BOOL_STACK: return bool_stack.size();
                case FLOAT_STACK: return double_stack.size();
                case NAME_STACK: return name_stack.size();
            }
            return 0;
        }
        
        virtual Type make_type() const;
        
        /* Needed for type based packing */
        virtual Code pop_stack_from_id(int id);

        /* exception handling */
        /* might want to add an 'exception' stack at some point */
        void error() {}
        
        bool done()        { return exec_stack.empty();  }
        int go(int n = 50); 
        int go_yield(int n = 50); 
        
        int go_trace(int n, std::vector<Type>& trace, std::vector<Code>* ins_ptr = 0, bool yield = false);
	
};
    

	
extern std::string print(const Env& env);
extern std::string print_config(const Env& env);
inline std::ostream& operator<<(std::ostream& os, const Env& env) { os << print(env); return os; }

void internal_error( std::string s );

/* Operations */
template <typename T> inline std::vector<T>& get_stack(Env& env)	{ return get_stack<T>(env); }
template <> inline std::vector<Exec>&   get_stack(Env& env)	{ return env.exec_stack; }
template <> inline std::vector<int>& get_stack(Env& env)	{ return env.int_stack;  }
template <> inline std::vector<double>& get_stack(Env& env)	{ return env.double_stack; }
template <> inline std::vector<bool>&   get_stack(Env& env)	{ return env.bool_stack; }
template <> inline std::vector<name_t>&   get_stack(Env& env)	{ return env.name_stack; }
template <> inline std::vector<Code>&   get_stack(Env& env)	{ return env.code_stack; }

/* pushing and popping */
template <typename T> inline void push(Env& env, T value) { get_stack<T>(env).push_back(value); }
template <typename T> inline T pop(Env& env) { T val = get_stack<T>(env).back(); get_stack<T>(env).pop_back(); return val; }

template <typename T> inline T& top(Env& env) { return get_stack<T>(env).back(); }
template <class T> inline    T& first(Env& env) { return top<T>(env); }
template <class T> inline    T& second(Env& env) { return get_stack<T>(env)[get_stack<T>(env).size()-2]; }    

/* Push calling convention */
inline void push_call(Env& env, Code code) { 
    env.push_guarded(code); 
    push(env, code);
}
 
template <typename T>
inline bool has_elements(Env& env, unsigned sz) { 
    if (get_stack<T>(env).size() < sz) {
        env.error();
        return false;
    }
    return true;
}

/* generic functions */
template <typename T>
inline unsigned dup(Env& env) {
    //if (not has_elements<T>(env, 1)) return 1;
    push<T>(env, get_stack<T>(env).back());
    return 1;
}

template <typename T>
inline unsigned swap(Env& env) {
    //if (not has_elements<T>(env,2)) return 1;

    std::vector<T>& stack = get_stack<T>(env);
    T tmp = stack.back();
    stack.back() = stack[stack.size()-2];
    stack[stack.size()-2] = tmp;
    return 1;
}

template <typename T>
inline unsigned equals(Env& env) {
    //if (stack.size() < 2) env.error();
    //else 
	push(env, pop<T>(env) == pop<T>(env));
    return 1;
}

template <>
inline unsigned equals<Code>(Env& env) {
    int effort = 0;
    //if (stack.size() < 2) env.error();
    //else 
	push(env, equal_to(pop<Code>(env), pop<Code>(env), effort));
    return effort;
}

template <>
inline unsigned equals<Exec>(Env& env) {
    int effort = 0;
    //if (stack.size() < 2) env.error();
    //else 
	push(env, equal_to(pop<Exec>(env).lock(), pop<Exec>(env).lock(), effort));
    return effort;
}

//template <class T> inline bool safe_stack(Env& env) { return get_stack<T>(env).size() > 0; }
//template <> inline bool safe_stack<int>(Env& env) { return get_stack<int>(env).size() > 1; } // as we need to pop from this

template <class T>
inline int safe_index(Env& env) {
    
    //if (not safe_stack<T>(env)) return -1;    
    //if (get_stack<int>(env).size() == 0) return -1;

    int index = pop<int>(env);
    int stacksize = get_stack<T>(env).size();
    
    index = (index <0)? 0: (index >= stacksize? stacksize-1:index);
    
    return stacksize-1-index; 
}

template <typename T>
inline unsigned yankdup(Env& env) {
    
    int index = safe_index<T>(env);
    //if (index == -1) return 1; // -1 signals cannot obtain index for whatever reason

    std::vector<T>& stack = get_stack<T>(env);

    stack.push_back(stack[index]);
    return 1;
}

template <class T>
inline unsigned protected_pop(Env& env) {
    pop<T>(env);
    return 1;
}

template <class T>
inline unsigned stackdepth(Env& env) {
    push<int>(env, get_stack<T>(env).size());
    return 1;
}

template <class T>
inline unsigned shove(Env& env) {
    
    int index = safe_index<T>(env);
    //if (index == -1) return 1;

    std::vector<T>& stack = get_stack<T>(env);

    stack.insert( stack.begin() + index, pop<T>(env) );
    
    return stack.size() - index + 1;    
}

template <class T>
inline unsigned yank(Env& env) {
    int index = safe_index<T>(env);
    //if (index == -1) return 1;
   
    std::vector<T>& stack = get_stack<T>(env);
    
    assert(index < (int)stack.size());
    assert(index >= 0);
    
    T value = stack[index];
    stack.erase(stack.begin() + index);
    stack.push_back(value);
    return stack.size() - index + 1;
}


template <class T>
unsigned rot(Env& env) {
    //if (!has_elements<T>(env,3)) return 1;
    T x = pop<T>(env);
    T y = pop<T>(env);
    T z = pop<T>(env);

    push<T>(env,y);
    push<T>(env,x);
    push<T>(env,z);
    return 1;
}

} // namespace push

#endif
