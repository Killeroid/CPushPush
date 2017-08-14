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

#include "StaticLibMacros.h"
DECLARE_OBJECT(FloatInstructions);

void initFloat() {
    Type binaryFloat = floatType + floatType;

    make_instruction(plus<double>,"FLOAT.+", binaryFloat, floatType);
    make_instruction(minus<double>,"FLOAT.-", binaryFloat, floatType);
    make_instruction(multiplies<double>, "FLOAT.*", binaryFloat, floatType);
    make_instruction(divides<double>, "FLOAT./", binaryFloat, floatType);
    make_instruction(bool2float, "FLOAT.FROMBOOLEAN", boolType, floatType);
    make_instruction(int2float, "FLOAT.FROMINTEGER", integerType, floatType);
    make_instruction(_fmod,"FLOAT.%", binaryFloat, floatType);
    make_instruction(smaller<double>,"FLOAT.<", binaryFloat, boolType);
    make_instruction(greater<double>, "FLOAT.>", binaryFloat, boolType);
    make_instruction(_max<double>, "FLOAT.MAX", binaryFloat, floatType);
    make_instruction(_min<double>, "FLOAT.MIN", binaryFloat, floatType);
    make_instruction(_cos, "FLOAT.COS", floatType, floatType);
    make_instruction(_sin, "FLOAT.SIN", floatType, floatType);
    make_instruction(_tan, "FLOAT.TAN", floatType, floatType);
    make_instruction(rand_float, "FLOAT.RAND", nullType, floatType);
    make_instruction(rand_float, "FLOAT.ERC", nullType, floatType);
}


} // namespace push

