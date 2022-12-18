#include <wrl.h>
#include <xaudio2.h>
#include <DxLib.h>
#include "Wave.h"

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	SetGraphMode(1280, 720, 32);
	ChangeWindowMode(true);
	SetWindowText(L"Test");

	// DxLibのサウンド機能を使わないように設定
	SetNotSoundFlag(true);
	DxLib_Init();

	//XAudio生成
	IXAudio2* xAudio2{ nullptr };
	if (FAILED(XAudio2Create(&xAudio2, 0)))
	{
		return -1;
	}

	// デバッグ設定
	XAUDIO2_DEBUG_CONFIGURATION debug{ 0 };
	debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	debug.BreakMask = XAUDIO2_LOG_ERRORS;
	xAudio2->SetDebugConfiguration(&debug, 0);

	// マスターヴォイス
	IXAudio2MasteringVoice* masterVoice{ nullptr };

	// マスターボイスの作成
	if (FAILED(xAudio2->CreateMasteringVoice(&masterVoice)))
	{
		return -1;
	}


	// waveファイルの読み込み
	Wave wave{};
	if (!wave.Load("./exp.wav"))
	{
		return -1;
	}
	IXAudio2SourceVoice* source1{ nullptr };
	IXAudio2SourceVoice* source2{ nullptr };
	WAVEFORMATEX format{
		wave.GetFmt().type,
		wave.GetFmt().channel,
		wave.GetFmt().sample,
		wave.GetFmt().byte,
		wave.GetFmt().block,
		wave.GetFmt().bit,
		0u
	};


	// ソースボイスの生成
	if (FAILED(xAudio2->CreateSourceVoice(&source1, &format)))
	{
		return -1;
	}
	if (FAILED(xAudio2->CreateSourceVoice(&source2, &format)))
	{
		return -1;
	}

	XAUDIO2_VOICE_SENDS send{};
	XAUDIO2_SEND_DESCRIPTOR s;
	s.Flags = 0;
	s.pOutputVoice = masterVoice;
	send.pSends = &s;
	send.SendCount = 1;
	//auto r = source1->SetOutputVoices(&send);

	// バッファのセット
	XAUDIO2_BUFFER buffer{};
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.pAudioData = wave.GetData().data();
	buffer.AudioBytes = static_cast<unsigned int>(wave.GetData().size() * sizeof(wave.GetData()[0]));
	buffer.LoopCount = 0;

	// ソースボイスにバッファを送る
	if (FAILED(source1->SubmitSourceBuffer(&buffer)))
	{
		return -1;
	}

	// 再生開始
	if (FAILED(source1->Start()))
	{
		return -1;
	}





	float nowVolume;
	masterVoice->GetVolume(&nowVolume);
	masterVoice->SetVolume(0.5f);

	source2->SetVolume(0.25f);
	bool isSource2Play{ false };

	while (true)
	{
		XAUDIO2_VOICE_STATE state{};
		source1->GetState(&state);

		if (ProcessMessage() != 0)
		{
			break;
		}
		if (state.BuffersQueued == 0&& !isSource2Play)
		{
			// ソースボイスにバッファを送る
			if (FAILED(source2->SubmitSourceBuffer(&buffer)))
			{
				return -1;
			}

			// 再生開始
			if (FAILED(source2->Start()))
			{
				return -1;
			}
			isSource2Play = true;
		}


		if (CheckHitKey(KEY_INPUT_UP))
		{
			nowVolume = std::clamp(nowVolume += 0.1f, 0.0f, 1.0f);
			masterVoice->SetVolume(nowVolume);
		}
		if (CheckHitKey(KEY_INPUT_DOWN))
		{
			nowVolume = std::clamp(nowVolume -= 0.1f, 0.0f, 1.0f);
			masterVoice->SetVolume(nowVolume);
		}
		

		SetDrawScreen(DX_SCREEN_BACK);
		ClsDrawScreen();

		ScreenFlip();
	}

	// ソースボイスの破棄
	source1->DestroyVoice();
	source2->DestroyVoice();

	// マスターボイスの破棄
	masterVoice->DestroyVoice();

	// XAudio2の破棄
	xAudio2->Release();
	DxLib::DxLib_End();
	return 0;
}