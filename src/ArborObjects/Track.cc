/// \file Track.cc
/*
 *
 * Track.cc source template automatically generated by a class generator
 * Creation date : jeu. sept. 24 2015
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


#include "ArborObjects/Track.h"

#include <algorithm>

namespace arbor_content
{

  Track::Track(const PandoraApi::Track::Parameters &parameters) :
    pandora::Track(parameters)
  {
    /* nop */
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  Track::~Track()
  {
    /* nop */
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  const TrackList &Track::GetNearbyTrackList() const
  {
    return m_nearbyTrackList;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode Track::AddNearbyTrack(const Track *pNearbyTrack)
  {
    if(NULL == pNearbyTrack)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    if(!m_nearbyTrackList.insert(pNearbyTrack).second)
      return pandora::STATUS_CODE_ALREADY_PRESENT;

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode Track::RemoveNearbyTrack(const Track *pNearbyTrack)
  {
    if(NULL == pNearbyTrack)
      return pandora::STATUS_CODE_INVALID_PARAMETER;

    TrackList::iterator findIter = std::find(m_nearbyTrackList.begin(), m_nearbyTrackList.end(), pNearbyTrack);

    if(m_nearbyTrackList.end() == findIter)
      return pandora::STATUS_CODE_NOT_FOUND;

    m_nearbyTrackList.erase(findIter);

    return pandora::STATUS_CODE_SUCCESS;
  }

  //------------------------------------------------------------------------------------------------------------------------------------------

  pandora::StatusCode Track::RemoveAllNearbyTracks()
  {
    m_nearbyTrackList.clear();
    return pandora::STATUS_CODE_SUCCESS;
  }

} 

