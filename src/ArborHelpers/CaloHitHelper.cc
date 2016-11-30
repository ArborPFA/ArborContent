/// \file CaloHitHelper.cc
/*
 *
 * CaloHitHelper.cc source template automatically generated by a class generator
 * Creation date : lun. mars 30 2015
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


#include "ArborHelpers/CaloHitHelper.h"

#include "Api/PandoraContentApi.h"

#include "Objects/Track.h"

#include "ArborApi/ArborContentApi.h"
#include "ArborObjects/CaloHit.h"

namespace arbor_content
{

  //--------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitHelper::RemoveCurrentConnections(const pandora::Algorithm &algorithm)
  {
    const pandora::CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(algorithm, pCaloHitList));

    return CaloHitHelper::RemoveConnections(pCaloHitList);
  }

  //--------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitHelper::RemoveConnections(const pandora::CaloHitList *const pCaloHitList)
  {
    for(pandora::CaloHitList::const_iterator iter = pCaloHitList->begin(), endIter = pCaloHitList->end() ;
        endIter != iter ; ++iter)
    {
      const CaloHit *const pCaloHit = dynamic_cast<const CaloHit *const>(*iter);

      if(NULL == pCaloHit)
        continue;

      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, ArborContentApi::RemoveAndDeleteAllConnections(pCaloHit));
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //--------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitHelper::ExtractCurrentSeedCaloHitList(const pandora::Algorithm &algorithm,
      pandora::CaloHitList &seedCaloHitList, bool discriminateLeafHits)
  {
    const pandora::CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(algorithm, pCaloHitList));

    return CaloHitHelper::ExtractSeedCaloHitList(pCaloHitList, seedCaloHitList, discriminateLeafHits);
  }

  //--------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitHelper::ExtractSeedCaloHitList(const pandora::CaloHitList *const pCaloHitList,
      pandora::CaloHitList &seedCaloHitList, bool discriminateLeafHits)
  {
    for(pandora::CaloHitList::const_iterator iter = pCaloHitList->begin(), endIter = pCaloHitList->end() ;
        endIter != iter ; ++iter)
    {
      const CaloHit *const pCaloHit = dynamic_cast<const CaloHit *const>(*iter);

      if(NULL == pCaloHit)
        continue;

      if(ArborContentApi::IsSeed(pCaloHit) && !(ArborContentApi::IsLeaf(pCaloHit) && discriminateLeafHits))
        seedCaloHitList.insert(pCaloHit);
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //--------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitHelper::ExtractCurrentLeafCaloHitList(const pandora::Algorithm &algorithm,
      pandora::CaloHitList &leafCaloHitList, bool discriminateSeedHits)
  {
    const pandora::CaloHitList *pCaloHitList = NULL;
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(algorithm, pCaloHitList));

    return CaloHitHelper::ExtractLeafCaloHitList(pCaloHitList, leafCaloHitList, discriminateSeedHits);
  }

  //--------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitHelper::ExtractLeafCaloHitList(const pandora::CaloHitList *const pCaloHitList,
      pandora::CaloHitList &leafCaloHitList, bool discriminateSeedHits)
  {
    for(pandora::CaloHitList::const_iterator iter = pCaloHitList->begin(), endIter = pCaloHitList->end() ;
        endIter != iter ; ++iter)
    {
      const CaloHit *const pCaloHit = dynamic_cast<const CaloHit *const>(*iter);

      if(NULL == pCaloHit)
        continue;

      if(ArborContentApi::IsLeaf(pCaloHit) && !(discriminateSeedHits && !ArborContentApi::IsSeed(pCaloHit)))
        leafCaloHitList.insert(pCaloHit);
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //--------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitHelper::BuildCaloHitList(const CaloHit *const pCaloHit, ConnectorDirection direction,
      pandora::CaloHitList &calohitList)
  {
    const ConnectorList &connectorList(ArborContentApi::GetConnectorList(pCaloHit, direction));

    for(ConnectorList::const_iterator iter = connectorList.begin(), endIter = connectorList.end() ;
        endIter != iter ; ++iter)
    {
      const Connector *const pConnector = *iter;

      const CaloHit *const pConnectedCaloHit = pConnector->Get(direction);

      if(!calohitList.insert(pConnectedCaloHit).second)
        return pandora::STATUS_CODE_FAILURE;

      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::BuildCaloHitList(pConnectedCaloHit, direction, calohitList));
    }


    return pandora::STATUS_CODE_SUCCESS;
  }

  //--------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitHelper::BuildCaloHitList(const CaloHit *const pCaloHit, ConnectorDirection direction, pandora::CaloHitList &calohitList,
      unsigned int connectionDepthLimit, unsigned int pseudoLayerLimit)
  {
    const ConnectorList &connectorList(ArborContentApi::GetConnectorList(pCaloHit, direction));

    if(0 == connectionDepthLimit)
      return pandora::STATUS_CODE_SUCCESS;

    for(ConnectorList::const_iterator iter = connectorList.begin(), endIter = connectorList.end() ;
        endIter != iter ; ++iter)
    {
      const Connector *const pConnector = *iter;
      const CaloHit *const pConnectedCaloHit = pConnector->Get(direction);

      const unsigned int pseudoLayerDifference =
          std::max(pConnectedCaloHit->GetPseudoLayer(), pCaloHit->GetPseudoLayer()) - std::min(pConnectedCaloHit->GetPseudoLayer(), pCaloHit->GetPseudoLayer());

      if(pseudoLayerDifference > pseudoLayerLimit)
        continue;

      if(!calohitList.insert(pConnectedCaloHit).second)
        return pandora::STATUS_CODE_FAILURE;

      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::BuildCaloHitList(pConnectedCaloHit, direction, calohitList, connectionDepthLimit-1, pseudoLayerLimit-pseudoLayerDifference));
    }


    return pandora::STATUS_CODE_SUCCESS;
  }

  //--------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode CaloHitHelper::GetMeanDirection(const CaloHit *const pCaloHit, ConnectorDirection connectorDirection,
      pandora::CartesianVector &direction, unsigned int depth)
  {
    if(NULL == pCaloHit)
      return pandora::STATUS_CODE_FAILURE;

    if(0 == depth)
      return pandora::STATUS_CODE_SUCCESS;

    const ConnectorList &connectorList(ArborContentApi::GetConnectorList(pCaloHit, connectorDirection));

    if(connectorList.empty())
      return pandora::STATUS_CODE_SUCCESS;

    const pandora::CartesianVector &positionVector(pCaloHit->GetPositionVector());

    for(ConnectorList::const_iterator iter = connectorList.begin() , endIter = connectorList.end() ;
        endIter != iter ; ++iter)
    {
      const Connector *const pConnector = *iter;
      const CaloHit *const pConnectedCaloHit = pConnector->Get(connectorDirection);

      pandora::CartesianVector differencePosition(pConnectedCaloHit->GetPositionVector() - positionVector);
      const float normalizationWeight = 1.f / (differencePosition.GetMagnitudeSquared());
      direction += (differencePosition * normalizationWeight);

      PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::GetMeanDirection(pConnectedCaloHit, connectorDirection,
          direction, depth-1));
    }

    return pandora::STATUS_CODE_SUCCESS;
  }

  //--------------------------------------------------------------------------------------------------------------------

  bool CaloHitHelper::CanConnect(const CaloHit *const pCaloHit1, const CaloHit *const pCaloHit2, const pandora::CartesianVector &expectedDirection,
      const float normaleMaxAngle, const float normaleMaxDistance,
      const float transverseMaxAngle, const float transverseMaxDistance)
  {
    if(NULL == pCaloHit1 || NULL == pCaloHit2)
      return false;

    if(pCaloHit1 == pCaloHit2)
      return false;

    if(pCaloHit2->GetPseudoLayer() < pCaloHit1->GetPseudoLayer())
      return false;

    const pandora::CartesianVector &cellNormaleVector(pCaloHit1->GetCellNormalVector());

    return CaloHitHelper::IsInRegionOfInterest(pCaloHit1->GetPositionVector(), pCaloHit2->GetPositionVector(),
        expectedDirection, cellNormaleVector, normaleMaxAngle, normaleMaxDistance, transverseMaxAngle, transverseMaxDistance);
  }

  //--------------------------------------------------------------------------------------------------------------------

  bool CaloHitHelper::CanConnect(const pandora::Track *const pTrack, const CaloHit *const pCaloHit,
      const float normaleMaxAngle, const float normaleMaxDistance,
      const float transverseMaxAngle, const float transverseMaxDistance)
  {
    if(NULL == pTrack || NULL == pCaloHit)
      return false;

    const pandora::CartesianVector &cellNormaleVector(pCaloHit->GetCellNormalVector());

    return CaloHitHelper::IsInRegionOfInterest(pTrack->GetTrackStateAtCalorimeter().GetPosition(), pCaloHit->GetPositionVector(),
        pTrack->GetTrackStateAtCalorimeter().GetMomentum(), cellNormaleVector, normaleMaxAngle, normaleMaxDistance, transverseMaxAngle, transverseMaxDistance);
  }

  //--------------------------------------------------------------------------------------------------------------------

  bool CaloHitHelper::IsInRegionOfInterest(const pandora::CartesianVector &startRegionPosition, const pandora::CartesianVector &testPosition, const pandora::CartesianVector &expectedDirection,
      const pandora::CartesianVector &normaleVector, const float normaleMaxAngle, const float normaleMaxDistance, const float transverseMaxAngle, const float transverseMaxDistance)
  {
    const float expectedAngle = normaleVector.GetOpeningAngle(expectedDirection);

    if(normaleMaxAngle <= transverseMaxAngle || normaleMaxDistance >= transverseMaxDistance)
      return false;

    const float maxDistance = (normaleMaxDistance - transverseMaxDistance)*std::cos(expectedAngle) + transverseMaxDistance;
    const float maxAngle = (normaleMaxAngle - transverseMaxAngle)*std::cos(expectedAngle) + transverseMaxAngle;

    const pandora::CartesianVector differenceVector = testPosition - startRegionPosition;

    const float angle = differenceVector.GetOpeningAngle(expectedDirection);
    const float distance = differenceVector.GetMagnitude();

    if(angle > maxAngle || distance > maxDistance)
      return false;

    return true;
  }

} 

