// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// idAssetGroup
StringVector idAssetGroup(StringVector x);
RcppExport SEXP _tmctools_idAssetGroup(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< StringVector >::type x(xSEXP);
    rcpp_result_gen = Rcpp::wrap(idAssetGroup(x));
    return rcpp_result_gen;
END_RCPP
}
// readDTM
Rcpp::DataFrame readDTM(std::string dtmFile, std::string srid);
RcppExport SEXP _tmctools_readDTM(SEXP dtmFileSEXP, SEXP sridSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type dtmFile(dtmFileSEXP);
    Rcpp::traits::input_parameter< std::string >::type srid(sridSEXP);
    rcpp_result_gen = Rcpp::wrap(readDTM(dtmFile, srid));
    return rcpp_result_gen;
END_RCPP
}
// writeDTM
int writeDTM(std::string dtmFile, std::string user, std::string hostname, std::string dbname, std::string tableName, std::string schema, std::string port, std::string srid);
RcppExport SEXP _tmctools_writeDTM(SEXP dtmFileSEXP, SEXP userSEXP, SEXP hostnameSEXP, SEXP dbnameSEXP, SEXP tableNameSEXP, SEXP schemaSEXP, SEXP portSEXP, SEXP sridSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type dtmFile(dtmFileSEXP);
    Rcpp::traits::input_parameter< std::string >::type user(userSEXP);
    Rcpp::traits::input_parameter< std::string >::type hostname(hostnameSEXP);
    Rcpp::traits::input_parameter< std::string >::type dbname(dbnameSEXP);
    Rcpp::traits::input_parameter< std::string >::type tableName(tableNameSEXP);
    Rcpp::traits::input_parameter< std::string >::type schema(schemaSEXP);
    Rcpp::traits::input_parameter< std::string >::type port(portSEXP);
    Rcpp::traits::input_parameter< std::string >::type srid(sridSEXP);
    rcpp_result_gen = Rcpp::wrap(writeDTM(dtmFile, user, hostname, dbname, tableName, schema, port, srid));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_tmctools_idAssetGroup", (DL_FUNC) &_tmctools_idAssetGroup, 1},
    {"_tmctools_readDTM", (DL_FUNC) &_tmctools_readDTM, 2},
    {"_tmctools_writeDTM", (DL_FUNC) &_tmctools_writeDTM, 8},
    {NULL, NULL, 0}
};

RcppExport void R_init_tmctools(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
