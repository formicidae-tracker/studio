#pragma once

#include <memory>

#include "Frame.hpp"



namespace fort {

namespace myrmidon {

namespace priv {
class Experiment;
};

class Experiment {
public:
	typedef std::shared_ptr<Experiment> Ptr;

private:
};

} //namespace mrymidon

} // namespace fort
