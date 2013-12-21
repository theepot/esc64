#pragma once

extern "C"
{
#include <vpi_user.h>
}

namespace VpiUtils
{

/**
 * For iterating over various verilog resources like modules and task arguments
 */
class VpiIterator
{
private:
	vpiHandle iterator;

public:
	VpiIterator(PLI_INT32 type, vpiHandle ref);
	~VpiIterator();
	vpiHandle Next();
};

vpiHandle findVerilogModule(vpiHandle handle, ...);

/**
 * For iterating over task arguments
 */
class ArgumentIterator
{
private:
	struct t_vpi_value argValue;
	vpiHandle argHandle;
	VpiIterator it;

public:
	ArgumentIterator();
	bool TryNext(PLI_INT32 format);
	const s_vpi_value& Next(PLI_INT32 format);
	const s_vpi_value& GetValue() const;

	p_vpi_vecval NextVec()	{ return Next(vpiVectorVal).value.vector; }
	PLI_INT32 NextInt()		{ return Next(vpiIntVal).value.integer; }
};

} //namespace VpiUtils
