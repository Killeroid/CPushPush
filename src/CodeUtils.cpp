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

#include <stdexcept>

#include "CodeUtils.h"
#include "RNG.h"
#include "Env.h" // symbol_table, instructions
#include "Literal.h"

#include <sstream>

using namespace std;

namespace push {
    
Code find_container(Code tree, Code subtree) 
{
    // check if *this* is a container of subtree
    const CodeArray& stack = tree->get_stack();
    for (unsigned i = 0; i < stack.size(); ++i) {
        if ( equal_to(subtree, stack[i])) {
            return tree;
        }
    }
    // else check if one of the children is a container (in reverse order)

    for (int i = stack.size()-1; i >= 0; --i) {
        Code container = find_container(stack[i], subtree);
        if (container->get_stack().size() > 0) { // container is always a list
            return container;
        }
    }

    return nil;
}

Code extract(Code code, unsigned val) {
    assert(val < code->size());

    if (val == 0) return code; // found
    val -= 1;
    const CodeArray& stack = code->get_stack();
    for (int i = stack.size()-1; i >= 0; --i) {
        if (val < stack[i]->size()) {
            return extract(stack[i], val);
        }
        val -= stack[i]->size();
    }
    // should not happen
    return code;
}

Code extract_binary(Code code, unsigned val) {
    assert(val < code->binary_size());

    if (val == 0) return code; // found
    val -= 1;
    CodeArray stack = code->get_stack();
    unsigned n = stack.size();
    for (unsigned i = 0; i < n; ++i) {

        if (val < stack.back()->binary_size())
            return extract_binary(stack.back(), val);
        
        val -= stack.back()->binary_size();
        
        stack.pop_back();
        if (val == 0) {
            if (stack.size() > 1) {
                return CodeList::adopt(stack); //Code(new CodeList(stack));
            }
            // else
            return stack.back();
        }
        --val;
    }
    // should not happen
    return code;
}

Code insert_binary(Code code, unsigned val, Code subcode) 
{
    assert(val < code->binary_size());

    if (val == 0) return subcode;
    val -= 1;

    CodeArray stack = code->get_stack();
    unsigned n = stack.size(); 
    assert(stack.size());
    
    for (int i = n-1; i >= 0; --i) {
        if (val < stack[i]->binary_size()) {
            Code newcode = insert_binary(stack[i], val, subcode);
            // if (newcode != nil)
            stack[i] = newcode;
            return CodeList::adopt(stack); //Code( new CodeList(stack) );
        }
        val -= stack.back()->binary_size();

        if (val == 0) {
            CodeArray newstack = subcode->get_stack();
            if (newstack.empty()) {
                newstack.push_back(subcode);
            }

            copy(stack.begin() + i, stack.end(), back_inserter(newstack));
            
            return CodeList::adopt(newstack); //Code( new CodeList(newstack));
        }
        --val;	
    }

    return code; // should not happen
}

Code insert(Code code, unsigned val, Code subcode) 
{
    assert(val < code->size());

    if (val == 0) return subcode;
    val -= 1;

    CodeArray stack = code->get_stack();
    
    assert(stack.size());
    
    for (int i = stack.size()-1; i >= 0; --i) {
        if (val < stack[i]->size()) {
            Code newcode = insert(stack[i], val, subcode);
            // if (newcode != nil)
            stack[i] = newcode;
            return CodeList::adopt(stack); //Code( new CodeList(stack) );
        }

        val -= stack[i]->size();
    }

    return code; // should not happen
}

Code remove(Code code, unsigned val) 
{
    assert(val < code->size());

    if (val == 0) return nil;
    val -= 1;

    CodeArray stack = code->get_stack();
    
    assert(stack.size());
    
    for (int i = stack.size()-1; i >= 0; --i) {
        if (val < stack[i]->size()) {

            if(val != 0) {
                stack[i] = remove(stack[i], val);
            } else {
                CodeArray::iterator start = stack.begin();
                while(i--) start++;

                stack.erase(start);
            }

            return CodeList::adopt(stack); //Code( new CodeList(stack) );
        }

        val -= stack[i]->size();
    }

    return code; // should not happen
}

bool contains(Code code, Code subtree)
{
    const vector<Code>& stack = code->get_stack();
    for (unsigned i = 0; i < stack.size(); ++i) {
        if (contains(stack[i], subtree))
            return true;
    }

    return equal_to(code, subtree);
}

bool member(Code code, Code query) {
    const CodeArray& stack = code->get_stack();
    if (stack.size() == 0) {
        return equal_to(code,query);
    }

    for (unsigned i = 0; i < stack.size(); ++i) {
        if (equal_to(stack[i], query))
            return true;
    }
    return false;
}

vector<unsigned> decompose(int number, int max_parts)
{
    vector<unsigned> result;
    if (number == 1 or max_parts == 1) {
        result.push_back(1);
        return result;
    }

    unsigned this_part = rng.random(number-1) + 1;
    result.push_back(this_part);
    std::vector<unsigned> sub = decompose(number-this_part,max_parts-1);
    for (unsigned i = 0; i < sub.size(); ++i) {
        result.push_back(sub[i]);
    }
    return result;
}

/* packs things from a stack in a piece of code  */
Code pack(Env& env, const Type& _tp) {
    Type type = abs(_tp);
    if (!type.can_pop_from(env)) return nil;
    
    std::vector<int> tp = type.get();
    CodeArray resultvec;
    for (unsigned i = 0; i < tp.size(); ++i) {
	
        CodeArray codevec;
        for (unsigned j = 0; j < unsigned(tp[i]); ++j) {
            Code code = env.pop_stack_from_id(i);
            codevec.push_back(code);	
        }
        
        if (codevec.size()) {
            resultvec.push_back(CodeList::adopt(codevec)); //Code(new CodeList(codevec)));
        }
    }
    return CodeList::adopt(resultvec); //Code(new CodeList(resultvec));
}

Code roulette_wheel(const CodeArray& instructions) {
    vector<double> counts(instructions.size());
    double total = 0;
    
    static double draws = 0.0;
    double temp = log(++draws);
    
    for (unsigned i = 0; i < counts.size(); ++i) {
        counts[i] = pow(instructions[i].refcount(), temp);
        total += counts[i];
        if (i > 0) counts[i] += counts[i-1];
    }

    unsigned which = upper_bound(counts.begin(), counts. end(), rng.uniform(total)) - counts.begin();

    return instructions[which];
}

Code make_terminal(Env& env) {
    static Code rnd = parse("CODE.RAND"); // special case
    
    const CodeArray& instruction_list = env.function_set->get_stack();
    
    Code ins = instruction_list[rng.random(instruction_list.size())];
    
    if ( ins != rnd && erc_set.find(ins) != erc_set.end() ) {
        (*ins)(env);
        
        // get everything that's produced in this env. Need to car twice
        // to get rid of the 'list of lists' that are produced by atoms
        Code retval = car(car(pack(env, env.make_type())));
        return retval;
    }

    return ins;
}

Code random_code_with_size(Env& env, unsigned points) {
    if (points == 1) {
        return make_terminal(env);
    }

    std::vector<unsigned> sizes_this_level = decompose(points-1, points-1);
    
    // shuffle
    for (int i = sizes_this_level.size()-1; i > 0; --i) {
        int j = rng.random(i+1);
        if (i != j) {
            std::swap(sizes_this_level[i], sizes_this_level[j]);
        }
    }
    
    CodeArray stack(sizes_this_level.size());
    for (unsigned i = 0; i < sizes_this_level.size(); ++i) {
        stack[i] = random_code_with_size(env, sizes_this_level[i]);
    }

    return CodeList::adopt(stack); //Code(new CodeList(stack));
}

Code random_code(Env& env, unsigned maxpoints)
{
    unsigned points = rng.random(maxpoints) + 1;
    env.clear_stacks();

    Code result = random_code_with_size(env, points);
    
    return result;
}

Code reverse(Code code) {
    const CodeArray& stack = code->get_stack();
    if (stack.size() == 0) return code;
    CodeArray newstack(stack.size());
    std::copy(stack.rbegin(), stack.rend(), newstack.begin());
    return CodeList::adopt(newstack); //Code(new CodeList(newstack));
}

void flatten(Code code, CodeArray& stack) {
    const CodeArray& st = code->get_stack();

    if (st.size() == 0) {
        stack.push_back(code);
    } else {
        for (unsigned i = 0; i < st.size(); ++i) {
            flatten(st[i], stack);
        }
    }
}

Code flatten(Code code) {
    CodeArray stack;
    stack.reserve(code->size());
    flatten(code, stack);
    return CodeList::adopt(stack); //Code(new CodeList(stack));
}

Code parse(std::string s) {
    std::istringstream is(s);
    return parse(is);
}

Code parse(std::istream& is) {
   
    vector<Code> stack;
    int brackets_open = 0;
    
    char c = (char) is.get();
    
    while (is) {

        // skip whitespace
        
        if (!is) break;
        
        if (c==' ' || c == '\n' || c == '\r' || c == '\t') {
            c = (char)is.get();
            continue;
        }
        
        if (c == '#') // skip to eoln
        {
            // this code will miss lines that end with just \r
            // while (is && char(is.get()) != '\n' && ) {}
            do {
                c = is.get();
            } while(is && c != '\n' && c != '\r');

            c = (char)is.get();
            continue;
        }

        if (c == '(') // sub
        {
            stack.push_back(nil);
            ++brackets_open;
            c = (char)is.get();
            continue;
        }

        if (c == ')') // sub done
        {
            Code top = stack.back(); stack.pop_back();
            top = reverse(top);
            if (*car(top) == *nil) top = cdr(top);
            
            if (stack.empty()) { stack.push_back(top);
            } else {
                stack.back() = cons(top, stack.back());
            }
            if (--brackets_open==0) break;
            c = (char)is.get();
            
            continue;
        }
        
        // atom
        
        std::string atom; atom += c;
        c = char(is.get());
        while(is and c != ' ' and c != '\n' and c != '\t' and c != '\r' and c != ')' and c !='(')
        {
            atom += c;
            c = (char) is.get();
        }
        
        // check BOOLEAN
        if (atom == "TRUE" || atom == "FALSE") {
            Code value = Code(new Literal<bool>(atom == "TRUE"?true:false));
            if (stack.empty()) {
                stack.push_back(value);
            } else {
                stack.back() = cons(value, stack.back());
            }
            continue;
        }
        
        String2CodeMap::const_iterator it = str2code_map.find(atom);
        if (it != str2code_map.end()) {
            if (stack.empty()) {
                stack.push_back(it->second);
            } else {
                stack.back() = cons(it->second, stack.back());
            }
            continue;
        }
        
        // now left are int and float, int first
        char* end;
        long val = strtol(atom.c_str(), &end,10);

        if (end == atom.c_str() + atom.size()) // it's an int
        {
            Code v = Code(new Literal<int>(val));
            if (stack.empty()) {
                stack.push_back(v);
            } else {
                stack.back() = cons(v,stack.back());
            }
            continue;
        }

        double dbl = strtod(atom.c_str(), &end);

        if (end == atom.c_str() + atom.size()) // it's a double
        {
            Code v = Code(new Literal<double>(dbl));
            if (stack.empty()) {
                stack.push_back(v);
            } else {
                stack.back() = cons(v,stack.back());
            }
            continue;
        }

        /* Parse 'extension' code. For instance 1:2:3 which designates a Point type. 
         * Note: an extension hook CAN NOT CONTAIN SPACES */
        Code extra_type = nil;
        for (unsigned i = 0; i < parse_extension_hooks.size(); ++i) {
            extra_type = parse_extension_hooks[i](atom);
            if (extra_type != nil) break;
        }
        
        if (extra_type != nil) { // one of the extension hooks managed to parse the atom
            if (stack.empty()) {
                stack.push_back(extra_type);
            } else {
                stack.back() = cons(extra_type, stack.back());
            }
            continue;
        }
        
        // finally, if all else fails, it's a name
        name_t index = lookup(atom);
        Code name = Code(new Literal<name_t>(index));

        if (stack.empty()) {
            stack.push_back(name);
        } else {
            stack.back() = cons( name, stack.back());
        }
         
        if (brackets_open == 0) break; // just an atom
    }
   
    if (stack.empty()) throw std::runtime_error("parse error");
    
    Code back = stack.back();
    
    if (stack.size() > 1) throw std::runtime_error("parse error");
    
    if (car(back) == nil) return cdr(back);
    
    return back;
}

void all_points(Code code, CodeArray& stack) {
    const CodeArray& st = code->get_stack();

	stack.push_back(code);

	for (unsigned i = 0; i < st.size(); ++i) {
	    all_points(st[i], stack);
	}
}

int discrepancy(Code code1, Code code2) {
	CodeArray points1, points2;
	CodeArray::iterator it1, it2;
	int count;

	all_points(code1, points1);
	all_points(code2, points2);

	count = points1.size() + points2.size();

	for(it1 = points1.begin(); it1 != points1.end(); it1++ ) {
		for(it2 = points2.begin(); it2 != points2.end(); it2++ ) {
			// printf("checking\n");
			// cout << *it1 << "\n";
			// cout << *it2 << "\n";

			if (equal_to(*it1, *it2)) {
				// if we find the matching point, remove it from the second list,
				// break out of the second loop, and subtract 2 from the score.

				points2.erase(it2);
				count -= 2;
				break;
			}
		}
	}

	return count;
}


Code swap_sublist_position(Code code, int p1, int p2) {
	CodeArray st = code->get_stack();

	int s = st.size();

	if(p1 >= s || p2 >= s) return code;

	std::swap(*(st.rbegin() + p1), *(st.rbegin() + p2));

	return Code(new CodeList(st)); // return changed code
}

DiversityPool::DiversityPool(int tolerance) {
	_tolerance = tolerance;
}

void DiversityPool::addIndividual(Code c) {
	for( int n=0; n< _species.size(); n++) {
		bool add = 1;

		for(int i = 0; i < _species[ n].size(); i++) {
			// if it's over the tolerance, it can't be in this species

			if(discrepancy(c, _species[ n][ i]) > _tolerance) {
				add = 0;
				break;
			}
		}

		if(add) {
			_species[ n].push_back(c);
			return;
		}
	}

	// if we're here, it must be a new species

	std::vector< Code > v;

	v.push_back( c);

	_species.push_back( v);
}


int DiversityPool::getSize() {
	return _species.size();
}


}// namespace push

