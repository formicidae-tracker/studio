#include "AntMetadataUTest.hpp"

#include <fort/myrmidon/TestSetup.hpp>

#include <fort/studio/Format.hpp>
#include <fort/studio/bridge/AntMetadataBridge.hpp>

#include <QAbstractItemModel>

#include <QTest>
#include <QSignalSpy>

void AntMetadataUTest::SetUp() {
	experiment = fmp::Experiment::Create(TestSetup::Basedir() / "antmetadata-utest.myrmidon");
	metadata = new AntMetadataBridge(nullptr);
}

void AntMetadataUTest::TearDown() {
	delete metadata;
	experiment.reset();
}

TEST_F(AntMetadataUTest,Activation) {
	QSignalSpy activated(metadata,SIGNAL(activated(bool)));

	EXPECT_FALSE(metadata->isActive());
	EXPECT_EQ(activated.count(),0);

	metadata->setExperiment(experiment);
	EXPECT_TRUE(metadata->isActive());
	ASSERT_EQ(activated.count(),1);
	EXPECT_TRUE(activated.last().at(0).toBool());

	metadata->setExperiment(experiment);
	EXPECT_TRUE(metadata->isActive());
	EXPECT_EQ(activated.count(),2);
	EXPECT_TRUE(activated.last().at(0).toBool());


	metadata->setExperiment(fmp::Experiment::Ptr());
	EXPECT_FALSE(metadata->isActive());
	EXPECT_EQ(activated.count(),3);
	EXPECT_FALSE(activated.last().at(0).toBool());

}

TEST_F(AntMetadataUTest,TypeModel) {
	auto m = metadata->typeModel();
	ASSERT_EQ(m->rowCount(),5);

	EXPECT_EQ(ToStdString(m->index(0,0).data(Qt::DisplayRole).toString()),
	          "Bool");
	EXPECT_EQ(m->index(0,0).data(Qt::UserRole+1).toInt(),
	          int(fmp::AntMetadata::Type::BOOL));

	EXPECT_EQ(ToStdString(m->index(1,0).data(Qt::DisplayRole).toString()),
	          "Int");
	EXPECT_EQ(m->index(1,0).data(Qt::UserRole+1).toInt(),
	          int(fmp::AntMetadata::Type::INT));

	EXPECT_EQ(ToStdString(m->index(2,0).data(Qt::DisplayRole).toString()),
	          "Double");
	EXPECT_EQ(m->index(2,0).data(Qt::UserRole+1).toInt(),
	          int(fmp::AntMetadata::Type::DOUBLE));


	EXPECT_EQ(ToStdString(m->index(3,0).data(Qt::DisplayRole).toString()),
	          "String");
	EXPECT_EQ(m->index(3,0).data(Qt::UserRole+1).toInt(),
	          int(fmp::AntMetadata::Type::STRING));

	EXPECT_EQ(ToStdString(m->index(4,0).data(Qt::DisplayRole).toString()),
	          "Time");
	EXPECT_EQ(m->index(4,0).data(Qt::UserRole+1).toInt(),
	          int(fmp::AntMetadata::Type::TIME));
}

TEST_F(AntMetadataUTest,ModificationTest) {
	metadata->setExperiment(experiment);

	QSignalSpy modified(metadata,SIGNAL(modified(bool)));
	QSignalSpy columnChanged(metadata,SIGNAL(metadataColumnChanged(const QString &, quint32)));
	QSignalSpy columnRemoved(metadata,SIGNAL(metadataColumnRemoved(const QString &)));
	ASSERT_TRUE(modified.isValid());
	ASSERT_TRUE(columnChanged.isValid());
	ASSERT_TRUE(columnRemoved.isValid());

	auto m = metadata->columnModel();
	EXPECT_EQ(m->rowCount(),0);
	EXPECT_FALSE(metadata->isModified());
	EXPECT_EQ(modified.count(),0);

	metadata->addMetadataColumn("foo",quint32(fmp::AntMetadata::Type::DOUBLE));
	EXPECT_EQ(m->rowCount(),1);
	ASSERT_EQ(modified.count(),1);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(ToStdString(columnChanged.last().at(0).toString()),
	          "foo");
	EXPECT_EQ(columnChanged.last().at(1).toInt(),
	          quint32(fmp::AntMetadata::Type::DOUBLE));

	metadata->addMetadataColumn("bar",quint32(fmp::AntMetadata::Type::STRING));
	EXPECT_EQ(columnChanged.count(),2);
	EXPECT_EQ(ToStdString(columnChanged.last().at(0).toString()),
	          "bar");
	EXPECT_EQ(columnChanged.last().at(1).toInt(),
	          quint32(fmp::AntMetadata::Type::STRING));

	//data is sorted
	ASSERT_EQ(m->rowCount(),2);
	EXPECT_EQ(ToStdString(m->index(0,0).data(Qt::DisplayRole).toString()),
	          "bar");
	EXPECT_EQ(ToStdString(m->index(1,0).data(Qt::DisplayRole).toString()),
	          "foo");


	metadata->setExperiment(experiment);
	EXPECT_FALSE(metadata->isModified());
	EXPECT_EQ(modified.count(),2);
	EXPECT_FALSE(modified.last().at(0).toBool());

	//cannot work
	m->setData(m->index(0,0),"foo",Qt::DisplayRole);
	EXPECT_FALSE(metadata->isModified());
	EXPECT_EQ(modified.count(),2);
	EXPECT_EQ(ToStdString(m->index(0,0).data(Qt::DisplayRole).toString()),
	          "bar");

	metadata->removeMetadataColumn("bar");
	EXPECT_EQ(m->rowCount(),1);
	EXPECT_TRUE(metadata->isModified());
	EXPECT_EQ(modified.count(),3);
	EXPECT_TRUE(modified.last().at(0).toBool());

	metadata->setExperiment(experiment);
	EXPECT_FALSE(metadata->isModified());
	EXPECT_EQ(modified.count(),4);
	EXPECT_FALSE(modified.last().at(0).toBool());
	ASSERT_EQ(columnRemoved.count(),1);
	EXPECT_EQ(ToStdString(columnRemoved.last().at(0).toString()),
	          "bar");

	m->setData(m->index(0,1),quint32(fmp::AntMetadata::Type::TIME),Qt::UserRole+2);
	EXPECT_TRUE(metadata->isModified());
	EXPECT_EQ(modified.count(),5);
	EXPECT_TRUE(modified.last().at(0).toBool());
	EXPECT_EQ(columnChanged.count(),3);
	EXPECT_EQ(ToStdString(columnChanged.last().at(0).toString()),
	          "foo");
	EXPECT_EQ(columnChanged.last().at(1).toInt(),
	          quint32(fmp::AntMetadata::Type::TIME));
	EXPECT_EQ(ToStdString(m->index(0,2).data(Qt::DisplayRole).toString()),
	          ToStdString(ToQString(fort::Time())));

	ASSERT_NO_THROW({
			auto ant = experiment->CreateAnt();
			ant->SetValue("foo",fort::Time::Parse("2019-11-02T23:46:23.453Z"),fort::Time::SinceEver());
		});

	metadata->setExperiment(experiment);
	EXPECT_FALSE(metadata->isModified());
	EXPECT_EQ(modified.count(),6);
	EXPECT_FALSE(modified.last().at(0).toBool());

	metadata->removeMetadataColumn("foo");
	EXPECT_FALSE(metadata->isModified());
	EXPECT_EQ(modified.count(),6);

	m->setData(m->index(0,1),quint32(fmp::AntMetadata::Type::BOOL),Qt::UserRole+2);
	EXPECT_FALSE(metadata->isModified());
	EXPECT_EQ(modified.count(),6);
	EXPECT_EQ(columnChanged.count(),3);
	EXPECT_EQ(ToStdString(m->index(0,1).data(Qt::DisplayRole).toString()),
	          "Time");

}
