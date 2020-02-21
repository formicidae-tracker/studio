#include "GlobalPropertyUTest.hpp"

#include <myrmidon/TestSetup.hpp>

#include <fort-studio/bridge/GlobalPropertyBridge.hpp>
#include <fort-studio/widget/GlobalPropertyWidget.hpp>
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
	QSignalSpy thresholdSignal(&globalProperties,SIGNAL(thresholdChanged(int)));
	QSignalSpy tagSizeSignal(&globalProperties,SIGNAL(tagSizeChanged(double)));

	EXPECT_FALSE(globalProperties.isModified());
	EXPECT_FALSE(globalProperties.isActive());

	EXPECT_EQ(globalProperties.name(),"");
	EXPECT_EQ(globalProperties.author(),"");
	EXPECT_EQ(globalProperties.comment(),"");
	EXPECT_EQ(globalProperties.tagFamily(),fort::tags::Family::Undefined);
	EXPECT_EQ(globalProperties.tagSize(),0.0);
	EXPECT_EQ(globalProperties.threshold(),255);

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
	ASSERT_EQ(thresholdSignal.count(),1);
	EXPECT_EQ(thresholdSignal.at(0).at(0).toInt(),experiment->Threshold());
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
	ASSERT_EQ(authorSignal.count(),2);
	EXPECT_EQ(authorSignal.at(1).at(0).toString(),"bar");

	globalProperties.setExperiment(experiment);
	EXPECT_FALSE(globalProperties.isModified());

	globalProperties.setComment("baz");
	EXPECT_TRUE(globalProperties.isModified());
	ASSERT_EQ(modifiedSignal.count(),5);
	EXPECT_TRUE(modifiedSignal.at(4).at(0).toBool());
	ASSERT_EQ(commentSignal.count(),2);
	EXPECT_EQ(commentSignal.at(1).at(0).toString(),"baz");

	globalProperties.setExperiment(experiment);
	EXPECT_FALSE(globalProperties.isModified());

	globalProperties.setTagFamily(fort::tags::Family::Tag36h11);
	EXPECT_TRUE(globalProperties.isModified());
	ASSERT_EQ(modifiedSignal.count(),7);
	EXPECT_TRUE(modifiedSignal.at(6).at(0).toBool());
	ASSERT_EQ(tagFamilySignal.count(),2);
	EXPECT_EQ(tagFamilySignal.at(1).at(0).value<fort::tags::Family>(),fort::tags::Family::Tag36h11);

	globalProperties.setExperiment(experiment);
	EXPECT_FALSE(globalProperties.isModified());

	globalProperties.setTagSize(0.7);
	EXPECT_TRUE(globalProperties.isModified());
	ASSERT_EQ(modifiedSignal.count(),9);
	EXPECT_TRUE(modifiedSignal.at(8).at(0).toBool());
	ASSERT_EQ(tagSizeSignal.count(),2);
	EXPECT_EQ(tagSizeSignal.at(1).at(0).toDouble(),0.7);

	globalProperties.setExperiment(experiment);
	EXPECT_FALSE(globalProperties.isModified());

	globalProperties.setThreshold(255);
	EXPECT_TRUE(globalProperties.isModified());
	ASSERT_EQ(modifiedSignal.count(),11);
	EXPECT_TRUE(modifiedSignal.at(10).at(0).toBool());
	ASSERT_EQ(thresholdSignal.count(),2);
	EXPECT_EQ(thresholdSignal.at(1).at(0).toInt(),254);

}


TEST_F(GlobalPropertyUTest,WidgetTest) {
	fmp::Experiment::Ptr experiment;
	GlobalPropertyBridge globalProperties(NULL);
	GlobalPropertyWidget globalPropertiesWidget(NULL);
	ASSERT_NO_THROW({
			experiment = fmp::priv::Experiment::NewFile(TestSetup::Basedir() / "globalProperty.myrmidon");
			globalPropertiesWidget.setup(&globalProperties);
		});

	EXPECT_FALSE(globalPropertiesWidget.d_ui->nameEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->nameEdit->text(),"");

	EXPECT_FALSE(globalPropertiesWidget.d_ui->authorEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->authorEdit->text(),"");

	EXPECT_FALSE(globalPropertiesWidget.d_ui->commentEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->commentEdit->toPlainText(),"");

	EXPECT_FALSE(globalPropertiesWidget.d_ui->tagSizeEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->tagSizeEdit->value(),1.0);

	EXPECT_FALSE(globalPropertiesWidget.d_ui->familySelector->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->familySelector->currentIndex(),-1);

	experiment->SetName("foo");
	experiment->SetAuthor("tests");
	experiment->SetComment("for tests");
	experiment->SetFamily(fort::tags::Family::Tag36h11);
	experiment->SetDefaultTagSize(0.67);

	globalProperties.setExperiment(experiment);

	EXPECT_TRUE(globalPropertiesWidget.d_ui->nameEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->nameEdit->text(),"foo");

	EXPECT_TRUE(globalPropertiesWidget.d_ui->authorEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->authorEdit->text(),"tests");

	EXPECT_TRUE(globalPropertiesWidget.d_ui->commentEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->commentEdit->toPlainText(),"for tests");

	EXPECT_TRUE(globalPropertiesWidget.d_ui->tagSizeEdit->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->tagSizeEdit->value(),0.67);

	EXPECT_TRUE(globalPropertiesWidget.d_ui->familySelector->isEnabled());
	EXPECT_EQ(globalPropertiesWidget.d_ui->familySelector->currentIndex(),0);

	QTest::keyClicks(globalPropertiesWidget.d_ui->nameEdit,"bar");
	EXPECT_EQ(globalProperties.name(),"foobar");

	QTest::keyClicks(globalPropertiesWidget.d_ui->authorEdit,"b");
	EXPECT_EQ(globalProperties.author(),"testsb");

	QTest::keyClick(globalPropertiesWidget.d_ui->tagSizeEdit,Qt::Key_Delete);
	QTest::keyClicks(globalPropertiesWidget.d_ui->tagSizeEdit,"2");
	EXPECT_EQ(globalProperties.tagSize(),2.67);


	QTest::keyClicks(globalPropertiesWidget.d_ui->commentEdit,"This is ");
	EXPECT_EQ(std::string(globalProperties.comment().toUtf8().constData()),
	          std::string("This is for tests"));

	auto s = globalPropertiesWidget.d_ui->familySelector;
	s->setCurrentIndex(1);
	EXPECT_EQ(globalProperties.tagFamily(),fort::tags::Family::Tag36ARTag);
}
