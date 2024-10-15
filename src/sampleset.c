#include <tchar.h>

#include "sampleset.h"
#include "config.h"
#include "log.h"
#include "bass.h"
#include "heap.h"

BOOL
    g_bEnableMouse,
    g_bEnableKeyboard;

HSAMPLE
    g_hMouseSample[MOUSE_BUTTON_LEN],
    g_hKeyboardSample[KEYBOARD_BUTTON_LEN];

// Mouse soundpack
MouseSoundpack *g_mspList[] = {
    // TODO
    NULL,
};

// Keyboard soundpack
#include "soundpack/cherrymx_black_abs.h"
#include "soundpack/cherrymx_black_pbt.h"
#include "soundpack/cherrymx_blue_abs.h"
#include "soundpack/cherrymx_blue_pbt.h"
#include "soundpack/cherrymx_brown_abs.h"
#include "soundpack/cherrymx_brown_pbt.h"
#include "soundpack/cherrymx_red_abs.h"
#include "soundpack/cherrymx_red_pbt.h"
#include "soundpack/eg_crystal_purple.h"
#include "soundpack/eg_oreo.h"
#include "soundpack/topre_purple_hybrid_pbt.h"

KeyboardSoundpack *g_kspList[] = {
    &kbsCherryMxBlackAbs,
    &kbsCherryMxBlackPbt,
    &kbsCherryMxBlueAbs,
    &kbsCherryMxBluePbt,
    &kbsCherryMxBrownAbs,
    &kbsCherryMxBrownPbt,
    &kbsCherryMxRedAbs,
    &kbsCherryMxRedPbt,
    &kbsEgCrystalPurple,
    &kbsEgOreo,
    &kbsToprePurpleHybridPbt,
    NULL,
};


void ChangeSampleset(HSAMPLE *phSampleset, size_t nSamplesetLength, PCTSTR pszSampleFile, SampleOffset *psoOffsets)
{
    HSAMPLE hSample = BASS_SampleLoad(FALSE, pszSampleFile, 0, 0, 1, TBASSUNICODE);
    __debug("Loaded soundpack file [id: %u, file: %s]", hSample, pszSampleFile);

    BASS_SAMPLE info;
    BASS_SampleGetInfo(hSample, &info);
    
    void *pData = HeapAlloc(g_hHeap, 0, info.length);
    __debug("Allocated memory for sample data [size: %u, address: %p]", info.length, pData);

    BASS_SampleGetData(hSample, pData);

    __debug("Slicing soundpack");
    for (size_t i=0; i<nSamplesetLength; i++)
    {
        // Unload previous sliced sample
        if (phSampleset[i] != 0)
        {
            __debug("Unloading previous sliced sample [id: %u, index: %d]", phSampleset[i], i);
            BASS_SampleFree(phSampleset[i]);
        }

        // Slice new sample
        if ((psoOffsets[i].start != 0) && (psoOffsets[i].length != 0))
        {
            DWORD   dwStart  = BASS_ChannelSeconds2Bytes(hSample, (double)psoOffsets[i].start  / 1000);
            DWORD   dwLength = BASS_ChannelSeconds2Bytes(hSample, (double)psoOffsets[i].length / 1000);
            HSAMPLE hSlice   = BASS_SampleCreate(dwLength, info.freq, info.chans, SAMPLE_MAX, 0);

            BASS_SampleSetData(hSlice, (void*) ((char*)pData + dwStart) );
            __debug("Sliced sample [id: %u, start: %u length: %u]", hSlice, dwStart, dwLength);

            phSampleset[i] = hSlice;
        }
    }

    HeapFree(g_hHeap, 0, pData);
    __debug("Freed sample data memory [address: %p]", pData);

    BASS_SampleFree(hSample);
    __debug("Unloaded soundpack file [id: %u]", hSample);
}

void PlaySample(HSAMPLE hSample)
{
    __debug("Playing sample [id: %u]", hSample);
    HCHANNEL hChannel = BASS_SampleGetChannel(hSample, 0);
    BASS_ChannelPlay(hChannel, TRUE);
}
