#include "Matchers.hpp"
#include "Ant.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

Matcher::Ptr Matcher::And(std::initializer_list<Ptr>  matchers) {
	class AndMatcher : public Matcher {
	private:
		std::vector<Ptr> d_matchers;
	public:
		AndMatcher(std::initializer_list<Ptr>  matchers)
			: d_matchers(matchers) {
		}
		void SetUpOnce(const ConstAntByID & ants) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&ants](const Ptr & matcher) { matcher->SetUpOnce(ants); });
		}

		void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
		           const InteractionFrame::ConstPtr & interactionFrame) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&](const Ptr & matcher) { matcher->SetUp(identifiedFrame,interactionFrame); });

		}

		bool Match(fort::myrmidon::AntID ant1,
		           fort::myrmidon::AntID ant2,
		           const std::vector<InteractionType> & type) override {
			for ( const auto & m : d_matchers ) {
				if ( m->Match(ant1,ant2,type) == false ) {
					return false;
				}
			}
			return true;
		}
	};

	return std::make_shared<AndMatcher>(matchers);
}

Matcher::Ptr Matcher::Or(std::initializer_list<Ptr> matchers) {
	class OrMatcher : public Matcher {
	private:
		std::vector<Ptr> d_matchers;
	public:
		OrMatcher(std::initializer_list<Ptr>  matchers)
			: d_matchers(matchers) {
		}
		void SetUpOnce(const ConstAntByID & ants) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&ants](const Ptr & matcher) { matcher->SetUpOnce(ants); });
		}

		void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
		           const InteractionFrame::ConstPtr & interactionFrame) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&](const Ptr & matcher) { matcher->SetUp(identifiedFrame,interactionFrame); });

		}

		bool Match(fort::myrmidon::AntID ant1,
		           fort::myrmidon::AntID ant2,
		           const std::vector<InteractionType> & types) override {
			for ( const auto & m : d_matchers ) {
				if ( m->Match(ant1,ant2,types) == true ) {
					return true;
				}
			}
			return false;
		}
	};
	return std::make_shared<OrMatcher>(matchers);
}

Matcher::Ptr Matcher::AntIDMatcher(AntID ID) {
	class AntIDMatcher : public Matcher {
	private:
		AntID d_id;
	public:
		AntIDMatcher (AntID ant)
			: d_id(ant) {
		}
		void SetUpOnce(const ConstAntByID & ants) override {
		}

		void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
		           const InteractionFrame::ConstPtr & interactionFrame) override {
		}

		bool Match(fort::myrmidon::AntID ant1,
		           fort::myrmidon::AntID ant2,
		           const std::vector<InteractionType> & types) override {
			return ant1 == d_id;
		}
	};
	return std::make_shared<AntIDMatcher>(ID);
}

Matcher::Ptr Matcher::AntColumnMatcher(const std::string & name, const AntStaticValue & value) {
	class AntColumnMatcher : public Matcher {
	private:
		std::string    d_name;
		AntStaticValue d_value;
		ConstAntByID   d_ants;
		Time           d_time;
	public:
		AntColumnMatcher (const std::string & name,
		                  const AntStaticValue & value)
			: d_name(name)
			, d_value(value){
		}
		void SetUpOnce(const ConstAntByID & ants) override {
			d_ants = ants;
		}

		void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
		           const InteractionFrame::ConstPtr & interactionFrame) override {
			if ( !identifiedFrame == false ) {
				d_time = identifiedFrame->FrameTime;
				return;
			}

			if ( !interactionFrame == false ) {
				d_time = interactionFrame->FrameTime;
				return;
			}
			throw std::runtime_error("This matcher requires current time through ant position or interaction, but none is available in the current context");

		}

		bool Match(fort::myrmidon::AntID ant1,
		           fort::myrmidon::AntID ant2,
		           const std::vector<InteractionType> & type) override {
			auto fi = d_ants.find(ant1);
			if ( fi == d_ants.end() ) {
				return false;
			}
			return fi->second->GetValue(d_name,d_time) == d_value;
		}
	};
	return std::make_shared<AntColumnMatcher>(name,value);
}

class AntDistanceMatcher : public Matcher {
private:
	double                                   d_distanceSquare;
	DenseMap<AntID,std::pair<double,double>> d_positions;
	bool                                     d_greater;
public:
	AntDistanceMatcher (double distance, bool greater)
		: d_distanceSquare(distance * distance)
		, d_greater(greater) {
	}
	void SetUpOnce(const ConstAntByID & ants) override {
	}

	void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
	           const InteractionFrame::ConstPtr & interactionFrame) override {
		if ( !identifiedFrame ) {
			throw std::runtime_error("This matcher requires ant position, which are unavailable in the current context");
		}
		d_positions.clear();
		for ( const auto & pa : identifiedFrame->Positions) {
			d_positions.insert(std::make_pair(pa.ID,std::make_pair(pa.Position.x(),pa.Position.y())));
		}
	}

	bool Match(fort::myrmidon::AntID ant1,
	           fort::myrmidon::AntID ant2,
	           const std::vector<InteractionType> & type) override {
		auto fi1 = d_positions.find(ant1);
		auto fi2 = d_positions.find(ant2);
		if ( fi1 == d_positions.end() || fi2 == d_positions.end() ) {
			return true;
		}
		double sDist = (Eigen::Vector2d(fi1->second.first,
		                                fi1->second.second)
		                - Eigen::Vector2d(fi2->second.first,
		                                  fi2->second.second)).squaredNorm();
		if ( d_greater == true ) {
			return d_distanceSquare < sDist;
		} else {
			return d_distanceSquare > sDist;
		}
	}
};


class AntAngleMatcher : public Matcher {
private:
	double                 d_angle;
	DenseMap<AntID,double> d_angles;
	bool                   d_greater;
public:
	AntAngleMatcher (double angle, bool greater)
		: d_angle(Isometry2Dd(angle,{0,0}).angle())
		, d_greater(greater) {
	}

	void SetUpOnce(const ConstAntByID & ants) override {

	}

	void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
	           const InteractionFrame::ConstPtr & interactionFrame) override {
		if ( !identifiedFrame ) {
			throw std::runtime_error("This matcher requires ant position, which are unavailable in the current context");
		}
		d_angles.clear();
		for ( const auto & pa : identifiedFrame->Positions) {
			d_angles.insert(std::make_pair(pa.ID,pa.Angle));
		}
	}

	bool Match(fort::myrmidon::AntID ant1,
	           fort::myrmidon::AntID ant2,
	           const std::vector<InteractionType> & type) override {
		auto fi1 = d_angles.find(ant1);
		auto fi2 = d_angles.find(ant2);
		if ( fi1 == d_angles.end() || fi2 == d_angles.end() ) {
			return true;
		}
		double angle = std::abs(fi1->second - fi2->second);
		if ( d_greater == true ) {
			return angle > d_angle;
		} else {
			return angle < d_angle;
		}
	};

};

Matcher::Ptr Matcher::AntDistanceSmallerThan(double distance) {
	return std::make_shared<AntDistanceMatcher>(distance,false);
}

Matcher::Ptr Matcher::AntDistanceGreaterThan(double distance) {
	return std::make_shared<AntDistanceMatcher>(distance,true);
}

Matcher::Ptr Matcher::AntAngleGreaterThan(double angle) {
	return std::make_shared<AntAngleMatcher>(angle,true);
}

Matcher::Ptr Matcher::AntAngleSmallerThan(double angle) {
	return std::make_shared<AntAngleMatcher>(angle,false);
}


} // namespace priv
} // namespace myrmidon
} // namespace fort
