//------------------------------------------------------------------------
// Copyright(c) 2023 pjmw.
//------------------------------------------------------------------------

#include "mypluginprocessor.h"
#include "myplugincids.h"
#include <fftw3.h>

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"

using namespace Steinberg;

namespace xD {
//------------------------------------------------------------------------
// plugin1Processor
//------------------------------------------------------------------------
plugin1Processor::plugin1Processor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kplugin1ControllerUID);
}

//------------------------------------------------------------------------
plugin1Processor::~plugin1Processor()
{}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Processor::initialize(FUnknown* context)
{
	// Here the Plug-in will be instantiated
	
	//---always initialize the parent-------
	tresult result=AudioEffect::initialize(context);
	// if everything Ok, continue
	if (result!=kResultOk)
	{
		return result;
	}

	//--- create Audio IO ------
	addAudioInput (STR16 ("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

	/* If you don't need an event bus, you can remove the next line */
	//addEventInput (STR16("Event In"),1);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Processor::terminate()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!
	//---do not forget to call parent ------
	return AudioEffect::terminate();
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Processor::setActive(TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	/*Steinberg::Vst::SpeakerArrangement arr;
	if (getBusArrangement (Steinberg::Vst::kOutput, 0, arr) != kResultTrue)
	{
		return kResultFalse;
	}
	int32 numChannels = Steinberg::Vst::SpeakerArr::getChannelCount (arr);
	if (numChannels == 0)
	{
		return kResultFalse;
	}*/

	SR=processSetup.sampleRate;
	if (state)
	{
		mem=new Vst::Sample32[size_t(numChannels*ML+20)]();
		for (int i=0;i<numChannels;i++)
		{
			//writeidx[i]=0+i*ML;
			//maxdelay*value-tdelay1
			readidx[i]=size_t(i*ML);
			writeidx[i]=size_t(tdelay1*SR+i*ML);
		}
		
		F1=new Vst::Sample32[TT1+10]();
		F1idx=0;
		for (int i=0;i<period1*SR;i++)
		{
			F1[i]=tdelay1*SR+amplitude1*tdelay1*SR*sin(2.0*3.1415926/(period1+1E-10)*double(i));
		}

	}
	if (!state)
	{
		memset(mem,0.0,size_t(numChannels*ML*sizeof(Vst::Sample32)));
		delete[] mem;
		delete[] F1;

	}
	return AudioEffect::setActive(state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Processor::process(Vst::ProcessData& data)
{
	//--- First : Read inputs parameter changes-----------

	//int32 numChannels=data.inputs[0].numChannels;
    if (data.inputParameterChanges)
    {
        int32 numParamsChanged=data.inputParameterChanges->getParameterCount();
        for (int32 index=0; index<numParamsChanged; index++)
        {
            if (auto* paramQueue=data.inputParameterChanges->getParameterData(index))
            {
                Vst::ParamValue value;
                int32 sampleOffset;
                int32 numPoints=paramQueue->getPointCount();
                switch (paramQueue->getParameterId())
                {
					case GainParams::kParamGainId:
                    {
						if (paramQueue->getPoint(numPoints-1,sampleOffset,value)==kResultTrue)
                        {
						    ingain=value;
						}
						break;
					}
					case GainParams::kParamDelayGainId:
                    {
						if (paramQueue->getPoint(numPoints-1,sampleOffset,value)==kResultTrue)
                        {
						    d1gain=value;
						}
						break;
					}
					case DelayParams::kParamDelayId:
                    {
					    if (paramQueue->getPoint(numPoints-1,sampleOffset,value)==kResultTrue)
                        {
						    tdelay1=maxdelay*value;
						}
						for (int i=0;i<numChannels;i++)
						{
							//writeidx[i]=0+i*ML;
							//maxdelay*value-tdelay1
							readidx[i]=size_t(i*ML);
							writeidx[i]=size_t(tdelay1*SR+i*ML);
						}
						F1idx=0;
						for (int i=0;i<period1*SR;i++)
						{
							F1[i]=tdelay1*SR+amplitude1*tdelay1*SR*sin(2.0*3.1415926/(period1+1E-10)*double(i));
						}
						break;
					}
					case DelayParams::kParamDelayPanId:
                    {
						if (paramQueue->getPoint(numPoints-1,sampleOffset,value)==kResultTrue)
                        {
						    pan1=value;
						}
						break;
					}
					case GainParams::kParamMasterId:
                    {
						if (paramQueue->getPoint(numPoints-1,sampleOffset,value)==kResultTrue)
                        {
						    master=2*value;
						}
						break;
					}
					case FlangerParams::kParamFlangerAmplitudeId:
                    {
						if (paramQueue->getPoint(numPoints-1,sampleOffset,value)==kResultTrue)
                        {
						    amplitude1=value;
						}
						for (int i=0;i<period1*SR;i++)
						{
							F1[i]=tdelay1*SR+amplitude1*tdelay1*SR*sin(2.0*3.1415926/(period1+1E-10)*double(i));
						}
						break;
					}
					case FlangerParams::kParamFlangerPeriodId:
                    {
						if (paramQueue->getPoint(numPoints-1,sampleOffset,value)==kResultTrue)
                        {
						    period1=flangermax*value;
						}
						for (int i=0;i<period1*SR;i++)
						{
							F1[i]=tdelay1*SR+amplitude1*tdelay1*SR*sin(2.0*3.1415926/(period1+1E-10)*double(i));
						}
						break;
					}
				}
			}
		}
	}
	
	//--- Here you have to implement your processing

	//---get audio buffers using helper-functions(vstaudioprocessoralgo.h)-------------
	uint32 sampleFramesSize=getSampleFramesSizeInBytes(processSetup,data.numSamples);
	void** in=getChannelBuffersPointer(processSetup,data.inputs[0]);
	void** out=getChannelBuffersPointer(processSetup,data.outputs[0]);

	if (data.numInputs == 0 || data.numSamples == 0)
    {
		return kResultOk;
	}

	if (data.inputs[0].silenceFlags != 0)
	{
		// mark output silence too
		data.outputs[0].silenceFlags = data.inputs[0].silenceFlags;
		memset(mem,0.0,size_t(numChannels*ML*sizeof(Vst::Sample32)));

		// the plug-in has to be sure that if it sets the flags silence that the output buffer are clear
		for (int32 i=0;i<numChannels;i++)
		{
			// do not need to be cleared if the buffers are the same (in this case input buffer are
			// already cleared by the host)
			if (in[i] != out[i])
			{
				memset(out[i], 0, sampleFramesSize);
			}
		}
		// nothing to do at this point
		return kResultOk;
	}
	
	//assert output
	data.outputs[0].silenceFlags = 0;

	//float gain=mGain;
	//float delay=mDelay;
	// for each channel
	//int32 del=2048;
	//int32 cunt0=0;
	//int32 cunt1=0;
	for (int32 i=0; i<numChannels; i++)
	{
		int32 samples = data.numSamples;
		Vst::Sample32* ptrIn = (Vst::Sample32*)in[i];//i=0=>LEFT CHANNNNEL
		//ptrIn++;
		Vst::Sample32* ptrOut = (Vst::Sample32*)out[i];
		//ptrOut++;
		//Vst::Sample32 tmp;
		//Vst::Sample32 tdel;
		// for each sample in this channel
		//while (--samples >= 0)
		for (int j=0; j<samples; j++)
		{
			// apply delay
			//tmp = (*ptrIn++) * gain;
			//(*ptrOut++) = tmp;
			
			//ptrIn[j]=ptrIn[j]*ingain;
			mem[writeidx[i]]=ptrIn[j];
			ptrOut[j]=(ptrIn[j]*ingain+mem[readidx[i]]*d1gain*((4.0*pan1-2)*((Vst::Sample32)i-0.5)+1))*master;//genius panning
			
			
			/*if (i==0)
			{
				mem0[cunt0]=&(*ptrIn++);
				if (cunt0!=4095)
				{
					cunt0++;
				}
				else
				{
					cunt0=0;
				}
			}
			if (i==1)
			{
				mem1[cunt1]=&(*ptrIn++);
				if (cunt1!=4095)
				{
					cunt1++;
				}
				else
				{
					cunt1=0;
				}
			}*/

			//FLANGER
			if(writeidx[i]-readidx[i]>0.0)
			{
				diff=writeidx[i]-readidx[i];
			}
			else
			{
				diff=SR*ML+writeidx[i]-readidx[i];
			}
			
			if (diff>F1[F1idx])
			{
				if (readidx[i]<(i+1)*ML-1)
				{
					readidx[i]++;
				}
				else 
				{
					readidx[i]=size_t(i*ML);
				}
			}
			else
			{
				goto skip;
			}

			
			//ALMOST END FLANGER PROCEDURE
			if (readidx[i]<(i+1)*ML-1)
			{
				readidx[i]++;
			}
			else 
			{
				readidx[i]=size_t(i*ML);
			}
			skip:;

			if (F1idx<period1*SR)
			{
				F1idx++;
			}
			else 
			{
				F1idx=0;
			}

			if (writeidx[i]<(i+1)*ML-1)
			{
				writeidx[i]++;
			}
			else 
			{
				writeidx[i]=size_t(i*ML);
			}

			
		}
	}
	


	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Processor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	//memset(mem,0.0,(int)numChannels*ML*sizeof(float)); //crashes the entire DAW
	//for (int i=0;i<numChannels;i++)
	//{
	//	readidx[i]=i*ML;
	//	writeidx[i]=tdelay1*SR+i*ML;
	//}	
	return AudioEffect::setupProcessing (newSetup);
	//--- called before any processing ----
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Processor::canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Processor::setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);
	
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API plugin1Processor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace xD
