#include <VpiUtils.hpp>

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <VirtualIOExtension.hpp>
#include <SimControl.hpp>

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

struct function_thingy {
	function_thingy(boost::function<void ()> func, PLI_INT32 return_size) : func(func), return_size(return_size) {}

	boost::function<void ()> func;
	PLI_INT32 return_size;
};

static PLI_INT32 tfCallback(PLI_BYTE8* p)
{
	((function_thingy*)p)->func();
	return 0;
}

static PLI_INT32 tfSize(PLI_BYTE8* p)
{
	return ((function_thingy*)p)->return_size;
}

void registerSysTF(const std::string& name, const boost::function<void ()>& func, PLI_INT32 type, PLI_INT32 return_type, PLI_INT32 size)
{
	s_vpi_systf_data systf;
	systf.type = type;
	systf.sysfunctype = return_type;
	systf.compiletf = NULL;
	systf.sizetf    = tfSize;
	systf.user_data = reinterpret_cast<PLI_BYTE8*>(new function_thingy(func, size));
	systf.tfname    = name.c_str();
	systf.calltf    = tfCallback;
	vpi_register_systf(&systf);
}

void setFunctionReturnValue(s_vpi_value val)
{
	vpiHandle vpisystfcall = vpi_handle(vpiSysTfCall, NULL);

	vpi_put_value(vpisystfcall, &val, NULL, vpiNoDelay);
}

} //namespace VpiUtils

extern "C" {
	void (*vlog_startup_routines[])() = { VirtualIO_entry, SimControl_entry, 0 };
}
