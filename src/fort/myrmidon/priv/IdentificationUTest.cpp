#include "IdentificationUTest.hpp"

#include "Identification.hpp"
#include "AntShapeType.hpp"
#include "AntMetadata.hpp"

namespace fort {
namespace myrmidon {
namespace priv {

void IdentificationUTest::SetUp() {
	d_identifier = std::make_shared<Identifier>();

	d_shapeTypes = std::make_shared<AntShapeTypeContainer>();
	d_metadata = std::make_shared<AntMetadata>();

	d_ant = d_identifier->CreateAnt(d_shapeTypes,d_metadata);
	d_list.clear();

	Time t;
	//0
	d_list.push_back(Identification::Accessor::Create(0,d_identifier,d_ant));
	//1
	d_list.push_back(Identification::Accessor::Create(1,d_identifier,d_ant));
	//2
	d_list.push_back(Identification::Accessor::Create(2,d_identifier,d_ant));
	t = Time::FromTimeT(11);
	d_list.back()->d_end = std::make_shared<Time>(t);

	//3
	d_list.push_back(Identification::Accessor::Create(3,d_identifier,d_ant));
	t = Time::FromTimeT(11);
	d_list.back()->d_start = std::make_shared<Time>(t);
	//4
	d_list.push_back(Identification::Accessor::Create(4,d_identifier,d_ant));
	t = Time::FromTimeT(14);
	d_list.back()->d_start = std::make_shared<Time>(t);
	//5
	d_list.push_back(Identification::Accessor::Create(5,d_identifier,d_ant));
	t = Time::FromTimeT(11);
	d_list.back()->d_end = std::make_shared<Time>(t);
	//6
	d_list.push_back(Identification::Accessor::Create(6,d_identifier,d_ant));
	t = Time::FromTimeT(11);
	d_list.back()->d_start = std::make_shared<Time>(t);
	t = Time::FromTimeT(14);
	d_list.back()->d_end = std::make_shared<Time>(t);
	//7
	d_list.push_back(Identification::Accessor::Create(7,d_identifier,d_ant));
	t = Time::FromTimeT(14);
	d_list.back()->d_start = std::make_shared<Time>(t);
	//8
	t = Time::FromTimeT(14);
	d_list.push_back(Identification::Accessor::Create(8,d_identifier,d_ant));
	d_list.back()->d_end = std::make_shared<Time>(t);
	//9
	t = Time::FromTimeT(15);
	d_list.push_back(Identification::Accessor::Create(9,d_identifier,d_ant));
	d_list.back()->d_start = std::make_shared<Time>(t);
	t = Time::FromTimeT(17);
	d_list.back()->d_end = std::make_shared<Time>(t);

	//10
	d_list.push_back(Identification::Accessor::Create(10,d_identifier,d_ant));
	t = Time::FromTimeT(16);
	d_list.back()->d_start = std::make_shared<Time>(t);
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
		std::ostringstream os;
		os << "List:BEGIN" << std::endl;
		for(auto i = d.Start; i != d.End; ++i) {
			os << **i << std::endl;
		}
		os << "List:END" << std::endl;

		auto res = Identification::SortAndCheckOverlap(d.Start,d.End);


		EXPECT_EQ(res.first,d.Expected.first) << os.str();
		EXPECT_EQ(res.second,d.Expected.second) << os.str();
	}

}


TEST_F(IdentificationUTest,TestIdentificationBoundary) {
	auto identifier = std::make_shared<Identifier>();
	auto shapeTypes = std::make_shared<AntShapeTypeContainer>();
	auto metadata = std::make_shared<AntMetadata>();
	auto ant1 = identifier->CreateAnt(shapeTypes,metadata);
	auto ant2 = identifier->CreateAnt(shapeTypes,metadata);
	Identification::Ptr ant1ID1,ant2ID1,ant1ID2,ant2ID2;
	ASSERT_NO_THROW({ant1ID1 = Identifier::AddIdentification(identifier,ant1->AntID(),0,NULL,NULL);});
	// the two ant cannot share the same tag
	ASSERT_THROW({ant2ID1 = Identifier::AddIdentification(identifier,ant2->AntID(),0,NULL,NULL);},OverlappingIdentification);
	ASSERT_NO_THROW({ant2ID1 = Identifier::AddIdentification(identifier,ant2->AntID(),1,NULL,NULL);});
	// we can always reduce the validity of ID1
	auto swapTime = std::make_shared<Time>(Time::FromTimeT(0));

	ASSERT_NO_THROW({
			ant1ID1->SetEnd(swapTime);
		});
	// overlaps with ant1ID1
	ASSERT_THROW({
			ant1ID2 = Identifier::AddIdentification(identifier,
			                                        ant1->AntID(),
			                                        0,
			                                        Time::ConstPtr(),
			                                        Time::ConstPtr());
		},OverlappingIdentification);
	// overlaps with ant2ID1
	ASSERT_THROW({
			ant1ID2 = Identifier::AddIdentification(identifier,
			                                        ant1->AntID(),
			                                        1,
			                                        swapTime,
			                                        Time::ConstPtr());
		},OverlappingIdentification);
	ASSERT_NO_THROW({
			ant2ID1->SetEnd(swapTime);
		});

	//works to swap the two id after frame a/10
	ASSERT_NO_THROW({
			ant1ID2 = Identifier::AddIdentification(identifier,
			                                        ant1->AntID(),
			                                        1,
			                                        swapTime,
			                                        Time::ConstPtr());
		});
	ASSERT_NO_THROW({
			ant2ID2 = Identifier::AddIdentification(identifier,
			                                        ant2->AntID(),
			                                        0,
			                                        swapTime,
			                                        Time::ConstPtr());
		});

	std::ostringstream oss;
	oss << *ant2ID2;
	EXPECT_EQ(oss.str(),"Identification{ID:0x000 ↦ 2, From:'{Time:1970-01-01T00:00:00Z}', To:'+∞'}");

}



} // namespace priv
} // namespace myrmidon
} // namespace fort
