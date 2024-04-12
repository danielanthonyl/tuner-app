#pragma once

#include <portaudio.h>
#include <stdio.h>
#include <kiss_fft.h>

class Frequency
{
public:
	void Initialize();
	void Terminate();

private:
	PaDeviceIndex deviceIndex = 8;
	const char* deviceName = nullptr;
	PaStream* stream;
	PaError paError;

	int callbackExecutionCounter = 0;

	static int streamCallback(const void* input, void* output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
};