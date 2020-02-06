#include "FileReadWriterUTest.hpp"


#include <myrmidon/ExperimentFile.pb.h>
#include <myrmidon/priv/proto/FileReadWriter.hpp>
#include <myrmidon/TestSetup.hpp>
namespace fort {
namespace myrmidon {
namespace priv {
namespace proto {

TEST_F(FileReadWriterUTest,TestBadIO) {
	typedef FileReadWriter<pb::FileHeader,pb::FileLine> RW;

	pb::FileHeader h;

	EXPECT_THROW({
			RW::Write(TestSetup::Basedir() / "does-no-exist-dir"/ "foo.myrmidon",
			          h,{
			          });
		},std::runtime_error);

	EXPECT_THROW({
			RW::Read(TestSetup::Basedir() / "does-no-exist-dir"/ "foo.myrmidon",
			         [](const pb::FileHeader & h) {
			         },
			         [](const pb::FileLine & line) {
			         });
		},std::runtime_error);

}

} //namespace proto
} //namespace priv
} //namespace myrmidon
} //namespace fort
