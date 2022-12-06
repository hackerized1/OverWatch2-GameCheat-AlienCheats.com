#pragma once
#include "includes.hpp"
#include "SDK.hpp"
#include "Config.hpp"

namespace JIN {
	struct CoreHooks
	{
		static uint64_t fnDecryptChildInParent(uint64_t parent, uint8_t compid);
		static uint64_t GetComponent(uint64_t ComponentParent, DWORD ComponentID);
		static uint64_t GetViewMatrix();
		static bool update_memory_query();
		static void scan_matrix_ex();
		static unsigned __int64 __fastcall decrypt_outline_xor(__int64 a1);
		static unsigned __int64 __fastcall DecryptVis(__int64 a1);
		static uint64_t __fastcall get_outline_struct(__int64 a1);
		static uint64_t __fastcall get_color_struct(__int64 a1);
		static Vector3 RootPos(DWORD_PTR VelocityBase);
	};
	class Core {
	public:
		void Init();
		friend struct CoreHooks;
	private:
	};
	inline auto g_Core = std::make_unique<Core>();
}