#pragma once

#include <memory>

namespace fort {
namespace myrmidon {
namespace priv {

template<typename Object,typename Context>
class HandleInContext {
public:
	HandleInContext(const std::shared_ptr<Object> & object,
	                const std::shared_ptr<Context> & context)
		: d_context(context)
		, d_object(object) {
	}

	inline Object & Get() {
		return *d_object;
	}

private:
	std::shared_ptr<Context> d_context;
	std::shared_ptr<Object>  d_object;
};

}
} // namespace myrmidon
} // namespace fort
