#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <myrmidon/TestSetup.hpp>

#include <QtGlobal>

void myHandler(QtMsgType, const QMessageLogContext &, const QString &){
}

int main(int argc, char ** argv) {
	::testing::InitGoogleTest(&argc, argv);
	::testing::InitGoogleMock(&argc, argv);


	auto & listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new TestSetup());


	qInstallMessageHandler(myHandler);

	return RUN_ALL_TESTS();
}
