/*
 *  Copyright (C) 2024 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "GUITextureJobManager.h"

#include "ServiceBroker.h"
#include "settings/AdvancedSettings.h"
#include "settings/SettingsComponent.h"
#include "windowing/WinSystem.h"

#include <algorithm>

using namespace std::chrono_literals;
using namespace KODI::GUILIB;

CGUITextureLoaderThread::CGUITextureLoaderThread(CGUITextureJobManager& manager,
                                                 unsigned int threadID)
  : CThread("TextureLoader"), m_threadID(threadID), m_manager(&manager)
{
  Create();
  SetPriority(ThreadPriority::LOWEST);
}

void CGUITextureLoaderThread::OnStartup()
{
  if (CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_guiAsyncTextureUpload)
    m_hasContext = CServiceBroker::GetWinSystem()->BindSecondaryGPUContext(m_threadID);
}

void CGUITextureLoaderThread::Process()
{
  while (std::unique_ptr<CImageLoader> image = m_manager->TakeNextImage())
  {
    image->DoWork(m_hasContext);
    image->m_callback->OnLoadComplete(std::move(image));
  }
}

CGUITextureJobManager::CGUITextureJobManager()
{
  std::unique_lock<CCriticalSection> lock(m_section);

  uint32_t maxThreads =
      CServiceBroker::GetSettingsComponent()->GetAdvancedSettings()->m_guiTextureThreads;
  for (uint32_t i = 0; i < maxThreads; ++i)
    m_textureThread.emplace_back(std::make_unique<CGUITextureLoaderThread>(*this, i));
}

CGUITextureJobManager::~CGUITextureJobManager()
{
  {
    std::unique_lock<CCriticalSection> lock(m_section);
    m_inDestruction = true;
  }
  m_condVar.notifyAll();
  m_textureThread.clear();
}

unsigned int CGUITextureJobManager::AddImageToQueue(std::unique_ptr<CImageLoader> image)
{
  std::unique_lock<CCriticalSection> lock(m_section);

  image->m_imageID = m_imageIDCounter;

  m_imageQueue.emplace_back(std::move(image));

  m_condVar.notify();

  return m_imageIDCounter++;
}

void CGUITextureJobManager::CancelImageLoad(unsigned int imageID)
{
  std::unique_lock<CCriticalSection> lock(m_section);

  m_imageQueue.erase(std::remove_if(m_imageQueue.begin(), m_imageQueue.end(),
                                    [imageID](const std::unique_ptr<CImageLoader>& loader)
                                    { return loader->m_imageID == imageID; }),
                     m_imageQueue.end());
}

std::unique_ptr<CImageLoader> CGUITextureJobManager::TakeNextImage()
{
  std::unique_lock<CCriticalSection> lock(m_section);

  if (m_imageQueue.empty() && !m_inDestruction)
  {
    m_condVar.wait(lock, [this]() { return !m_imageQueue.empty() || m_inDestruction; });
  }

  if (!m_inDestruction)
  {
    std::unique_ptr<CImageLoader> image = std::move(m_imageQueue.front());
    m_imageQueue.pop_front();
    return image;
  }

  return nullptr;
}
