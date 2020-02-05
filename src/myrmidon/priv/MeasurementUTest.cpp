#include "MeasurementUTest.hpp"

#include "Measurement.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(MeasurementUTest,CanDecomposeURI) {

	struct TestData {
		fs::path ParentURI,TDDURI;
		FrameID FID;
		TagID   TID;
	};

	std::vector<TestData> testdata =
		{
		 { "/foo/bar/baz/frames/234/closeups/345","/foo/bar/baz",
		   234,
		   345,
		 },
		};

	for (const auto & d : testdata) {
		Measurement m(d.ParentURI,
		              0,
		              Eigen::Vector2d(),
		              Eigen::Vector2d(),
		              1.0);
		fs::path tddURI;
		FrameID  FID;
		TagID    TID;

		EXPECT_NO_THROW(m.DecomposeURI(tddURI,FID,TID));

		EXPECT_EQ(tddURI.generic_string(),
		          d.TDDURI.generic_string());

		EXPECT_EQ(FID,
		          d.FID);

		EXPECT_EQ(TID,
		          d.TID);

	}
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
