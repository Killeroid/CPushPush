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

#include <iostream>
#include "Type.h"
#include "Env.h"

namespace push {

//Type::Type(const Env& env) : type(env.nStacks()) {}

std::ostream& operator<<(std::ostream& os, const Type& tp) {
    if (tp.get().size() == 0) os << 0 << ' ';
    
    for (unsigned i = 0; i < tp.get().size(); ++i) {
        os << tp.get()[i] << ' ';
    }
    return os;
}   
	
bool Type::can_pop_from(const Env& env) const {
    for (unsigned i = _start; i < type.size(); ++i) {
        if (type[i] > 0 and env.get_stack_size(i) < static_cast<unsigned>(type[i])) {
            //std::cerr << "Fail on " << i << ' ' << env.get_stack_size(i) << std::endl;
            return false;
        }
    }
    return true;
}

bool Type::operator==(const Type& other) const {
    
    unsigned mins = std::min(type.size(), other.type.size());

    for (unsigned i = 0; i < mins; ++i) {
        if (type[i] != other.type[i]) return false;
    }
    
    for (unsigned i = mins; i < type.size(); ++i) {
        if (type[i] != 0) return false;
    }

    for (unsigned i = mins; i < other.type.size(); ++i) {
        if (other.type[i] != 0) return false;
    }
    
    return true;
}

void Type::abs() {
    for (unsigned i = 0; i < type.size(); ++i) 
        type[i] = std::abs(type[i]);
    
}

}



