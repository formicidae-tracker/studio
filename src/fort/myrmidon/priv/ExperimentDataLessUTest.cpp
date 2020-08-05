#include "ExperimentDataLessUTest.hpp"

#include <fort/myrmidon/priv/Experiment.hpp>
#include <fort/myrmidon/TestSetup.hpp>



namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(ExperimentDataLessUTest,DataLessSupports) {

	try {
		Experiment::OpenDataLess(TestSetup::Basedir() / "test-0.1.myrmidon");
		ADD_FAILURE() << "No exception thrown when opening outdated myrmidon file";
	} catch ( const std::runtime_error & e ) {
		EXPECT_STREQ("Uncorrect myrmidon file version 0.1.0: data-less opening is only supported for myrmidon file version above 0.2.0",e.what());
	}

}


} // namespace priv
} // namespace myrmidon
} // namespace fort
