#include "Code.h"
#include "Literal.h"
#include "Instruction.h"


namespace push {

using namespace std;
    
template<class T>
class UncheckedInstruction : public Instruction {
    
    public:

    UncheckedInstruction(const Instruction& ins) : Instruction(ins.get_op(), ins.to_string()) {}

    bool can_run() const { return true; }
    unsigned operator()(Env& env) const { get_op()(env); }
};

class CompiledCode : public TypedAtom {
    
    vector<TypedAtom*> ptrs;
    Code original_code;
    
    Type pre;
    Type post;
    
    public :

    CompiledCode() : ptrs(0), original_code(nil), pre(nullType), post(nullType) {}
    
    CompiledCode(Type pr, Type po, const CodeArray& org, const vector<TypedAtom*>& ptr) 
	:   ptrs(ptr),
	    original_code(new CodeList(org)),
	    pre(pr),
	    post(po) 
	    {}
    
    bool can_run(const Env& env) const {
	return pre.can_pop_from(env);
    }
    
    /* batch run */
    unsigned operator()(Env& env) const {
	if (!can_run(env)) return 1;
	for (unsigned i = 0; i < ptrs.size(); ++i) {
	    (*ptrs[i])(env);
	}
	return ptrs.size();
    }
    
    const Type& get_precondition()  const { return pre; }
    const Type& get_postcondition() const { return post; }

    std::string to_string() const { return std::string("COMPILEDCODE ") + original_code->to_string(); }
    
    Code get_code() const { return original_code; }
    
};

Code compile(Code code, const Type& precondition, const Type& desired_condition) {
    
    code = flatten(code);
    CodeArray array = code->get_stack();
    
    vector<Code> result;
    vector<TypedAtom*> compiled;
    
    Type postcondition = precondition;
    
    while (array.size()) {

	code = array.back();
	array.pop_back();
	
	TypedAtom* instruction = dynamic_cast<TypedAtom*>(code.get());

	if (instruction == 0) continue; // skip if not typed
	    
	Type pre = instruction->get_precondition();
	if (pre[get_stack_num<Exec>()] != 0) continue; // skip if touches exec stack 
	
	Type newpost = postcondition - pre;
	    
	if (!newpost.is_positive()) continue; // skip if pops too much
	
	Type post = instruction->get_postcondition();
	
	if (post[get_stack_num<Exec>()] != 0) continue; // skip if touches exec stack
	
	newpost += post;

	// done, update arrays
	
	result.push_back(code);
	compiled.push_back(instruction);
	postcondition = newpost;
    }
    
    // Check if we need to delete superfluous instructions
    if (! (desired_condition == nullType) && precondition == nullType ) { // TODO: handle precondition
	// we will prune the instructions, it is however not guarenteed that we'll get exactly the same functionality
		
	Type delta = postcondition - desired_condition;

	if (! delta.is_positive() ) { // we have too few elements 
	    return Code( new CompiledCode);
	}
	
	CodeArray new_result;
	vector<TypedAtom*> new_compiled;
	
	Type desired = desired_condition;
	
	for (int i = result.size()-1; i >= 0; --i) {
	    
	    code = result[i];
	    TypedAtom* instruction = static_cast<TypedAtom*>(code.get());
	
	    Type new_desired = desired - instruction->get_postcondition();
	    
	    if (!new_desired.is_positive()) continue; // skip
	    
	    new_desired += instruction->get_precondition();
	    
	    if (new_desired.is_positive()) { 
		new_result.push_back(code);
		new_compiled.push_back(instruction);
		desired = new_desired;
	    } // else we skip the instruction
	}
	
	std::reverse(new_result.begin(), new_result.end());
	std::reverse(new_compiled.begin(), new_compiled.end());
	
	result = new_result;
	compiled = new_compiled;
	
    }
    
    // reverse code array
    std::reverse(result.begin(), result.end());
   

    return Code( new CompiledCode(precondition, postcondition, result, compiled));
    
}

Code clean(Code code, const Type& precondition, const Type& desired_condition) {
    Code res = compile(code, precondition, desired_condition);
    CompiledCode* cc = dynamic_cast<CompiledCode*>(code.get());
    if (cc == 0) return nil;
    return cc->get_code();
}

} // namespace push

