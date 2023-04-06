#include <mzpch.h>
#include <Audio/SoundSystem.h>

SoundSystem::SoundSystem()
{
	// Initialise Format Data.
	format.cbSize = 24932;
	format.wFormatTag = 1;
	format.nChannels = 2;
	format.nSamplesPerSec = 44100;
	format.nAvgBytesPerSec = 176400;
	format.nBlockAlign = 4;
	format.wBitsPerSample = 16;

	// Create instance of XAudio2 Engine.
	CHECK_ERROR(XAudio2Create(&engine, 0, XAUDIO2_DEFAULT_PROCESSOR));

	// Create Mastering Voice
	CHECK_ERROR(engine->CreateMasteringVoice(&masterVoice));

	// Create the number of channels specified and add them to inactive list.
	for (int i = 0; i < nChannel; i++)
		idleChannels.push_back(std::make_unique<Channel>(*this));
}

SoundSystem& SoundSystem::Get()
{
	static SoundSystem instance;
	return instance;
}

void SoundSystem::PlaySoundBuffer(Sound& s, bool bLoop, float vol, float freqMod)
{
	std::lock_guard<std::mutex> lock(mutex);
	if (!idleChannels.empty())
	{
		activeChannels.push_back(std::move(idleChannels.back()));
		idleChannels.pop_back();
		activeChannels.back()->PlaySoundBuffer(s, bLoop, vol, freqMod);
	}
}

void SoundSystem::DeactivateChannel(Channel& channel)
{
	std::lock_guard<std::mutex> lock(mutex);
	auto i = std::find_if(activeChannels.begin(), activeChannels.end(), [&channel](const std::unique_ptr<Channel>& chan) -> bool
		{
			return &channel == chan.get();
		});

	idleChannels.push_back(std::move(*i));
	activeChannels.erase(i);
}

// Channel

SoundSystem::Channel::Channel(SoundSystem& ss)
{
	static VoiceCallback voiceCallback;
	ZeroMemory(&xaBuffer, sizeof(xaBuffer));
	xaBuffer.pContext = this;
	ss.engine->CreateSourceVoice(&source, &ss.format, 0u, 2.f, &voiceCallback);
}

SoundSystem::Channel::~Channel()
{
	if (source != nullptr)
	{
		source->DestroyVoice();
		source = nullptr;
	}

	if (sound != nullptr)
		sound == nullptr;
}

void SoundSystem::Channel::PlaySoundBuffer(Sound& s, bool bLoop, float vol, float freqMod)
{
	s.AddChannel(*this);
	sound = &s;
	xaBuffer.pAudioData = s.data.get();
	xaBuffer.AudioBytes = s.nBytes;
	if (bLoop)
		xaBuffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	source->SubmitSourceBuffer(&xaBuffer, nullptr);
	source->SetFrequencyRatio(freqMod);
	source->SetVolume(vol);
	source->Start();
}

void SoundSystem::Channel::Stop()
{
	if (!source)
	{
		LOG_ERROR("Failed to stop sound due to null source");
		return;
	}

	if (!sound)
	{
		LOG_ERROR("Failed to stop sound due to null sound");
		return;
	}

	source->Stop();
	source->FlushSourceBuffers();
}

void SoundSystem::Channel::VoiceCallback::OnBufferEnd(void* bufferContext)
{
	Channel& chan = *(Channel*)bufferContext;
	chan.Stop();
	chan.sound->RemoveChannel(chan);
	chan.sound = nullptr;
	Get().DeactivateChannel(chan);
}

// Sound

Sound::Sound(const std::string filename)
{
	unsigned int fileSize = 0;
	std::unique_ptr<BYTE[]> pFileIn;
	try
	{
		{
			std::ifstream file;
			file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try
			{
				std::string s = "Content/Audio/";
				s.append(filename);

				file.open(s, std::ios::binary);
			}
			catch (std::exception e)
			{
				LOG_ERROR("Error opening file!"); LOG_FLUSH();
				throw SoundSystem::FileError(
					std::string("error opening file: ") + e.what());
			}

			{
				int fourcc;
				file.read(reinterpret_cast<char*>(&fourcc), 4);
				if (fourcc != 'FFIR')
				{
					throw SoundSystem::FileError("bad fourCC");
				}
			}

			file.read(reinterpret_cast<char*>(&fileSize), 4);
			fileSize += 8; // entry doesn't include the fourcc or itself
			if (fileSize <= 44)
			{
				throw SoundSystem::FileError("file too small");
			}

			file.seekg(0, std::ios::beg);
			pFileIn = std::make_unique<BYTE[]>(fileSize);
			file.read(reinterpret_cast<char*>(pFileIn.get()), fileSize);
		}

		if (*reinterpret_cast<const int*>(&pFileIn[8]) != 'EVAW')
		{
			throw SoundSystem::FileError("format not WAVE");
		}

		//look for 'fmt ' chunk id
		WAVEFORMATEX format;
		bool bFilledFormat = false;
		for (unsigned int i = 12; i < fileSize; )
		{
			if (*reinterpret_cast<const int*>(&pFileIn[i]) == ' tmf')
			{
				memcpy(&format, &pFileIn[i + 8], sizeof(format));
				bFilledFormat = true;
				break;
			}
			// chunk size + size entry size + chunk id entry size + word padding
			i += (*reinterpret_cast<const int*>(&pFileIn[i + 4]) + 9) & 0xFFFFFFFE;
		}
		if (!bFilledFormat)
		{
			throw SoundSystem::FileError("fmt chunk not found");
		}

		// compare format with sound system format
		{
			const WAVEFORMATEX sysFormat = SoundSystem::GetFormat();

			if (format.nChannels != sysFormat.nChannels)
			{
				throw SoundSystem::FileError("bad wave format (nChannels)");
			}
			else if (format.wBitsPerSample != sysFormat.wBitsPerSample)
			{
				throw SoundSystem::FileError("bad wave format (wBitsPerSample)");
			}
			else if (format.nSamplesPerSec != sysFormat.nSamplesPerSec)
			{
				throw SoundSystem::FileError("bad wave format (nSamplesPerSec)");
			}
			else if (format.wFormatTag != sysFormat.wFormatTag)
			{
				throw SoundSystem::FileError("bad wave format (wFormatTag)");
			}
			else if (format.nBlockAlign != sysFormat.nBlockAlign)
			{
				throw SoundSystem::FileError("bad wave format (nBlockAlign)");
			}
			else if (format.nAvgBytesPerSec != sysFormat.nAvgBytesPerSec)
			{
				throw SoundSystem::FileError("bad wave format (nAvgBytesPerSec)");
			}
		}

		//look for 'data' chunk id
		bool bFilledData = false;
		for (unsigned int i = 12; i < fileSize; )
		{
			const int chunkSize = *reinterpret_cast<const int*>(&pFileIn[i + 4]);
			if (*reinterpret_cast<const int*>(&pFileIn[i]) == 'atad')
			{
				data = std::make_unique<BYTE[]>(chunkSize);
				nBytes = chunkSize;
				memcpy(data.get(), &pFileIn[i + 8], nBytes);

				bFilledData = true;
				break;
			}
			// chunk size + size entry size + chunk id entry size + word padding
			i += (chunkSize + 9) & 0xFFFFFFFE;
		}
		if (!bFilledData)
		{
			throw SoundSystem::FileError("data chunk not found");
		}
	}
	catch (SoundSystem::Error e)
	{
		throw e;
	}
	catch (std::exception e)
	{
		throw SoundSystem::FileError(e.what());
	}
}

Sound::~Sound()
{
	{
		std::lock_guard<std::mutex> lock(mutex);
		for (auto channel : activeChannels)
		{
			channel->Stop();
		}
	}

	bool allChannelsDeactivated = false;
	do
	{
		std::lock_guard<std::mutex> lock(mutex);
		allChannelsDeactivated = activeChannels.empty();
	} while (!allChannelsDeactivated);
}

void Sound::Play(bool bLoop, float vol, float freqMod)
{
	SoundSystem::Get().PlaySoundBuffer(*this, bLoop, vol, freqMod);
}

void Sound::AddChannel(SoundSystem::Channel& c)
{
	std::lock_guard<std::mutex> lock(mutex);
	activeChannels.push_back(&c);
}

void Sound::RemoveChannel(SoundSystem::Channel& c)
{
	std::lock_guard<std::mutex> lock(mutex);
	activeChannels.erase(std::find(activeChannels.begin(), activeChannels.end(), &c));
}