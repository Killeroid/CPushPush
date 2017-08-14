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

/* This file implements a simple genetic programming system using the push3 library.
 * It reads a configuration push program (by default test.config) and a file that specifies
 * the input/output relation that is to be learned. First row of this file gives the 
 * number of fields and then the number of records to read in. It will then try to find a program that 
 * solves the problem, i.e., fits all fitness cases perfectly. */

#include <fstream>
#include <vector>
#include <valarray>
#include <set>

#include "CodeUtils.h"
#include "RNG.h"
#include "GeneticOps.h"
#include "Instruction.h"
#include "Literal.h"

using namespace std;
using namespace push;

unsigned pop_size = 100;
unsigned nGenerations = 100;

/* Add some instructions to the push engine for setting various parameters */
unsigned set_popsize(Env& env) { 
    pop_size = pop<int>(env);
    return 1;
}
Code pp = make_instruction(set_popsize,"GP.POPSIZE", integerType, nullType);

unsigned set_generations(Env& env) {
    nGenerations = pop<int>(env);
    return 1;
}
Code ppd = make_instruction(set_generations,"GP.NGENS", integerType, nullType);

/* Individual: piece of code, fitness and effort  */
struct Individual {
    Individual() : code(), fitness(-1), effort(1) {}

    Code code;
    double fitness;
    unsigned effort;
    
    vector<push::Type> trace;
    
    // test for 'better', which is first fitness, then size
    /*bool operator<(const Individual& other) const {
	if (fitness > other.fitness)
	    return true;
	if (fitness == other.fitness && effort < other.effort)
	    return true;
	return false;
    }*/

    bool operator<(const Individual& other) const {
	if (fitness > other.fitness)
	    return true;
	return false;
    }
};

// the best guy so far plus the data
Individual best;
vector<int> targets;
vector<vector<int> > inputs;

// The push evaluator, creates an Individual from code (Individual = code + fitness)
Individual evaluate(Code code, const Env& org) 
{
    Individual indy;
    indy.code = code;
    
    indy.effort = 0;
    indy.fitness = 0;
    
    // The working environment
    static Env env(1000);
    
    for (unsigned i = 0; i < targets.size(); ++i) {
	
	// Copy Org over Env to make sure that we're working with the right function set
	env = org; 
	env.clear_stacks();

	// fill stacks
	for (unsigned j = 0; j < inputs[i].size(); ++j)
	{
	    int val = inputs[i][j];
	    push::push<int>(env,val);
	}

	// push calling convention: set up code both on execution and code stack
	push_call(env,code);
	
	unsigned effort = env.go_trace(env.parameters.evalpush_limit, indy.trace, 0); // true: yield on error
	
	indy.effort += effort;
	    
	if (effort >= env.parameters.evalpush_limit) // time's up
	{
	    indy.fitness -= 1./targets.size();
	    //break; // quit evaluating
	}

	if (has_elements<int>(env,1)) {
	    int val = push::pop<int>(env);
	    double dist = val - targets[i];
	    dist = abs(dist);
	    //indy.fitness += std::max(0., 1.0-dist*dist/(targets[i]*targets[i]) );
	    
	    if (val ==  targets[i]) {
		indy.fitness += 1;
	    }
	    else {
		break; // break out when a single failure occurs
	    }
	}
	else break; // break out when a single failure occurs
    }
    
    return indy;
}

struct Sorter {
    const vector<Individual>& pop;
    Sorter(const vector<Individual>& _pop) : pop(_pop) {}
    bool operator()(unsigned i, unsigned j) const {
	return pop[i] < pop[j];
    }
};

void select(const vector<Individual>& pop, vector<unsigned>& tour) {
    for (unsigned i = 0; i < tour.size(); ++i) {
	tour[i] = rng.random(pop.size());
    }
    sort(tour.begin(), tour.end(), Sorter(pop));
}
    
vector<Individual> init(const Env& env) {
    
    /* Initialize population, use workEnv to do the actual work */
    static Env workEnv(1000);

    vector<Individual> pop(pop_size);
    static Code rnd = parse("CODE.RAND");
    
    for (unsigned i = 0; i < pop_size; ++i) {
	workEnv = env; // reset workEnv
	(*rnd)(workEnv); // create random program
	if (not has_elements<Code>(workEnv,1)) { --i; continue; }
	Code code = push::pop<Code>(workEnv); // get random program

	pop[i] = evaluate( code, env);
    
	if (pop[i] < best) best = pop[i];
    }
    
    return pop;
}

std::string print_code(Code code, Env& env) {
    
    if (code->size() == 1) {
	std::string result = code->to_string();
	
	// check if it's a name
	Literal<name_t>* name = dynamic_cast< Literal<name_t>* >(code.get());
	if (name) {
	    result += string(":") + get_code(name->get())->to_string();
	}
	
	return result;
    }
    
    std::string result = "( ";
    const CodeArray& pieces = code->get_stack();
    for (int i = pieces.size()-1; i >= 0; --i) {
	result += print_code(pieces[i], env);
	result += " ";
    }
    return result + " )";
}

vector<Individual> run(const Env& env, vector<Individual> pop) {
   
    vector<unsigned> tournament(5);
   
    Env workEnv(env); // initialize environment (used for breeding)
    
    sort(pop.begin(), pop.end());
    best = pop[0];
    CodeBase& swapcode = *parse("CODE.SWAP"); 
   
    int best_loc = 0;
    int total_effort = 0;
    int n_top = 0;
    int n_produced = 0;  
    for (unsigned gen = 0; gen < nGenerations; ++gen) {


	double sumf = 0.0;
	double sumsize = 0.0;
	for (unsigned i = 0; i < pop.size(); ++i) 
	{ 
	    sumf += pop[i].fitness; 
	    sumsize += pop[i].code->size();
	} 
	
	cerr << "Gen:     " << gen << '\n';
	cerr << "Fitness: " << best.fitness << '\n';
	cerr << "Avg:     " << sumf/pop.size() << '\n';
	cerr << "Avg sz:  " << sumsize/pop.size() << '\n';
	cerr << "Effort:  " << best.effort << '\n';
	cerr << "Total:   " << total_effort << '\n';
	cerr << "Top:     " << n_top << '\n';
	//cerr << "Expand:  " << print_code(best.code, workEnv) << '\n';
	cerr << "Prod.:   " << n_produced << '\n';
	cerr << "Code:    " << best.code << "\n\n"; 
	
	cerr << "Dict:    " << dict_size() << "\n";
	
	total_effort = 0;
	n_top = 0;
	n_produced=0;

	
	for (unsigned i = 1; i < pop_size; ++i) {
	    // select, vary, breed
	    
	    select(pop, tournament);
	    
	  //  for (unsigned j = 0; j < tournament.size(); ++j) {
	//	cerr << pop[tournament[j]].fitness << ' ';
	    //}
	    //cerr << endl;
	    
	    //cerr << "tours " << tournament[0] << ' ' << tournament[1] << ' ' << tournament[2] << endl;
	    //cerr << "perfs " << newpop[0].fitness << ' ' << pop[tournament[0]].fitness << ' ' << pop[tournament[1]].fitness << ' ' << pop[tournament[2]].fitness << endl;
	    
	    Individual indy = pop[tournament[0]];
	    workEnv.clear_stacks();

	    push::push(workEnv, indy.code);
	    push::push(workEnv, pop[tournament[1]].code);
		
	    if (rng.flip(0.5)) { // should be a parameter
		if (rng.flip(0.5))
		    subtree_mutation(workEnv);
		else
		    deletion_mutation(workEnv);
	    }
	    else
	    {
		swapcode(workEnv);
		subtree_xover(workEnv);
	    }
		
	    Code code = push::pop<Code>(workEnv);
	   
	    
	    if (*code == *indy.code || *code == *pop[tournament[1]].code) {
		continue;
	    }
	    
	    // evaluate
	    Individual newindy = evaluate(code, env);
	    total_effort += newindy.effort;

	    if (indy.trace == newindy.trace || pop[tournament[1]].trace == newindy.trace) {
		continue;
	    }
	    
	    n_produced++; 
	    int loser = tournament.back(); //rng.random(pop.size()); // tournament.back();
	    pop[loser] = newindy;
	    
	    if (pop[loser].fitness >= best.fitness) {
		best = pop[loser];
		n_top++;
	    }
	}
    }

    return pop;
}

int main(int argc, char* argv[] ) {
    push::init_push(); 
    // need config file and file with target info
    std::string config_file = argc<2?"test.config":argv[1];
    std::string target_file = argc<3?"test.inputs":argv[2];
    
    ifstream config( config_file.c_str() );
    
    if (!config) {
	cerr << "Could not read config file " << config_file << endl;
	cerr << "\nUsage: " << argv[0] << " [configfile] [inputfile]\n";
	return 1;
    }
    
    Env env;
    
    cerr << "Reading config file" << endl;
    // execute configuration file
    env.configure(parse(config));
    
    cerr << "Random seed " << env.parameters.random_seed << '\n';
    
    ifstream target( target_file.c_str() );
    
    if (!target) {
	cerr << " Could not read target file " << target_file << endl;
	cerr << "\nUsage: " << argv[0] << " [configfile] [inputfile]\n";
	return 1;
    }
   
    // read in data
    int nRecords, nFields;
    target >> nFields >> nRecords;
    
    targets.resize(nRecords);
    
    inputs.resize(0);
    inputs.resize(nRecords, vector<int>(nFields-1));
    
    for (int i = 0; i < nRecords; ++i) {
	for (int j = 0; j < nFields-1; ++j) {
	    target >> inputs[i][j];
	}
	target >> targets[i];
    }
    
    
    vector<Individual> pop = init(env.next());
    
    env.next().clear_stacks(); // make sure all stacks are cleared (but not names)
    
    
    //cerr << "Running for " << nGenerations << " generations" << endl;
    //cerr << "Next env function set " << env.next().function_set << endl;
    // run using the next env!
    run(env.next(),pop);
}



