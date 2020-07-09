#include "MeasurementUTest.hpp"

#include "Measurement.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

TEST_F(MeasurementUTest,CanDecomposeURI) {

	struct TestData {
		std::string         ParentURI,TDDURI;
		priv::FrameID       FrameID;
		priv::TagID         TagID;
		MeasurementType::ID MTID;
	};

	//it should be able to extract both from hexadecimal on decimal to avoid opening issues
	std::vector<TestData> testdata =
		{
		 { "/foo/bar/baz/frames/234/closeups/0x159","/foo/bar/baz",
		   234,
		   345,
		   42,
		 },
		 { "/foo/bar/baz/frames/234/closeups/345","/foo/bar/baz",
		   234,
		   345,
		   42,
		 },
		};

	for (const auto & d : testdata) {
		Measurement m(d.ParentURI,
		              d.MTID,
		              Eigen::Vector2d(),
		              Eigen::Vector2d(),
		              1.0);
		try {
			auto [tddURI,frameID,tagID,mtID] = Measurement::DecomposeURI(m.URI());

			EXPECT_EQ(tddURI,
			          d.TDDURI);

			EXPECT_EQ(frameID,
			          d.FrameID);

			EXPECT_EQ(tagID,
			          d.TagID) << "When testing " << d.ParentURI;
		} catch (const std::exception & e ) {
			ADD_FAILURE() << "Unexpected exception: " << e.what();
		}

	}

	struct ErrorData {
		std::string URI,Reason;
	};

	std::vector<ErrorData> errordata =
		{
		 {"/measurements/?","cannot parse MeasurementType::ID"},
		 {"/measurement/32","no 'measurements' in URI"},
		 {"-/measurements/32","cannot parse TagID"},
		 {"/closeup/0x023/measurements/32","no 'closeups' in URI"},
		 {"a/closeups/0x023/measurements/32","cannot parse FrameID"},
		 {"frame/234568923312/closeups/0x023/measurements/32","no 'frames' in URI"},
		 {"frames/234568923312/closeups/35/measurements/32","no URI for TrackingDataDirectory"},
		 {"/frames/234568923312/closeups/0x023/measurements/32","no URI for TrackingDataDirectory"},
		};

	for (const auto & d :errordata) {
		try {
			auto [tddURI,frameID,tagID,mtID] = Measurement::DecomposeURI(d.URI);
			ADD_FAILURE() << "It throw nothing";
		} catch(const std::runtime_error & e) {
			EXPECT_EQ("Invalid URI '" + d.URI + "':" + d.Reason,
			          std::string(e.what()));
		} catch (...) {
			ADD_FAILURE() << "It throw something else";
		}
	}

}


} // namespace priv
} // namespace myrmidon
} // namespace fort
