
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
#include "Word.h"

using namespace std;
using namespace push;

int main()
{
    
    Code code = parse("(fib : fib)");
   
    {
	Env env;
	push_call(env, code);
	env.go(100);
    }
    if (dict_size() != 1) return 1;

    collect_garbage();
    
    if (dict_size() != 1) return 1;
   
    code = nil;
    collect_garbage();
    
    if (dict_size() != 0) return 1;
    
    code = parse("(foo : bar bar : (foo bar) )");
    {
	Env env;
	push_call(env, code);
	env.go(100);
	cout << env << endl;
    }

    if (dict_size() != 2) return 1;
    
    collect_garbage();
    if (dict_size() != 2) return 1;
    
    cout << "before destroy " << dict_size() << endl;
    code = nil;

    collect_garbage();
    if (dict_size() != 0) return 1; 
   
    code = parse("(foo : bar bar : baz baz : (foo baz) )");
    
    {
	Env env;
	push_call(env, code);
	env.go(100);
    }

    Code baz = parse("baz");
    
    cout << "Baz is bound to " << get_code(static_cast<Literal<name_t>*>(baz.get())->get()) << endl;
    
    collect_garbage();
    
    if (dict_size() != 3) return 1;
    
    code = nil;

    collect_garbage();

    if (dict_size() != 1) return 1;
   
    baz = nil;
    
    collect_garbage(); 
    
    if (dict_size() != 0) return 1;

    code = parse("(foo : (baz baz))");
    collect_garbage();
   
    if (dict_size() != 2) {
	cout << "Err: dict size = " << endl;
	return 1;
    }
   
    code = nil;
    collect_garbage();

    if (dict_size() != 0) return 1;
    
    return 0;
}


