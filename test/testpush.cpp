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
#include <string>

#include "CodeUtils.h"
#include "Env.h"
#include "StaticInit.h"

using namespace std;
using namespace push;

int main()
{
    init_push();

    Env env;
    Code code = parse(std::cin);

    push_call(env, code);
    unsigned r = 0;
    while( not env.done()) {
        r += env.go(1000);
    }
    
    vector<bool>& stack = get_stack<bool>(env);
    if (stack.empty() || stack.back() == false) {
        cout << code << endl;
        cout << env;
        return 1;
    }
    return 0;
}


