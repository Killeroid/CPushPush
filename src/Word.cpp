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

#include <sstream>
#include <list>

#include "Literal.h"
#include "RNG.h"
#include "Word.h"
#include "CodeUtils.h"

using namespace std;

namespace push {

typedef detail::weak_ptr<simple_name> name_ref;

struct NameInfo {
    std::string name;
    
    bool unset;
    Code binding;
    
    name_ref reference;
    std::list<unsigned>::iterator location;

    NameInfo() : name(""), unset(true), binding(nil), reference(0) {}

    unsigned refcount() const { if (name=="") return 0; return reference.refcount(); }
    
};

// the data
static std::vector<unsigned>	free_idx;
static std::list<unsigned>	used_idx;
static std::vector<NameInfo>   globals;

unsigned dict_size(void) { return used_idx.size(); }

void expand() {
    
    // first a round of garbage collection 
    collect_garbage();
    
    if (free_idx.size() > 0) return; // if gc helped, do not expand
    
    unsigned org_size = globals.size();
    unsigned newsize = (org_size*2 < 64)? 64 : org_size*2;
    globals.resize(newsize);
    free_idx.reserve(newsize-org_size);
    for (unsigned i = org_size; i < globals.size(); ++i) {
        free_idx.push_back(i);
    }
}

void destroy(unsigned idx) {
    
    if (globals[idx].name=="") return; // invalid, can occur with last round of gc
    
    free_idx.push_back(idx);
    used_idx.erase( globals[idx].location );
    
    globals[idx] = NameInfo();
}

name_t lookup(std::string name) {
    // lookup is O(n) (only used when parsing)
    for (std::list<unsigned>::iterator it = used_idx.begin(); it != used_idx.end(); ++it) {
        unsigned idx = *it;
        if (globals[idx].name == name) { // found
            return globals[idx].reference.lock();
        }
    }
    // create new name

    if (free_idx.empty()) expand();
    
    unsigned idx = free_idx.back(); 
    free_idx.pop_back();
    
    used_idx.push_front(idx);
    
    name_t new_name(new simple_name(idx));
    
    NameInfo info;
    info.name = name;
    info.reference = name_ref(new_name);
    info.location = used_idx.begin(); // remember where we left it
    info.unset = true;
    
    globals[idx] = info;
    
    
    return new_name;
}

string print(name_t index) {
    return globals[index->idx()].name;
}

name_t rand_name() {
    
    // 64 bits should be enough for everyone!
    static unsigned long long n = 0;
    
    ostringstream os;
    
    os << "_" << n++; // underscore signals 'internal' name
    
    return lookup(os.str()); // should be unique
}

name_t rand_bound_name() {
    
    name_t result;

    if (globals.size() == 0) return rand_name(); // this is neccessary for the thing to work as an ERC
     
    unsigned choice = rng.random(used_idx.size());
    
    std::list<unsigned>::iterator it = used_idx.begin();

    for(;;) {
        if (choice-- == 0) return globals[ *it ].reference.lock();
        ++it;
    }
   
    // cannot happen
    return globals[0].reference.lock();
}

Code get_code(name_t name) {
    
    NameInfo& info = globals[ name->idx() ];

    static Code quote = parse("NAME.QUOTE");
    
    if (info.unset) {
        vector<Code> code(2);
        code[0] = Code( new Literal<name_t>(name)); 
        code[1] = quote;
        return Code( new CodeList(code) );
    }
    
    return info.binding;
}

void set_code(name_t name, Code code) {
    globals[ name->idx() ].binding = code;
    globals[ name->idx() ].unset = false;
}

void remove_refs(vector<int>& v, Code code) {
    
    const Literal<name_t>* l = dynamic_cast<const Literal<name_t>* >( code.get() );

    if (l != 0) {
        unsigned idx = l->get()->idx();
        v[idx]--;
    }
    
    const CodeArray& stack = code->get_stack();
    for (unsigned i = 0; i < stack.size(); ++i) {
        remove_refs(v,stack[i]);
    }
}

/* garbage collector takes care of circular names
 * it's a simple mark-sweep algorithm */
void collect_garbage() {

    vector<int> refc(globals.size());
    for (unsigned i = 0; i < refc.size(); ++i) {
        refc[i] += globals[i].refcount();	// get the refcount
        remove_refs(refc, globals[i].binding);  // remove those references that are referred to by the code
    }
  
    // if there are still non-zero references, these must be from 'live' references
    for (unsigned i = 0; i < refc.size(); ++i) {
        if (globals[i].name == "") continue;

        if (refc[i] == 0) { // sweep
            destroy(i);
        }
        assert(refc[i] >= 0);
    }
}

} // namespace push
