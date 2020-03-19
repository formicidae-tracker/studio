#include "MetadataColumnUTest.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

void MetadataColumnUTest::SetUp() {
	manager =  std::make_shared<MetadataColumn::Manager>();
}
void MetadataColumnUTest::TearDown() {
	manager.reset();
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
