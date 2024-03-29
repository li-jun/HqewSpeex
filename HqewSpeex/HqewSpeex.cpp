// HqewSpeex.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "HqewSpeex.h"
#include "speex\speex.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "speex/speex_header.h"
#include "speex/speex_echo.h"
#include "speex/speex_preprocess.h" 

#define FRAME_SIZE 160

spx_int16_t encoder_input[FRAME_SIZE];
void *encoder_state;
SpeexBits encoder_bits;
//const SpeexMode speex_nb_mode;

 
extern "C" __declspec(dllexport) void __stdcall encoder_init(int quality)
{
    encoder_state = speex_encoder_init(&speex_nb_mode);
    speex_encoder_ctl(encoder_state, SPEEX_SET_QUALITY, &quality);
    speex_bits_init(&encoder_bits);
}

extern "C" __declspec(dllexport) void __stdcall encoder_dispose()
{
    speex_encoder_destroy(encoder_state);
    speex_bits_destroy(&encoder_bits);
}

extern "C" __declspec(dllexport) int __stdcall encoder_encode(const short *data, char *output)
{
    for (int i = 0; i < FRAME_SIZE; i++)
        encoder_input[i] = data[i];
    speex_bits_reset(&encoder_bits);
    speex_encode_int(encoder_state, encoder_input, &encoder_bits);
    return speex_bits_write(&encoder_bits, output, 200);
}


spx_int16_t decoder_output[FRAME_SIZE];
void *decoder_state;
SpeexBits decoder_bits;

extern "C" __declspec(dllexport) void __stdcall decoder_init()
{
    decoder_state = speex_decoder_init(&speex_nb_mode);
    int tmp = 1;
    speex_decoder_ctl(decoder_state, SPEEX_SET_ENH, &tmp);
    speex_bits_init(&decoder_bits);
}
extern "C" __declspec(dllexport) void __stdcall decoder_dispose()
{
    speex_decoder_destroy(decoder_state);
    speex_bits_destroy(&decoder_bits);
}
extern "C" __declspec(dllexport) void __stdcall decoder_decode(int nbBytes, char *data, short *output)
{
    speex_bits_read_from(&decoder_bits, data, nbBytes);
	speex_decode_int(decoder_state, &decoder_bits, decoder_output);    
	//speex_decode(decoder_state, &decoder_bits, decoder_output);
    for (int i = 0; i < FRAME_SIZE; i++)
    {
        output[i] = decoder_output[i];
    }
}
																	   )


/***************************************************  回音消除 **************************************/

bool      m_bSpeexEchoHasInit;
SpeexEchoState*   m_SpeexEchoState;
SpeexPreprocessState* m_pPreprocessorState;
int      m_nFilterLen;
int      m_nSampleRate;
float*   m_pfNoise;

extern "C" __declspec(dllexport) void __stdcall SpeexEchoCapture(short* input_frame, short* output_frame)
{
    speex_echo_capture(m_SpeexEchoState, input_frame, output_frame);
}

extern "C" __declspec(dllexport) void __stdcall SpeexEchoPlayback(short* echo_frame)
{
    speex_echo_playback(m_SpeexEchoState, echo_frame);
}

extern "C" __declspec(dllexport) void __stdcall SpeexEchoReset()
{
    if (m_SpeexEchoState != NULL)
    {
        speex_echo_state_destroy(m_SpeexEchoState);
        m_SpeexEchoState = NULL;
    }
    if (m_pPreprocessorState != NULL)
    {
        speex_preprocess_state_destroy(m_pPreprocessorState);
        m_pPreprocessorState = NULL;
    }
    if (m_pfNoise != NULL)
    {
        delete []m_pfNoise;
        m_pfNoise = NULL;
    }
    m_bSpeexEchoHasInit = false;
}

extern "C" __declspec(dllexport) void __stdcall SpeexEchoInit(int filter_length, int sampling_rate ,bool associatePreprocesser)
{
    SpeexEchoReset(); 

    if (filter_length<=0 || sampling_rate<=0)
    {
      m_nFilterLen  = 160*8;
      m_nSampleRate = 8000;
    }
    else
    {
      m_nFilterLen  = filter_length;
      m_nSampleRate = sampling_rate;
    }

    m_SpeexEchoState = speex_echo_state_init(FRAME_SIZE, m_nFilterLen);
    m_pPreprocessorState = speex_preprocess_state_init(FRAME_SIZE, m_nSampleRate);
    if(associatePreprocesser)
    {
        speex_preprocess_ctl(m_pPreprocessorState, SPEEX_PREPROCESS_SET_ECHO_STATE,m_SpeexEchoState);
    }
    m_pfNoise = new float[FRAME_SIZE+1];
    m_bSpeexEchoHasInit = true;
}

extern "C" __declspec(dllexport) void __stdcall SpeexEchoDoAEC(short* mic, short* ref, short* out)
{
    if (!m_bSpeexEchoHasInit)
    {
      return;
    }

    speex_echo_cancellation(m_SpeexEchoState,(const __int16 *) mic,(const __int16 *) ref,(__int16 *) out);
}