#include <fstream>
#include <iostream>
#include <sstream>
#include <vtk/vtkPLYReader.h>
#include <vtk/vtkPolyData.h>
#include <vtk/vtkSmartPointer.h>
#include <vtk/vtkTriangleFilter.h>
#include "utils.h"

// Reads face data of PLY file and writes into text and returns the number of
// vertices.
// [[Rcpp::export]]
int readPlyVTK( const std::string& plyFile )
{
  vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
  reader->SetFileName( plyFile.c_str() );
  vtkSmartPointer<vtkPolyData> mesh;
  vtkSmartPointer<vtkTriangleFilter> triangles = vtkSmartPointer<vtkTriangleFilter>::New();
  triangles->SetInputConnection( reader->GetOutputPort() );
  triangles->Update();
  mesh = triangles->GetOutput();
  std::ofstream ply;
  ply.open( "vtk.ply" );
  vtkCell* face;
  for( vtkIdType i = 0; i < mesh->GetNumberOfCells(); i++ )
  {
    face = mesh->GetCell( i );
    ply << face->GetNumberOfPoints();
    for( int j = 0; j < face->GetNumberOfPoints(); j++ )
      ply << " " << face->GetPointId( j );
    ply << "\n";
  }
  ply.close();
  writePlyHeader( "text.ply", mesh->GetNumberOfPoints(), mesh->GetNumberOfCells() );
  return mesh->GetNumberOfPoints();
}

