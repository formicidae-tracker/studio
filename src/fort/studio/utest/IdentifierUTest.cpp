#include "IdentifierUTest.hpp"

#include <fort/studio/bridge/IdentifierBridge.hpp>
#include <fort/studio/widget/AntListWidget.hpp>

#include <fort/myrmidon/TestSetup.hpp>
#include <fort/myrmidon/priv/Identifier.hpp>
#include <fort/myrmidon/priv/AntShapeType.hpp>
#include <fort/myrmidon/priv/AntMetadata.hpp>

#include <QTest>
#include <QSignalSpy>
#include <QSortFilterProxyModel>

#include <fort/studio/Format.hpp>


void IdentifierUTest::SetUp() {
	ASSERT_NO_THROW({
			experiment = fmp::Experiment::Create(TestSetup::Basedir() / "identifierUTest.myrmidon");
			experiment->Save(TestSetup::Basedir() / "identifierUTest.myrmidon");
			identifier = new IdentifierBridge(NULL);
		});
}

void IdentifierUTest::TearDown() {
	delete identifier;
	experiment.reset();
}

TEST_F(IdentifierUTest,ActivationTest) {
	QSignalSpy activated(identifier,SIGNAL(activated(bool)));

	EXPECT_FALSE(identifier->isActive());
	EXPECT_EQ(activated.count(),0);

	identifier->setExperiment(experiment);
	EXPECT_TRUE(identifier->isActive());
	ASSERT_EQ(activated.count(),1);
	EXPECT_TRUE(activated.last().at(0).toBool());

	identifier->setExperiment(experiment);
	EXPECT_TRUE(identifier->isActive());
	EXPECT_EQ(activated.count(),2);
	EXPECT_TRUE(activated.last().at(0).toBool());

	identifier->setExperiment(fmp::Experiment::Ptr());
	EXPECT_FALSE(identifier->isActive());
	EXPECT_EQ(activated.count(),3);
	EXPECT_FALSE(activated.last().at(0).toBool());

}


TEST_F(IdentifierUTest,IdentificationModification) {
	fmp::Ant::ConstPtr ant;
	QSignalSpy modified(identifier,SIGNAL(modified(bool)));
	QSignalSpy identificationCreated(identifier,SIGNAL(identificationCreated(fmp::Identification::ConstPtr)));
	QSignalSpy identificationDeleted(identifier,SIGNAL(identificationDeleted(fmp::Identification::ConstPtr)));

	ASSERT_NO_THROW({
			ant = experiment->CreateAnt();
			identifier->setExperiment(experiment);
		});
	ASSERT_FALSE(!ant);


	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),0);

	auto identification = identifier->addIdentification(ant->AntID(),
	                                                    1,
	                                                    fort::Time::SinceEver(),
	                                                    fort::Time::FromTimeT(1));

	EXPECT_TRUE(identifier->isModified());
	ASSERT_EQ(modified.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	ASSERT_EQ(identificationCreated.count(),1);
	EXPECT_EQ(identificationCreated.last().at(0).value<fmp::Identification::ConstPtr>(),
	          identification);
	auto m = identifier->model();
	EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),
	          "0x001");
	EXPECT_EQ(ToStdString(m->data(m->index(0,1)).toString()),
	          "001");


	identification = identifier->addIdentification(ant->AntID(),
	                                               2,
	                                               fort::Time::FromTimeT(2),
	                                               fort::Time::Forever());

	EXPECT_EQ(identificationCreated.count(),2);
	EXPECT_EQ(identificationCreated.last().at(0).value<fmp::Identification::ConstPtr>(),
	          identification);
	EXPECT_EQ(m->rowCount(),2);
	EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),
	          "0x001");
	EXPECT_EQ(ToStdString(m->data(m->index(1,0)).toString()),
	          "0x002");

	EXPECT_EQ(ToStdString(m->data(m->index(0,1)).toString()),
	          "001");
	EXPECT_EQ(ToStdString(m->data(m->index(1,1)).toString()),
	          "001");


	EXPECT_EQ(modified.count(),1);

	identifier->setExperiment(experiment);
	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),2);

	identifier->deleteIdentification(identification);
	EXPECT_TRUE(identifier->isModified());
	EXPECT_EQ(modified.count(),3);
	EXPECT_TRUE(modified.last().at(0).toBool());
	ASSERT_EQ(identificationDeleted.count(),1);
	EXPECT_EQ(identificationDeleted.last().at(0).value<fmp::Identification::ConstPtr>(),
	          identification);
	EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),
	          "0x001");

	EXPECT_EQ(ToStdString(m->data(m->index(0,1)).toString()),
	          "001");
	EXPECT_EQ(m->rowCount(),1);

}
