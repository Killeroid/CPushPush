#include <fstream>
#include <string>

#include "pushC.h"
#include "CodeUtils.h"
#include "CustomInstruction.h"
#include "PointExtension.h"
#include "Env.h"
#include "GeneticOps.h"

using namespace std;
using namespace push;

PushEnvironment *pushEnvironmentNew(int seed) {
	PointEnv *environment = new PointEnv;

	// environment->force_next();

	initPointInstructions();

	get_stack<int>(*environment ).push_back( seed);
	Code seedI = parse("ENV.RANDOM-SEED");
	(*seedI)( *environment);

	return (PushEnvironment *)environment;
}

void pushEnvironmentReadConfigFile(PushEnvironment *env, char *file) {
	PointEnv *environment = (PointEnv*)env;
	std::ifstream config( file );
	Code code;

	if(!config) {
		cerr << "Could not read config file " << file << endl;
		return;
	}

    environment->configure(parse(config));
}

PushCustomInstruction *pushAddCallbackInstruction(PushEnvironment *env, unsigned int (*callback)(PushEnvironment *environment, void *d), void (*freeData)(void *d), char *name, void *callbackData) {
	Env &environment = ((Env*)env)->next();
	std::string s = name;

	unsigned (*cf)(Env&,void*) = (unsigned (*)(Env&,void*))callback;
	void (*ff)(void*) = (void (*)(void*))freeData;

	CustomInstruction *ci = new CustomInstruction( cf, s, callbackData) ;

	ci->setDataFreeCallback(freeData);

	Code codeCi(ci);

	//(environment).set_code( (environment).insert_string(s), codeCi );
	set_code( (environment).insert_string(s), codeCi );

	name_t dictname = lookup(s); 

	Code nameCode = Code( new Literal<name_t>(dictname));
	environment.function_set = cons( nameCode, environment.function_set);

	return (PushCustomInstruction*)ci;
}

void pushAddMacroInstruction(PushEnvironment *env, char *name, PushCode *value) {
	Env &environment = ((Env*)env)->next();
	std::string s = name;

    Code codeCi(*(Code*)value);

    //(environment).set_code( (environment).insert_string(s), codeCi );
    set_code( (environment).insert_string(s), codeCi );

    name_t dictname = lookup(s);
    Code function_set = environment.function_set;
    Code nameCode = Code( new Literal<name_t>(dictname));
    environment.function_set = cons( nameCode, environment.function_set);
}

void pushEnvironmentFree(PushEnvironment *environment) {
	delete (Env*)environment;
}

char *pushGetConfigString(PushEnvironment *environment) {
	return strdup(print_config(((PointEnv*)environment)->next()).c_str());
}

char *pushGetStacksString(PushEnvironment *env) {
	PointEnv &environment = (PointEnv&)((PointEnv*)env)->next();
	return strdup(print(environment).c_str());
}

char *pushGetExecStackString(PushEnvironment *env) {
	std::ostringstream os;
	Env &environment = ((Env*)env)->next();
	environment.print_exec_stack(os);

	return strdup(os.str().c_str());
}

char *pushCodeGetString(PushCode *code) {
	if(!code) return strdup("( )");

	return strdup(str(*(Code*)code).c_str());
}

void pushClearStacks(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	environment.clear_stacks();
	environment.next().clear_stacks();
	environment.quote_name_flag = false;
}

PushCode *pushParse(char *string) {
	std::string s = string;
	return (PushCode*)new Code(parse(string));
}

void pushCodeFree(PushCode *code) {
	delete (Code*)code;
}

int pushCodeSize(PushCode *code) {
	return (*(Code*)code)->size();
}

void pushEnvironmentSetListLimit(PushEnvironment *env, int limit) {
	Env &environment = ((Env*)env)->next();

	environment.parameters.max_points_in_program = limit;
}

int pushEnvironmentGetEvalPushLimit(PushEnvironment *env, int limit) {
	Env &environment = ((Env*)env)->next();

	return environment.parameters.evalpush_limit;
}

int pushRun(PushEnvironment *env, PushCode *code, int steps) {
	Env &environment = ((Env*)env)->next();

	push_call(environment, *(Code*)code);

	return environment.go(steps);
}

int pushStep(PushEnvironment *env, int steps) {
	Env &environment = ((Env*)env)->next();

	return environment.go(steps);
}

int pushIntStackSize(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	return get_stack<int>(environment).size();
}

void pushIntStackPop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	if(get_stack<int>(environment).size() < 1) return; 
	get_stack<int>(environment).pop_back();
}

int pushIntStackTop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	if(get_stack<int>(environment).size() < 1) return 0; 
	return get_stack<int>(environment).back();
}

void pushIntStackPush(PushEnvironment *env, int value) {
	Env &environment = ((Env*)env)->next();
	get_stack<int>(environment).push_back(value);
}


int pushBoolStackSize(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	return get_stack<bool>(environment).size();
}

void pushBoolStackPop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	if(get_stack<bool>(environment).size() < 1) return; 
	get_stack<bool>(environment).pop_back();
}

int pushBoolStackTop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	if(get_stack<bool>(environment).size() < 1) return 0; 
	return get_stack<bool>(environment).back();
}

void pushBoolStackPush(PushEnvironment *env, int value) {
	Env &environment = ((Env*)env)->next();
	get_stack<bool>(environment).push_back(value);
}



int pushFloatStackSize(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	return get_stack<double>(environment).size();
}

void pushFloatStackPop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	if(get_stack<double>(environment).size() < 1) return; 
	get_stack<double>(environment).pop_back();
}

double pushFloatStackTop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	if(get_stack<double>(environment).size() < 1) return 0; 
	return get_stack<double>(environment).back();
}

void pushFloatStackPush(PushEnvironment *env, double value) {
	Env &environment = ((Env*)env)->next();
	get_stack<double>(environment).push_back(value);
}


int pushNameStackSize(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	return get_stack<name_t>(environment).size();
}

void pushNameStackPop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	if(get_stack<name_t>(environment).size() < 1) return; 
	get_stack<name_t>(environment).pop_back();
}

PushName *pushNameStackTop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	if(get_stack<name_t>(environment).size() < 1) return 0; 
	
	// make sure the name will be valid at least until the next garbage collection
	set_code( 
		rand_name(), 
		Code( new Literal<name_t>(top<name_t>(environment)))
	    ); // rand_name will bind to this guy and will keep it alive until it is collected
	
	return (PushName*) &get_stack<name_t>(environment).back(); // will this work?
}

void pushNameStackPush(PushEnvironment *env, PushName *value) {
	Env &environment = ((Env*)env)->next();
	get_stack<name_t>(environment).push_back(* (name_t*) value);
}


int pushCodeStackSize(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();
	return get_stack<Code>(environment).size();
}

void pushCodeStackPop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();

	if(get_stack<Code>(environment).size() < 1) return; 
	get_stack<Code>(environment).pop_back();
}

PushCode *pushCodeStackTop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();

	if(get_stack<Code>(environment).size() < 1) return NULL; 

	Code result = get_stack<Code>(environment).back();

	return (PushCode*)new Code(result);
}

void pushCodeStackPush(PushEnvironment *env, PushCode *value) {
	Env &environment = ((Env*)env)->next();

	get_stack<Code>(environment).push_back(*(Code*)value);
}


int pushVectorStackSize(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();

	return get_stack<point_t>(environment).size();
}

void pushVectorStackPop(PushEnvironment *env) {
	Env &environment = ((Env*)env)->next();

	if(get_stack<point_t>(environment).size() < 1) return; 
	get_stack<point_t>(environment).pop_back();
}

PushVector *pushVectorStackTop(PushEnvironment *env, PushVector *value) {
	Env &environment = ((Env*)env)->next();

	if(get_stack<point_t>(environment).size() < 1) {
		value->x = 0;
	 	value->y = 0;
	 	value->z = 0;
	 	return value;
	}

    point_t point = top<point_t>(environment);

	value->x = point[0];
	value->y = point[1];
	value->z = point[2];

	return value;
}

void pushVectorStackPush(PushEnvironment *env, PushVector *value) {
	Env &environment = ((Env*)env)->next();
	std::valarray<double> point(3);

	point[0] = value->x;
	point[1] = value->y;
	point[2] = value->z;

	get_stack<point_t>(environment).push_back(point);
}

/////////////////////////////////////////// GENETIC OPERATORS ////////////////

PushCode *pushCodeCrossover(PushEnvironment *env, PushCode *p1, PushCode *p2) {
	Env &environment = ((Env*)env)->next();
	static Env workEnv(1000);

	get_stack<Code>(workEnv).push_back(*(Code*)p1);
	get_stack<Code>(workEnv).push_back(*(Code*)p2);

	workEnv.parameters.max_points_in_program = environment.parameters.max_points_in_program;

	subtree_xover(workEnv);

	PushCode *result = (PushCode*)new Code(get_stack<Code>(workEnv).back());
	get_stack<Code>(workEnv).pop_back();

	return result;
}

PushCode *pushCodeSubtreeMutate(PushEnvironment *env, PushCode *p1, int size) {
	Env &environment = ((Env*)env)->next();

	environment.parameters.max_points_in_random_expression = size;

	get_stack<Code>(environment).push_back(*(Code*)p1);

	subtree_fair_mutation(environment);

	PushCode *result = (PushCode*)new Code(get_stack<Code>(environment).back());
	get_stack<Code>(environment).pop_back();

	return result;
}

PushCode *pushCodeDeletionMutate(PushCode *code) {
	static Env workEnv(1000);

	workEnv.clear();

	get_stack<Code>(workEnv).push_back(*(Code*)code);

	deletion_mutation_no_empty_list(workEnv);

	PushCode *result = (PushCode*)new Code(get_stack<Code>(workEnv).back());
	get_stack<Code>(workEnv).pop_back();

	return result;
}

PushCode *pushCodeFlattenMutate(PushCode *c) {
	Code code = Code(*(Code*)c);

    int r = rng.random((code)->size());

	if(extract(code, r)->size() <= 1) return (PushCode*)new Code(code);

	return (PushCode*)new Code(insert(code, r, flatten(extract(code, r))));

}

PushCode *pushCodeRandom(PushEnvironment *env, int size) {
	PointEnv &environment = (PointEnv&)((PointEnv*)env)->next();
	static Code rnd = parse("CODE.RAND");

	environment.parameters.max_points_in_random_expression = size;

	(*rnd)(environment);

	PushCode *result = (PushCode*)new Code(get_stack<Code>(environment).back());
	get_stack<Code>(environment).pop_back();

	return result;
}
