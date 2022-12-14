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

	// DxLib�̃T�E���h�@�\���g��Ȃ��悤�ɐݒ�
	SetNotSoundFlag(true);
	DxLib_Init();

	//XAudio����
	IXAudio2* xAudio2{ nullptr };
	if (FAILED(XAudio2Create(&xAudio2, 0)))
	{
		return -1;
	}

	// �f�o�b�O�ݒ�
	XAUDIO2_DEBUG_CONFIGURATION debug{ 0 };
	debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	debug.BreakMask = XAUDIO2_LOG_ERRORS;
	xAudio2->SetDebugConfiguration(&debug, 0);

	// �}�X�^�[���H�C�X
	IXAudio2MasteringVoice* masterVoice{ nullptr };

	// �}�X�^�[�{�C�X�̍쐬
	if (FAILED(xAudio2->CreateMasteringVoice(&masterVoice)))
	{
		return -1;
	}


	// wave�t�@�C���̓ǂݍ���
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

	// �\�[�X�{�C�X�̐���
	if (FAILED(xAudio2->CreateSourceVoice(&source, &format)))
	{
		return -1;
	}

	// �o�b�t�@�̃Z�b�g
	XAUDIO2_BUFFER buffer{};
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.pAudioData = wave.GetData().data();
	buffer.AudioBytes = static_cast<unsigned int>(wave.GetData().size() * sizeof(wave.GetData()[0]));
	buffer.LoopCount = 0;

	// �\�[�X�{�C�X�Ƀo�b�t�@�𑗂�
	if (FAILED(source->SubmitSourceBuffer(&buffer)))
	{
		return -1;
	}

	// �Đ��J�n
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

	// �\�[�X�{�C�X�̔j��
	source->DestroyVoice();

	// �}�X�^�[�{�C�X�̔j��
	masterVoice->DestroyVoice();

	// XAudio2�̔j��
	xAudio2->Release();
	DxLib_End();
	return 0;
}