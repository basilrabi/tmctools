// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// exportPly
Rcpp::String exportPly(const std::string& db_user, const std::string& db_host, const std::string& db_name, const std::string& schema, const std::string& db_table, const std::string& srid, const std::string& port, const unsigned int& digits, const bool& binary);
RcppExport SEXP _tmctools_exportPly(SEXP db_userSEXP, SEXP db_hostSEXP, SEXP db_nameSEXP, SEXP schemaSEXP, SEXP db_tableSEXP, SEXP sridSEXP, SEXP portSEXP, SEXP digitsSEXP, SEXP binarySEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type db_user(db_userSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type db_host(db_hostSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type db_name(db_nameSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type schema(schemaSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type db_table(db_tableSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type srid(sridSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type port(portSEXP);
    Rcpp::traits::input_parameter< const unsigned int& >::type digits(digitsSEXP);
    Rcpp::traits::input_parameter< const bool& >::type binary(binarySEXP);
    rcpp_result_gen = Rcpp::wrap(exportPly(db_user, db_host, db_name, schema, db_table, srid, port, digits, binary));
    return rcpp_result_gen;
END_RCPP
}
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
// plyToDB
void plyToDB(const std::string& ply, const std::string& db_user, const std::string& db_host, const std::string& db_name, const std::string& db_table, const std::string& schema, const std::string& port, const std::string& srid);
RcppExport SEXP _tmctools_plyToDB(SEXP plySEXP, SEXP db_userSEXP, SEXP db_hostSEXP, SEXP db_nameSEXP, SEXP db_tableSEXP, SEXP schemaSEXP, SEXP portSEXP, SEXP sridSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type ply(plySEXP);
    Rcpp::traits::input_parameter< const std::string& >::type db_user(db_userSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type db_host(db_hostSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type db_name(db_nameSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type db_table(db_tableSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type schema(schemaSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type port(portSEXP);
    Rcpp::traits::input_parameter< const std::string& >::type srid(sridSEXP);
    plyToDB(ply, db_user, db_host, db_name, db_table, schema, port, srid);
    return R_NilValue;
END_RCPP
}
// plyBinToText
void plyBinToText(const std::string& in_ply, const std::string& out_ply);
RcppExport SEXP _tmctools_plyBinToText(SEXP in_plySEXP, SEXP out_plySEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type in_ply(in_plySEXP);
    Rcpp::traits::input_parameter< const std::string& >::type out_ply(out_plySEXP);
    plyBinToText(in_ply, out_ply);
    return R_NilValue;
END_RCPP
}
// readDTM
Rcpp::DataFrame readDTM(std::string dtm, std::string srid);
RcppExport SEXP _tmctools_readDTM(SEXP dtmSEXP, SEXP sridSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type dtm(dtmSEXP);
    Rcpp::traits::input_parameter< std::string >::type srid(sridSEXP);
    rcpp_result_gen = Rcpp::wrap(readDTM(dtm, srid));
    return rcpp_result_gen;
END_RCPP
}
// readPly
Rcpp::DataFrame readPly(const std::string& ply, const std::string& srid);
RcppExport SEXP _tmctools_readPly(SEXP plySEXP, SEXP sridSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const std::string& >::type ply(plySEXP);
    Rcpp::traits::input_parameter< const std::string& >::type srid(sridSEXP);
    rcpp_result_gen = Rcpp::wrap(readPly(ply, srid));
    return rcpp_result_gen;
END_RCPP
}
// writeDTM
int writeDTM(std::string dtm, std::string db_user, std::string db_host, std::string db_name, std::string db_table, std::string schema, std::string port, std::string srid);
RcppExport SEXP _tmctools_writeDTM(SEXP dtmSEXP, SEXP db_userSEXP, SEXP db_hostSEXP, SEXP db_nameSEXP, SEXP db_tableSEXP, SEXP schemaSEXP, SEXP portSEXP, SEXP sridSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< std::string >::type dtm(dtmSEXP);
    Rcpp::traits::input_parameter< std::string >::type db_user(db_userSEXP);
    Rcpp::traits::input_parameter< std::string >::type db_host(db_hostSEXP);
    Rcpp::traits::input_parameter< std::string >::type db_name(db_nameSEXP);
    Rcpp::traits::input_parameter< std::string >::type db_table(db_tableSEXP);
    Rcpp::traits::input_parameter< std::string >::type schema(schemaSEXP);
    Rcpp::traits::input_parameter< std::string >::type port(portSEXP);
    Rcpp::traits::input_parameter< std::string >::type srid(sridSEXP);
    rcpp_result_gen = Rcpp::wrap(writeDTM(dtm, db_user, db_host, db_name, db_table, schema, port, srid));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_tmctools_exportPly", (DL_FUNC) &_tmctools_exportPly, 9},
    {"_tmctools_idAssetGroup", (DL_FUNC) &_tmctools_idAssetGroup, 1},
    {"_tmctools_plyToDB", (DL_FUNC) &_tmctools_plyToDB, 8},
    {"_tmctools_plyBinToText", (DL_FUNC) &_tmctools_plyBinToText, 2},
    {"_tmctools_readDTM", (DL_FUNC) &_tmctools_readDTM, 2},
    {"_tmctools_readPly", (DL_FUNC) &_tmctools_readPly, 2},
    {"_tmctools_writeDTM", (DL_FUNC) &_tmctools_writeDTM, 8},
    {NULL, NULL, 0}
};

RcppExport void R_init_tmctools(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
