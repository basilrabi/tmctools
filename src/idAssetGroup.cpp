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
std::string str_to_upper( std::string x )
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
  std::string tempStr;
  tempStr = getLetterCount( x );

  for ( unsigned int i = 0; i < 26; i++ )
  {
    z = ( int ) y[i] - ( int ) tempStr[i];

    if ( z < 0 )
      return false;
  }

  return true;
}

//' Identify asset group
//'
//' Identifies the asset group of a given equipment (or entity) which is the
//'   recipient of the item withdrawn from warehouse.
//'   This data can be seen from warehouse issuance slips.
//'
//'   Below are the identified groups:
//'   \itemize{
//'   \item Contractors
//'     \describe{
//'     \item{4K}{4K Development Corporation}
//'     \item{HPK}{HPK Construction}
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
//'     \item{ADT}{articulated dump truck}
//'     \item{ATV}{all-terrain vehicle}
//'     \item{CM}{concrete mixer}
//'     \item{CT}{bulldozer}
//'     \item{DT}{dump truck}
//'     \item{FASTCRAFT}{}
//'     \item{FL}{fuel lorry}
//'     \item{FORKLIFT}{}
//'     \item{FS}{flying squid}
//'     \item{FT}{fire truck}
//'     \item{FTR}{farm tractors}
//'     \item{GS}{power generator set}
//'     \item{GT}{garbage truck}
//'     \item{HDT}{HOWO dump truck}
//'     \item{LM}{lawn mower}
//'     \item{MC}{motorcycle}
//'     \item{MDT}{mini dump truck}
//'     \item{ML}{man lift}
//'     \item{MOC}{mobile crusher}
//'     \item{MOS}{mobile screener}
//'     \item{MT}{maintenance truck}
//'     \item{MTX}{mini tracked excavator}
//'     \item{MWL}{mini wheeled pay loader}
//'     \item{PATROLBOAT}{}
//'     \item{PB}{pump boat}
//'     \item{PMT}{prime mover truck}
//'     \item{RG}{road grader}
//'     \item{SB}{service bus}
//'     \item{SP}{service pickup}
//'     \item{ST}{service truck}
//'     \item{TL}{tracked pay loader}
//'     \item{TWL}{tower light}
//'     \item{TX}{tracked excavator}
//'     \item{VC}{vibrating compactor}
//'     \item{WC}{wheeled crane}
//'     \item{WL}{wheeled pay loader}
//'     \item{WM}{welding machine}
//'     \item{WP}{water pump}
//'     \item{WT}{water tank}
//'     \item{WTL}{water lorry}
//'     \item{WX}{wheeled excavator}
//'     \item{YBM}{Yoshida Boring Machine}
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
  std::string testChar;

  // declare getLetterCount constants to avoid repeating in the loop
  String glc_ADT       = getLetterCount( "ADT" );
  String glc_ATV       = getLetterCount( "ATV" );
  String glc_MC        = getLetterCount( "CM" );
  String glc_CT        = getLetterCount( "CT" );
  String glc_DT1       = getLetterCount( "DT" );
  String glc_DT2       = getLetterCount( "D" );
  String glc_DT3       = getLetterCount( "DTUP" );
  String glc_DT4       = getLetterCount( "DTPDT" );
  String glc_DT5       = getLetterCount( "DTT" );
  String glc_DT6       = getLetterCount( "DST" );
  String glc_EQ        = getLetterCount( "EQ" );
  String glc_FL        = getLetterCount( "FL" );
  String glc_FORKLIFT1 = getLetterCount( "FORKLIFT" );
  String glc_FORKLIFT2 = getLetterCount( "FRL" );
  String glc_FORKLIFT3 = getLetterCount( "MFL" );
  String glc_FT        = getLetterCount( "FT" );
  String glc_FTR       = getLetterCount( "FTR" );
  String glc_GS        = getLetterCount( "GS" );
  String glc_GT        = getLetterCount( "GT" );
  String glc_LM        = getLetterCount( "LM" );
  String glc_MDT       = getLetterCount( "MDT" );
  String glc_MOS1      = getLetterCount( "MOS" );
  String glc_MOS2      = getLetterCount( "MS" );
  String glc_MT        = getLetterCount( "MT" );
  String glc_MTX       = getLetterCount( "MTX" );
  String glc_MWL       = getLetterCount( "MWL" );
  String glc_PB        = getLetterCount( "PB" );
  String glc_PMT       = getLetterCount( "PMT" );
  String glc_RG        = getLetterCount( "RG" );
  String glc_SB        = getLetterCount( "SB" );
  String glc_SECURITY  = getLetterCount( "SECURITY" );
  String glc_SP        = getLetterCount( "SP" );
  String glc_ST        = getLetterCount( "ST" );
  String glc_TL        = getLetterCount( "TL" );
  String glc_TX1       = getLetterCount( "TX" );
  String glc_TX2       = getLetterCount( "TZ" );
  String glc_TX3       = getLetterCount( "X" );
  String glc_VC1       = getLetterCount( "V" );
  String glc_VC2       = getLetterCount( "VC" );
  String glc_WC        = getLetterCount( "WC" );
  String glc_WL1       = getLetterCount( "WL" );
  String glc_WL2       = getLetterCount( "W" );
  String glc_WL3       = getLetterCount( "WLRGCT" );
  String glc_WM        = getLetterCount( "WM" );
  String glc_WP1       = getLetterCount( "WP" );
  String glc_WP2       = getLetterCount( "WTLWP" );
  String glc_WT        = getLetterCount( "WT" );
  String glc_WTL       = getLetterCount( "WTL" );
  String glc_WX        = getLetterCount( "WX" );
  String glc_YBM       = getLetterCount( "YBM" );

  for ( R_xlen_t i = 0; i < n; i++ )
  {

    if ( x[i] == NA_STRING )
      continue;

    a = x[i];
    b = x[i];

    a = str_to_upper( a );
    testChar = a.get_cstring();
    b = getLetterCount( b );

    // FIXME
    // Re-arrange according to frequency in data

    if ( std::regex_match( testChar, std::regex( ".*TACOPHIL.*" ) ) )
      equip[i] = "TACOPHIL";

    else if ( std::regex_match( testChar, std::regex( ".*4K.*" ) ) )
      equip[i] = "4K";

    else if ( std::regex_match( testChar, std::regex( ".*PACE.*" ) ) ||
              std::regex_match( testChar, std::regex( ".*PRI?N?CE *ACE.*" ) ) )
      equip[i] = "PACE";

    else if ( std::regex_match( testChar, std::regex( ".*BRGY.*" ) ) ||
              std::regex_match( testChar, std::regex( ".*BGRY.*" ) ) )
      equip[i] = "LGU";

    else if ( std::regex_match( testChar, std::regex( ".*SMCC.*" ) ) )
      equip[i] = "SMCC";

    else if ( std::regex_match( testChar, std::regex( "(^|(.*\\s+))ADMIN.*" ) ) )
      equip[i] = "ADMIN";

    else if ( std::regex_match( testChar, std::regex( ".*CIK?P.*" ) ) )
      equip[i] = "CIP";

    else if ( b == glc_MC )
    {

      if ( std::regex_match( testChar, std::regex( ".*MC.*" ) ) )
        equip[i] = "MC";

      else
        equip[i] = "CM";
    }

    else if ( std::regex_match( testChar, std::regex( ".*MI?XE?R.*" ) ) )
      equip[i] = "CM";

    else if ( b == glc_CT )
      equip[i] = "CT";

    else if ( b == glc_DT1 || b == glc_DT2 || b == glc_DT3 ||
              b == glc_DT4 || b == glc_DT5 || b == glc_DT6 )
    {
      if ( std::regex_match( testChar, std::regex( ".*23[8-9].*" ) ) ||
           std::regex_match( testChar, std::regex( ".*24[0-9].*" ) ) )
        equip[i] = "HDT";

      else
        equip[i] = "DT";
    }


    else if ( b == glc_EQ ||
              std::regex_match( testChar, std::regex( ".*EQ\\s*[[:digit:]].*" ) ) )
      equip[i] = "EQ";

    else if ( std::regex_match( testChar, std::regex( ".*FAST?\\s?CRAFT.*" ) ) )
      equip[i] = "FASTCRAFT";

    else if ( b == glc_FL )
      equip[i] = "FL";

    else if ( b == glc_FORKLIFT1 || b == glc_FORKLIFT2 || b == glc_FORKLIFT3 )
      equip[i] = "FORKLIFT";

    else if ( std::regex_match( testChar, std::regex( "^F\\s*S.*" ) ) ||
              std::regex_match( testChar, std::regex( ".*SQUID.*" ) ) )
      equip[i] = "FS";

    else if ( b == glc_FT )
      equip[i] = "FT";

    else if ( b == glc_FTR )
      equip[i] = "FTR";

    else if ( b == glc_GS ||
              std::regex_match( testChar, std::regex( ".*GEN\\s*SET.*" ) ) ||
              std::regex_match( testChar, std::regex( ".*GENERATOR.*" ) ) ||
              std::regex_match( testChar, std::regex( "^GS\\s+\\d+.*" ) ) )
      equip[i] = "GS";

    else if ( b == glc_GT )
      equip[i] = "GT";

    else if ( std::regex_match( testChar, std::regex( ".*HPK.*" ) ) )
      equip[i] = "HPK";

    else if ( b == glc_LM )
    {

      if ( std::regex_match( testChar, std::regex( ".*L\\s*M.*" ) ) )
        equip[i] = "LM";

      else
        equip[i] = "ML";
    }

    else if ( std::regex_match( testChar, std::regex( ".*MOW?ER.*" ) ) )
      equip[i] = "LM";

    else if ( std::regex_match( testChar, std::regex( ".*MANLIFT.*" ) ) )
      equip[i] = "ML";

    else if ( b == glc_MDT ||
              std::regex_match( testChar, std::regex( ".*MD+T\\b.*" ) ) )
      equip[i] = "MDT";

    else if ( std::regex_match( testChar, std::regex( ".*MA?R?KE?TI?N?G.*" ) ) ||
              std::regex_match( testChar, std::regex( ".*MKGT.*" ) ) )
      equip[i] = "MKTG";

    else if ( std::regex_match( testChar, std::regex( "^CRUSHER.*" ) ) ||
              std::regex_match( testChar, std::regex( "^M.*CRU?SHE?E?R.*" ) ) )
      equip[i] = "MOC";

    else if ( std::regex_match( testChar, std::regex( ".*SCREE?N(E|I)?R?.*" ) ) ||
              ( b == glc_MOS1 ) ||
              ( b == glc_MOS2 ) )
      equip[i] = "MOS";

    else if ( b == glc_MT ||
              std::regex_match( testChar, std::regex( "^MT\\s*\\d+.*" ) ) )
      equip[i] = "MT";

    else if ( b == glc_MTX )
      equip[i] = "MTX";

    else if ( b == glc_MWL )
      equip[i] = "MWL";

    else if ( std::regex_match( testChar, std::regex( ".*NBC.*" ) ) ||
              std::regex_match( testChar, std::regex( "^NB?C?\\s+.*" ) ) )
      equip[i] = "NBI";

    else if ( std::regex_match( testChar, std::regex( ".*PATROL\\s*BOAT.*" ) ) )
      equip[i] = "PATROLBOAT";

    else if ( std::regex_match( testChar, std::regex( ".*POA?LA?RIS.*" ) ) )
      equip[i] = "POLARIS";

    else if ( b == glc_PB ||
              std::regex_match( testChar, std::regex( ".*PUMP\\s*BOAT.*" ) ) )
      equip[i] = "PB";

    else if ( b == glc_PMT )
      equip[i] = "PMT";

    else if ( b == glc_RG )
      equip[i] = "RG";

    else if ( b == glc_SB )
      equip[i] = "SB";

    else if ( std::regex_match( testChar, std::regex( ".*SD(M|N)\\s*C.*" ) ) ||
              std::regex_match( testChar, std::regex( ".*SMDC.*" ) ) )
      equip[i] = "SDMC";

    else if ( b == glc_SECURITY )
      equip[i] = "SECURITY";

    else if ( std::regex_match( testChar, std::regex( ".*SKAFF.*" ) ) )
      equip[i] = "SKAFF";

    else if ( b == glc_SP )
      equip[i] = "SP";

    else if ( b == glc_ST )
      equip[i] = "ST";

    else if ( b == glc_TL )
      equip[i] = "TL";

    else if ( std::regex_match( testChar, std::regex( ".*TRG?C.*" ) ) )
      equip[i] = "TRGC";

    else if ( b == glc_TX1 ||
              std::regex_match( testChar, std::regex( "^TX\\s+.*" ) ) ||
              b == glc_TX2 ||
              b == glc_TX3 )
      equip[i] = "TX";

    else if ( b == glc_VC2 || b == glc_VC1 )
      equip[i] = "VC";

    else if ( b == glc_WC ||
              std::regex_match( testChar, std::regex( ".*CRANE.*" ) ) )
      equip[i] = "WC";

    else if ( b == glc_WM &&
              std::regex_match( testChar, std::regex( ".*WM.*" ) ) )
      equip[i] = "WM";

    else if ( b == glc_WL1 ||
              b == glc_WL2 ||
              b == glc_WL3)
      equip[i] = "WL";

    else if ( b == glc_WP1 ||
              std::regex_match( testChar, std::regex( ".*H2(0|O)\\s*PUMP.*" ) ) ||
              b == glc_WP2 ||
              isSubset( "WATER PUMP", b ) )
      equip[i] = "WP";

    else if ( b == glc_WTL )
    {

      if ( std::regex_match( testChar, std::regex( ".*WTL.*" ) ) ||
           std::regex_match( testChar, std::regex( ".*WLT.*" ) ) )
        equip[i] = "WTL";

      else if ( std::regex_match( testChar, std::regex( ".*TWL.*" ) ) )
        equip[i] = "TWL";

      else
        equip[i] = "ZZZ";
    }

    else if ( b == glc_WT )
      equip[i] = "WT";

    else if ( std::regex_match( testChar, std::regex( ".*WX.*" ) ) ||
              b == glc_WX )
      equip[i] = "WX";

    else if ( b == glc_YBM ||
              std::regex_match( testChar, std::regex( ".*YBB?M.*" ) ) )
      equip[i] = "YBM";

    else if ( std::regex_match( testChar, std::regex( ".*VOLVO\\s*DT.*" ) ) )
      equip[i] = "DT";

    else if ( std::regex_match( testChar, std::regex( "^MC\\s*\\d+.*" ) ) )
      equip[i] = "MC";

    else if ( b == glc_ATV )
      equip[i] = "ATV";

    else if ( b == glc_ADT )
      equip[i] = "ADT";

    // Exotic cases

    else if ( testChar == "219" )
      equip[i] = "DT";

    else if ( testChar == "FV VI" )
      equip[i] = "FS";

    else if ( testChar == "R6 11" )
      equip[i] = "RG";

    else
      equip[i] = "ZZZ";
  }

  return equip;
}
