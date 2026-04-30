#include "dbgprint.h"
#include <print>
namespace dbg {
	void print(const std::string msg)
	{
#ifdef __VERBOSE__
		std::print("{}", msg);
#endif
	}
}

