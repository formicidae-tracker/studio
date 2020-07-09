#include "Matchers.hpp"
#include "Ant.hpp"


namespace fort {
namespace myrmidon {
namespace priv {

Matcher::~Matcher() {}

Matcher::Ptr Matcher::And(const std::vector<Ptr>  &matchers) {
	class AndMatcher : public Matcher {
	private:
		std::vector<Ptr> d_matchers;
	public:
		AndMatcher(const std::vector<Ptr> & matchers)
			: d_matchers(matchers) {
		}
		virtual ~AndMatcher() {}
		void SetUpOnce(const ConstAntByID & ants) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&ants](const Ptr & matcher) { matcher->SetUpOnce(ants); });
		}

		void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
		           const CollisionFrame::ConstPtr & collisionFrame) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&](const Ptr & matcher) { matcher->SetUp(identifiedFrame,collisionFrame); });

		}

		bool Match(fort::myrmidon::AntID ant1,
		           fort::myrmidon::AntID ant2,
		           const fort::myrmidon::InteractionTypes & type) override {
			for ( const auto & m : d_matchers ) {
				if ( m->Match(ant1,ant2,type) == false ) {
					return false;
				}
			}
			return true;
		}

		void Format(std::ostream & out ) const override {
			std::string prefix = "( ";
			for ( const auto & m : d_matchers ) {
				out << prefix;
				m->Format(out);
				prefix = " && ";
			}
			out << " )";
		}
	};

	return std::make_shared<AndMatcher>(matchers);
}

Matcher::Ptr Matcher::Or(const std::vector<Ptr> & matchers) {
	class OrMatcher : public Matcher {
	private:
		std::vector<Ptr> d_matchers;
	public:
		OrMatcher(const std::vector<Ptr> &  matchers)
			: d_matchers(matchers) {
		}
		virtual ~OrMatcher() {}
		void SetUpOnce(const ConstAntByID & ants) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&ants](const Ptr & matcher) { matcher->SetUpOnce(ants); });
		}

		void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
		           const CollisionFrame::ConstPtr & collisionFrame) override {
			std::for_each(d_matchers.begin(),d_matchers.end(),
			              [&](const Ptr & matcher) { matcher->SetUp(identifiedFrame,collisionFrame); });

		}

		bool Match(fort::myrmidon::AntID ant1,
		           fort::myrmidon::AntID ant2,
		           const fort::myrmidon::InteractionTypes & types) override {
			for ( const auto & m : d_matchers ) {
				if ( m->Match(ant1,ant2,types) == true ) {
					return true;
				}
			}
			return false;
		}

		void Format(std::ostream & out ) const override {
			std::string prefix = "( ";
			for ( const auto & m : d_matchers ) {
				out << prefix;
				m->Format(out);
				prefix = " || ";
			}
			out << " )";
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
		virtual ~AntIDMatcher() {}
		void SetUpOnce(const ConstAntByID & ants) override {
		}

		void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
		           const CollisionFrame::ConstPtr & collisionFrame) override {
		}

		bool Match(fort::myrmidon::AntID ant1,
		           fort::myrmidon::AntID ant2,
		           const fort::myrmidon::InteractionTypes & types) override {
			if ( ant2 != 0 && ant2 == d_id ) {
				return true;
			}
			return ant1 == d_id;
		}

		void Format(std::ostream & out ) const override {
			out << "Ant.ID == " << Ant::FormatID(d_id);
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
		virtual ~AntColumnMatcher() {}
		void SetUpOnce(const ConstAntByID & ants) override {
			d_ants = ants;
		}

		void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
		           const CollisionFrame::ConstPtr & collisionFrame) override {
			if ( !identifiedFrame == false ) {
				d_time = identifiedFrame->FrameTime;
				return;
			}

			if ( !collisionFrame == false ) {
				d_time = collisionFrame->FrameTime;
				return;
			}
			throw std::runtime_error("This matcher requires current time through ant position or interaction, but none is available in the current context");

		}

		bool Match(fort::myrmidon::AntID ant1,
		           fort::myrmidon::AntID ant2,
		           const fort::myrmidon::InteractionTypes & types) override {
			auto fi = d_ants.find(ant2);
			if ( fi != d_ants.end()
			     && fi->second->GetValue(d_name,d_time) == d_value ) {
				return true;
			}

			fi = d_ants.find(ant1);
			if ( fi == d_ants.end() ) {
				return false;
			}
			return fi->second->GetValue(d_name,d_time) == d_value;
		}

		void Format(std::ostream & out ) const override {
			out << "Ant.'" << d_name << "' == " << d_value;
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
	virtual ~AntDistanceMatcher() {}
	void SetUpOnce(const ConstAntByID & ants) override {
	}

	void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
	           const CollisionFrame::ConstPtr & collisionFrame) override {
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
	           const fort::myrmidon::InteractionTypes & types) override {
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

	void Format(std::ostream & out ) const override {
		out << "Distance(Ant1, Ant2) " << (d_greater == true ? ">" : "<" ) << " " << std::sqrt(d_distanceSquare);
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
	virtual ~AntAngleMatcher() {}
	void SetUpOnce(const ConstAntByID & ants) override {

	}

	void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
	           const CollisionFrame::ConstPtr & collisionFrame) override {
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
	           const fort::myrmidon::InteractionTypes & types) override {
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

	void Format(std::ostream & out ) const override {
		out << "Angle(Ant1, Ant2) " << (d_greater == true ? ">" : "<" ) << " " << d_angle;
	}


};


class InteractionTypeSingleMatcher : public Matcher {
private:
	AntShapeTypeID d_type;
public:
	InteractionTypeSingleMatcher (AntShapeTypeID type)
		: d_type(type) {
	}
	virtual ~InteractionTypeSingleMatcher() {}
	void SetUpOnce(const ConstAntByID & ants) override {
	}

	void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
	           const CollisionFrame::ConstPtr & collisionFrame) override {
	}

	bool Match(fort::myrmidon::AntID ant1,
	           fort::myrmidon::AntID ant2,
	           const fort::myrmidon::InteractionTypes & types) override {
		if (ant2 == 0) { return true; }
		for ( size_t i = 0; i < types.rows(); ++i ) {
			if ( types.row(i) == Eigen::Matrix<uint32_t,1,2>(d_type,d_type) ) {
				return true;
			}
		}
		return false;
	}

	void Format(std::ostream & out ) const override {
		out << "InteractionType (" << d_type << " - " << d_type << ")";
	}
};

class InteractionTypeDualMatcher : public Matcher {
private:
	AntShapeTypeID d_type1,d_type2;
public:
	InteractionTypeDualMatcher(AntShapeTypeID type1,AntShapeTypeID type2) {
		if ( type1 < type2 ) {
			d_type1 = type1;
			d_type2 = type2;
		} else if ( type1 > type2 ) {
			d_type1 = type2;
			d_type2 = type1;
		} else {
			throw std::runtime_error("type1 must be different than type2");
		}
	}
	virtual ~InteractionTypeDualMatcher() {}
	void SetUpOnce(const ConstAntByID & ants) override {
	}

	void SetUp(const IdentifiedFrame::ConstPtr & identifiedFrame,
	           const CollisionFrame::ConstPtr & collisionFrame) override {
	}

	bool Match(fort::myrmidon::AntID ant1,
	           fort::myrmidon::AntID ant2,
	           const fort::myrmidon::InteractionTypes & types) override {
		if (ant2 == 0) { return true; }
		for ( size_t i = 0; i < types.rows(); ++i ) {
			if ( types.row(i) == Eigen::Matrix<uint32_t,1,2>(d_type1,d_type2)
			     || types.row(i) == Eigen::Matrix<uint32_t,1,2>(d_type2,d_type1)) {
				return true;
			}
		}

		return false;
	}

	void Format(std::ostream & out ) const override {
		out << "InteractionType (" << d_type1 << " - " << d_type2 << ")";
	}
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

Matcher::Ptr Matcher::InteractionType(AntShapeTypeID type1,
                                      AntShapeTypeID type2) {
	if ( type1 != type2 ) {
		return std::make_shared<InteractionTypeDualMatcher>(type1,type2);
	}
	return std::make_shared<InteractionTypeSingleMatcher>(type1);
}

} // namespace priv
} // namespace myrmidon
} // namespace fort

std::ostream & operator<<(std::ostream & out, const fort::myrmidon::priv::Matcher & m) {
	m.Format(out);
	return out;
}
