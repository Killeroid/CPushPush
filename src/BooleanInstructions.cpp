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

#include "Instruction.h"
#include "Env.h"

#include "Literal.h"
#include "BoolOps.h"
#include "NumOps.h"
#include "RandOps.h"

namespace push {



int initBool() {
    /* BOOLEAN */
    Type binaryBool = boolType + boolType;

    make_instruction(_and, "BOOLEAN.AND", binaryBool, boolType);
    make_instruction(_or, "BOOLEAN.OR", binaryBool, boolType);
    make_instruction(_not, "BOOLEAN.NOT", boolType, boolType);
    make_instruction(_nor, "BOOLEAN.NOR", binaryBool, boolType);
    make_instruction(_nand, "BOOLEAN.NAND", binaryBool, boolType);
    make_instruction(float2bool, "BOOLEAN.FROMFLOAT", floatType, boolType);
    make_instruction(int2bool, "BOOLEAN.FROMINTEGER", integerType, boolType);
    make_instruction(rand_bool, "BOOLEAN.RAND", nullType, boolType);
    make_instruction(rand_bool, "BOOLEAN.ERC", nullType, boolType);
}

} // namespace push

