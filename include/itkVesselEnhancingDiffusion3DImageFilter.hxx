
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
#ifndef itkVesselEnhancingDiffusion3DImageFilter_hxx
#define itkVesselEnhancingDiffusion3DImageFilter_hxx


#include "itkCastImageFilter.h"
#include "itkConstShapedNeighborhoodIterator.h"
#include "itkHessianRecursiveGaussianImageFilter.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkMinimumMaximumImageFilter.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkNumericTraits.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

#include <iostream>

namespace itk
{

// constructor
template <typename PixelType, unsigned int NDimension>
VesselEnhancingDiffusion3DImageFilter<PixelType, NDimension>::VesselEnhancingDiffusion3DImageFilter()
  : m_TimeStep(NumericTraits<Precision>::Zero)

{
  this->SetNumberOfRequiredInputs(1);
}

// printself for debugging
template <typename PixelType, unsigned int NDimension>
void
VesselEnhancingDiffusion3DImageFilter<PixelType, NDimension>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "TimeStep                 : " << m_TimeStep << std::endl;
  os << indent << "Iterations             : " << m_Iterations << std::endl;
  os << indent << "RecalculateVesselness      : " << m_RecalculateVesselness << std::endl;
  os << indent << "Scales     : ";
  for (float m_Scale : m_Scales)
  {
    os << m_Scale << " ";
  }
  os << std::endl;
  os << indent << "Epsilon                 : " << m_Epsilon << std::endl;
  os << indent << "Omega                   : " << m_Omega << std::endl;
  os << indent << "Sensitivity             : " << m_Sensitivity << std::endl;
  os << indent << "DarkObjectLightBackground  : " << m_DarkObjectLightBackground << std::endl;
}
// singleiter
template <typename PixelType, unsigned int NDimension>
void
VesselEnhancingDiffusion3DImageFilter<PixelType, NDimension>::VED3DSingleIteration(
  typename PrecisionImageType::Pointer ci)
{
  bool rec(false);
  if ((m_CurrentIteration == 1) || (m_RecalculateVesselness == 0) ||
      (m_CurrentIteration % m_RecalculateVesselness == 0))
  {
    rec = true;
    if (m_Verbose)
    {
      std::cout << "v ";
      std::cout.flush();
    }
    MaxVesselResponse(ci);
    DiffusionTensor();
  }
  if (m_Verbose)
  {
    if (!rec)
    {
      std::cout << ". ";
      std::cout.flush();
    }
  }


  // calculate d = nonlineardiffusion(ci)
  // using 3x3x3 stencil, afterwards copy
  // result from d back to ci
  typename PrecisionImageType::Pointer d = PrecisionImageType::New();
  d->SetOrigin(ci->GetOrigin());
  d->SetSpacing(ci->GetSpacing());
  d->SetDirection(ci->GetDirection());
  d->SetRegions(ci->GetLargestPossibleRegion());
  d->Allocate();
  d->FillBuffer(NumericTraits<Precision>::Zero);

  // shapedneighborhood iter, zeroflux boundary condition
  // division into faces and inner region
  using BT = ZeroFluxNeumannBoundaryCondition<PrecisionImageType>;
  using NT = ConstShapedNeighborhoodIterator<PrecisionImageType, BT>;
  using FT = typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<PrecisionImageType>;
  BT                      b;
  typename NT::RadiusType r;
  r.Fill(1);


  // offsets
  const typename NT::OffsetType oxp = { { 1, 0, 0 } };
  const typename NT::OffsetType oxm = { { -1, 0, 0 } };
  const typename NT::OffsetType oyp = { { 0, 1, 0 } };
  const typename NT::OffsetType oym = { { 0, -1, 0 } };
  const typename NT::OffsetType ozp = { { 0, 0, 1 } };
  const typename NT::OffsetType ozm = { { 0, 0, -1 } };

  const typename NT::OffsetType oxpyp = { { 1, 1, 0 } };
  const typename NT::OffsetType oxmym = { { -1, -1, 0 } };
  const typename NT::OffsetType oxpym = { { 1, -1, 0 } };
  const typename NT::OffsetType oxmyp = { { -1, 1, 0 } };

  const typename NT::OffsetType oxpzp = { { 1, 0, 1 } };
  const typename NT::OffsetType oxmzm = { { -1, 0, -1 } };
  const typename NT::OffsetType oxpzm = { { 1, 0, -1 } };
  const typename NT::OffsetType oxmzp = { { -1, 0, 1 } };

  const typename NT::OffsetType oypzp = { { 0, 1, 1 } };
  const typename NT::OffsetType oymzm = { { 0, -1, -1 } };
  const typename NT::OffsetType oypzm = { { 0, 1, -1 } };
  const typename NT::OffsetType oymzp = { { 0, -1, 1 } };

  // fixed weights (timers)
  const typename PrecisionImageType::SpacingType ispacing = ci->GetSpacing();
  const Precision                                rxx = m_TimeStep / (2.0 * ispacing[0] * ispacing[0]);
  const Precision                                ryy = m_TimeStep / (2.0 * ispacing[1] * ispacing[1]);
  const Precision                                rzz = m_TimeStep / (2.0 * ispacing[2] * ispacing[2]);
  const Precision                                rxy = m_TimeStep / (4.0 * ispacing[0] * ispacing[1]);
  const Precision                                rxz = m_TimeStep / (4.0 * ispacing[0] * ispacing[2]);
  const Precision                                ryz = m_TimeStep / (4.0 * ispacing[1] * ispacing[2]);

  // faces
  FT                        fc;
  typename FT::FaceListType fci = fc(ci, d->GetLargestPossibleRegion(), r);
  typename FT::FaceListType fxx = fc(m_Dxx, d->GetLargestPossibleRegion(), r);
  typename FT::FaceListType fxy = fc(m_Dxy, d->GetLargestPossibleRegion(), r);
  typename FT::FaceListType fxz = fc(m_Dxz, d->GetLargestPossibleRegion(), r);
  typename FT::FaceListType fyy = fc(m_Dyy, d->GetLargestPossibleRegion(), r);
  typename FT::FaceListType fyz = fc(m_Dyz, d->GetLargestPossibleRegion(), r);
  typename FT::FaceListType fzz = fc(m_Dzz, d->GetLargestPossibleRegion(), r);

  typename FT::FaceListType::iterator fitci, fitxx, fitxy, fitxz, fityy, fityz, fitzz;

  for (fitci = fci.begin(),
      fitxx = fxx.begin(),
      fitxy = fxy.begin(),
      fitxz = fxz.begin(),
      fityy = fyy.begin(),
      fityz = fyz.begin(),
      fitzz = fzz.begin();
       fitci != fci.end();
       ++fitci, ++fitxx, ++fitxy, ++fitxz, ++fityy, ++fityz, ++fitzz)
  {
    // output iter
    ImageRegionIterator<PrecisionImageType> dit(d, *fitci);

    // input iters
    NT itci(r, ci, *fitci);
    NT itxx(r, m_Dxx, *fitxx);
    NT itxy(r, m_Dxy, *fitxy);
    NT itxz(r, m_Dxz, *fitxz);
    NT ityy(r, m_Dyy, *fityy);
    NT ityz(r, m_Dyz, *fityz);
    NT itzz(r, m_Dzz, *fitzz);

    itci.OverrideBoundaryCondition(&b);
    itxx.OverrideBoundaryCondition(&b);
    itxy.OverrideBoundaryCondition(&b);
    itxz.OverrideBoundaryCondition(&b);
    ityy.OverrideBoundaryCondition(&b);
    ityz.OverrideBoundaryCondition(&b);
    itzz.OverrideBoundaryCondition(&b);

    // setting active offsets (yeah there must
    // be some smarter way of doing this)
    itci.ClearActiveList();
    itxx.ClearActiveList();
    itxy.ClearActiveList();
    itxz.ClearActiveList();
    ityy.ClearActiveList();
    ityz.ClearActiveList();
    itzz.ClearActiveList();

    itci.ActivateOffset(oxp);
    itxx.ActivateOffset(oxp);
    itxy.ActivateOffset(oxp);
    itxz.ActivateOffset(oxp);
    ityy.ActivateOffset(oxp);
    ityz.ActivateOffset(oxp);
    itzz.ActivateOffset(oxp);

    itci.ActivateOffset(oxm);
    itxx.ActivateOffset(oxm);
    itxy.ActivateOffset(oxm);
    itxz.ActivateOffset(oxm);
    ityy.ActivateOffset(oxm);
    ityz.ActivateOffset(oxm);
    itzz.ActivateOffset(oxm);

    itci.ActivateOffset(oyp);
    itxx.ActivateOffset(oyp);
    itxy.ActivateOffset(oyp);
    itxz.ActivateOffset(oyp);
    ityy.ActivateOffset(oyp);
    ityz.ActivateOffset(oyp);
    itzz.ActivateOffset(oyp);

    itci.ActivateOffset(oym);
    itxx.ActivateOffset(oym);
    itxy.ActivateOffset(oym);
    itxz.ActivateOffset(oym);
    ityy.ActivateOffset(oym);
    ityz.ActivateOffset(oym);
    itzz.ActivateOffset(oym);

    itci.ActivateOffset(ozp);
    itxx.ActivateOffset(ozp);
    itxy.ActivateOffset(ozp);
    itxz.ActivateOffset(ozp);
    ityy.ActivateOffset(ozp);
    ityz.ActivateOffset(ozp);
    itzz.ActivateOffset(ozp);

    itci.ActivateOffset(ozm);
    itxx.ActivateOffset(ozm);
    itxy.ActivateOffset(ozm);
    itxz.ActivateOffset(ozm);
    ityy.ActivateOffset(ozm);
    ityz.ActivateOffset(ozm);
    itzz.ActivateOffset(ozm);

    itci.ActivateOffset(oxpyp);
    itxx.ActivateOffset(oxpyp);
    itxy.ActivateOffset(oxpyp);
    itxz.ActivateOffset(oxpyp);
    ityy.ActivateOffset(oxpyp);
    ityz.ActivateOffset(oxpyp);
    itzz.ActivateOffset(oxpyp);

    itci.ActivateOffset(oxmym);
    itxx.ActivateOffset(oxmym);
    itxy.ActivateOffset(oxmym);
    itxz.ActivateOffset(oxmym);
    ityy.ActivateOffset(oxmym);
    ityz.ActivateOffset(oxmym);
    itzz.ActivateOffset(oxmym);

    itci.ActivateOffset(oxpym);
    itxx.ActivateOffset(oxpym);
    itxy.ActivateOffset(oxpym);
    itxz.ActivateOffset(oxpym);
    ityy.ActivateOffset(oxpym);
    ityz.ActivateOffset(oxpym);
    itzz.ActivateOffset(oxpym);

    itci.ActivateOffset(oxmyp);
    itxx.ActivateOffset(oxmyp);
    itxy.ActivateOffset(oxmyp);
    itxz.ActivateOffset(oxmyp);
    ityy.ActivateOffset(oxmyp);
    ityz.ActivateOffset(oxmyp);
    itzz.ActivateOffset(oxmyp);

    itci.ActivateOffset(oxpzp);
    itxx.ActivateOffset(oxpzp);
    itxy.ActivateOffset(oxpzp);
    itxz.ActivateOffset(oxpzp);
    ityy.ActivateOffset(oxpzp);
    ityz.ActivateOffset(oxpzp);
    itzz.ActivateOffset(oxpzp);

    itci.ActivateOffset(oxmzm);
    itxx.ActivateOffset(oxmzm);
    itxy.ActivateOffset(oxmzm);
    itxz.ActivateOffset(oxmzm);
    ityy.ActivateOffset(oxmzm);
    ityz.ActivateOffset(oxmzm);
    itzz.ActivateOffset(oxmzm);

    itci.ActivateOffset(oxpzm);
    itxx.ActivateOffset(oxpzm);
    itxy.ActivateOffset(oxpzm);
    itxz.ActivateOffset(oxpzm);
    ityy.ActivateOffset(oxpzm);
    ityz.ActivateOffset(oxpzm);
    itzz.ActivateOffset(oxpzm);

    itci.ActivateOffset(oxmzp);
    itxx.ActivateOffset(oxmzp);
    itxy.ActivateOffset(oxmzp);
    itxz.ActivateOffset(oxmzp);
    ityy.ActivateOffset(oxmzp);
    ityz.ActivateOffset(oxmzp);
    itzz.ActivateOffset(oxmzp);

    itci.ActivateOffset(oypzp);
    itxx.ActivateOffset(oypzp);
    itxy.ActivateOffset(oypzp);
    itxz.ActivateOffset(oypzp);
    ityy.ActivateOffset(oypzp);
    ityz.ActivateOffset(oypzp);
    itzz.ActivateOffset(oypzp);

    itci.ActivateOffset(oymzm);
    itxx.ActivateOffset(oymzm);
    itxy.ActivateOffset(oymzm);
    itxz.ActivateOffset(oymzm);
    ityy.ActivateOffset(oymzm);
    ityz.ActivateOffset(oymzm);
    itzz.ActivateOffset(oymzm);

    itci.ActivateOffset(oypzm);
    itxx.ActivateOffset(oypzm);
    itxy.ActivateOffset(oypzm);
    itxz.ActivateOffset(oypzm);
    ityy.ActivateOffset(oypzm);
    ityz.ActivateOffset(oypzm);
    itzz.ActivateOffset(oypzm);

    itci.ActivateOffset(oymzp);
    itxx.ActivateOffset(oymzp);
    itxy.ActivateOffset(oymzp);
    itxz.ActivateOffset(oymzp);
    ityy.ActivateOffset(oymzp);
    ityz.ActivateOffset(oymzp);
    itzz.ActivateOffset(oymzp);

    // run for each face diffusion
    for (itci.GoToBegin(),
         dit.GoToBegin(),
         itxx.GoToBegin(),
         itxy.GoToBegin(),
         itxz.GoToBegin(),
         ityy.GoToBegin(),
         ityz.GoToBegin(),
         itzz.GoToBegin();
         !itci.IsAtEnd();
         ++itci, ++dit, ++itxx, ++itxy, ++itxz, ++ityy, ++ityz, ++itzz)
    {
      // weights
      const Precision xp = itxx.GetPixel(oxp) + itxx.GetCenterPixel();
      const Precision xm = itxx.GetPixel(oxm) + itxx.GetCenterPixel();
      const Precision yp = ityy.GetPixel(oyp) + ityy.GetCenterPixel();
      const Precision ym = ityy.GetPixel(oym) + ityy.GetCenterPixel();
      const Precision zp = itzz.GetPixel(ozp) + itzz.GetCenterPixel();
      const Precision zm = itzz.GetPixel(ozm) + itzz.GetCenterPixel();

      const Precision xpyp = itxy.GetPixel(oxpyp) + itxy.GetCenterPixel();
      const Precision xmym = itxy.GetPixel(oxmym) + itxy.GetCenterPixel();
      const Precision xpym = -itxy.GetPixel(oxpym) - itxy.GetCenterPixel();
      const Precision xmyp = -itxy.GetPixel(oxmyp) - itxy.GetCenterPixel();

      const Precision xpzp = itxz.GetPixel(oxpzp) + itxz.GetCenterPixel();
      const Precision xmzm = itxz.GetPixel(oxmzm) + itxz.GetCenterPixel();
      const Precision xpzm = -itxz.GetPixel(oxpzm) - itxz.GetCenterPixel();
      const Precision xmzp = -itxz.GetPixel(oxmzp) - itxz.GetCenterPixel();

      const Precision ypzp = ityz.GetPixel(oypzp) + ityz.GetCenterPixel();
      const Precision ymzm = ityz.GetPixel(oymzm) + ityz.GetCenterPixel();
      const Precision ypzm = -ityz.GetPixel(oypzm) - ityz.GetCenterPixel();
      const Precision ymzp = -ityz.GetPixel(oymzp) - ityz.GetCenterPixel();

      // evolution
      const Precision cv = itci.GetCenterPixel();
      dit.Value() = cv + +rxx * (xp * (itci.GetPixel(oxp) - cv) + xm * (itci.GetPixel(oxm) - cv)) +
                    ryy * (yp * (itci.GetPixel(oyp) - cv) + ym * (itci.GetPixel(oym) - cv)) +
                    rzz * (zp * (itci.GetPixel(ozp) - cv) + zm * (itci.GetPixel(ozm) - cv)) +
                    rxy * (xpyp * (itci.GetPixel(oxpyp) - cv) + xmym * (itci.GetPixel(oxmym) - cv) +
                           xpym * (itci.GetPixel(oxpym) - cv) + xmyp * (itci.GetPixel(oxmyp) - cv)) +
                    rxz * (xpzp * (itci.GetPixel(oxpzp) - cv) + xmzm * (itci.GetPixel(oxmzm) - cv) +
                           xpzm * (itci.GetPixel(oxpzm) - cv) + xmzp * (itci.GetPixel(oxmzp) - cv)) +
                    ryz * (ypzp * (itci.GetPixel(oypzp) - cv) + ymzm * (itci.GetPixel(oymzm) - cv) +
                           ypzm * (itci.GetPixel(oypzm) - cv) + ymzp * (itci.GetPixel(oymzp) - cv));
    }
  }

  // copying
  ImageRegionConstIterator<PrecisionImageType> iti(d, d->GetLargestPossibleRegion());
  ImageRegionIterator<PrecisionImageType>      ito(ci, ci->GetLargestPossibleRegion());
  for (iti.GoToBegin(), ito.GoToBegin(); !iti.IsAtEnd(); ++iti, ++ito)
  {
    ito.Value() = iti.Value();
  }
}

// maxvesselresponse
template <typename PixelType, unsigned int NDimension>
void
VesselEnhancingDiffusion3DImageFilter<PixelType, NDimension>::MaxVesselResponse(
  const typename PrecisionImageType::Pointer im)
{

  // alloc memory for hessian/tensor
  m_Dxx = PrecisionImageType::New();
  m_Dxx->SetOrigin(im->GetOrigin());
  m_Dxx->SetSpacing(im->GetSpacing());
  m_Dxx->SetDirection(im->GetDirection());
  m_Dxx->SetRegions(im->GetLargestPossibleRegion());
  m_Dxx->Allocate();
  m_Dxx->FillBuffer(NumericTraits<Precision>::One);

  m_Dxy = PrecisionImageType::New();
  m_Dxy->SetOrigin(im->GetOrigin());