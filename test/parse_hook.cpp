
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

#include <iostream>
#include <string>

#include "StaticInit.h"
#include "Literal.h"
#include "CodeUtils.h"
#include "Env.h"

using namespace std;
using namespace push;

int main()
{
    Code code = parse("pi");
    
    
    if (dynamic_cast<Literal<double>* >(code.get()) == 0) 
	return 1;
    return 0;
}


// simple test for the extensions hook
Code parsePI (std::string atom) {
    
    if (atom == "pi" || atom == "PI") {
	return Code(new Literal<double>(M_PI)); // return literal containing the value PI
    }
    
    // failed
    return nil;
}

// add the hook to push
static int dummy = static_initializer.register_parse_hook(parsePI);

