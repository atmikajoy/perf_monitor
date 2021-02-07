#pragma comment( lib, "pdh.lib" )
#include "perfmon.h"
#include <iostream>

int main()
{
	CPerfMon perfmon;

	if (perfmon.Initialize())
	{
		std::cout << "ok\n";
		auto idx = perfmon.AddCounter(CNTR_MEMINUSE_BYTES);
		auto idx2 = perfmon.AddCounter(CNTR_CPU);
		auto idx3 = perfmon.AddCounter(CNTR_DISK_IO_BYTES);
		
		std::cout << "counter index == " << idx << " and " << idx2 << ' ' << idx3 << '\n';
		std::cout << "perfmon.CollectQueryData() returned " << perfmon.CollectQueryData() << '\n';
		Sleep(500);

		for (int i = 0; i < 10; ++i)
		{
			std::cout << "perfmon.CollectQueryData() returned " << perfmon.CollectQueryData() << '\n';
			Sleep(1000);
			const auto value = perfmon.GetCounterValue(idx);
			const auto gb = value / (1024.0 * 1024 * 1024);
			std::cout << "mem committed  == " << gb << " gb\n";

			const auto value2 = perfmon.GetCounterValue(idx2);
			std::cout << "processor use == " << value2 << " %\n";

			const auto value3 = perfmon.GetCounterValue(idx3);
			std::cout << "disk io bytes == " << value3 / (1024.0) << " KB/sec\n";

			std::cin.get();
		}

		double min = 0, max = 0, mean = 0;
		perfmon.GetStatistics(&min, &max, &mean, idx3);
		std::cout << min/1024.0 << ' ' << max/1024.0 << ' ' << mean/1024.0 << '\n';

		perfmon.Uninitialize();
	}
}