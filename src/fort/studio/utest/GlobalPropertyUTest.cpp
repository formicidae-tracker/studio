#include "GlobalPropertyUTest.hpp"

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/studio/MyrmidonTypes/TrackingDataDirectory.hpp>

#include <fort/studio/bridge/ExperimentBridge.hpp>
#include <fort/studio/bridge/GlobalPropertyBridge.hpp>

#include <fort/studio/widget/GlobalPropertyWidget.hpp>


#include "ui_GlobalPropertyWidget.h"


#include <QSignalSpy>
#include <QTest>


TEST_F(GlobalPropertyUTest,SignalStateTest) {
	fmp::Experiment::Ptr experiment;
	GlobalPropertyBridge globalProperties(NULL);
	ASSERT_NO_THROW({
			experiment = fmp::priv::Experiment::NewFile(TestSetup::Basedir() / "globalProperty.myrmidon");
		});

	QSignalSpy activatedSignal(&globalProperties,SIGNAL(activated(bool)));
	QSignalSpy modifiedSignal(&globalProperties,SIGNAL(modified(bool)));
	QSignalSpy nameSignal(&globalProperties,SIGNAL(nameChanged(QString)));
	QSignalSpy authorSignal(&globalProperties,SIGNAL(authorChanged(QString)));
	QSignalSpy commentSignal(&globalProperties,SIGNAL(commentChanged(QString)));
	QSignalSpy tagFamilySignal(&globalProperties,SIGNAL(tagFamilyChanged(fort::tags::Family)));
	QSignalSpy tagSizeSignal(&globalProperties,SIGNAL(tagSizeChanged(double)));

	EXPECT_FALSE(globalProperties.isModified());
	EXPECT_FALSE(globalProperties.isActive());

	EXPECT_EQ(globalProperties.name(),"");
	EXPECT_EQ(globalProperties.author(),"");
	EXPECT_EQ(globalProperties.comment(),"");
	EXPECT_EQ(globalProperties.tagFamily(),fort::tags::Family::Undefined);
	EXPECT_EQ(globalProperties.tagSize(),0.0);

	globalProperties.setExperiment(experiment);

	ASSERT_EQ(nameSignal.count(),1);
	EXPECT_EQ(nameSignal.at(0).at(0).toString(),"");
	ASSERT_EQ(authorSignal.count(),1);
	EXPECT_EQ(authorSignal.at(0).at(0).toString(),"");
	ASSERT_EQ(commentSignal.count(),1);
	EXPECT_EQ(commentSignal.at(0).at(0).toString(),"");
	ASSERT_EQ(tagFamilySignal.count(),1);
	EXPECT_EQ(tagFamilySignal.at(0).at(0).value<fort::tags::Family>(),
	          fort::tags::Family::Undefined);
	ASSERT_EQ(tagSizeSignal.count(),1);
	EXPECT_EQ(tagSizeSignal.at(0).at(0).toDouble(),1.0);


	EXPECT_FALSE(globalProperties.isModified());
	EXPECT_EQ(modifiedSignal.count(),0);
	ASSERT_EQ(activatedSignal.count(),1);
	EXPECT_TRUE(activatedSignal.at(0).at(0).toBool());
	EXPECT_TRUE(globalProperties.isActive());

	globalProperties.setName("foo");
	EXPECT_TRUE(globalProperties.isModified());
	ASSERT_EQ(modifiedSignal.count(),1);
	EXPECT_TRUE(modifiedSignal.at(0).at(0).toBool());
	ASSERT_EQ(nameSignal.count(),2);
	EXPECT_EQ(nameSignal.at(1).at(0).toString(),"foo");

	globalProperties.setExperiment(experiment);
	EXPECT_FALSE(globalProperties.isModified());

	globalProperties.setAuthor("bar");
	EXPECT_TRUE(globalProperties.isModified());
	ASSERT_EQ(modifiedSignal.count(),3);
	EXPECT_TRUE(modifiedSignal.at(2).at(0).toBool());
	ASSERT_EQ(authorSignal.count(),3);
	EXPECT_EQ(authorSignal.last().at(0).toString(),"bar");

	globalProperties.setExperiment(experiment);
	EXPECT_FALSE(globalProperties.isModified());

	globalProperties.setComment("baz");
	EXPECT_TRUE(globalProperties.isModified());
	ASSERT_EQ(modifiedSignal.count(),5);
	EXPECT_TRUE(modifiedSignal.at(4).at(0).toBool());
	ASSERT_EQ(commentSignal.count(),4);
	EXPECT_EQ(commentSignal.last().at(0).toString(),"baz");

	globalProperties.setExperiment(experiment);
	EXPECT_FALSE(globalProperties.isModified());

	globalProperties.setTagSize(0.7);
	EXPECT_TRUE(globalProperties.isModified());
	ASSERT_EQ(modifiedSignal.count(),7);
	EXPECT_TRUE(modifiedSignal.at(6).at(0).toBool());
	ASSERT_EQ(tagSizeSignal.count(),5);
	EXPECT_EQ(tagSizeSignal.last().at(0).toDouble(),0.7);

	globalProperties.setExperiment(experiment);
	EXPECT_FALSE(globalProperties.isModified());

	ASSERT_EQ(tagFamilySignal.count(),5);
	ASSERT_NO_THROW({
			auto s = experiment->CreateSpace("foo");
			auto tdd = fmp::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir() );
			experiment->AddTrackingDataDirectory(s,tdd);
		});
	globalProperties.onTDDModified();
	EXPECT_EQ(tagFamilySignal.count(),6);
	EXPECT_EQ(tagFamilySignal.last().at(0).value<fort::tags::Family>(),
	          fort::tags::Family::Tag36h11);

}


TEST_F(GlobalPropertyUTest,WidgetTest) {
	fmp::Experiment::Ptr experiment;

	ExperimentBridge experimentBridge(NULL);
	GlobalPropertyWidget globalPropertiesWidget(NULL);
	auto globalProperties = experimentBridge.globalProperties();
	ASSERT_NO_THROW({
			experiment = fmp::priv::Experiment::Create(TestSetup::Basedir() / "globalProperty.myrmidon");
			experiment->Save(TestSetup::Basedir() / "globalProperty.myrmidon");
			globalPropertiesWidget.setup(&experimentBridge);
		});

	EXPECT_FALSE(globalPropertiesWidget.d_ui->nameEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->nameEdit->text(),"");

	EXPECT_FALSE(globalPropertiesWidget.d_ui->authorEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->authorEdit->text(),"");

	EXPECT_FALSE(globalPropertiesWidget.d_ui->commentEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->commentEdit->toPlainText(),"");

	EXPECT_FALSE(globalPropertiesWidget.d_ui->tagSizeEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->tagSizeEdit->value(),1.0);

	experiment->SetName("foo");
	experiment->SetAuthor("tests");
	experiment->SetComment("for tests");
	experiment->SetDefaultTagSize(0.67);

	globalProperties->setExperiment(experiment);

	EXPECT_TRUE(globalPropertiesWidget.d_ui->nameEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->nameEdit->text(),"foo");

	EXPECT_TRUE(globalPropertiesWidget.d_ui->authorEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->authorEdit->text(),"tests");

	EXPECT_TRUE(globalPropertiesWidget.d_ui->commentEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->commentEdit->toPlainText(),"for tests");

	EXPECT_TRUE(globalPropertiesWidget.d_ui->tagSizeEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->tagSizeEdit->value(),0.67);

	QTest::keyClicks(globalPropertiesWidget.d_ui->nameEdit,"bar");
	EXPECT_EQ(globalProperties->name(),"foobar");

	QTest::keyClicks(globalPropertiesWidget.d_ui->authorEdit,"b");
	EXPECT_EQ(globalProperties->author(),"testsb");

	QTest::keyClick(globalPropertiesWidget.d_ui->tagSizeEdit,Qt::Key_Delete);
	QTest::keyClicks(globalPropertiesWidget.d_ui->tagSizeEdit,"2");
	EXPECT_EQ(globalProperties->tagSize(),2.67);


	QTest::keyClicks(globalPropertiesWidget.d_ui->commentEdit,"This is ");
	EXPECT_EQ(std::string(globalProperties->comment().toUtf8().constData()),
	          std::string("This is for tests"));

	// no tracking data directory: its undefined
	EXPECT_EQ(globalProperties->tagFamily(),fort::tags::Family::Undefined);
	EXPECT_EQ(std::string(globalPropertiesWidget.d_ui->familyValueLabel->text().toUtf8().constData()),
	          "undefined");
	ASSERT_NO_THROW({
			auto s = experiment->CreateSpace("foo");
			auto tdd = fmp::TrackingDataDirectory::Open(TestSetup::Basedir() / "foo.0000",TestSetup::Basedir() );
			experiment->AddTrackingDataDirectory(s,tdd);
		});
	globalProperties->onTDDModified();
	EXPECT_EQ(std::string(globalPropertiesWidget.d_ui->familyValueLabel->text().toUtf8().constData()),
	          "36h11");
}
