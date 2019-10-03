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


TEST_F(IdentificationUTest,TestIdentificationBoundary) {
	auto identifier = Identifier::Create();
	auto ant1 = identifier->CreateAnt();
	auto ant2 = identifier->CreateAnt();
	Identification::Ptr ant1ID1,ant2ID1,ant1ID2,ant2ID2;
	ASSERT_NO_THROW({ant1ID1 = identifier->AddIdentification(ant1->ID(),0,NULL,NULL);});
	// the two ant cannot share the same tag
	ASSERT_THROW({ant2ID1 = identifier->AddIdentification(ant2->ID(),0,NULL,NULL);},OverlappingIdentification);
	ASSERT_NO_THROW({ant2ID1 = identifier->AddIdentification(ant2->ID(),1,NULL,NULL);});
	// we can always reduce the validity of ID1
	ASSERT_NO_THROW({
			auto ant1ID1end = std::make_shared<FramePointer>();
			ant1ID1end->Path = "a";
			ant1ID1end->Frame = 10;
			ant1ID1->SetEnd(ant1ID1end);
		});
	auto ant1ID2start = std::make_shared<FramePointer>();
	ant1ID2start->Path = "a";
	ant1ID2start->Frame = 10;
	// overlaps with ant1ID1
	ASSERT_THROW({ant1ID2 = identifier->AddIdentification(ant1->ID(),0,NULL,NULL);},OverlappingIdentification);
	// still overlaps with ant1ID1 as end == start
	ASSERT_THROW({ant1ID2 = identifier->AddIdentification(ant1->ID(),0,ant1ID2start,NULL);},OverlappingIdentification);
	ant1ID2start->Frame = 11;
	// overlaps with ant2ID1
	ASSERT_THROW({ant1ID2 = identifier->AddIdentification(ant1->ID(),1,ant1ID2start,NULL);},OverlappingIdentification);
	ASSERT_NO_THROW({
			auto ant2ID1end = std::make_shared<FramePointer>();
			ant2ID1end->Path = "a";
			ant2ID1end->Frame = 10;
			ant2ID1->SetEnd(ant2ID1end);
		});
	ant1ID2start->Frame = 10;
	// overlaps with ant2ID1 as end == start
	ASSERT_THROW({ant1ID2 = identifier->AddIdentification(ant2->ID(),1,ant1ID2start,NULL);},OverlappingIdentification);
	ant1ID2start->Frame = 11;

	//works to swap the two id after frame a/10
	ASSERT_NO_THROW({ant1ID2 = identifier->AddIdentification(ant1->ID(),1,ant1ID2start,NULL);});
	ASSERT_NO_THROW({ant2ID2 = identifier->AddIdentification(ant2->ID(),0,ant1ID2start,NULL);});

}
