#include "IdentifierUTest.hpp"

#include <fort/studio/bridge/IdentifierBridge.hpp>
#include <fort/studio/bridge/SelectedAntBridge.hpp>
#include <fort/studio/widget/AntListWidget.hpp>
#include "ui_AntListWidget.h"

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
	EXPECT_EQ(ant->AntID(),1);
	EXPECT_TRUE(identifier->isModified());
	ASSERT_EQ(modified.count(),1);
	ASSERT_EQ(antCreated.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(antCreated.last().at(0).value<fmp::Ant::ConstPtr>(),
	          ant);
	ASSERT_EQ(m->rowCount(),1);
	EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),
	          std::string("001 <no-tags>"));

	identifier->setExperiment(experiment);
	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),2);
	EXPECT_FALSE(modified.last().at(0).toBool());

	identifier->deleteAnt(ant->AntID());
	EXPECT_TRUE(identifier->isModified());
	EXPECT_EQ(modified.count(),3);
	EXPECT_TRUE(modified.last().at(0).toBool());
	ASSERT_EQ(antDeleted.count(),1);
	EXPECT_EQ(antDeleted.last().at(0).value<fm::Ant::ID>(),ant->AntID());

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
	          "001 ↤ {1}");

	identification = identifier->addIdentification(ant->AntID(),
	                                               2,
	                                               std::make_shared<fm::Time>(fm::Time::FromTimeT(2)),
	                                               fm::Time::ConstPtr());

	EXPECT_EQ(identificationCreated.count(),2);
	EXPECT_EQ(identificationCreated.last().at(0).value<fmp::Identification::ConstPtr>(),
	          identification);
	EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),
	          "001 ↤ {1,2}");

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
	          "001 ↤ {1}");

}


TEST_F(IdentifierUTest,DisplayStateModification) {
	fmp::Ant::ConstPtr ant;
	QSignalSpy modified(identifier,SIGNAL(modified(bool)));
	QSignalSpy displayChanged(identifier,SIGNAL(antDisplayChanged(quint32,fm::Color,fmp::Ant::DisplayState)));
	QSignalSpy hiddenChanged(identifier,SIGNAL(numberHiddenAntChanged(quint32)));
	QSignalSpy soloChanged(identifier,SIGNAL(numberSoloAntChanged(quint32)));
	ASSERT_NO_THROW({
			ant = experiment->CreateAnt();
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
		quint32                NHidden,NSolo;
		bool                   HiddenChanged,SoloChanged;
	};

	std::vector<TestData> testdata =
		{
		 {
		  [&]() { hideItem->setCheckState(Qt::Checked);},
		  fmp::Ant::DisplayState::HIDDEN,
		  Qt::Checked,Qt::Unchecked,
		  1,0,
		  true,false,
		 },
		 {
		  [&]() { soloItem->setCheckState(Qt::Checked);},
		  fmp::Ant::DisplayState::SOLO,
		  Qt::Unchecked,Qt::Checked,
		  0,1,
		  true,true,
		 },
		 {
		  [&]() { hideItem->setCheckState(Qt::Checked);},
		  fmp::Ant::DisplayState::HIDDEN,
		  Qt::Checked,Qt::Unchecked,
		  1,0,
		  true,true,
		 },
		 {
		  [&]() { hideItem->setCheckState(Qt::Unchecked);},
		  fmp::Ant::DisplayState::VISIBLE,
		  Qt::Unchecked,Qt::Unchecked,
		  0,0,
		  true,false,
		 },
		 {
		  [&]() { soloItem->setCheckState(Qt::Checked);},
		  fmp::Ant::DisplayState::SOLO,
		  Qt::Unchecked,Qt::Checked,
		  0,1,
		  false,true,
		 },
		 {
		  [&]() { soloItem->setCheckState(Qt::Unchecked);},
		  fmp::Ant::DisplayState::VISIBLE,
		  Qt::Unchecked,Qt::Unchecked,
		  0,0,
		  false,true,
		 },
		};

	size_t modifiedCount = 0;
	size_t signalCount = 0;
	size_t hiddenCount = 1;
	size_t soloCount = 1;
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


		if ( d.HiddenChanged == true ) {
			++hiddenCount;
		}
		if ( d.SoloChanged == true ) {
			++soloCount;
		}

		EXPECT_EQ(hiddenChanged.count(),hiddenCount);
		if ( hiddenChanged.count() > 0 ) {
			EXPECT_EQ(hiddenChanged.last().at(0).toInt(),d.NHidden);
		}
		EXPECT_EQ(identifier->numberHiddenAnt(),d.NHidden);

		EXPECT_EQ(soloChanged.count(),soloCount);
		if ( soloChanged.count() > 0 ) {
			EXPECT_EQ(soloChanged.last().at(0).toInt(),d.NSolo);
		}
		EXPECT_EQ(identifier->numberSoloAnt(),d.NSolo);

		EXPECT_EQ(ant->DisplayStatus(),d.DisplayState);
		EXPECT_EQ(displayChanged.last().at(0).toInt(),ant->AntID());
		EXPECT_EQ(displayChanged.last().at(2).value<fmp::Ant::DisplayState>(),d.DisplayState);
		EXPECT_EQ(hideItem->checkState(),d.Hide);
		EXPECT_EQ(soloItem->checkState(),d.Solo);

		++modifiedCount;
		identifier->setExperiment(experiment);
		++hiddenCount;
		++soloCount;
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


TEST_F(IdentifierUTest,DisplayColorModification) {
	fmp::Ant::ConstPtr ant;
	QSignalSpy modified(identifier,SIGNAL(modified(bool)));
	QSignalSpy displayChanged(identifier,SIGNAL(antDisplayChanged(quint32,fm::Color,fmp::Ant::DisplayState)));

	ASSERT_NO_THROW({
			ant = experiment->CreateAnt();
			identifier->setExperiment(experiment);
		});
	ASSERT_FALSE(!ant);

	QColor newColor(12,34,56);
	fm::Color newColorFmp({12,34,56});

	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),0);

	auto m = identifier->antModel();
	auto selector = QItemSelectionModel(m);
	//invalid index
	identifier->setAntDisplayColor(selector.selection(),newColor);
	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),0);
	EXPECT_EQ(displayChanged.count(),0);
	// invalid index
	selector.select(m->index(0,1),QItemSelectionModel::Select);
	identifier->setAntDisplayColor(selector.selection(),newColor);
	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),0);
	EXPECT_EQ(displayChanged.count(),0);

	// invalid color
	selector.select(m->index(0,1),QItemSelectionModel::Clear);
	selector.select(m->index(0,0),QItemSelectionModel::Select);
	identifier->setAntDisplayColor(selector.selection(),QColor());
	EXPECT_FALSE(identifier->isModified());
	EXPECT_EQ(modified.count(),0);
	EXPECT_EQ(displayChanged.count(),0);


	identifier->setAntDisplayColor(selector.selection(),newColor);
	EXPECT_TRUE(identifier->isModified());
	ASSERT_EQ(modified.count(),1);
	ASSERT_EQ(displayChanged.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(displayChanged.last().at(0).toInt(),ant->AntID());
	EXPECT_EQ(displayChanged.last().at(1).value<fm::Color>(),
	          newColorFmp);
	EXPECT_EQ(ant->DisplayColor(),
	          newColorFmp);

}

TEST_F(IdentifierUTest,AntSelection) {
	fmp::Ant::ConstPtr ant[3];
	QSignalSpy antSelected(identifier->selectedAnt(),SIGNAL(activated(bool)));
	ASSERT_NO_THROW({
			ant[0] = experiment->CreateAnt();
			ant[1] = experiment->CreateAnt();
			ant[2] = experiment->CreateAnt();
			identifier->setExperiment(experiment);
		});
	ASSERT_FALSE(!ant[0]);
	ASSERT_FALSE(!ant[1]);
	ASSERT_FALSE(!ant[2]);

	EXPECT_EQ(antSelected.count(),1);

	identifier->setExperiment(experiment);

	EXPECT_EQ(antSelected.count(),2);
	auto m = identifier->antModel();

	identifier->selectAnt(m->index(0,1));
	EXPECT_EQ(antSelected.count(),2);
	identifier->selectAnt(m->index(0,2));
	EXPECT_EQ(antSelected.count(),2);

	identifier->selectAnt(m->index(0,0));
	ASSERT_EQ(antSelected.count(),3);
	EXPECT_EQ(antSelected.last().at(0).toBool(),
	          true);
	EXPECT_EQ(identifier->selectedAnt()->selectedID(),ant[0]->AntID());

}



TEST_F(IdentifierUTest,AntListWidgetTest) {
	AntListWidget widget(NULL);
	widget.setup(identifier);
	auto ui = widget.d_ui;

	QSignalSpy antCreated(identifier,SIGNAL(antCreated(fmp::Ant::ConstPtr)));
	QSignalSpy antDeleted(identifier,SIGNAL(antDeleted(quint32)));
	QSignalSpy displayChanged(identifier,SIGNAL(antDisplayChanged(quint32,fm::Color,fmp::Ant::DisplayState)));
	QSignalSpy antSelected(identifier,SIGNAL(antSelected(fmp::Ant::Ptr)));


	EXPECT_FALSE(ui->addButton->isEnabled());
	EXPECT_FALSE(ui->deleteButton->isEnabled());
	EXPECT_FALSE(ui->colorBox->isEnabled());
	EXPECT_FALSE(ui->filterEdit->isEnabled());

	EXPECT_EQ(ToStdString(ui->antLabel->text()),
	          "Number: 0");

	identifier->setExperiment(experiment);

	EXPECT_TRUE(ui->addButton->isEnabled());
	EXPECT_FALSE(ui->deleteButton->isEnabled());
	EXPECT_FALSE(ui->colorBox->isEnabled());
	EXPECT_TRUE(ui->filterEdit->isEnabled());

	auto s = ui->tableView->selectionModel();
	auto m = widget.d_sortedModel;

	EXPECT_EQ(ToStdString(ui->antLabel->text()),
	          "Number: 0");

	for ( size_t i = 0; i < 10; ++i ) {
		QTest::mouseClick(ui->addButton,Qt::LeftButton);
		if ( i == 0 ) {
			ASSERT_EQ(antCreated.count(),i+1);
		} else {
			EXPECT_EQ(antCreated.count(),i+1);
		}
		auto ant = antCreated.last().at(0).value<fmp::Ant::ConstPtr>();
		EXPECT_EQ(ant->AntID(),i+1);
		EXPECT_EQ(ant->DisplayColor(),fmp::Palette::Default().At(0));
		ASSERT_EQ(m->rowCount(),i+1);
		EXPECT_EQ(ToStdString(m->data(m->index(i,0)).toString()),
		          fmp::Ant::FormatID(i+1) + " <no-tags>");
		EXPECT_EQ(ToStdString(ui->antLabel->text()),
		          "Number: " + std::to_string(i+1));


	}

	ui->filterEdit->setText("1|A");
	EXPECT_EQ(m->rowCount(),2);
	if ( m->rowCount() >=2 ) {
		EXPECT_EQ(ToStdString(m->data(m->index(0,0)).toString()),
		          "001 <no-tags>");
		EXPECT_EQ(ToStdString(m->data(m->index(1,0)).toString()),
		          "010 <no-tags>");
	}
	EXPECT_EQ(ToStdString(ui->antLabel->text()),
	          "Number: 10");

	ui->filterEdit->setText("");
	EXPECT_EQ(m->rowCount(),10);


	auto selectRow = [s,m](int i) {
		                 for ( size_t ii = 0; ii < 4; ++ii ) {
			                 s->select(m->index(i,ii),QItemSelectionModel::Select);
		                 }
	                 };
	s->clear();
	selectRow(3);
	selectRow(5);
	EXPECT_TRUE(ui->deleteButton->isEnabled());
	QTest::mouseClick(ui->deleteButton,Qt::LeftButton);
	ASSERT_EQ(antDeleted.count(),2);
	EXPECT_EQ(antDeleted.at(0).at(0).toInt(),4);
	EXPECT_EQ(antDeleted.at(1).at(0).toInt(),6);
	EXPECT_EQ(ToStdString(ui->antLabel->text()),
	          "Number: 8");


	EXPECT_FALSE(ui->colorBox->isEnabled());

	selectRow(0);
	selectRow(1);
	EXPECT_TRUE(ui->colorBox->isEnabled());
	ui->colorBox->setCurrentIndex(4);
	ASSERT_EQ(displayChanged.count(),2);
	EXPECT_EQ(displayChanged.at(0).at(0).toInt(),1);
	EXPECT_EQ(displayChanged.at(0).at(1).value<fm::Color>(),
	          fmp::Palette::Default().At(2));
	EXPECT_EQ(displayChanged.at(1).at(0).toInt(),2);
	EXPECT_EQ(displayChanged.at(1).at(1).value<fm::Color>(),
	          fmp::Palette::Default().At(2));


	auto sm = static_cast<QStandardItemModel*>(identifier->antModel());
	QStandardItem * hide[2] =
		{
		 sm->itemFromIndex(sm->index(0,1)),sm->itemFromIndex(sm->index(1,1)),
		};
	QStandardItem * solo[2] =
		{
		 sm->itemFromIndex(sm->index(0,2)),sm->itemFromIndex(sm->index(1,2)),
		};

	hide[0]->setCheckState(Qt::Checked);
	EXPECT_TRUE(ui->showAllButton->isEnabled());
	EXPECT_FALSE(ui->unsoloAllButton->isEnabled());
	QTest::mouseClick(ui->showAllButton,Qt::LeftButton);
	EXPECT_FALSE(ui->showAllButton->isEnabled());
	EXPECT_FALSE(ui->unsoloAllButton->isEnabled());
	EXPECT_EQ(hide[0]->checkState(),Qt::Unchecked);


	solo[1]->setCheckState(Qt::Checked);
	hide[0]->setCheckState(Qt::Checked);
	EXPECT_TRUE(ui->showAllButton->isEnabled());
	EXPECT_TRUE(ui->unsoloAllButton->isEnabled());
	QTest::mouseClick(ui->unsoloAllButton,Qt::LeftButton);
	EXPECT_TRUE(ui->showAllButton->isEnabled());
	EXPECT_FALSE(ui->unsoloAllButton->isEnabled());
	EXPECT_EQ(hide[0]->checkState(),Qt::Checked);
	EXPECT_EQ(solo[1]->checkState(),Qt::Unchecked);

	solo[1]->setCheckState(Qt::Checked);
	EXPECT_TRUE(ui->showAllButton->isEnabled());
	EXPECT_TRUE(ui->unsoloAllButton->isEnabled());
	QTest::mouseClick(ui->showAllButton,Qt::LeftButton);
	EXPECT_FALSE(ui->showAllButton->isEnabled());
	EXPECT_FALSE(ui->unsoloAllButton->isEnabled());
	EXPECT_EQ(hide[0]->checkState(),Qt::Unchecked);
	EXPECT_EQ(hide[1]->checkState(),Qt::Unchecked);
	EXPECT_EQ(solo[0]->checkState(),Qt::Unchecked);
	EXPECT_EQ(solo[1]->checkState(),Qt::Unchecked);

}
