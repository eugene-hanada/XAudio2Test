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
	IXAudio2SourceVoice* source{ nullptr };
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
	if (FAILED(xAudio2->CreateSourceVoice(&source, &format)))
	{
		return -1;
	}

	// バッファのセット
	XAUDIO2_BUFFER buffer{};
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.pAudioData = wave.GetData().data();
	buffer.AudioBytes = static_cast<unsigned int>(wave.GetData().size() * sizeof(wave.GetData()[0]));
	buffer.LoopCount = 0;

	// ソースボイスにバッファを送る
	if (FAILED(source->SubmitSourceBuffer(&buffer)))
	{
		return -1;
	}

	// 再生開始
	if (FAILED(source->Start()))
	{
		return -1;
	}

	while (true)
	{
		XAUDIO2_VOICE_STATE state{};
		source->GetState(&state);

		if (ProcessMessage() != 0 && state.BuffersQueued == 0)
		{
			break;
		}
		SetDrawScreen(DX_SCREEN_BACK);
		ClsDrawScreen();

		ScreenFlip();
	}

	// ソースボイスの破棄
	source->DestroyVoice();

	// マスターボイスの破棄
	masterVoice->DestroyVoice();

	// XAudio2の破棄
	xAudio2->Release();
	DxLib_End();
	return 0;
}