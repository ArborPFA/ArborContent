/*
 *
 * GeometryHelper.cc source template automatically generated by a class generator
 * Creation date : jeu. avr. 9 2015
 *
 * This file is part of ArborContent libraries.
 * 
 * ArborContent is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 * 
 * ArborContent is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with ArborContent.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * @author Remi Ete
 * @copyright CNRS , IPNL
 */


#include "ArborHelpers/GeometryHelper.h"

#include "Managers/GeometryManager.h"
#include "Objects/SubDetector.h"
#include "Objects/Helix.h"
#include "Objects/CaloHit.h"

namespace arbor_content
{

  pandora::StatusCode GeometryHelper::GetClosestDistanceBetweenLines(const pandora::CartesianVector &point1, const pandora::CartesianVector &direction1,
      const pandora::CartesianVector &point2, const pandora::CartesianVector &direction2, float &distance)
  {
    distance = 0.f;

    try
    {
      const pandora::CartesianVector unitDirection1 = direction1.GetUnitVector();
      const pandora::CartesianVector unitDirection2 = direction2.GetUnitVector();
      const pandora::CartesianVector w0 = point1 - point2;
      const float b = unitDirection1.GetCosOpeningAngle(unitDirection2);
      const float d = unitDirection1.GetDotProduct(w0);
      const float e = unitDirection2.GetDotProduct(w0);
      const float denom = 1 - b*b;

      // parallel lines
      if(unitDirection1 == unitDirection2)
      {
        // compute the distance from point2 to line1
        return GeometryHelper::GetClosestDistanceToLine(point1, unitDirection1, point2, distance);
      }
      else
      {
        // compute the real impact parameter between the two lines
        const pandora::CartesianVector impactParameterVector = w0 + ( unitDirection1*((b*e - d)/denom) - unitDirection2*((e - b*d)/denom) );
        distance = impactParameterVector.GetMagnitude();
      }
    }
    catch(pandora::StatusCodeException &statusCodeException)
    {
      return statusCodeException.GetStatusCode();
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetClosestDistanceToLine(const pandora::CartesianVector &point1, const pandora::CartesianVector &direction1,
      const pandora::CartesianVector &point2, float &distance)
  {
    try
    {
      // compute the distance from point2 to line1
      const pandora::CartesianVector unitDirection1 = direction1.GetUnitVector();
      float k = unitDirection1.GetDotProduct(point2-point1);
      const pandora::CartesianVector normale = point1 + unitDirection1*k - point2;
      distance = normale.GetMagnitude();
    }
    catch(pandora::StatusCodeException &statusCodeException)
    {
      return statusCodeException.GetStatusCode();
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetProjectionOnLine(const pandora::CartesianVector &linePoint, const pandora::CartesianVector &direction,
      const pandora::CartesianVector &point, pandora::CartesianVector &projection)
  {
    try
    {
      projection = linePoint + direction.GetUnitVector() * ((point-linePoint).GetMagnitude() * direction.GetCosOpeningAngle(point-linePoint));
    }
    catch(pandora::StatusCodeException &statusCodeException)
    {
      return statusCodeException.GetStatusCode();
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetCrossingPointsBetweenLines(const pandora::CartesianVector &point1, const pandora::CartesianVector &direction1,
      const pandora::CartesianVector &point2, const pandora::CartesianVector &direction2,
      pandora::CartesianVector &crossingPoint1, pandora::CartesianVector &crossingPoint2)
  {
    try
    {
      // parallel lines never cross
      if(direction1.GetUnitVector() == direction2.GetUnitVector())
        return pandora::STATUS_CODE_INVALID_PARAMETER;

      const pandora::CartesianVector unitDirection1 = direction1.GetUnitVector();
      const pandora::CartesianVector unitDirection2 = direction2.GetUnitVector();
      const pandora::CartesianVector w0 = point1 - point2;
      const float b = unitDirection1.GetCosOpeningAngle(unitDirection2);
      const float d = unitDirection1.GetDotProduct(w0);
      const float e = unitDirection2.GetDotProduct(w0);
      const float denom = 1 - b*b;

      crossingPoint1 = point1 + unitDirection1*(b*e-d)*(1.f/denom);
      crossingPoint2 = point2 + unitDirection2*(e - b*d)*(1.f/denom);
    }
    catch(pandora::StatusCodeException &statusCodeException)
    {
      return statusCodeException.GetStatusCode();
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetLinePlaneCrossingPoint(const pandora::CartesianVector &linePoint, const pandora::CartesianVector &lineDirection,
      float planeA, float planeB, float planeC, float planeD, pandora::CartesianVector &crossingPoint)
  {
    const float num = planeA*linePoint.GetX() + planeB*linePoint.GetY() + planeC*linePoint.GetZ() + planeD;
    const float denom = planeA*lineDirection.GetX() + planeB*lineDirection.GetY() + planeC*lineDirection.GetZ();

    if(fabs(denom) < std::numeric_limits<float>::epsilon())
      return pandora::STATUS_CODE_FAILURE;

    const float k = num/denom;

    const float xCrossingPoint = linePoint.GetX() + k*lineDirection.GetX();
    const float yCrossingPoint = linePoint.GetY() + k*lineDirection.GetY();
    const float zCrossingPoint = linePoint.GetZ() + k*lineDirection.GetZ();

    crossingPoint.SetValues(xCrossingPoint, yCrossingPoint, zCrossingPoint);

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetECalHCalEndCapGapSize(const pandora::Pandora &pandora, float &gapSize)
  {
    const float ecalEndCapOuterZCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::ECAL_ENDCAP).GetOuterZCoordinate();
    const float hcalEndCapInnerZCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::HCAL_ENDCAP).GetInnerZCoordinate();

    if(ecalEndCapOuterZCoordinate > hcalEndCapInnerZCoordinate)
      return pandora::STATUS_CODE_FAILURE;

    gapSize = hcalEndCapInnerZCoordinate - ecalEndCapOuterZCoordinate;

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetECalHCalBarrelGapSize(const pandora::Pandora &pandora, float &gapSize)
  {
    const float ecalBarrelOuterRCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::ECAL_BARREL).GetOuterRCoordinate();
    const float hcalBarrelInnerRCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::HCAL_BARREL).GetInnerRCoordinate();

    if(ecalBarrelOuterRCoordinate > hcalBarrelInnerRCoordinate)
      return pandora::STATUS_CODE_FAILURE;

    gapSize = hcalBarrelInnerRCoordinate - ecalBarrelOuterRCoordinate;

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetTrackerECalEndCapGapSize(const pandora::Pandora &pandora, float &gapSize)
  {
    const float ecalEndCapInnerZCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::ECAL_ENDCAP).GetInnerZCoordinate();
    const float trackerOuterZCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::INNER_TRACKER).GetOuterZCoordinate();

    if(trackerOuterZCoordinate > ecalEndCapInnerZCoordinate)
      return pandora::STATUS_CODE_FAILURE;

    gapSize = ecalEndCapInnerZCoordinate - trackerOuterZCoordinate;

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetTrackerECalBarrelGapSize(const pandora::Pandora &pandora, float &gapSize)
  {
    const float ecalBarrelInnerRCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::ECAL_BARREL).GetInnerRCoordinate();
    const float trackerOuterRCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::INNER_TRACKER).GetOuterRCoordinate();

    if(trackerOuterRCoordinate > ecalBarrelInnerRCoordinate)
      return pandora::STATUS_CODE_FAILURE;

    gapSize = ecalBarrelInnerRCoordinate - trackerOuterRCoordinate;

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetEndcapGapSeparation(const pandora::Pandora &pandora, const pandora::CaloHit *const pEcalCaloHit,
      const pandora::CaloHit *const pHcalCaloHit, float &gapSeparation)
  {
    if(pEcalCaloHit->GetHitRegion() != pandora::ENDCAP || pEcalCaloHit->GetHitType() != pandora::ECAL)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    if(pHcalCaloHit->GetHitRegion() != pandora::ENDCAP || pHcalCaloHit->GetHitType() != pandora::HCAL)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    // same end cap side required
    if(pEcalCaloHit->GetPositionVector().GetZ()*pHcalCaloHit->GetPositionVector().GetZ() < 0)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    const float hcalEndCapInnerZCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::HCAL_ENDCAP).GetInnerZCoordinate();
    const float ecalEndCapOuterZCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::ECAL_ENDCAP).GetOuterZCoordinate();

    const pandora::CartesianVector hitsDirection = (pHcalCaloHit->GetPositionVector() - pEcalCaloHit->GetPositionVector()).GetUnitVector();

    pandora::CartesianVector ecalFrontFaceCrossingPoint(0.f, 0.f, 0.f);
    pandora::CartesianVector hcalFrontFaceCrossingPoint(0.f, 0.f, 0.f);

    int endCapSide = pEcalCaloHit->GetPositionVector().GetZ() > 0 ? 1 : -1;

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, GeometryHelper::GetLinePlaneCrossingPoint(
        pEcalCaloHit->GetPositionVector(), hitsDirection,
        0.f, 0.f, static_cast<float>(endCapSide), -1.f*endCapSide*ecalEndCapOuterZCoordinate,
        ecalFrontFaceCrossingPoint));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, GeometryHelper::GetLinePlaneCrossingPoint(
        pEcalCaloHit->GetPositionVector(), hitsDirection,
        0.f, 0.f, static_cast<float>(endCapSide), -1.f*endCapSide*hcalEndCapInnerZCoordinate,
        hcalFrontFaceCrossingPoint));

    gapSeparation = (ecalFrontFaceCrossingPoint - hcalFrontFaceCrossingPoint).GetMagnitude();

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetBarrelGapSeparation(const pandora::Pandora &pandora, const pandora::CaloHit *const pEcalCaloHit,
      const pandora::CaloHit *const pHcalCaloHit, float &gapSeparation)
  {
    if(pEcalCaloHit->GetHitRegion() != pandora::BARREL || pEcalCaloHit->GetHitType() != pandora::ECAL)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    if(pHcalCaloHit->GetHitRegion() != pandora::BARREL || pHcalCaloHit->GetHitType() != pandora::HCAL)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    const float ecalBarrelOuterRCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::ECAL_BARREL).GetOuterRCoordinate();
    const float hcalBarrelInnerRCoordinate = pandora.GetGeometry()->GetSubDetector(pandora::HCAL_BARREL).GetInnerRCoordinate();

    const pandora::CartesianVector hitsDirection = (pHcalCaloHit->GetPositionVector() - pEcalCaloHit->GetPositionVector()).GetUnitVector();

    // Get normale vectors to ecal and hcal in the gap region
    pandora::CartesianVector ecalBarrelOuterNormaleVector(0.f, 0.f, 0.f);
    pandora::CartesianVector hcalBarrelInnerNormaleVector(0.f, 0.f, 0.f);

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, GeometryHelper::GetOuterNormaleVector(
        pandora, pandora::ECAL_BARREL, pEcalCaloHit->GetPositionVector(), ecalBarrelOuterNormaleVector));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, GeometryHelper::GetInnerNormaleVector(
        pandora, pandora::HCAL_BARREL, pHcalCaloHit->GetPositionVector(), hcalBarrelInnerNormaleVector));

    // Get the crossing point with ecal and hcal in the gap region
    pandora::CartesianVector ecalFrontFaceCrossingPoint(0.f, 0.f, 0.f);
    pandora::CartesianVector hcalFrontFaceCrossingPoint(0.f, 0.f, 0.f);

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, GeometryHelper::GetLinePlaneCrossingPoint(
        pEcalCaloHit->GetPositionVector(), hitsDirection,
        ecalBarrelOuterNormaleVector.GetX(), ecalBarrelOuterNormaleVector.GetY(), ecalBarrelOuterNormaleVector.GetZ(),
        -1.f*ecalBarrelOuterRCoordinate, ecalFrontFaceCrossingPoint));

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, GeometryHelper::GetLinePlaneCrossingPoint(
        pHcalCaloHit->GetPositionVector(), hitsDirection,
        hcalBarrelInnerNormaleVector.GetX(), hcalBarrelInnerNormaleVector.GetY(), hcalBarrelInnerNormaleVector.GetZ(),
        -1.f*hcalBarrelInnerRCoordinate, hcalFrontFaceCrossingPoint));

    gapSeparation = (ecalFrontFaceCrossingPoint - hcalFrontFaceCrossingPoint).GetMagnitude();

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetOuterNormaleVector(const pandora::Pandora &pandora, pandora::SubDetectorType type, const pandora::CartesianVector &point,
      pandora::CartesianVector &normaleVector)
  {
    const unsigned int outerSymmetryOrder = pandora.GetGeometry()->GetSubDetector(type).GetOuterSymmetryOrder();

    if(0 == outerSymmetryOrder)
    {
      normaleVector.SetValues(point.GetX(), point.GetY(), 0.f);
      return pandora::STATUS_CODE_SUCCESS;
    }

    const float rPoint = std::sqrt(point.GetX()*point.GetX() + point.GetY()*point.GetY());
    const float phiPoint = point.GetY() > 0 ?
        std::acos(point.GetX() / rPoint) : std::acos(-point.GetX() / rPoint) + M_PI;

    const float phiShift = (2 * M_PI / static_cast<float>(outerSymmetryOrder)) / 2.f;

    for(unsigned int i=0 ; i<outerSymmetryOrder+1 ; i++)
    {
      const float phi = 2 * M_PI * (static_cast<float>(i) / static_cast<float>(outerSymmetryOrder));
      const float phiMin = phi - phiShift;// + barrelOuterPhiCoordinate;
      const float phiMax = phi + phiShift;// + barrelOuterPhiCoordinate;

      if(phiPoint > phiMin && phiPoint < phiMax)
      {
        normaleVector.SetValues(std::cos(phi), std::sin(phi), 0.f);
        break;
      }
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetInnerNormaleVector(const pandora::Pandora &pandora, pandora::SubDetectorType type, const pandora::CartesianVector &point,
      pandora::CartesianVector &normaleVector)
  {
    const unsigned int innerSymmetryOrder = pandora.GetGeometry()->GetSubDetector(type).GetInnerSymmetryOrder();

    if(0 == innerSymmetryOrder)
    {
      normaleVector.SetValues(point.GetX(), point.GetY(), 0.f);
      return pandora::STATUS_CODE_SUCCESS;
    }

    const float rPoint = std::sqrt(point.GetX()*point.GetX() + point.GetY()*point.GetY());
    const float phiPoint = point.GetY() > 0 ?
        std::acos(point.GetX() / rPoint) : std::acos(-point.GetX() / rPoint) + M_PI;

    const float phiShift = (2 * M_PI / static_cast<float>(innerSymmetryOrder)) / 2.f;

    for(unsigned int i=0 ; i<innerSymmetryOrder ; i++)
    {
      const float phi = 2 * M_PI * (static_cast<float>(i) / static_cast<float>(innerSymmetryOrder));
      const float phiMin = phi - phiShift;
      const float phiMax = phi + phiShift;

      if(phiPoint > phiMin && phiPoint < phiMax)
      {
        normaleVector.SetValues(std::cos(phi), std::sin(phi), 0.f);
        break;
      }
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetProjectionOnHelix(const pandora::Helix &helix, const pandora::CartesianVector &point, pandora::CartesianVector &projection)
  {
    pandora::CartesianVector pointInXY(0.f, 0.f, 0.f), pointInZ(0.f, 0.f, 0.f);

    const pandora::StatusCode statusCode1(helix.GetPointInXY(point.GetX(), point.GetY(), point.GetX(), point.GetY(), helix.GetReferencePoint(), pointInXY));
    const pandora::StatusCode statusCode2(helix.GetPointInZ(point.GetZ(), helix.GetReferencePoint(), pointInZ));

    if(pandora::STATUS_CODE_SUCCESS != statusCode1 && pandora::STATUS_CODE_SUCCESS != statusCode2)
      return pandora::STATUS_CODE_NOT_FOUND;

    if(statusCode2 != pandora::STATUS_CODE_SUCCESS)
    {
      projection = pointInXY;
      return pandora::STATUS_CODE_SUCCESS;
    }

    if(statusCode1 != pandora::STATUS_CODE_SUCCESS)
    {
      projection = pointInZ;
      return pandora::STATUS_CODE_SUCCESS;
    }

    const float distanceInXY((pointInXY-point).GetMagnitude()), distanceInZ((pointInZ-point).GetMagnitude());
    projection = distanceInXY < distanceInZ ? pointInXY : pointInZ;

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode GeometryHelper::GetDistanceToHelix(const pandora::Helix &helix, const pandora::CartesianVector &point, float &distanceToHelix)
  {
    distanceToHelix = std::numeric_limits<float>::max();

    pandora::CartesianVector projectionOnHelix(0.f, 0.f, 0.f);
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, GeometryHelper::GetProjectionOnHelix(helix, point, projectionOnHelix));

    distanceToHelix = (point-projectionOnHelix).GetMagnitude();

    return pandora::STATUS_CODE_SUCCESS;
  }

} 

