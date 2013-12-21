#include <VpiUtils.hpp>

#include <cassert>
#include <cstdlib>
#include <cstdarg>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace VpiUtils
{

VpiIterator::VpiIterator(PLI_INT32 type, vpiHandle ref) :
		iterator(NULL)
{
	iterator = vpi_iterate(type, ref);
	assert(iterator);
}

VpiIterator::~VpiIterator()
{
	if(iterator != NULL)
	{
		vpi_free_object(iterator);
	}
}

vpiHandle VpiIterator::Next()
{
	return vpi_scan(iterator);
}

vpiHandle findVerilogModule(vpiHandle handle, ...)
{
	va_list vlist;
	va_start(vlist, handle);

	const char* arg;
	while((arg = va_arg(vlist, const char *)))
	{
		handle = vpi_handle_by_name(arg, handle);
		if(!handle)
		{
			va_end(vlist);
			std::stringstream ss;
			ss << "Unable to find `" << arg << "'\n";
			throw std::runtime_error(ss.str());
		}
	}

	va_end(vlist);
	return handle;
}

ArgumentIterator::ArgumentIterator() :
	argValue(),
	argHandle(NULL),
	it(VpiIterator(vpiArgument, vpi_handle(vpiSysTfCall, NULL)))
{
}

bool ArgumentIterator::TryNext(PLI_INT32 format)
{
	argValue.format = format;
	argHandle = it.Next();
	if(argHandle != NULL)
	{
		vpi_get_value(argHandle, &argValue);
		return true;
	}

	return false;
}

const struct t_vpi_value& ArgumentIterator::Next(PLI_INT32 format)
{
	if(!TryNext(format))
	{
		throw std::runtime_error("Unable to get next argument");
	}
	return argValue;
}

const s_vpi_value& ArgumentIterator::GetValue() const
{
	return argValue;
}

} //namespace VpiUtils
