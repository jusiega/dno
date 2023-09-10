//------------------------------------------------------------------------
// Copyright(c) 2023 pjmw.
//------------------------------------------------------------------------

#include "myplugincontroller.h"
#include "myplugincids.h"
#include "pluginterfaces/base/ustring.h"


using namespace Steinberg;

namespace xD {

//------------------------------------------------------------------------
// plugin1Controller Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Controller::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
	{
		return result;
	}

	// Here you could register some parameters
	parameters.addParameter (STR16 ("Input Gain"), STR16 ("dB"), 0, 1, Vst::ParameterInfo::kCanAutomate, GainParams::kParamGainId);
	parameters.addParameter (STR16 ("Delay 1 Time"), STR16 ("s"), 0, 0.5, Vst::ParameterInfo::kCanAutomate, DelayParams::kParamDelayId);
	parameters.addParameter (STR16 ("Delay 1 Gain"), STR16 ("dB"), 0, 1, Vst::ParameterInfo::kCanAutomate, GainParams::kParamDelayGainId);
	parameters.addParameter (STR16 ("Delay 1 Pan"), STR16 ("L/R"), 0, 0.5, Vst::ParameterInfo::kCanAutomate, DelayParams::kParamDelayPanId);
	parameters.addParameter (STR16 ("Flanger 1 Amplitude"), STR16 ("%"), 0, 0.5, Vst::ParameterInfo::kCanAutomate, FlangerParams::kParamFlangerAmplitudeId);
	parameters.addParameter (STR16 ("Flanger 1 Period"), STR16 ("s"), 0, 0.5, Vst::ParameterInfo::kCanAutomate, FlangerParams::kParamFlangerPeriodId);
	parameters.addParameter (STR16 ("Master"), STR16 ("dB"), 0, 0.5, Vst::ParameterInfo::kCanAutomate, GainParams::kParamMasterId);
	
	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Controller::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Controller::setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Controller::setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Controller::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//NO USE - GUI
IPlugView* PLUGIN_API plugin1Controller::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
        return nullptr;
	}
	return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Controller::setParamNormalized (Vst::ParamID tag, Vst::ParamValue value)
{
	// called by host to update your parameters
	tresult result = EditControllerEx1::setParamNormalized (tag, value);
	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Controller::getParamStringByValue (Vst::ParamID tag, Vst::ParamValue valueNormalized, Vst::String128 string)
{
	// called by host to get a string for given normalized value of a specific parameter
	// (without having to set the value!)

	UString128 result;
	switch (tag)
	{
		case kParamGainId:
		{
			double gejn=20.0*log10(1.0*valueNormalized);
			result.printFloat(gejn);
			break;
			/*int ldel = (int32)(32766 * valueNormalized * valueNormalized);
			if (ldel<4) ldel=4;
			result.printInt (ldel * 1000. / sampleRate);
			break;*/
		}
		case kParamDelayGainId:
		{
			double gejn=20.0*log10(1.0*valueNormalized);
			result.printFloat(gejn);
			break;
		}
		case kParamDelayPanId:
		{
			double pan=2.0*valueNormalized-1.0;
			result.printFloat(pan);
			break;
		}
		case kParamMasterId:
		{
			double gejn=20.0*log10(2.0*valueNormalized);
			result.printFloat(gejn);
			break;
		}
		case kParamDelayId:
		{
			double tajm=valueNormalized*maxdelay;
			result.printFloat(tajm);
			break;
		}
		case kParamFlangerAmplitudeId:
		{
			//UString128 p="%";
			double procent=valueNormalized*100;
			result.printFloat(procent);
			//result.append(p);
			break;
		}
		case kParamFlangerPeriodId:
		{
			double sek=valueNormalized*flangermax;
			result.printFloat(sek);
			break;
		}
		default:
			return EditControllerEx1::getParamStringByValue (tag, valueNormalized, string);
	}
	result.copyTo (string, 128);
	return kResultTrue;
	//return EditControllerEx1::getParamStringByValue (tag, valueNormalized, string);
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Controller::getParamValueByString (Vst::ParamID tag, Vst::TChar* string, Vst::ParamValue& valueNormalized)
{
	// called by host to get a normalized value from a string representation of a specific parameter
	// (without having to set the value!)
	return EditControllerEx1::getParamValueByString (tag, string, valueNormalized);
	///////////?????????????????????????????????
}

//------------------------------------------------------------------------
} // namespace xD
