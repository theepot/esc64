#pragma once

#include <boost/function.hpp>

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

	s_vpi_vecval* NextVec()	{ return Next(vpiVectorVal).value.vector; }
	PLI_INT32 NextInt()		{ return Next(vpiIntVal).value.integer; }

};

void registerSysTF(const std::string& name, const boost::function<void ()>& func, PLI_INT32 type, PLI_INT32 return_type = 0, PLI_INT32 size = 1);
void setFunctionReturnValue(s_vpi_value val);

std::ostream& operator << (std::ostream &s, const s_vpi_vecval& rhs);

} //namespace VpiUtils
