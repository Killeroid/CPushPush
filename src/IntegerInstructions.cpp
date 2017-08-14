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

#include "Instruction.h"
#include "Env.h"

#include "Literal.h"
#include "NumOps.h"
#include "RandOps.h"

namespace push {

void initInt() {
    Type binaryInt = integerType + integerType;
    
    make_instruction(plus<int>, "INTEGER.+", binaryInt, integerType);
    make_instruction(minus<int>,"INTEGER.-", binaryInt, integerType);
    make_instruction(multiplies<int>, "INTEGER.*", binaryInt, integerType);
    make_instruction(divides<int>, "INTEGER./", binaryInt, integerType);
    make_instruction(bool2int, "INTEGER.FROMBOOLEAN", boolType, integerType);
    make_instruction(float2int, "INTEGER.FROMFLOAT", floatType, integerType);
    make_instruction(int_mod,"INTEGER.%", binaryInt, integerType);
    make_instruction(smaller<int>,"INTEGER.<", binaryInt, boolType);
    make_instruction(greater<int>,"INTEGER.>", binaryInt, boolType);
    make_instruction(_max<int>, "INTEGER.MAX", binaryInt, integerType);
    make_instruction(_min<int>, "INTEGER.MIN", binaryInt, integerType);
    make_instruction(rand_int, "INTEGER.RAND", nullType, integerType);
    make_instruction(rand_int, "INTEGER.ERC", nullType, integerType);
}


} // namespace push
