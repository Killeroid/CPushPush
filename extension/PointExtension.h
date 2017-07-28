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

#include "Env.h"
#include "RNG.h"
#include "Literal.h"
#include "Instruction.h"

using namespace std;

namespace push {

typedef valarray<double> point_t;
    
const int POINT_STACK = LAST_STANDARD_STACK+1;
template <> inline size_t get_stack_num<point_t>()         { return POINT_STACK; }

Type pointType = make_type<point_t>(); //(POINT_STACK+1, POINT_STACK,1);

void initPointInstructions();

class PointEnv : public Env
{
    public:
    vector<point_t> point_stack;
	
	PointEnv():Env() { initPointInstructions(); function_set = instructions; };
    ~PointEnv() {}
    PointEnv(const PointEnv& other) {
	operator=(other);	
    }

    PointEnv& operator=(const PointEnv& other) {
	Env::operator=(other);
	point_stack = other.point_stack;
	return *this;
    }

    void force_next() {
	// this is a hack to force a PointEnv next_env -- for some reason
	// it was previously creating a regular Env.
	next_env = 0;
    	if (next_env == 0) next_env = PointEnv::clone();
    }

    virtual Env* clone() const { 
	PointEnv* newenv = new PointEnv(*this);
	return newenv;
    }

    void clear_stacks() {
	Env::clear_stacks();
	point_stack.clear();
    }

    unsigned get_stack_size(int which) const {
	if (which == POINT_STACK) {
	    return point_stack.size();
	}

	return Env::get_stack_size(which);
    }

    Type make_type() const {
	return Env::make_type() + Type(POINT_STACK+1, POINT_STACK, point_stack.size());
    }
    
    Code pop_stack_from_id(int id)  {
	if (id == POINT_STACK) {
	    point_t val = point_stack.back();
	    point_stack.pop_back();
	    return Code(new Literal< point_t >(val));
	}
	
	return Env::pop_stack_from_id(id);
    }
    
};

// specialize get_stack<T> for point_t (push, pop, top all use get_stack<T>)

template <> std::vector< point_t >& get_stack(Env& env) {
    return static_cast<PointEnv&>(env).point_stack;
}

// printing 
std::ostream& operator<<(ostream& os, point_t point) {
    for (unsigned i = 0; i < point.size(); ++i) {
	os << point[i];
	if (i != point.size() - 1) os << ':';
    }
}
// specialization of Literal<point_t> does_equal (valarray overloads '==' differently)

template <>
bool does_equal<point_t>(const point_t& a, const point_t& b) {
    // probably should use some tolerance here
    for (unsigned i = 0; i < a.size(); ++i) {
	if (a[i] != b[i]) return false;
    }
    return true;
}

std::string print(const PointEnv& env) {
    std::ostringstream os;
    os.setf(std::ios_base::showpoint);

	os << print((Env&)env);
	
    os << "\t(";
    for (unsigned i = 0; i < env.point_stack.size(); ++i)
    os << env.point_stack[i];
	os << ' ';
    os << ")\n";

	return os.str();
}


template <typename Func>
unsigned env_cast(Env& env) {
    return Func( static_cast<PointEnv&>(env));
}

// and make an instruction

unsigned point_add(Env& env) {
    point_t add = pop<point_t>(env);
    top<point_t>(env) += add;
    return 1;
}

unsigned point_sub(Env& env) {
    point_t sub = pop<point_t>(env);

    top<point_t>(env) -= sub;
    return 1;
}

unsigned point_mul(Env& env) {
	double mul = pop<double>(env);
	point_t point = top<point_t>(env);

	for (unsigned i = 0; i < point.size(); ++i) {
		point[i] *= mul;
	}

    return 1;
}

unsigned point_div(Env& env) {
	double div = pop<double>(env);
	point_t point = top<point_t>(env);

	for (unsigned i = 0; i < point.size(); ++i) {
		point[i] /= div;
	}

    return 1;
}

unsigned point_cross(Env& env) {
}

unsigned float_pointdot(Env& env) {
	point_t p1, p2;

	p1 = pop<point_t>(env);
	p2 = pop<point_t>(env);

	push(env, p1[0]*p2[0] + p1[1]*p2[1] + p1[2]*p2[2]);
}

unsigned point_fromfloat(Env& env) {
    std::valarray<double> p(0.0, 3);

	p[0] = p[1] = p[2] = pop<double>(env);

	push(env, p);
}

unsigned point_from3floats(Env& env) {
    std::valarray<double> p(0.0, 3);

	p[2] = pop<double>(env);
	p[1] = pop<double>(env);
	p[0] = pop<double>(env);

	push(env, p);
}

unsigned float_frompointx(Env& env) {
	push(env, pop<point_t>(env)[0]);
}

unsigned float_frompointy(Env& env) {
	push(env, pop<point_t>(env)[1]);
}

unsigned float_frompointz(Env& env) {
	push(env, pop<point_t>(env)[2]);
}

unsigned float_frompointxyz(Env& env) {
	point_t p = pop<point_t>(env);

	push(env, p[0]);
	push(env, p[1]);
	push(env, p[2]);
}

unsigned point_setx(Env& env) {
	top<point_t>(env)[0] = pop<double>(env);
}

unsigned point_sety(Env& env) {
	top<point_t>(env)[1] = pop<double>(env);
}

unsigned point_setz(Env& env) {
	top<point_t>(env)[2] = pop<double>(env);
}

unsigned point_length(Env& env) {
	point_t point = top<point_t>(env);
	double total = 0;

	for (unsigned i = 0; i < point.size(); ++i) {
		total += point[i] * point[i];
	}

	push(env, sqrt(total));
}

inline unsigned rand_point(Env& env) {
    double mn = env.parameters.min_random_float;
    double mx = env.parameters.max_random_float;

    std::valarray<double> p(0.0, 3);

    p[0] = rng.uniform() * (mx-mn) + mn;
    p[1] = rng.uniform() * (mx-mn) + mn;
    p[2] = rng.uniform() * (mx-mn) + mn;
 
	get_stack<point_t>(env).push_back(p);

    return 1;
}

Code parse_point(std::string atom) {
	std::string number;
	point_t p(0.0, 3);
	int n = 0;

	if(atom.find_first_not_of("1234567890:.") != -1 || atom.find_first_of(":") == -1) {
		return nil;
	}

	// printf("%s is a vector\n", atom.c_str());

	while(n < 3) { 
		p[n++] = atof(atom.c_str());

		int index = atom.find(":");

		if(index != -1) atom.erase(0, index + 1);
		else break;
	}

	return Code( new Literal< point_t >(p));
}

void initPointInstructions() {
// add generic instructions
static Code yd = make_instruction(yankdup<point_t>, "POINT.YANKDUP", integerType + pointType, pointType);

// static Code pop = make_instruction(pop<point_t>, "POINT.POP", pointType, nullType);

static Code pff = make_instruction(point_fromfloat, "POINT.FROMFLOAT", floatType, pointType);
static Code pf3f = make_instruction(point_from3floats, "POINT.FROM3FLOATS", floatType+floatType+floatType, pointType);

static Code ffx = make_instruction(float_frompointx, "FLOAT.FROMPOINTX", pointType, floatType);
static Code ffy = make_instruction(float_frompointy, "FLOAT.FROMPOINTY", pointType, floatType);
static Code ffz = make_instruction(float_frompointz, "FLOAT.FROMPOINTZ", pointType, floatType);

static Code ffxyz = make_instruction(float_frompointxyz, "FLOAT.FROMPOINTXYZ", pointType, floatType+floatType+floatType);

static Code psx = make_instruction(point_setx, "POINT.SETX", floatType+pointType, nullType);
static Code psy = make_instruction(point_sety, "POINT.SETY", floatType+pointType, nullType);
static Code psz = make_instruction(point_setz, "POINT.SETZ", floatType+pointType, nullType);

static Code add = make_instruction( point_add, "POINT.+", pointType+pointType, pointType);
static Code sub = make_instruction( point_sub, "POINT.-", pointType+pointType, pointType);
static Code mul = make_instruction( point_mul, "POINT.*", pointType+floatType, pointType);
static Code div = make_instruction( point_div, "POINT./", pointType+floatType, pointType);
static Code length = make_instruction( point_length, "POINT.LENGTH", pointType, floatType);
static Code rand = make_instruction( rand_point, "POINT.RAND", nullType, pointType);
static Code erc = make_instruction( rand_point, "POINT.ERC", nullType, pointType);

static int point_parse = static_initializer.register_parse_hook(parse_point);

}

} // namespace push
