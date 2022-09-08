#ifndef __TIME__H__
#define __TIME__H__

#include "chrono"
#include "stdio.h"

#include "Utils.h"
#include <algorithm>
#include <cstddef>
#include <map>
#include <vcruntime.h>

namespace rv
{
	using std::chrono::duration_cast;
	using std::chrono::high_resolution_clock;

	struct TimerTypeBase
	{
	};
	struct TimerMilisseconds : public TimerTypeBase
	{
	};
	struct TimerSeconds : public TimerTypeBase
	{
	};

	template <int RegId>
	static inline int SampleCount = 0;

	template <int StrId, typename T = TimerTypeBase>
	class ScopedTimerBase
	{
	  private:
		static std::vector<long long> reg;

		int sampleIt = 0;
		high_resolution_clock::time_point begTime;

	  public:
		ScopedTimerBase()
		{
			fprintf(stdout, "Samples %i", SampleCount<StrId>);
			reg.resize(SampleCount<StrId>);
			begTime = high_resolution_clock::now();
		}
		~ScopedTimerBase() { PrintTime(); }

		inline void CaptureSample()
		{
			++SampleCount<StrId>;
			auto endTime = high_resolution_clock::now();
			reg[sampleIt] += duration_cast<std::chrono::milliseconds>(endTime - begTime).count();
			sampleIt = (sampleIt + 1) % SampleCount<StrId>;
		}

		void PrintTimeSamples()
		{
			for (int sampleId = 0; sampleId < SampleCount<StrId>; sampleId++)
			{
				fprintf(stdout, "[%i:%llims]\t", sampleId, reg[sampleId]);
			}
			fprintf(stdout, "\n");
		};
	};

	template <size_t N>
	struct StringLiteral
	{
		constexpr StringLiteral(const char (&str)[N]) { std::copy_n(str, N, value); }

		char value[N];
	};

	template <StringLiteral strId, typename T = TimerTypeBase>
	class ScopedTimer : public ScopedTimerBase<crc32({StrArgs..., '\0'}), T>
	{
	};

} // namespace rv

#endif //!__TIME__H__