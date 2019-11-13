#include "NotYetImplementedUTest.hpp"

#include "NotYetImplemented.hpp"

namespace foo {
namespace bar {

bool baz(int v) {
	throw MYRMIDON_NOT_YET_IMPLEMENTED();
}

} // namespace foo
} // namespace bar



TEST_F(NotYetImplementedUTest,FormatsPrettyException) {
	try {
		foo::bar::baz(123);
	} catch (const NotYetImplemented & e){
		EXPECT_EQ(std::string(e.what()),std::string("bool foo::bar::baz(int) is not yet implemented"));
		return;
	}
	ADD_FAILURE() << "Should have thrown NotYetImplemented exception";
}
