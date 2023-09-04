//------------------------------------------------------------------------
// Copyright(c) 2023 My Plug-in Company.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace MyCompanyName {
//------------------------------------------------------------------------
enum GainParams : Steinberg::Vst::ParamID
{
    kParamGainId = 102, // should be a unique id...
};

static const Steinberg::FUID kdnoProcessorUID (0x5BF9FF88, 0xC4485B3F, 0xBE955E11, 0xE988BE0D);
static const Steinberg::FUID kdnoControllerUID (0x5D82A466, 0xD6C854E3, 0x83B41BA1, 0x5C5EAA87);

#define dnoVST3Category "Fx"

//------------------------------------------------------------------------
} // namespace MyCompanyName
