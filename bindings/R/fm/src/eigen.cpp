#include "eigen.h"

#include "Rcpp.h"


namespace Rcpp {
template <> Eigen::Vector2d as(SEXP vv) {
	NumericVector v(vv);
	if ( v.size() != 2 ) {
		throw std::runtime_error("Converting to Eigen::Vectro2d needs a N=numeric vector f size 2");
	}
	return Eigen::Vector2d(v[0],v[1]);
}

template <> SEXP wrap(const Eigen::Vector2d & v) {
	Rcpp::NumericVector res(2);
	res[0] = v.x();
	res[1] = v.y();
	return res;
}

template <> fort::myrmidon::Vector2dList as(SEXP l) {
	Rcpp::List list(l);
	fort::myrmidon::Vector2dList res;
	res.reserve(list.size());
	for ( const auto & v : list ) {
		res.push_back(as<Eigen::Vector2d>(v));
	}
	return res;
}

template <> SEXP wrap(const fort::myrmidon::Vector2dList & list) {
	Rcpp::List res;
	for ( const auto & v : list ) {
		res.push_back(v);
	}
	return res;
}

}
