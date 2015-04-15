  /// \file TreeClusteringAlgorithm.cc
/*
 *
 * TreeClusteringAlgorithm.cc source template automatically generated by a class generator
 * Creation date : mar. avr. 14 2015
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


#include "ArborClustering/TreeClusteringAlgorithm.h"

#include "ArborObjects/CaloHit.h"
#include "ArborObjects/Connector.h"

#include "ArborHelpers/CaloHitHelper.h"

#include "Pandora/AlgorithmHeaders.h"

namespace arbor_content
{

pandora::StatusCode TreeClusteringAlgorithm::Run()
{
	const pandora::CaloHitList *pCaloHitList = NULL;
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::GetCurrentList(*this, pCaloHitList));

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->BuildClusters(pCaloHitList));

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode TreeClusteringAlgorithm::BuildClusters(const pandora::CaloHitList *const pCaloHitList) const
{
	if(NULL == pCaloHitList)
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	if(pCaloHitList->empty())
		return pandora::STATUS_CODE_SUCCESS;

	for(pandora::CaloHitList::const_iterator iter = pCaloHitList->begin(), endIter = pCaloHitList->end() ;
			endIter != iter ; ++iter)
	{
		const CaloHit *const pCaloHit = dynamic_cast<const CaloHit *const>(*iter);

		if(NULL == pCaloHit)
			return pandora::STATUS_CODE_FAILURE;

	if(pCaloHit->IsSeed() && pCaloHit->GetConnectorList(BACKWARD_DIRECTION).size() > 0)
		std::cout << "Mis-identified seeds ! " << std::endl;

	if(pCaloHit->IsLeaf() && pCaloHit->GetConnectorList(FORWARD_DIRECTION).size() > 0)
		std::cout << "Mis-identified leafs ! " << std::endl;

		if(!pCaloHit->IsSeed() || !PandoraContentApi::IsAvailable(*this, *iter))
			continue;

		PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->BuildCluster(pCaloHit));
	}

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode TreeClusteringAlgorithm::BuildCluster(const CaloHit *const pSeedCaloHit) const
{
	if(NULL == pSeedCaloHit)
		return pandora::STATUS_CODE_FAILURE;

	if(!pSeedCaloHit->IsSeed())
		return pandora::STATUS_CODE_INVALID_PARAMETER;

	pandora::CaloHitList clusterCaloHitList;
	clusterCaloHitList.insert(pSeedCaloHit);
	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, CaloHitHelper::BuildCaloHitList(pSeedCaloHit, FORWARD_DIRECTION, clusterCaloHitList));

	for(pandora::CaloHitList::const_iterator iter = clusterCaloHitList.begin(), endIter = clusterCaloHitList.end() ;
			endIter != iter ; ++iter)
	{
		if(!PandoraContentApi::IsAvailable(*this, *iter))
			std::cout << "Warning adding non available hit to cluster !" << std::endl;
	}


	const pandora::Cluster *pCluster = NULL;
	PandoraContentApi::ClusterParameters clusterParameters;
	clusterParameters.m_caloHitList = clusterCaloHitList;

	PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::Cluster::Create(*this, clusterParameters, pCluster));

	return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode TreeClusteringAlgorithm::ReadSettings(const pandora::TiXmlHandle /*xmlHandle*/)
{
	return pandora::STATUS_CODE_SUCCESS;
}


} 
