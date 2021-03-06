  /// \file TrackPreparationAlgorithm.cc
/*
 *
 * TrackPreparationAlgorithm.cc source template automatically generated by a class generator
 * Creation date : sam. mars 21 2015
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
 * @author John Marshall
 */

#include "Pandora/AlgorithmHeaders.h"

#include "ArborUtility/TrackPreparationAlgorithm.h"

namespace arbor_content
{

TrackPreparationAlgorithm::TrackPreparationAlgorithm() :
    m_shouldMakeAssociations(true),
    m_shouldMakePfoTrackList(true)
{
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode TrackPreparationAlgorithm::Run()
{
    // Create candidate track list, containing all tracks that could be associated to clusters and so used in final pfo creation
	pandora::TrackList candidateTrackList;

    for (pandora::StringVector::const_iterator iter = m_candidateListNames.begin(), iterEnd = m_candidateListNames.end(); iter != iterEnd; ++iter)
    {
        const pandora::TrackList *pTrackList = NULL;

        if (pandora::STATUS_CODE_SUCCESS != PandoraContentApi::GetList<pandora::TrackList>(*this, *iter, pTrackList))
            continue;

        for (pandora::TrackList::const_iterator trackIter = pTrackList->begin(), trackIterEnd = pTrackList->end(); trackIter != trackIterEnd; ++trackIter)
        {
            if ((*trackIter)->IsAvailable())
                candidateTrackList.insert(*trackIter);
        }
    }

    // Set this list of candidate pfo tracks to be the current track list
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, candidateTrackList, m_mergedCandidateListName));
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<pandora::Track>(*this, m_mergedCandidateListName));

    if (m_shouldMakeAssociations)
    {
        // Remove existing and calculate new track-cluster associations
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RemoveCurrentTrackClusterAssociations(*this));

        for (pandora::StringVector::const_iterator iter = m_associationAlgorithms.begin(), iterEnd = m_associationAlgorithms.end();
            iter != iterEnd; ++iter)
        {
            PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::RunDaughterAlgorithm(*this, *iter));
        }
    }

    if (m_shouldMakePfoTrackList)
    {
        // Filter the candidate track list to identify the parent tracks of charged pfos
    	pandora::TrackList pfoTrackList;
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, this->CreatePfoTrackList(candidateTrackList, pfoTrackList));

        // Save the filtered list and set it to be the current list for future algorithms
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::SaveList(*this, pfoTrackList, m_pfoTrackListName));
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, PandoraContentApi::ReplaceCurrentList<pandora::Track>(*this, m_pfoTrackListName));
    }

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode TrackPreparationAlgorithm::CreatePfoTrackList(const pandora::TrackList &inputTrackList, pandora::TrackList &pfoTrackList) const
{
	pandora::TrackList siblingTracks;

    for (pandora::TrackList::const_iterator iter = inputTrackList.begin(), iterEnd = inputTrackList.end(); iter != iterEnd; ++iter)
    {
        const pandora::Track *const pTrack = *iter;

        if (!pTrack->GetParentTrackList().empty())
            continue;

        // Sibling tracks as first evidence of pfo target
        const pandora::TrackList &siblingTrackList(pTrack->GetSiblingTrackList());

        if (!siblingTrackList.empty())
        {
            if (siblingTracks.end() != siblingTracks.find(pTrack))
                continue;

            if (this->HasAssociatedClusters(pTrack))
            {
                pfoTrackList.insert(pTrack);
                siblingTracks.insert(siblingTrackList.begin(), siblingTrackList.end());
            }
        }
        // Single parent track as pfo target
        else if (this->HasAssociatedClusters(pTrack))
        {
            pfoTrackList.insert(pTrack);
        }
    }

    return pandora::STATUS_CODE_SUCCESS;
}

//------------------------------------------------------------------------------------------------------------------------------------------

bool TrackPreparationAlgorithm::HasAssociatedClusters(const pandora::Track *const pTrack, const bool readSiblingInfo) const
{
    if ((pTrack->CanFormPfo() && pTrack->HasAssociatedCluster()) || (pTrack->CanFormClusterlessPfo()))
        return true;

    if (!pTrack->IsAvailable())
        throw pandora::StatusCodeException(pandora::STATUS_CODE_FAILURE);

    // Consider any sibling tracks
    if (readSiblingInfo)
    {
        const pandora::TrackList &siblingTrackList(pTrack->GetSiblingTrackList());

        for (pandora::TrackList::const_iterator iter = siblingTrackList.begin(), iterEnd = siblingTrackList.end(); iter != iterEnd; ++iter)
        {
            if (this->HasAssociatedClusters(*iter, false))
                return true;
        }
    }

    // Consider any daughter tracks
    const pandora::TrackList &daughterTrackList(pTrack->GetDaughterTrackList());

    for (pandora::TrackList::const_iterator iter = daughterTrackList.begin(), iterEnd = daughterTrackList.end(); iter != iterEnd; ++iter)
    {
        if (this->HasAssociatedClusters(*iter))
            return true;
    }

    return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------

pandora::StatusCode TrackPreparationAlgorithm::ReadSettings(const pandora::TiXmlHandle xmlHandle)
{
    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadVectorOfValues(xmlHandle,
        "CandidateListNames", m_candidateListNames));

    if (m_candidateListNames.empty())
        return pandora::STATUS_CODE_INVALID_PARAMETER;

    PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "MergedCandidateListName", m_mergedCandidateListName));

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShouldMakeAssociations", m_shouldMakeAssociations));

    if (m_shouldMakeAssociations)
    {
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ProcessAlgorithmList(*this, xmlHandle,
            "trackClusterAssociationAlgorithms", m_associationAlgorithms));
    }

    PANDORA_RETURN_RESULT_IF_AND_IF(pandora::STATUS_CODE_SUCCESS, pandora::STATUS_CODE_NOT_FOUND, !=, pandora::XmlHelper::ReadValue(xmlHandle,
        "ShouldMakePfoTrackList", m_shouldMakePfoTrackList));

    if (m_shouldMakePfoTrackList)
    {
        PANDORA_RETURN_RESULT_IF(pandora::STATUS_CODE_SUCCESS, !=, pandora::XmlHelper::ReadValue(xmlHandle,
            "PfoTrackListName", m_pfoTrackListName));
    }

    return pandora::STATUS_CODE_SUCCESS;
}

} // namespace lc_content
