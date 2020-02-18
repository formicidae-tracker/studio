#include "IdentifierUTest.hpp"

#include "IdentifierBridge.hpp"
#include "AntListWidget.hpp"
#include "ui_AntListWidget.h"

#include <myrmidon/TestSetup.hpp>
#include <myrmidon/priv/Identifier.hpp>

#include <QTest>
#include <QSignalSpy>

#include "Format.hpp"


void IdentifierUTest::SetUp() {
	ASSERT_NO_THROW({
			experiment = fmp::Experiment::NewFile(TestSetup::Basedir() / "identifierUTest.myrmidon");
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

TEST_F(IdentifierUTest,AntModificationTest) {
	ASSERT_NO_THROW({
			identifier->setExperiment(experiment);
		});

	QSignalSpy modified(identifier,SIGNAL(modified(bool)));
	QSignalSpy antCreated(identifier,SIGNAL(antCreated(fmp::Ant::ConstPtr)));
	QSignalSpy antDeleted(identifier,SIGNAL(antDeleted(quint32)));
	auto m = identifier->antModel();

	EXPECT_EQ(m->rowCount(),0);
	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),0);


	auto ant = identifier->createAnt();
	EXPECT_EQ(ant->ID(),1);
	EXPECT_TRUE(identifier->isModified());
	ASSERT_EQ(modified.count(),1);
	ASSERT_EQ(antCreated.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(antCreated.last().at(0).value<fmp::Ant::ConstPtr>(),
	          ant);
	ASSERT_EQ(m->rowCount(),1);
	EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),
	          std::string("0x0001 <no-tags>"));

	identifier->setExperiment(experiment);
	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),2);
	EXPECT_FALSE(modified.last().at(0).toBool());

	identifier->removeAnt(ant->ID());
	EXPECT_TRUE(identifier->isModified());
	EXPECT_EQ(modified.count(),3);
	EXPECT_TRUE(modified.last().at(0).toBool());
	ASSERT_EQ(antDeleted.count(),1);
	EXPECT_EQ(antDeleted.last().at(0).value<fm::Ant::ID>(),ant->ID());

}

TEST_F(IdentifierUTest,IdentificationModification) {
	fmp::Ant::ConstPtr ant;
	QSignalSpy modified(identifier,SIGNAL(modified(bool)));
	QSignalSpy identificationCreated(identifier,SIGNAL(identificationCreated(fmp::Identification::ConstPtr)));
	QSignalSpy identificationDeleted(identifier,SIGNAL(identificationDeleted(fmp::Identification::ConstPtr)));

	ASSERT_NO_THROW({
			ant = experiment->Identifier().CreateAnt();
			identifier->setExperiment(experiment);
		});
	ASSERT_FALSE(!ant);


	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),0);

	auto identification = identifier->addIdentification(ant->ID(),
	                                                    1,
	                                                    fm::Time::ConstPtr(),
	                                                    std::make_shared<fm::Time>(fm::Time::FromTimeT(1)));

	EXPECT_TRUE(identifier->isModified());
	ASSERT_EQ(modified.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	ASSERT_EQ(identificationCreated.count(),1);
	EXPECT_EQ(identificationCreated.last().at(0).value<fmp::Identification::ConstPtr>(),
	          identification);
	auto m = identifier->antModel();
	EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),
	          "0x0001 ↤ {1}");

	identification = identifier->addIdentification(ant->ID(),
	                                               2,
	                                               std::make_shared<fm::Time>(fm::Time::FromTimeT(2)),
	                                               fm::Time::ConstPtr());

	EXPECT_EQ(identificationCreated.count(),2);
	EXPECT_EQ(identificationCreated.last().at(0).value<fmp::Identification::ConstPtr>(),
	          identification);
	EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),
	          "0x0001 ↤ {1,2}");

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
	          "0x0001 ↤ {1}");

}


TEST_F(IdentifierUTest,DisplayStateModification) {
	fmp::Ant::ConstPtr ant;
	QSignalSpy modified(identifier,SIGNAL(modified(bool)));
	QSignalSpy displayChanged(identifier,SIGNAL(antDisplayChanged(quint32,fmp::Color,fmp::Ant::DisplayState)));

	ASSERT_NO_THROW({
			ant = experiment->Identifier().CreateAnt();
			identifier->setExperiment(experiment);
		});
	ASSERT_FALSE(!ant);

	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),0);
	auto m = static_cast<QStandardItemModel*>(identifier->antModel());
	auto hideItem = m->itemFromIndex(m->index(0,1));
	auto soloItem = m->itemFromIndex(m->index(0,2));

	ASSERT_FALSE(hideItem == NULL);
	ASSERT_FALSE(soloItem == NULL);


	struct TestData {
		std::function<void()> Action;
		fmp::Ant::DisplayState DisplayState;
		Qt::CheckState         Hide,Solo;
	};

	std::vector<TestData> testdata =
		{
		 {
		  [&]() { hideItem->setCheckState(Qt::Checked);},
		  fmp::Ant::DisplayState::HIDDEN,
		  Qt::Checked,Qt::Unchecked
		 },
		 {
		  [&]() { soloItem->setCheckState(Qt::Checked);},
		  fmp::Ant::DisplayState::SOLO,
		  Qt::Unchecked,Qt::Checked
		 },
		 {
		  [&]() { hideItem->setCheckState(Qt::Checked);},
		  fmp::Ant::DisplayState::HIDDEN,
		  Qt::Checked,Qt::Unchecked
		 },
		 {
		  [&]() { hideItem->setCheckState(Qt::Unchecked);},
		  fmp::Ant::DisplayState::VISIBLE,
		  Qt::Unchecked,Qt::Unchecked
		 },
		 {
		  [&]() { soloItem->setCheckState(Qt::Checked);},
		  fmp::Ant::DisplayState::SOLO,
		  Qt::Unchecked,Qt::Checked
		 },
		 {
		  [&]() { soloItem->setCheckState(Qt::Unchecked);},
		  fmp::Ant::DisplayState::VISIBLE,
		  Qt::Unchecked,Qt::Unchecked
		 },
		};

	size_t modifiedCount = 0;
	size_t signalCount = 0;
	for ( const auto & d : testdata ) {
		++modifiedCount;
		++signalCount;
		d.Action();
		EXPECT_TRUE(identifier->isModified());
		if (modifiedCount == 1) {
			ASSERT_EQ(modified.count(),modifiedCount);
		} else {
			EXPECT_EQ(modified.count(),modifiedCount);
		}
		EXPECT_TRUE(modified.last().at(0).toBool());

		if ( signalCount == 1 ) {
			ASSERT_EQ(displayChanged.count(),signalCount);
		} else {
			EXPECT_EQ(displayChanged.count(),signalCount);
		}
		EXPECT_EQ(ant->DisplayStatus(),d.DisplayState);
		EXPECT_EQ(displayChanged.last().at(0).toInt(),ant->ID());
		EXPECT_EQ(displayChanged.last().at(2).value<fmp::Ant::DisplayState>(),d.DisplayState);
		EXPECT_EQ(hideItem->checkState(),d.Hide);
		EXPECT_EQ(soloItem->checkState(),d.Solo);

		++modifiedCount;
		identifier->setExperiment(experiment);
		EXPECT_FALSE(identifier->isModified());
		EXPECT_EQ(modified.count(),modifiedCount);
		EXPECT_FALSE(modified.last().at(0).toBool());

		m = static_cast<QStandardItemModel*>(identifier->antModel());
		hideItem = m->itemFromIndex(m->index(0,1));
		soloItem = m->itemFromIndex(m->index(0,2));

		ASSERT_FALSE(hideItem == NULL);
		ASSERT_FALSE(soloItem == NULL);

	}

}

TEST_F(IdentifierUTest,AntListWidgetTest) {


}
