#include <Rcpp.h>
#include <algorithm>
#include <regex>

using namespace Rcpp;

// String to upper
//
// Converts the string to uppercase
//
// @param x a character string
// @return a character string without lower case
std::string stringToUpper( std::string x )
{
  std::string data = x;
  std::transform( data.begin(), data.end(), data.begin(), ::toupper );
  return data;
}

String concat( IntegerVector x )
{
  String z;
  CharacterVector y = as<CharacterVector>( x );
  z = collapse( y );
  return z;
}

// Get letter count
//
// Gets the frequency per letter of a string
//
// @param x a character string
// @return a string of at least 26 length having only digits as characters
String getLetterCount( std::string x )
{

  unsigned int i;
  std::string tempChar;

  std::transform( x.begin(), x.end(), x.begin(), ::toupper );

  // 26-element vector representing the alphabet counts
  IntegerVector letterCounts {0,0,0,0,0,0,0,0,0,0,0,0,0,
                              0,0,0,0,0,0,0,0,0,0,0,0,0};

  letterCounts.names() = CharacterVector( {
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
    "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y" ,"Z"} );

  for ( i = 0; i < x.length(); i++ )
  {
    tempChar = x[i];

    if ( std::regex_match( tempChar, std::regex( "[[:upper:]]" ) ) )
      letterCounts[letterCounts.findName( tempChar )]++;
  }

  return concat( letterCounts );
}

// Is the string a subset?
//
// Test whether the string \code{x} is a subset of another string.
//
// @param x a character string to be tested
// @param y a string return by \code{\link{getLetterCount}}
//
//   \code{y} comes from a string wherein the string \code{x} might be a subset.
// @return logical value
bool isSubset( std::string x, std::string y )
{

  int z;
  std::string temp_str;
  temp_str = getLetterCount( x );

  for ( unsigned int i = 0; i < 26; i++ )
  {
    z = ( int ) y[i] - ( int ) temp_str[i];

    if ( z < 0 )
      return false;
  }

  return true;
}

//' Identify asset group
//'
//' Identifies the asset group of a given equipment (or entity) which is the
//'   recipient of the item withdrawn from warehouse.
//'   This data can be seen from warehouse issuance slips prior to
//'   implementation of SAP.
//'
//'   Below are the identified groups:
//'   \itemize{
//'   \item Contractors
//'     \describe{
//'     \item{4K}{4K Development Corporation}
//'     \item{CKDI}{Cagdianao Konstruct Development Incorporated}
//'     \item{HPK}{HPK Construction}
//'     \item{MIZPEH}{}
//'     \item{MTEL}{}
//'     \item{NBI}{Nickelbase Incorporated}
//'     \item{PACE}{PrinceAce Corporation}
//'     \item{POLARIS}{Polaris Construction & General Services}
//'     \item{SDMC}{Southernphil Development and Marketing Corporation (FITZ-SDMC)}
//'     \item{SKAFF}{SKAFF Construkt Incorporated}
//'     \item{SMCC}{Sumitomo-Mitsui Construction Company}
//'     \item{TACOPHIL}{}
//'     \item{TRGC}{Tango Romeo General Construction}
//'     }
//'   \item Equipment
//'     \describe{
//'     \item{AC}{air compressor}
//'     \item{AD}{articulated dump truck}
//'     \item{AT}{all-terrain vehicle}
//'     \item{CM}{concrete mixer}
//'     \item{CT}{bulldozer}
//'     \item{DM}{Drill Machine}
//'     \item{DT}{dump truck}
//'     \item{FC}{fast craft}
//'     \item{FL}{forklift}
//'     \item{FT}{fuel truck}
//'     \item{FM}{farm tractors}
//'     \item{FR}{fire truck}
//'     \item{FS}{flying squid}
//'     \item{GS}{power generator set}
//'     \item{GT}{garbage truck}
//'     \item{HDT}{HOWO dump truck}
//'     \item{LC}{landing craft tank}
//'     \item{LM}{lawn mower}
//'     \item{MC}{mobile crusher}
//'     \item{MD}{mini dump truck}
//'     \item{ML}{man lift}
//'     \item{MR}{motorcycle}
//'     \item{MS}{mobile screener}
//'     \item{MT}{maintenance truck}
//'     \item{MX}{mini tracked excavator}
//'     \item{MW}{mini wheeled pay loader}
//'     \item{PATROLBOAT}{}
//'     \item{PB}{pump boat}
//'     \item{PM}{prime mover truck}
//'     \item{RG}{road grader}
//'     \item{SB}{service bus}
//'     \item{ST}{service truck}
//'     \item{SV}{service car}
//'     \item{TL}{tower light}
//'     \item{TX}{tracked excavator}
//'     \item{VR}{vibrating compactor}
//'     \item{WL}{wheeled pay loader}
//'     \item{WM}{welding machine}
//'     \item{WP}{water pump}
//'     \item{WR}{wheeled crane}
//'     \item{WT}{water truck}
//'     \item{WX}{wheeled excavator}
//'     }
//'   \item Fixed assets
//'     \describe{
//'     \item{ADMIN}{Admin building}
//'     \item{EQ}{Engineer's Quarters}
//'     \item{CIP}{construction in-progress}
//'     }
//'   \item Other parties
//'     \describe{
//'     \item{LGU}{local government unit}
//'     \item{MKTG}{marketing}
//'     \item{NCIP}{National Commission on Indigenous Peoples}
//'     \item{SECURITY}{}
//'     }
//'   \item Unknown
//'     \itemize{
//'     \item ZZZ
//'     }
//'   }
//'
//' @param x a character vector representing the equipment
//' @return a character vector representing the asset group
//' @export
// [[Rcpp::export]]
StringVector idAssetGroup( StringVector x )
{

  R_xlen_t n = x.length();
  StringVector equip( clone( x ) );
  String a, b;
  std::string test_char;

  // declare getLetterCount constants to avoid repeating in the loop
  String glc_ac        = getLetterCount( "AC" );
  String glc_adt       = getLetterCount( "ADT" );
  String glc_atv       = getLetterCount( "ATV" );
  String glc_mc        = getLetterCount( "CM" );
  String glc_ct        = getLetterCount( "CT" );
  String glc_dt1       = getLetterCount( "DT" );
  String glc_dt2       = getLetterCount( "D" );
  String glc_dt3       = getLetterCount( "DTUP" );
  String glc_dt4       = getLetterCount( "DTPDT" );
  String glc_dt5       = getLetterCount( "DTT" );
  String glc_dt6       = getLetterCount( "DST" );
  String glc_eq        = getLetterCount( "EQ" );
  String glc_fl        = getLetterCount( "FL" );
  String glc_forklift1 = getLetterCount( "FORKLIFT" );
  String glc_forklift2 = getLetterCount( "FRL" );
  String glc_forklift3 = getLetterCount( "MFL" );
  String glc_ft        = getLetterCount( "FT" );
  String glc_ftr       = getLetterCount( "FTR" );
  String glc_gs        = getLetterCount( "GS" );
  String glc_gt        = getLetterCount( "GT" );
  String glc_lm        = getLetterCount( "LM" );
  String glc_mdt       = getLetterCount( "MDT" );
  String glc_mos1      = getLetterCount( "MOS" );
  String glc_mos2      = getLetterCount( "MS" );
  String glc_mt        = getLetterCount( "MT" );
  String glc_mtx       = getLetterCount( "MTX" );
  String glc_mwl       = getLetterCount( "MWL" );
  String glc_pb        = getLetterCount( "PB" );
  String glc_pmt       = getLetterCount( "PMT" );
  String glc_rg        = getLetterCount( "RG" );
  String glc_sb        = getLetterCount( "SB" );
  String glc_security  = getLetterCount( "SECURITY" );
  String glc_sp        = getLetterCount( "SP" );
  String glc_st        = getLetterCount( "ST" );
  String glc_tl        = getLetterCount( "TL" );
  String glc_tx1       = getLetterCount( "TX" );
  String glc_tx2       = getLetterCount( "TZ" );
  String glc_tx3       = getLetterCount( "X" );
  String glc_vc1       = getLetterCount( "V" );
  String glc_vc2       = getLetterCount( "VC" );
  String glc_wc        = getLetterCount( "WC" );
  String glc_wl1       = getLetterCount( "WL" );
  String glc_wl2       = getLetterCount( "W" );
  String glc_wl3       = getLetterCount( "WLRGCT" );
  String glc_wm        = getLetterCount( "WM" );
  String glc_wp1       = getLetterCount( "WP" );
  String glc_wp2       = getLetterCount( "WTLWP" );
  String glc_wtl       = getLetterCount( "WTL" );
  String glc_wx        = getLetterCount( "WX" );
  String glc_ybm       = getLetterCount( "YBM" );

  for ( R_xlen_t i = 0; i < n; i++ )
  {

    if ( x[i] == NA_STRING )
      continue;

    a = x[i];
    b = x[i];

    a = stringToUpper( a );
    test_char = a.get_cstring();
    b = getLetterCount( b );

    // FIXME
    // Re-arrange according to frequency in data

    if ( std::regex_match( test_char, std::regex( ".*TACOPHIL.*" ) ) )
      equip[i] = "TACOPHIL";

    else if ( std::regex_match( test_char, std::regex( ".*4K.*" ) ) )
      equip[i] = "4K";

    else if ( b == glc_ac )
      equip[i] = "AC";

    else if ( std::regex_match( test_char, std::regex( ".*PACE.*" ) ) ||
              std::regex_match( test_char, std::regex( ".*PRI?N?CE *ACE.*" ) ) )
      equip[i] = "PACE";

    else if ( std::regex_match( test_char, std::regex( ".*BRGY.*" ) ) ||
              std::regex_match( test_char, std::regex( ".*BGRY.*" ) ) )
      equip[i] = "LGU";

    else if ( std::regex_match( test_char, std::regex( ".*SMCC.*" ) ) )
      equip[i] = "SMCC";

    else if ( std::regex_match( test_char, std::regex( "(^|(.*\\s+))ADMIN.*" ) ) )
      equip[i] = "ADMIN";

    else if ( std::regex_match( test_char, std::regex( ".*NCIP.*" ) ) )
      equip[i] = "NCIP";

    else if ( std::regex_match( test_char, std::regex( ".*CIK?P.*" ) ) )
      equip[i] = "CIP";

    else if ( std::regex_match( test_char, std::regex( "^\\s*CKDI.*" ) ) )
      equip[i] = "CKDI";

    else if ( b == glc_mc )
    {

      if ( std::regex_match( test_char, std::regex( ".*MC.*" ) ) )
        equip[i] = "MR";

      else
        equip[i] = "CM";
    }

    else if ( std::regex_match( test_char, std::regex( ".*MI?XE?R.*" ) ) )
      equip[i] = "CM";

    else if ( b == glc_ct )
      equip[i] = "CT";

    else if ( b == glc_dt1 || b == glc_dt2 || b == glc_dt3 ||
              b == glc_dt4 || b == glc_dt5 || b == glc_dt6 )
    {
      if ( std::regex_match( test_char, std::regex( ".*23[8-9].*" ) ) ||
           std::regex_match( test_char, std::regex( ".*24[0-9].*" ) ) )
        equip[i] = "HDT";

      else
        equip[i] = "DT";
    }


    else if ( b == glc_eq ||
              std::regex_match( test_char, std::regex( ".*EQ\\s*[[:digit:]].*" ) ) )
      equip[i] = "EQ";

    else if ( std::regex_match( test_char, std::regex( ".*FAST?\\s?CRAFT.*" ) ) )
      equip[i] = "FC";

    else if ( b == glc_fl )
      equip[i] = "FT";

    else if ( b == glc_forklift1 || b == glc_forklift2 || b == glc_forklift3 )
      equip[i] = "FL";

    else if ( std::regex_match( test_char, std::regex( "^F\\s*S.*" ) ) ||
              std::regex_match( test_char, std::regex( ".*SQUID.*" ) ) )
      equip[i] = "FS";

    else if ( b == glc_ft )
      equip[i] = "FR";

    else if ( b == glc_ftr )
      equip[i] = "FM";

    else if ( b == glc_gs ||
              std::regex_match( test_char, std::regex( ".*GEN\\s*SET.*" ) ) ||
              std::regex_match( test_char, std::regex( ".*GENERATOR.*" ) ) ||
              std::regex_match( test_char, std::regex( "^GS(\\s+|-)\\d+.*" ) ) )
      equip[i] = "GS";

    else if ( b == glc_gt )
      equip[i] = "GT";

    else if ( std::regex_match( test_char, std::regex( ".*HPK.*" ) ) )
      equip[i] = "HPK";

    else if ( std::regex_match( test_char, std::regex( "^\\s*LCT.*" ) ) )
      equip[i] = "LC";

    else if ( b == glc_lm )
    {

      if ( std::regex_match( test_char, std::regex( ".*L\\s*M.*" ) ) )
        equip[i] = "LM";

      else
        equip[i] = "ML";
    }

    else if ( std::regex_match( test_char, std::regex( ".*MOW?ER.*" ) ) )
      equip[i] = "LM";

    else if ( std::regex_match( test_char, std::regex( ".*MANLIFT.*" ) ) )
      equip[i] = "ML";

    else if ( std::regex_match( test_char, std::regex( ".*MIZPEH.*" ) ) )
      equip[i] = "MIZPEH";

    else if ( b == glc_mdt ||
              std::regex_match( test_char, std::regex( ".*MD+T\\b.*" ) ) )
      equip[i] = "MD";

    else if ( std::regex_match( test_char, std::regex( ".*MA?R?KE?TI?N?G.*" ) ) ||
              std::regex_match( test_char, std::regex( ".*MKGT.*" ) ) )
      equip[i] = "MKTG";

    else if ( std::regex_match( test_char, std::regex( "^CRUSHER.*" ) ) ||
              std::regex_match( test_char, std::regex( "^M.*CRU?SHE?E?R.*" ) ) ||
              std::regex_match( test_char, std::regex( ".*MOBILE\\s*CRUSH?E?R?.*" ) ) )
      equip[i] = "MC";

    else if ( std::regex_match( test_char, std::regex( ".*SCREE?N(E|I)?R?.*" ) ) ||
              ( b == glc_mos1 ) ||
              ( b == glc_mos2 ) )
      equip[i] = "MS";

    else if ( std::regex_match( test_char, std::regex( ".*MTEL.*" ) ) )
      equip[i] = "MTEL";

    else if ( b == glc_mt ||
              std::regex_match( test_char, std::regex( "^MT\\s*\\d+.*" ) ) )
      equip[i] = "MT";

    else if ( b == glc_mtx ||
              std::regex_match( test_char, std::regex( ".*MTX-\\d+.*" ) ) )
      equip[i] = "MX";

    else if ( b == glc_mwl )
      equip[i] = "MW";

    else if ( std::regex_match( test_char, std::regex( ".*NB(C|I).*" ) ) ||
              std::regex_match( test_char, std::regex( "^NB?(C|I)?\\s+.*" ) ) )
      equip[i] = "NBI";

    else if ( std::regex_match( test_char, std::regex( ".*PATROL\\s*BOAT.*" ) ) )
      equip[i] = "PATROLBOAT";

    else if ( std::regex_match( test_char, std::regex( ".*POA?LA?RIS.*" ) ) )
      equip[i] = "POLARIS";

    else if ( b == glc_pb ||
              std::regex_match( test_char, std::regex( ".*PUMP\\s*BOAT.*" ) ) )
      equip[i] = "PB";

    else if ( b == glc_pmt )
      equip[i] = "PM";

    else if ( b == glc_rg )
      equip[i] = "RG";

    else if ( b == glc_sb )
      equip[i] = "SB";

    else if ( std::regex_match( test_char, std::regex( ".*SD(M|N)\\s*C.*" ) ) ||
              std::regex_match( test_char, std::regex( ".*SMDC.*" ) ) )
      equip[i] = "SDMC";

    else if ( b == glc_security )
      equip[i] = "SECURITY";

    else if ( std::regex_match( test_char, std::regex( ".*SKAFF.*" ) ) )
      equip[i] = "SKAFF";

    else if ( b == glc_sp ||
              std::regex_match( test_char, std::regex( "^SP-\\d+\\s+.*" ) ) )
      equip[i] = "SV";

    else if ( b == glc_st )
      equip[i] = "ST";

    else if ( b == glc_tl )
      equip[i] = "TL";

    else if ( std::regex_match( test_char, std::regex( ".*TRG?C.*" ) ) )
      equip[i] = "TRGC";

    else if ( b == glc_tx1 ||
              std::regex_match( test_char, std::regex( "^TX\\s+.*" ) ) ||
              b == glc_tx2 ||
              b == glc_tx3 )
      equip[i] = "TX";

    else if ( b == glc_vc2 || b == glc_vc1 )
      equip[i] = "VR";

    else if ( b == glc_wc ||
              std::regex_match( test_char, std::regex( ".*CRANE.*" ) ) )
      equip[i] = "WR";

    else if ( b == glc_wm &&
              std::regex_match( test_char, std::regex( ".*WM.*" ) ) )
      equip[i] = "WM";

    else if ( b == glc_wl1 ||
              b == glc_wl2 ||
              b == glc_wl3)
      equip[i] = "WL";

    else if ( b == glc_wp1 ||
              std::regex_match( test_char, std::regex( ".*H2(0|O)\\s*PUMP.*" ) ) ||
              b == glc_wp2 ||
              isSubset( "WATER PUMP", b ) )
      equip[i] = "WP";

    else if ( b == glc_wtl )
    {

      if ( std::regex_match( test_char, std::regex( ".*WTL.*" ) ) ||
           std::regex_match( test_char, std::regex( ".*WLT.*" ) ) )
        equip[i] = "WT";

      else if ( std::regex_match( test_char, std::regex( ".*TWL.*" ) ) )
        equip[i] = "TL";

      else
        equip[i] = "ZZZ";
    }

    else if ( std::regex_match( test_char, std::regex( ".*WX.*" ) ) ||
              b == glc_wx )
      equip[i] = "WX";

    else if ( b == glc_ybm ||
              std::regex_match( test_char, std::regex( ".*YBB?M.*" ) ) )
      equip[i] = "DM";

    else if ( std::regex_match( test_char, std::regex( ".*VOLVO\\s*DT.*" ) ) )
      equip[i] = "DT";

    else if ( std::regex_match( test_char, std::regex( "^MC\\s*\\d+.*" ) ) ||
              std::regex_match( test_char, std::regex( ".*MOTORCYCLE.*" ) ) )
      equip[i] = "MR";

    else if ( b == glc_atv )
      equip[i] = "AT";

    else if ( b == glc_adt )
      equip[i] = "AD";

    else if ( std::regex_match( test_char, std::regex( "^DT\\s*-?\\s*\\d+.*" ) ) )
      equip[i] = "DT";

    // Exotic cases

    else if ( test_char == "219" )
      equip[i] = "DT";

    else if ( test_char == "FV VI" )
      equip[i] = "FS";

    else if ( test_char == "R6 11" )
      equip[i] = "RG";

    else
      equip[i] = "ZZZ";
  }

  return equip;
}
