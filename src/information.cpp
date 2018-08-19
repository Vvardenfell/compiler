#include "information.h"
#include "exception.h"


std::ostream& operator<<(std::ostream& out, FundamentalType type) {
	switch(type) {
	case FundamentalType::NONE: return out.write("NONE", 4);
	case FundamentalType::ERROR: return out.write("ERROR", 5);
	case FundamentalType::INT: return out.write("INT", 3);
	case FundamentalType::ARRAY: return out.write("ARRAY", 5);
	case FundamentalType::INT_ARRAY: return out.write("INT_ARRAY", 9);
	default: throw UnsupportedFundamentalTypeException("operator<<(std::ostream& out, FundamentalType type)", type);
	}
}
