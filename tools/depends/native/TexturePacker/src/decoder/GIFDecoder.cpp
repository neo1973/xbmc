/*
 *      Copyright (C) 2014 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include <cstring>
#include <utility>
#include "GIFDecoder.h"
#include "GifHelper.h"

// returns true for gif files, otherwise returns false
bool GIFDecoder::CanDecode(const std::string &filename)
{
  return std::string::npos != filename.rfind(".gif",filename.length() - 4, 4);
}

bool GIFDecoder::LoadFile(const std::string &filename, DecodedFrames &frames)
{
  std::unique_ptr<GifHelper> gifImage(new GifHelper());
  if (gifImage->LoadGif(filename.c_str()))
  {
    const auto& extractedFrames = gifImage->GetFrames();
    if (extractedFrames.size() > 0)
    {
      for (const auto& ef : extractedFrames)
      {
        DecodedFrame frame;
        const auto frameSize = gifImage->GetPitch() * gifImage->GetHeight();
        
        frame.rgbaImage.pixels = ef->m_pImage;
        frame.rgbaImage.height = gifImage->GetHeight();
        frame.rgbaImage.width = gifImage->GetWidth();
        frame.rgbaImage.bbp = 32;
        frame.rgbaImage.pitch = gifImage->GetPitch();
        frame.delay = ef->m_delay;
        
        frames.frameList.push_back(std::move(frame));
      }
    }
    frames.user = std::move(gifImage);
    return true;
  }
  else
  {
    return false;
  }
}

void GIFDecoder::FreeDecodedFrames(DecodedFrames &frames)
{
  frames.clear();
}

void GIFDecoder::FillSupportedExtensions()
{
  m_supportedExtensions.push_back(".gif");
}
