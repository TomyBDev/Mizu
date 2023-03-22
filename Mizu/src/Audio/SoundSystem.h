#pragma once

#include <Tools/ComManager.h>

class SoundSystem
{
public:
	class Error : public std::runtime_error
	{
	public:
		Error(std::string s)
			:
			runtime_error(std::string("SoundSystem::Error: ") + s)
		{}
	};
	class FileError : public Error
	{
	public:
		FileError(std::string s)
			:
			Error(std::string("SoundSystem::FileError: ") + s)
		{}
	};

	class Channel
	{
	private:
		class VoiceCallback : public IXAudio2VoiceCallback
		{
		public:
			void STDMETHODCALLTYPE OnStreamEnd() override {}
			void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
			void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 samplesRequired) override {}
			void STDMETHODCALLTYPE OnBufferEnd(void* bufferContext) override;
			void STDMETHODCALLTYPE OnBufferStart(void* bufferContext) override {}
			void STDMETHODCALLTYPE OnLoopEnd(void* bufferContext) override {}
			void STDMETHODCALLTYPE OnVoiceError(void* bufferContext, HRESULT error) override {}
		};
	public:
		Channel(SoundSystem& soundSystem);
		~Channel();

		void PlaySoundBuffer(class Sound& s, bool bLoop, float vol, float freqMod);

		void Stop();

	private:
		IXAudio2SourceVoice* source;
		Sound* sound;
		XAUDIO2_BUFFER xaBuffer;
	};

	static SoundSystem& Get();
	static WAVEFORMATEX& GetFormat() { return Get().format; }

	void PlaySoundBuffer(class Sound& s, bool bLoop, float vol, float freqMod);

private:
	SoundSystem();

	void DeactivateChannel(Channel& channel);

	ComManager comManager;
	Microsoft::WRL::ComPtr<IXAudio2> engine;
	IXAudio2MasteringVoice* masterVoice;
	WAVEFORMATEX format;
	std::vector<std::unique_ptr<Channel>> idleChannels;
	std::vector<std::unique_ptr<Channel>> activeChannels;
	std::mutex mutex;
	const int nChannel = 64;
};

class Sound
{
	friend SoundSystem::Channel;
public:
	Sound(const std::string filename);
	~Sound();

	void Play(bool bLoop = false, float vol = 1.f, float freqMod = 1.0f);

private:
	void AddChannel(SoundSystem::Channel& c);
	void RemoveChannel(SoundSystem::Channel& c);

	UINT32 nBytes = 0;
	std::unique_ptr<BYTE[]> data;
	std::vector<SoundSystem::Channel*> activeChannels;
	std::mutex mutex;
};