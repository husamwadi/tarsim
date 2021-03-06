//============================================================================
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2014 National Technology & Engineering Solutions of Sandia, LLC (NTESS).
//  Copyright 2014 UT-Battelle, LLC.
//  Copyright 2014 Los Alamos National Security.
//
//  Under the terms of Contract DE-NA0003525 with NTESS,
//  the U.S. Government retains certain rights in this software.
//
//  Under the terms of Contract DE-AC52-06NA25396 with Los Alamos National
//  Laboratory (LANL), the U.S. Government retains certain rights in
//  this software.
//============================================================================
#include <vtkm/cont/ArrayHandleCast.h>
#include <vtkm/cont/DataSet.h>
#include <vtkm/cont/DeviceAdapterAlgorithm.h>
#include <vtkm/cont/Timer.h>
#include <vtkm/io/reader/VTKDataSetReader.h>
#include <vtkm/io/writer/VTKDataSetWriter.h>

#include <vtkm/worklet/CosmoTools.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

using DeviceAdapter = VTKM_DEFAULT_DEVICE_ADAPTER_TAG;

void TestCosmoCenterFinder(const char* fileName)
{
  std::cout << std::endl
            << "Testing Cosmology MBP Center Finder Filter on one halo " << fileName << std::endl;

  // Open the file for reading
  std::ifstream inFile(fileName);
  if (inFile.fail())
  {
    std::cout << "File does not exist " << fileName << std::endl;
    return;
  }

  // Read in number of particles and locations
  int nParticles;
  inFile >> nParticles;

  float* xLocation = new float[nParticles];
  float* yLocation = new float[nParticles];
  float* zLocation = new float[nParticles];
  std::cout << "Running MBP on " << nParticles << std::endl;

  for (vtkm::Id p = 0; p < nParticles; p++)
  {
    inFile >> xLocation[p] >> yLocation[p] >> zLocation[p];
  }

  vtkm::cont::ArrayHandle<vtkm::Float32> xLocArray =
    vtkm::cont::make_ArrayHandle<vtkm::Float32>(xLocation, nParticles);
  vtkm::cont::ArrayHandle<vtkm::Float32> yLocArray =
    vtkm::cont::make_ArrayHandle<vtkm::Float32>(yLocation, nParticles);
  vtkm::cont::ArrayHandle<vtkm::Float32> zLocArray =
    vtkm::cont::make_ArrayHandle<vtkm::Float32>(zLocation, nParticles);

  // Output MBP particleId pairs array
  vtkm::Pair<vtkm::Id, vtkm::Float32> nxnResult;
  vtkm::Pair<vtkm::Id, vtkm::Float32> mxnResult;

  vtkm::cont::Timer<DeviceAdapter> total;
  vtkm::cont::Timer<DeviceAdapter> timer;

  // Create the worklet and run it
  vtkm::Float32 particleMass = 1.08413e+09f;

  vtkm::worklet::CosmoTools cosmoTools;
  cosmoTools.RunMBPCenterFinderNxN(
    xLocArray, yLocArray, zLocArray, nParticles, particleMass, nxnResult, DeviceAdapter());
  vtkm::Float64 nxnTime = timer.GetElapsedTime();

  std::cout << "**** NxN MPB = " << nxnResult.first << "  potential = " << nxnResult.second
            << std::endl;
  std::cout << "**** Time for NxN: " << nxnTime << std::endl;

  timer.Reset();
  cosmoTools.RunMBPCenterFinderMxN(
    xLocArray, yLocArray, zLocArray, nParticles, particleMass, mxnResult, DeviceAdapter());
  vtkm::Float64 estTime = timer.GetElapsedTime();

  std::cout << "**** MxN MPB = " << mxnResult.first << "  potential = " << mxnResult.second
            << std::endl;
  std::cout << "**** Time for MxN: " << estTime << std::endl;

  if (nxnResult.first == mxnResult.first)
    std::cout << "FOUND CORRECT PARTICLE " << mxnResult.first << " with potential "
              << nxnResult.second << std::endl;
  else
    std::cout << "ERROR DID NOT FIND SAME PARTICLE" << std::endl;

  xLocArray.ReleaseResources();
  yLocArray.ReleaseResources();
  zLocArray.ReleaseResources();

  delete[] xLocation;
  delete[] yLocation;
  delete[] zLocation;
}

/////////////////////////////////////////////////////////////////////
//
// Form of the input file in ASCII
// Line 1: number of particles in the file
// Line 2+: (float) xLoc (float) yLoc (float) zLoc
//
// CosmoCenterFinder data.cosmotools
//
/////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
  if (argc < 2)
  {
    std::cout << "Usage: " << std::endl << "$ " << argv[0] << " <input_file>" << std::endl;
    return 1;
  }

  TestCosmoCenterFinder(argv[1]);

  return 0;
}
