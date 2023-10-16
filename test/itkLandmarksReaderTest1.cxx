/*=========================================================================

  Program:   Lesion Sizing Toolkit
  Module:    itkLandmarksReaderTest1.cxx

  Copyright (c) Kitware Inc.
  All rights reserved.
  See Copyright.txt or https://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "itkLandmarksReader.h"
#include "itkSpatialObject.h"
#include "itkSpatialObjectReader.h"
#include "itkTestingMacros.h"


int
itkLandmarksReaderTest1(int argc, char * argv[])
{
  if (argc < 2)
  {
    std::cerr << "Missing parameters." << std::endl;
    std::cerr << "Usage: " << argv[0];
    std::cerr << " landmarksFile ";
    return EXIT_FAILURE;
  }


  constexpr unsigned int Dimension = 3;

  using InputSpatialObjectType = itk::LandmarkSpatialObject<Dimension>;

  //
  //  Reading the landmarks file with the itkLandmarksReader.
  //
  using LandmarksReaderType = itk::LandmarksReader<Dimension>;

  LandmarksReaderType::Pointer landmarksReader = LandmarksReaderType::New();

  std::string inputFileName = argv[1];

  landmarksReader->SetFileName(inputFileName);
  landmarksReader->SetFileName(argv[1]);

  std::string recoveredFileName = landmarksReader->GetFileName();

  if (recoveredFileName != inputFileName)
  {
    std::cerr << "Error in Set/GetFileName()" << std::endl;
    return EXIT_FAILURE;
  }

  ITK_TRY_EXPECT_NO_EXCEPTION(landmarksReader->Update());


  InputSpatialObjectType::ConstPointer landmarkSpatialObject1 = landmarksReader->GetOutput();

  //
  // Reading the landmarks file by using direct ITK classes
  //
  using SpatialObjectReaderType = itk::SpatialObjectReader<3, unsigned short>;

  SpatialObjectReaderType::Pointer landmarkPointsReader = SpatialObjectReaderType::New();

  ITK_EXERCISE_BASIC_OBJECT_METHODS(landmarkPointsReader, SpatialObjectReader, Object);

  landmarkPointsReader->SetFileName(argv[1]);
  landmarkPointsReader->Update();

  SpatialObjectReaderType::GroupPointer group = landmarkPointsReader->GetGroup();

  if (!group)
  {
    std::cerr << "Test failed!" << std::endl;
    std::cerr << "No Group." << std::endl;
    return EXIT_FAILURE;
  }

  std::cout << "Number of children in the group:" << group->GetNumberOfChildren(1) << std::endl;

  using ObjectListType = SpatialObjectReaderType::GroupType::ObjectListType;

  ObjectListType * groupChildren = group->GetChildren(999999);

  ObjectListType::const_iterator spatialObjectItr = groupChildren->begin();


  const InputSpatialObjectType * landmarkSpatialObject2 = nullptr;

  while (spatialObjectItr != groupChildren->end())
  {
    std::string objectName = (*spatialObjectItr)->GetTypeName();
    if (objectName == "LandmarkSpatialObject")
    {
      landmarkSpatialObject2 = dynamic_cast<const InputSpatialObjectType *>(spatialObjectItr->GetPointer());
    }
    spatialObjectItr++;
  }

  using LandmarkPointLis