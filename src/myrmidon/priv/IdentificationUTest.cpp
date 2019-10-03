#include "IdentificationUTest.hpp"

#include "Identification.hpp"

using namespace fort::myrmidon::priv;

void IdentificationUTest::SetUp() {
	d_identifier = Identifier::Create();
	d_ant = d_identifier->CreateAnt();
	d_list.clear();
	//0
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	//1
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	//2
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	d_list.back()->d_end = std::make_shared<FramePointer>(FramePointer{
			.Frame = 11
		});
	//3
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	d_list.back()->d_start = std::make_shared<FramePointer>(FramePointer{
			.Frame = 12
		});
	//4
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	d_list.back()->d_start = std::make_shared<FramePointer>(FramePointer{
			.Frame = 14
		});
	//5
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	d_list.back()->d_end = std::make_shared<FramePointer>(FramePointer{
			.Frame = 11
		});
	//6
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	d_list.back()->d_start = std::make_shared<FramePointer>(FramePointer{
			.Frame = 12
		});
	d_list.back()->d_end = std::make_shared<FramePointer>(FramePointer{
			.Frame = 14
		});
	//7
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	d_list.back()->d_start = std::make_shared<FramePointer>(FramePointer{
			.Frame = 15
		});
	//8
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	d_list.back()->d_end = std::make_shared<FramePointer>(FramePointer{
			.Frame = 14
		});
	//9
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	d_list.back()->d_start = std::make_shared<FramePointer>(FramePointer{
			.Frame = 15
		});
	d_list.back()->d_end = std::make_shared<FramePointer>(FramePointer{
			.Frame = 17
		});

	//10
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	d_list.back()->d_start = std::make_shared<FramePointer>(FramePointer{
			.Frame = 16
		});
}



TEST_F(IdentificationUTest,CanCheckOverlaps) {
	struct TestData {
		Identification::List::iterator Start,End;
		std::pair<Identification::List::const_iterator,Identification::List::const_iterator> Expected;
	};

	std::vector<TestData> data
		= {
		   TestData{.Start=Identification::List::iterator(),
		            .End=Identification::List::iterator(),
		            .Expected=std::make_pair(Identification::List::const_iterator(),
		                                     Identification::List::const_iterator()),
		   },
		   TestData{.Start=d_list.begin(),
		            .End=d_list.begin()+1,
		            .Expected=std::make_pair(Identification::List::const_iterator(),
		                                     Identification::List::const_iterator()),
		   },
		   TestData{.Start=d_list.begin(),
		            .End=d_list.begin()+2,
		            .Expected=std::make_pair(d_list.begin(),d_list.begin()+1),
		   },
		   TestData{.Start=d_list.begin()+2,
		            .End=d_list.begin()+4,
		            .Expected=std::make_pair(Identification::List::const_iterator(),
		                                     Identification::List::const_iterator()),
		   },
		   TestData{.Start=d_list.begin()+2,
		            .End=d_list.begin()+5,
		            .Expected=std::make_pair(d_list.begin()+3,d_list.begin()+4),
		   },
		   TestData{.Start=d_list.begin()+5,
		            .End=d_list.begin()+8,
		            .Expected=std::make_pair(Identification::List::const_iterator(),
		                                     Identification::List::const_iterator()),
		   },
		   TestData{.Start=d_list.begin()+8,
		            .End=d_list.begin()+11,
		            .Expected=std::make_pair(d_list.begin()+9,d_list.begin()+10),
		   },


	};

	for(auto & d: data) {
		auto res = Identification::SortAndCheckOverlap(d.Start,d.End);
		EXPECT_EQ(res.first,d.Expected.first);
		EXPECT_EQ(res.second,d.Expected.second);
	}


}
