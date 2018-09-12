#include <RcppArmadillo.h>
// [[Rcpp::depends(RcppArmadillo)]]

using namespace Rcpp;
using namespace arma;

//' Interpolate
//'
//' This is used in a spatial study for limonite face samples.
//'
//' @param modelT data frame of prediction model
//' @param sample data frame of sample coordinates
//' @param radius numeric value representing buffer radius
//' @param vario List contaning variogram models
//' @return a dataframe containing the following:
//'   \enumerate{
//'     \item interpolated grade of element
//'     \item interpolation variance
//'     \item interpolation variance using absolute value of kriging weight
//'   }
//' @export
// [[Rcpp::export]]
DataFrame predictVars( const DataFrame  modelT,
                       const DataFrame& sample,
                       const double     radius,
                       const List&      vario )
{

  Environment tmctools      = Environment::namespace_env( "tmctools" );
  Function    variogramLine = tmctools["variogramLine"];

  // Copy model values
  DataFrame     model     = clone( modelT );
  IntegerVector mId       = model["id"      ];
  NumericVector mNi       = model["Ni"      ];
  NumericVector mFe       = model["Fe"      ];
  NumericVector mCo       = model["Co"      ];
  NumericVector mMg       = model["Mg"      ];
  NumericVector mSi       = model["Si"      ];
  NumericVector mCr       = model["Cr"      ];
  NumericVector mAl       = model["Al"      ];
  NumericVector mNiVar    = model["NiVar"   ];
  NumericVector mFeVar    = model["FeVar"   ];
  NumericVector mCoVar    = model["CoVar"   ];
  NumericVector mMgVar    = model["MgVar"   ];
  NumericVector mSiVar    = model["SiVar"   ];
  NumericVector mCrVar    = model["CrVar"   ];
  NumericVector mAlVar    = model["AlVar"   ];
  NumericVector mNiVarAbs = model["NiVarAbs"];
  NumericVector mFeVarAbs = model["FeVarAbs"];
  NumericVector mCoVarAbs = model["CoVarAbs"];
  NumericVector mMgVarAbs = model["MgVarAbs"];
  NumericVector mSiVarAbs = model["SiVarAbs"];
  NumericVector mCrVarAbs = model["CrVarAbs"];
  NumericVector mAlVarAbs = model["AlVarAbs"];
  IntegerVector mNiN      = model["NiN"     ];
  IntegerVector mFeN      = model["FeN"     ];
  IntegerVector mCoN      = model["CoN"     ];
  IntegerVector mMgN      = model["MgN"     ];
  IntegerVector mSiN      = model["SiN"     ];
  IntegerVector mCrN      = model["CrN"     ];
  IntegerVector mAlN      = model["AlN"     ];
  NumericVector mX        = model["X"       ];
  NumericVector mY        = model["Y"       ];

  // Copy sample values
  NumericVector sNi = sample["Ni"];
  NumericVector sFe = sample["Fe"];
  NumericVector sCo = sample["Co"];
  NumericVector sMg = sample["Mg"];
  NumericVector sSi = sample["Si"];
  NumericVector sCr = sample["Cr"];
  NumericVector sAl = sample["Al"];
  NumericVector sX  = sample["X" ];
  NumericVector sY  = sample["Y" ];

  // Initialize subset of sample values
  NumericVector subElement;
  NumericVector subX;
  NumericVector subY;

  NumericVector distFromPt;
  NumericVector krigWeight;

  List varFit;

  for ( unsigned int i = 0; i < mId.length(); i++ )
  {

    // Compute distance of each sample from the predicted point
    distFromPt = sqrt( pow( sX - mX[i], 2 ) + pow( sY - mY[i], 2 ) );

    // Ni Prediction -----------------------------------------------------------

    // Select non-NA sample points within a radius around the point to be predicted
    subX       =  sX[( distFromPt < radius ) & !is_na( sNi )];
    subY       =  sY[( distFromPt < radius ) & !is_na( sNi )];
    subElement = sNi[( distFromPt < radius ) & !is_na( sNi )];

    mat distMatNi( subX.length() + 1, subX.length() + 1 );

    if ( subX.length() > 1 )
    {

      // Record number of samples within radius
      mNiN[i] = subX.length();

      // Get fitted variogram model
      varFit = vario["Ni"];

      // Compute for distance matrix of inter-sample points
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        for ( unsigned int b = 0; b < subX.length(); b++ )
        {
          distMatNi( a, b ) = sqrt( pow( subX[a] - subX[b], 2 ) + pow( subY[a] - subY[b], 2 ) );
        }
      }
      // Convert distance to semi-variance
      distMatNi = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatNi ) ) );
      colvec cv( subX.length() + 1, fill::ones);
      rowvec rv( subX.length() + 1, fill::ones);
      distMatNi.row( subX.length() ) = rv;
      distMatNi.col( subX.length() ) = cv;
      distMatNi( subX.length(), subX.length() ) = 0;

      // Compute for distance matrix of sample point to point-to-be-predicted
      mat distMatModNi( subX.length() + 1, 1 );
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        distMatModNi( a, 0 ) = sqrt( pow( subX[a] - mX[i], 2 ) + pow( subY[a] - mY[i], 2 ) );
      }
      // Convert distance to semi-variance
      distMatModNi = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatModNi ) ) );
      distMatModNi( subX.length(), 0 ) = 1;

      // Solve for kriging weights
      vec solveKrig( subX.length() + 1 );
      solveKrig  = solve( distMatNi, distMatModNi );
      krigWeight = solveKrig.subvec( 0, subX.length() - 1 );

      // Record Predicted Value
      mNi[i] = sum( krigWeight * subElement );

      // Record Interpolation Variance
      mNiVar[i] = sum( krigWeight * ( pow( mNi[i] - subElement, 2 ) ) );

      // Record Interpolation Variance using absolute kriging weight
      mNiVarAbs[i] = sum( abs( krigWeight ) * ( pow( mNi[i] - subElement, 2 ) ) );

    }

    else if ( subX.length() > 0 )
    {
      mNi[i]  = subElement[0];
      mNiN[i] = 1;
    }

    else
    {
      mNiN[i] = 0;
    }

    // Fe Prediction -----------------------------------------------------------

    // Select non-NA sample points within a radius around the point to be predicted
    subX       =  sX[( distFromPt < radius ) & !is_na( sFe )];
    subY       =  sY[( distFromPt < radius ) & !is_na( sFe )];
    subElement = sFe[( distFromPt < radius ) & !is_na( sFe )];

    mat distMatFe( subX.length() + 1, subX.length() + 1 );

    if ( subX.length() > 1 )
    {

      // Record number of samples within radius
      mFeN[i] = subX.length();

      // Get fitted variogram model
      varFit = vario["Fe"];

      // Compute for distance matrix of inter-sample points
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        for ( unsigned int b = 0; b < subX.length(); b++ )
        {
          distMatFe( a, b ) = sqrt( pow( subX[a] - subX[b], 2 ) + pow( subY[a] - subY[b], 2 ) );
        }
      }
      // Convert distance to semi-variance
      distMatFe = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatFe ) ) );
      colvec cv( subX.length() + 1, fill::ones);
      rowvec rv( subX.length() + 1, fill::ones);
      distMatFe.row( subX.length() ) = rv;
      distMatFe.col( subX.length() ) = cv;
      distMatFe( subX.length(), subX.length() ) = 0;

      // Compute for distance matrix of sample point to point-to-be-predicted
      mat distMatModFe( subX.length() + 1, 1 );
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        distMatModFe( a, 0 ) = sqrt( pow( subX[a] - mX[i], 2 ) + pow( subY[a] - mY[i], 2 ) );
      }
      // Convert distance to semi-variance
      distMatModFe = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatModFe ) ) );
      distMatModFe( subX.length(), 0 ) = 1;

      // Solve for kriging weights
      vec solveKrig( subX.length() + 1 );
      solveKrig  = solve( distMatFe, distMatModFe );
      krigWeight = solveKrig.subvec( 0, subX.length() - 1 );

      // Record Predicted Value
      mFe[i] = sum( krigWeight * subElement );

      // Record Interpolation Variance
      mFeVar[i] = sum( krigWeight * ( pow( mFe[i] - subElement, 2 ) ) );

      // Record Interpolation Variance using absolute kriging weight
      mFeVarAbs[i] = sum( abs( krigWeight ) * ( pow( mFe[i] - subElement, 2 ) ) );

    }

    else if ( subX.length() > 0 )
    {
      mFe[i]  = subElement[0];
      mFeN[i] = 1;
    }

    else
    {
      mFeN[i] = 0;
    }

    // Co Prediction -----------------------------------------------------------

    // Select non-NA sample points within a radius around the point to be predicted
    subX       =  sX[( distFromPt < radius ) & !is_na( sCo )];
    subY       =  sY[( distFromPt < radius ) & !is_na( sCo )];
    subElement = sCo[( distFromPt < radius ) & !is_na( sCo )];

    mat distMatCo( subX.length() + 1, subX.length() + 1 );

    if ( subX.length() > 1 )
    {

      // Record number of samples within radius
      mCoN[i] = subX.length();

      // Get fitted variogram model
      varFit = vario["Co"];

      // Compute for distance matrix of inter-sample points
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        for ( unsigned int b = 0; b < subX.length(); b++ )
        {
          distMatCo( a, b ) = sqrt( pow( subX[a] - subX[b], 2 ) + pow( subY[a] - subY[b], 2 ) );
        }
      }
      // Convert distance to semi-variance
      distMatCo = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatCo ) ) );
      colvec cv( subX.length() + 1, fill::ones);
      rowvec rv( subX.length() + 1, fill::ones);
      distMatCo.row( subX.length() ) = rv;
      distMatCo.col( subX.length() ) = cv;
      distMatCo( subX.length(), subX.length() ) = 0;

      // Compute for distance matrix of sample point to point-to-be-predicted
      mat distMatModCo( subX.length() + 1, 1 );
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        distMatModCo( a, 0 ) = sqrt( pow( subX[a] - mX[i], 2 ) + pow( subY[a] - mY[i], 2 ) );
      }
      // Convert distance to semi-variance
      distMatModCo = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatModCo ) ) );
      distMatModCo( subX.length(), 0 ) = 1;

      // Solve for kriging weights
      vec solveKrig( subX.length() + 1 );
      solveKrig  = solve( distMatCo, distMatModCo );
      krigWeight = solveKrig.subvec( 0, subX.length() - 1 );

      // Record Predicted Value
      mCo[i] = sum( krigWeight * subElement );

      // Record Interpolation Variance
      mCoVar[i] = sum( krigWeight * ( pow( mCo[i] - subElement, 2 ) ) );

      // Record Interpolation Variance using absolute kriging weight
      mCoVarAbs[i] = sum( abs( krigWeight ) * ( pow( mCo[i] - subElement, 2 ) ) );

    }

    else if ( subX.length() > 0 )
    {
      mCo[i]  = subElement[0];
      mCoN[i] = 1;
    }

    else
    {
      mCoN[i] = 0;
    }

    // Mg Prediction -----------------------------------------------------------

    // Select non-NA sample points within a radius around the point to be predicted
    subX       =  sX[( distFromPt < radius ) & !is_na( sMg )];
    subY       =  sY[( distFromPt < radius ) & !is_na( sMg )];
    subElement = sMg[( distFromPt < radius ) & !is_na( sMg )];

    mat distMatMg( subX.length() + 1, subX.length() + 1 );

    if ( subX.length() > 1 )
    {

      // Record number of samples within radius
      mMgN[i] = subX.length();

      // Get fitted variogram model
      varFit = vario["Mg"];

      // Compute for distance matrix of inter-sample points
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        for ( unsigned int b = 0; b < subX.length(); b++ )
        {
          distMatMg( a, b ) = sqrt( pow( subX[a] - subX[b], 2 ) + pow( subY[a] - subY[b], 2 ) );
        }
      }
      // Convert distance to semi-variance
      distMatMg = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatMg ) ) );
      colvec cv( subX.length() + 1, fill::ones);
      rowvec rv( subX.length() + 1, fill::ones);
      distMatMg.row( subX.length() ) = rv;
      distMatMg.col( subX.length() ) = cv;
      distMatMg( subX.length(), subX.length() ) = 0;

      // Compute for distance matrix of sample point to point-to-be-predicted
      mat distMatModMg( subX.length() + 1, 1 );
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        distMatModMg( a, 0 ) = sqrt( pow( subX[a] - mX[i], 2 ) + pow( subY[a] - mY[i], 2 ) );
      }
      // Convert distance to semi-variance
      distMatModMg = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatModMg ) ) );
      distMatModMg( subX.length(), 0 ) = 1;

      // Solve for kriging weights
      vec solveKrig( subX.length() + 1 );
      solveKrig  = solve( distMatMg, distMatModMg );
      krigWeight = solveKrig.subvec( 0, subX.length() - 1 );

      // Record Predicted Value
      mMg[i] = sum( krigWeight * subElement );

      // Record Interpolation Variance
      mMgVar[i] = sum( krigWeight * ( pow( mMg[i] - subElement, 2 ) ) );

      // Record Interpolation Variance using absolute kriging weight
      mMgVarAbs[i] = sum( abs( krigWeight ) * ( pow( mMg[i] - subElement, 2 ) ) );

    }

    else if ( subX.length() > 0 )
    {
      mMg[i]  = subElement[0];
      mMgN[i] = 1;
    }

    else
    {
      mMgN[i] = 0;
    }

    // Si Prediction -----------------------------------------------------------

    // Select non-NA sample points within a radius around the point to be predicted
    subX       =  sX[( distFromPt < radius ) & !is_na( sSi )];
    subY       =  sY[( distFromPt < radius ) & !is_na( sSi )];
    subElement = sSi[( distFromPt < radius ) & !is_na( sSi )];

    mat distMatSi( subX.length() + 1, subX.length() + 1 );

    if ( subX.length() > 1 )
    {

      // Record number of samples within radius
      mSiN[i] = subX.length();

      // Get fitted variogram model
      varFit = vario["Si"];

      // Compute for distance matrix of inter-sample points
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        for ( unsigned int b = 0; b < subX.length(); b++ )
        {
          distMatSi( a, b ) = sqrt( pow( subX[a] - subX[b], 2 ) + pow( subY[a] - subY[b], 2 ) );
        }
      }
      // Convert distance to semi-variance
      distMatSi = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatSi ) ) );
      colvec cv( subX.length() + 1, fill::ones);
      rowvec rv( subX.length() + 1, fill::ones);
      distMatSi.row( subX.length() ) = rv;
      distMatSi.col( subX.length() ) = cv;
      distMatSi( subX.length(), subX.length() ) = 0;

      // Compute for distance matrix of sample point to point-to-be-predicted
      mat distMatModSi( subX.length() + 1, 1 );
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        distMatModSi( a, 0 ) = sqrt( pow( subX[a] - mX[i], 2 ) + pow( subY[a] - mY[i], 2 ) );
      }
      // Convert distance to semi-variance
      distMatModSi = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatModSi ) ) );
      distMatModSi( subX.length(), 0 ) = 1;

      // Solve for kriging weights
      vec solveKrig( subX.length() + 1 );
      solveKrig  = solve( distMatSi, distMatModSi );
      krigWeight = solveKrig.subvec( 0, subX.length() - 1 );

      // Record Predicted Value
      mSi[i] = sum( krigWeight * subElement );

      // Record Interpolation Variance
      mSiVar[i] = sum( krigWeight * ( pow( mSi[i] - subElement, 2 ) ) );

      // Record Interpolation Variance using absolute kriging weight
      mSiVarAbs[i] = sum( abs( krigWeight ) * ( pow( mSi[i] - subElement, 2 ) ) );

    }

    else if ( subX.length() > 0 )
    {
      mSi[i]  = subElement[0];
      mSiN[i] = 1;
    }

    else
    {
      mSiN[i] = 0;
    }

    // Cr Prediction -----------------------------------------------------------

    // Select non-NA sample points within a radius around the point to be predicted
    subX       =  sX[( distFromPt < radius ) & !is_na( sCr )];
    subY       =  sY[( distFromPt < radius ) & !is_na( sCr )];
    subElement = sCr[( distFromPt < radius ) & !is_na( sCr )];

    mat distMatCr( subX.length() + 1, subX.length() + 1 );

    if ( subX.length() > 1 )
    {

      // Record number of samples within radius
      mCrN[i] = subX.length();

      // Get fitted variogram model
      varFit = vario["Cr"];

      // Compute for distance matrix of inter-sample points
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        for ( unsigned int b = 0; b < subX.length(); b++ )
        {
          distMatCr( a, b ) = sqrt( pow( subX[a] - subX[b], 2 ) + pow( subY[a] - subY[b], 2 ) );
        }
      }
      // Convert distance to semi-variance
      distMatCr = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatCr ) ) );
      colvec cv( subX.length() + 1, fill::ones);
      rowvec rv( subX.length() + 1, fill::ones);
      distMatCr.row( subX.length() ) = rv;
      distMatCr.col( subX.length() ) = cv;
      distMatCr( subX.length(), subX.length() ) = 0;

      // Compute for distance matrix of sample point to point-to-be-predicted
      mat distMatModCr( subX.length() + 1, 1 );
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        distMatModCr( a, 0 ) = sqrt( pow( subX[a] - mX[i], 2 ) + pow( subY[a] - mY[i], 2 ) );
      }
      // Convert distance to semi-variance
      distMatModCr = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatModCr ) ) );
      distMatModCr( subX.length(), 0 ) = 1;

      // Solve for kriging weights
      vec solveKrig( subX.length() + 1 );
      solveKrig  = solve( distMatCr, distMatModCr );
      krigWeight = solveKrig.subvec( 0, subX.length() - 1 );

      // Record Predicted Value
      mCr[i] = sum( krigWeight * subElement );

      // Record Interpolation Variance
      mCrVar[i] = sum( krigWeight * ( pow( mCr[i] - subElement, 2 ) ) );

      // Record Interpolation Variance using absolute kriging weight
      mCrVarAbs[i] = sum( abs( krigWeight ) * ( pow( mCr[i] - subElement, 2 ) ) );

    }

    else if ( subX.length() > 0 )
    {
      mCr[i]  = subElement[0];
      mCrN[i] = 1;
    }

    else
    {
      mCrN[i] = 0;
    }

    // Al Prediction -----------------------------------------------------------

    // Select non-NA sample points within a radius around the point to be predicted
    subX       =  sX[( distFromPt < radius ) & !is_na( sAl )];
    subY       =  sY[( distFromPt < radius ) & !is_na( sAl )];
    subElement = sAl[( distFromPt < radius ) & !is_na( sAl )];

    mat distMatAl( subX.length() + 1, subX.length() + 1 );

    if ( subX.length() > 1 )
    {

      // Record number of samples within radius
      mAlN[i] = subX.length();

      // Get fitted variogram model
      varFit = vario["Al"];

      // Compute for distance matrix of inter-sample points
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        for ( unsigned int b = 0; b < subX.length(); b++ )
        {
          distMatAl( a, b ) = sqrt( pow( subX[a] - subX[b], 2 ) + pow( subY[a] - subY[b], 2 ) );
        }
      }
      // Convert distance to semi-variance
      distMatAl = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatAl ) ) );
      colvec cv( subX.length() + 1, fill::ones);
      rowvec rv( subX.length() + 1, fill::ones);
      distMatAl.row( subX.length() ) = rv;
      distMatAl.col( subX.length() ) = cv;
      distMatAl( subX.length(), subX.length() ) = 0;

      // Compute for distance matrix of sample point to point-to-be-predicted
      mat distMatModAl( subX.length() + 1, 1 );
      for ( unsigned int a = 0; a < subX.length(); a++ )
      {
        distMatModAl( a, 0 ) = sqrt( pow( subX[a] - mX[i], 2 ) + pow( subY[a] - mY[i], 2 ) );
      }
      // Convert distance to semi-variance
      distMatModAl = as<mat>( variogramLine( varFit["fit"], Named( "dist_vector", distMatModAl ) ) );
      distMatModAl( subX.length(), 0 ) = 1;

      // Solve for kriging weights
      vec solveKrig( subX.length() + 1 );
      solveKrig  = solve( distMatAl, distMatModAl );
      krigWeight = solveKrig.subvec( 0, subX.length() - 1 );

      // Record Predicted Value
      mAl[i] = sum( krigWeight * subElement );

      // Record Interpolation Variance
      mAlVar[i] = sum( krigWeight * ( pow( mAl[i] - subElement, 2 ) ) );

      // Record Interpolation Variance using absolute kriging weight
      mAlVarAbs[i] = sum( abs( krigWeight ) * ( pow( mAl[i] - subElement, 2 ) ) );

    }

    else if ( subX.length() > 0 )
    {
      mAl[i]  = subElement[0];
      mAlN[i] = 1;
    }

    else
    {
      mAlN[i] = 0;
    }

  }

  return model;
}
