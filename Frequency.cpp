#include "Frequency.h"
#include <vector>

//Frequency::Frequency()
//{
//}

void Frequency::Initialize()
	{
		paError = Pa_Initialize();

		if (paError != paNoError)
		{
			printf("PA ERROR: %s\n", Pa_GetErrorText(paError));
		}

		/* OPEN STREAM */

		const PaDeviceInfo* deviceInfo = Pa_GetDeviceInfo(deviceIndex);
		deviceName = deviceInfo->name;

		PaStreamParameters inputParameters{};
		inputParameters.device = deviceIndex;
		inputParameters.channelCount = deviceInfo->maxInputChannels;
		inputParameters.hostApiSpecificStreamInfo = NULL;
		inputParameters.sampleFormat = paFloat32;
		inputParameters.suggestedLatency = deviceInfo->defaultLowInputLatency;

		unsigned long nyquistFreq = static_cast<unsigned long>(deviceInfo->defaultSampleRate / 2);


		paError = Pa_OpenStream(&stream,
			&inputParameters,
			NULL,
			deviceInfo->defaultSampleRate,
			nyquistFreq,
			paNoFlag,
			streamCallback,
			this);

		if (paError)
		{
			printf("PA ERROR: %s\n", Pa_GetErrorText(paError));
		}

		paError = Pa_StartStream(stream);

		if (paError)
		{
			printf("PA ERROR: %s\n", Pa_GetErrorText(paError));
		}
	}

void Frequency::Terminate()
{
	paError = Pa_StopStream(stream);

	if (paError)
	{
		printf("PA ERROR: %s\n", Pa_GetErrorText(paError));
	}

	paError = Pa_CloseStream(stream);

	if (paError)
	{
		printf("PA ERROR: %s\n", Pa_GetErrorText(paError));
	}
}

int Frequency::streamCallback(const void* input, void* output,
	unsigned long frameCount,
	const PaStreamCallbackTimeInfo * timeInfo,
	PaStreamCallbackFlags statusFlags,
	void* userData)
{
	Frequency* frequency = static_cast<Frequency*>(userData);
	PaDeviceIndex deviceIndex = frequency->deviceIndex;

	float* inputStream = (float*)input;
	int inputChannels = Pa_GetDeviceInfo(deviceIndex)->maxInputChannels;
	double sampleRate = Pa_GetDeviceInfo(deviceIndex)->defaultSampleRate;
	unsigned long bufferSize = frameCount * inputChannels;
	int fftSize = static_cast<int>(sampleRate / 2);

	std::vector<float> left(fftSize);
	std::vector<float> right(fftSize);
	int leftIndex = 0;
	int rightIndex = 0;

	for (int index = 0; index < static_cast<int>(bufferSize); index++) {
		if (index % 2 == 0) {
			if (leftIndex < fftSize) {
				left[leftIndex] = inputStream[index];
				leftIndex++;
			}
			else {
				// Handle buffer overflow
				break;
			}
		}
		else {
			if (rightIndex < fftSize) {
				right[rightIndex] = inputStream[index];
				rightIndex++;
			}
			else {
				// Handle buffer overflow
				break;
			}
		}
	}

	for (int index = frameCount; index < fftSize; index++)
	{
		left[index] = 0.0;
		right[index] = 0.0;
	}

	kiss_fft_cpx* sampleStreamInput = new kiss_fft_cpx[fftSize];
	kiss_fft_cpx* sampleStreamOutput = new kiss_fft_cpx[fftSize];

	for (int index = 0; index < fftSize; index++)
	{
		sampleStreamInput[index].r = left[index];
		sampleStreamInput[index].i = 0;
	}

	kiss_fft_cfg configuration = kiss_fft_alloc(fftSize, 0, NULL, NULL);
	kiss_fft(configuration, sampleStreamInput, sampleStreamOutput);

	double peakMagnitude = 0.0;
	int peakIndex = 0;

	for (int index = 0; index < fftSize / 2; index++)
	{
		double outputFrequencyMagnitude = sampleStreamOutput[index].r;

		if (outputFrequencyMagnitude > peakMagnitude)
		{
			peakMagnitude = outputFrequencyMagnitude;
			peakIndex = index;
		}
	}

	double frequencyBin = static_cast<double>(peakIndex) * (static_cast<double>(bufferSize) / static_cast<double>(fftSize));

	system("cls");
	printf("PEAK INDEX CHANNEL: %d\n", peakIndex);
	printf("PEAK MAGNE CHANNEL: %f\n", peakMagnitude);
	printf("FREQUENCY         : %f\n", frequencyBin);

	frequency->callbackExecutionCounter++;

	/*delete[] left;
	delete[] right;*/
	delete[] sampleStreamInput;
	delete[] sampleStreamOutput;

	return paContinue;
}