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

#ifndef _BOOL_OPS_H__
#define _BOOL_OPS_H__

#include "Env.h"

namespace push {
    
inline unsigned _and(Env& env) {
    //if (not has_elements<bool>(env,2)) return 1;

    bool first = pop<bool>(env);
    bool second = pop<bool>(env);
    push<bool>(env, first and second);
    
    return 1;
}

inline unsigned _or(Env& env) {
    //if (not has_elements<bool>(env,2)) return 1;

    bool first = pop<bool>(env);
    bool second = pop<bool>(env);
    push<bool>(env, first or second);
    
    return 1;
}

inline unsigned _not(Env& env) {
    //if (not has_elements<bool>(env,1)) return 1;

    bool first = pop<bool>(env);
    push<bool>(env, !first);
    
    return 1;
}

inline unsigned _nor(Env& env) {
    //if (not has_elements<bool>(env,2)) return 1;
    bool first = pop<bool>(env);
    bool second = pop<bool>(env);
    push<bool>(env, !(first or second));
    return 1;
}

inline unsigned _nand(Env& env) {
    //if (not has_elements<bool>(env,2)) return 1;
    bool first = pop<bool>(env);
    bool second = pop<bool>(env);
    push<bool>(env, !(first and second));
    return 1;
}

} // namespace push
#endif
