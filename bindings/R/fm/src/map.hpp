#pragma once

#define FM_MAPUINT32_NAME(Key,Class) fm ## Class ## By ## Key

#define FM_IMPLEMENT_MAPUINT32_WITH_DETAILS(Key,Class,KeyNM,ClassNM,Printer) \
	typedef std::map<KeyNM::Key,ClassNM::Class> FM_MAPUINT32_NAME(Key,Class); \
	\
	void fm ## Class ## By ## Key ## _show(const FM_MAPUINT32_NAME(Key,Class) * m) { \
		Rcpp::Rcout << "fm" #Class "ByID (\n"; \
		for ( const auto & [k,v] : *m  ) { \
			Rcpp::Rcout << "  " << k << " ->"; \
			Printer; \
		} \
		Rcpp::Rcout << ")\n"; \
	} \
	Rcpp::List fm ## Class ## By ## Key ## _asR(const FM_MAPUINT32_NAME(Key,Class) * m ) { \
		Rcpp::List res; \
		for ( const auto & [k,v] : *m ) { \
			res.push_back(v); \
		} \
		return res; \
	} \
	RCPP_EXPOSED_CLASS_NODECL(FM_MAPUINT32_NAME(Key,Class))

#define FM_IMPLEMENT_MAPUINT32(Key,Class) FM_IMPLEMENT_MAPUINT32_WITH_DETAILS(Key,Class,fort::myrmidon,fort::myrmidon,{ fm ## Class ## _show(&v); })

#define FM_DECLARE_MAPUINT32(Key,Class,ClassName)	  \
	Rcpp::class_<FM_MAPUINT32_NAME(Key,Class)>(ClassName) \
	.const_method("show",&fm ## Class ## By ## Key ## _show) \
	.const_method("as.list",&fm ## Class ## By ## Key ## _asR) \
	.const_method("[[",&FM_MAPUINT32_NAME(Key,Class)::at) \
	.const_method("size",&FM_MAPUINT32_NAME(Key,Class)::size)
