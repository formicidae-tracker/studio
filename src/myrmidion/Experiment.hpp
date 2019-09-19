#pragma once

#include <memory>

#include "Frame.hpp"



namespace fort {

namespace myrmidion {

namespace priv { class Experiment; };

class Experiment {
public:
	typedef std::shared_ptr<Experiment> Ptr;

	static Ptr Open(const std::string & filename);


	Frame::Ptr GetFrame(Frame::ID ID);

private:
	std::unique_ptr<priv::Experiment> d_priv;
};

} //namespace mrymidion

} // namespace fort
