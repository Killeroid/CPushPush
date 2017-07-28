#ifndef TYPED_ATOM_H
#define TYPED_ATOM_H

#include "Code.h"
#include "Type.h"

namespace push {
    
class TypedAtom : public CodeAtom {
    
    public:

	virtual const Type& get_precondition() const = 0;
	virtual const Type& get_postcondition() const = 0;
    
};


} // namespace push
#endif
