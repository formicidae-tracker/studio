// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;


RcppExport SEXP _rcpp_module_boot_ant();
RcppExport SEXP _rcpp_module_boot_antStaticValue();
RcppExport SEXP _rcpp_module_boot_color();
RcppExport SEXP _rcpp_module_boot_duration();
RcppExport SEXP _rcpp_module_boot_identification();
RcppExport SEXP _rcpp_module_boot_shapes();
RcppExport SEXP _rcpp_module_boot_space();
RcppExport SEXP _rcpp_module_boot_time();
RcppExport SEXP _rcpp_module_boot_zone();

static const R_CallMethodDef CallEntries[] = {
    {"_rcpp_module_boot_ant", (DL_FUNC) &_rcpp_module_boot_ant, 0},
    {"_rcpp_module_boot_antStaticValue", (DL_FUNC) &_rcpp_module_boot_antStaticValue, 0},
    {"_rcpp_module_boot_color", (DL_FUNC) &_rcpp_module_boot_color, 0},
    {"_rcpp_module_boot_duration", (DL_FUNC) &_rcpp_module_boot_duration, 0},
    {"_rcpp_module_boot_identification", (DL_FUNC) &_rcpp_module_boot_identification, 0},
    {"_rcpp_module_boot_shapes", (DL_FUNC) &_rcpp_module_boot_shapes, 0},
    {"_rcpp_module_boot_space", (DL_FUNC) &_rcpp_module_boot_space, 0},
    {"_rcpp_module_boot_time", (DL_FUNC) &_rcpp_module_boot_time, 0},
    {"_rcpp_module_boot_zone", (DL_FUNC) &_rcpp_module_boot_zone, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_fm(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
