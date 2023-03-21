#pragma once

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

		void Play(class Sound& s);

		void Stop();

	private:
		IXAudio2SourceVoice* source;
		Sound* sound;
		XAUDIO2_BUFFER xaBuffer;
	};

	static SoundSystem& Get();
	static WAVEFORMATEX& GetFormat() { return Get().format; }

	void PlaySound(class Sound& s);

	~SoundSystem();

private:
	SoundSystem();

	void DeactivateChannel(Channel& channel);

	Microsoft::WRL::ComPtr<IXAudio2> engine;
	IXAudio2MasteringVoice* masterVoice;
	WAVEFORMATEX format;
	std::vector<std::unique_ptr<Channel>> idleChannels;
	std::vector<std::unique_ptr<Channel>> activeChannels;
	const int nChannel = 64;
};

class Sound
{
	friend SoundSystem::Channel;
public:
	Sound(const std::wstring& filename);
	~Sound();

	void Play();

private:
	void AddChannel(SoundSystem::Channel& c);
	void RemoveChannel(SoundSystem::Channel& c);

	UINT32 nBytes = 0;
	std::unique_ptr<BYTE[]> data;
	std::vector<SoundSystem::Channel*> activeChannels;
};