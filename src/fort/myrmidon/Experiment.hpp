#pragma once

#include <memory>


namespace fort {
namespace myrmidon {

namespace priv {
class Experiment;
} // namespace priv

class Experiment {
public:
	typedef std::shared_ptr<Experiment> Ptr;

private:
};

} //namespace mrymidon
} // namespace fort
