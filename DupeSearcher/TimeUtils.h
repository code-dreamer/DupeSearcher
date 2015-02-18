#pragma once

namespace TimeUtils {
;

template<typename TimeT = std::chrono::milliseconds>
struct Measure
{
	template<typename Function, typename ...Args>
	static TimeT TimedCall(Function&& func, Args&&... args)
	{
		auto start = std::chrono::system_clock::now();
		func(std::forward<Args>(args)...);
		return std::chrono::duration_cast<TimeT>(std::chrono::system_clock::now() - start);
	}
};

template<typename TimeT = std::chrono::milliseconds>
std::wstring ToHumanString(const TimeT& duration)
{
	using namespace std;
	using namespace std::chrono;

	int hours = duration_cast<chrono::hours>(duration).count();
	int days = hours / 24;
	auto daysDuration = std::chrono::hours(days * 24);
	hours -= days * 24;
	auto hoursDuration = std::chrono::hours(hours);

	auto minutesDuration = duration_cast<chrono::minutes>(duration) - hoursDuration - daysDuration;
	auto secondsDuration = duration_cast<chrono::seconds>(duration)- minutesDuration - hoursDuration - daysDuration;
	auto msecondsDuration = duration_cast<chrono::milliseconds>(duration) - secondsDuration - minutesDuration - hoursDuration - daysDuration;

	wstringstream result;
	if (daysDuration != chrono::hours::zero())
		result << daysDuration.count() << " day(s) ";
	if (hoursDuration != chrono::hours::zero())
		result << hoursDuration.count() << " hr. ";
	if (minutesDuration != chrono::minutes::zero())
		result << minutesDuration.count() << " min. ";
	if (secondsDuration != chrono::seconds::zero())
		result << secondsDuration.count() << " sec. ";
	if (msecondsDuration != chrono::milliseconds::zero())
		result << msecondsDuration.count() << " msec. ";

	return result.str();
}

} // namespace TimeUtils
