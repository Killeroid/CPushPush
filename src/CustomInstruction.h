#ifndef _CUSTOM_INSTRUCTIONS_H__
#define _CUSTOM_INSTRUCTIONS_H__

#include "Instruction.h"
#include "Env.h"

using namespace push;

typedef unsigned (*CustomOperator)(Env&, void*);

namespace push {

class CustomInstruction : public Instruction {
	public:

        CustomInstruction(CustomOperator op, std::string name, void *callbackData):Instruction(NULL, name) {
            callbackOp_ = op; callbackData_ = callbackData;
        }

        ~CustomInstruction() {
            // if they provide a callback to free the callback data
            if(callbackData_ && freeCallbackData_) freeCallbackData_(callbackData_);	
        }

        unsigned operator()(Env& env) const {
            callbackOp_(env, callbackData_); return 1;
        }

        void setDataFreeCallback(void (*c)(void *d)) {
            freeCallbackData_ = c;
        }

	protected:
        void *callbackData_;
        CustomOperator callbackOp_;
        void (*freeCallbackData_)(void *data);
};

inline
Code make_custom_instruction(CustomOperator op, std::string name, void *callbackData) {
    return static_initializer.register_pushfunc( Code(new CustomInstruction(op, name, callbackData) ) );
}

}

#endif // _CUSTOM_INSTRUCTIONS_H__
