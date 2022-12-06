#include "includes.hpp"
#include "SDK.hpp"
#include "Core.hpp"
namespace JIN 
{
	void MainThread() {
		__try {
			g_Core->Init();
		}
		__except (1) { }
	}
}
int main()
{
	using namespace JIN;
	MainThread();
	return 0;
}