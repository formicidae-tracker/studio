#pragma once

#include <gtest/gtest.h>

#include <fort/myrmidon/priv/Experiment.hpp>

#include <fort/studio/workspace/AntGeometryWorkspace.hpp>

class ExperimentBridge;

namespace fmp = fort::myrmidon::priv;

class WorkspaceUTest {
protected:
	void SetUp(const std::string & name);
	void TearDown();

	fmp::Experiment::Ptr experiment;
	ExperimentBridge   * bridge;
	QMainWindow        * mainWindow;
};

class AntShapeWorkspaceUTest : public testing::Test, public WorkspaceUTest {
protected:
	void SetUp();
	void TearDown();

	AntShapeWorkspace * shapeWs;
};
