/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkDescoteauxSheetnessImageFilter_h
#define itkDescoteauxSheetnessImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkMath.h"

namespace itk
{

/** \class DescoteauxSheetnessImageFilter
 *
 * \brief Computes a measure of Sheetness from the Hessian Eigenvalues
 *
 * Based on the "Sheetness" measure proposed by Decouteaux et. al.
 *
 * M.Descoteaux, M.Audette, K.Chinzei, el al.:
 * "Bone enhancement filtering: Application to sinus bone segmentation
 *  and simulation of pituitary surgery."
 *  In: MICCAI.  (2005) 9-16
 *
 * \ingroup IntensityImageFilters  Multithreaded
 * \ingroup LesionSizingToolkit
 */
namespace Function
{

template <typename TInput, typename TOutput>
class Sheetness
{
public:
  Sheetness()
  {
    m_Alpha = 0.5; // suggested value in the paper
    m_Gamma = 0.5; // suggested value in the paper;
    m_C = 1.0;
    m_DetectBrightSheets = true;
  }
  ~Sheetness() = default;
  bool
  operator!=(const Sheetness &)