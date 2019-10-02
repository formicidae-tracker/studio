#pragma once

#include <stdexcept>

namespace fort {

namespace myrmidon {

namespace priv {

template<typename T>
class DeletedReference : public std::runtime_error {
public:
	inline DeletedReference() noexcept : std::runtime_error(std::string("Deleted reference to ") + typeid(T).name() + " object") {};
	inline virtual ~DeletedReference() noexcept {}
};

} // namespace priv

} // namespace myrmidon

} // fort
