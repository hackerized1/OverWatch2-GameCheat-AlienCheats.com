#include "Core.hpp"
#include "ThreadFunction.hpp"
namespace JIN {
	inline uint64_t CoreHooks::fnDecryptChildInParent(uint64_t parent, uint8_t compid)
	{
		__try
		{
			if (parent)
			{
				unsigned __int64 v1 = parent;
				unsigned __int64 v2 = (uint64_t)1 << (uint64_t)(compid & 0x3F);
				unsigned __int64 v3 = v2 - 1;
				unsigned __int64 v4 = compid & 0x3F;
				unsigned __int64 v5 = compid / 0x3F;
				unsigned __int64 v6 = SDK->RPM<uint64_t>(v1 + 8 * (uint32_t)v5 + 0xE8);
				__int64 v7 = (v2 & SDK->RPM<uint64_t>(v1 + 8 * (uint32_t)v5 + 0xE8)) >> v4;
				unsigned __int64 v8 = (v3 & v6) - (((v3 & v6) >> 1) & 0x5555555555555555);
				unsigned __int64 v9 = SDK->RPM<uint64_t>(SDK->RPM<uint64_t>(v1 + 0x58) + 8 * (SDK->RPM<uint8_t>((uint32_t)v5 + v1 + 0x108) + ((0x101010101010101 * (((v8 & 0x3333333333333333) + ((v8 >> 2) & 0x3333333333333333) + (((v8 & 0x3333333333333333) + ((v8 >> 2) & 0x3333333333333333)) >> 4)) & 0xF0F0F0F0F0F0F0F)) >> 0x38)));
				uint64_t Key1 = SDK->GlobalKey1;
				uint64_t Key2 = SDK->GlobalKey2;
				uint64_t v10 = (unsigned __int64)(unsigned int)(2 * v9 - __ROL4__(SDK->RPM<_QWORD>(SDK->dwGameBase + 0x3F9BF60 + (Key1 >> 0x34)), 9)) << 0x20;
				uint64_t v11 = Key2 ^ ((unsigned int)v9 | (unsigned int)v10 | ((unsigned int)v9 | v9 & 0xFFFFFFFF00000000ui64 ^ v10) & 0xFFFFFFFF00000000ui64 ^ ((unsigned __int64)(((unsigned int)v9 | (unsigned int)v10) ^ (unsigned int)~(SDK->RPM<_QWORD>(SDK->dwGameBase + 0x3F9BF60 + (Key1 & 0xFFF)) >> 0x20)) << 0x20));
				uint64_t v12 = (unsigned int)v11 | v11 & 0xFFFFFFFF00000000ui64 ^ ((unsigned __int64)((unsigned int)v11 ^ 0x203423DA) << 0x20);
				uint64_t v13 = -(int)v7 & ((unsigned int)Key2 ^ ((unsigned int)v9 | (unsigned int)v10) | v12 & 0xFFFFFFFF00000000ui64 ^ (((unsigned int)v12 ^ (SDK->RPM<_QWORD>(SDK->dwGameBase + 0x3F9BF60 + (Key1 >> 0x34)) >> 0x20)) << 0x20));
				return v13;
			}
		}
		__except (1) {
		}
	}
	inline BOOL CheckValidPointer(DWORD_PTR Addr, SIZE_T Size)
	{
		if (Size < 0)
			return false;

		BYTE* temp = new BYTE[Size];
		BOOL bRead = ReadProcessMemory(SDK->hProcess, (PVOID)(Addr), temp, Size, NULL);
		delete[] temp;
		return bRead;
	}
	inline uint64_t CoreHooks::GetComponent(uint64_t ComponentParent, DWORD ComponentID)
	{
		__try
		{
			uint64_t Result = 0;

			uint64_t pPointerTable = ComponentParent + 0x58;

			uint64_t PointerTable = SDK->RPM<uint64_t>(pPointerTable + 0x0);
			uint32_t PointerCount = SDK->RPM<uint32_t>(pPointerTable + 0x8);

			if (!CheckValidPointer(PointerTable, 0x8 * PointerCount))
				return 0;

			for (int i = 0; i < PointerCount; i++)
			{
				uint64_t CurrentComponent = fnDecryptChildInParent(ComponentParent, ComponentID);

				if (!CurrentComponent || !CheckValidPointer(CurrentComponent, 0x510))
					continue;


				if (SDK->RPM<BYTE>(CurrentComponent + 0x10) == ComponentID)
				{
					Result = CurrentComponent;
					break;
				}
			}

			return Result;
		}
		__except (1)
		{
			return 0;
		}
	}
	inline bool compress_mbis = false;
	inline std::vector<MEMORY_BASIC_INFORMATION64> mbis = {};

	inline bool CoreHooks::update_memory_query()
	{
		MEMORY_BASIC_INFORMATION64 mbi = { 0, };
		MEMORY_BASIC_INFORMATION64 old = { 0, };
		uintptr_t current_address = 0x7ffe0000;
		std::vector<MEMORY_BASIC_INFORMATION64> addresses;
		while (true)
		{
			if (!VirtualQueryEx(SDK->hProcess, (LPVOID)current_address, (PMEMORY_BASIC_INFORMATION)&mbi, sizeof(MEMORY_BASIC_INFORMATION64)))
				break;
			if ((mbi.State & 0x1000) != 0 && (mbi.Protect & 0x100) == 0)
			{
				if (old.BaseAddress + old.RegionSize == mbi.BaseAddress && compress_mbis)
					old.RegionSize += mbi.RegionSize;
				else
					addresses.push_back(mbi);

				old = mbi;
			}
			current_address = mbi.BaseAddress + mbi.RegionSize;
		}

		mbis = addresses;

		return (mbis.size() > 0);
	}

	inline void CoreHooks::scan_matrix_ex()
	{
		if (!update_memory_query())
			return;

		Matrix view_matrix{};
		Vector3 local_pos = localEntity.pos;
		printf("local_pos %.2f %.2f %.2f\n", local_pos.X, local_pos.Y, local_pos.Z);

		for (size_t i = 0; i < mbis.size(); ++i)
		{
			MEMORY_BASIC_INFORMATION64 mbi = mbis[i];

			char* buffer = new char[mbi.RegionSize];
			SDK->read_buf(mbi.BaseAddress, buffer, mbi.RegionSize);

			for (int j = 0; j < mbi.RegionSize; j++)
			{
				memcpy(&view_matrix, &buffer[j], sizeof(Matrix));
				auto local_camera = view_matrix.GetCameraVec();

				if (local_pos.DistTo(local_camera) < 2.0f)
				{
					printf("vm found! %p %.2f %.2f %.2f | %.2f %.2f %.2f\n", mbi.BaseAddress + j, local_pos.X, local_pos.Y, local_pos.Z, local_camera.X, local_camera.Y, local_camera.Z);
				}
			}

			delete[]buffer;
		}

		printf("finished\n");
	}
	inline uint64_t CoreHooks::GetViewMatrix()
	{
		return viewMatrix_ptr;
	}

	inline Vector3 CoreHooks::RootPos(DWORD_PTR BoneBase) {
		__try {
			uint64_t encrypt_buf[10] = { 0, };
			if (BoneBase != 0) {
				if (SDK->RPM<uintptr_t>(BoneBase + 0x1110) != 0) {
					ReadProcessMemory(SDK->hProcess, PVOID(BoneBase + 0x1110), encrypt_buf, sizeof(encrypt_buf), NULL);
					__m128 decrypt_buf{};

					uint64_t decrypt_key = encrypt_buf[2] + offset::root_pos_xor_key[min(encrypt_buf[2] & 7, 7)];

					for (int i = 0; i < 2; i++)
					{
						((uint64_t*)&decrypt_buf)[i] = encrypt_buf[i] ^ decrypt_key;
						decrypt_key += offset::root_pos_xor_key[min(decrypt_key & 7, 7)];
					}

					return Vector3{ decrypt_buf.m128_f32[0], decrypt_buf.m128_f32[1], decrypt_buf.m128_f32[2] };
				}
				else
					return Vector3{ 0, 0, 0 };
			}
			else
				return Vector3{ 0, 0, 0 };
		}
		__except (1) {
			return Vector3{ 0, 0, 0 };
		}
	}
	inline unsigned __int64 __fastcall CoreHooks::decrypt_outline_xor(__int64 a1) //48 89 5c 24 ? 44 89 4c 24 ? 48 89 4c 24 ? 55 56 57 41 54 41 55 41 56 41 57 48 8d ac 24
	{
		__int64 v1; // rbx
		unsigned __int64 v2; // rdi
		unsigned __int64 v3; // rax
		__int64 v4; // rbx
		unsigned __int64 v5; // rdx
		unsigned __int64 v6; // rcx
		__m128i v7; // xmm1
		__m128i v8; // xmm2
		__m128i v9; // xmm0
		__m128i v10; // xmm1

		v2 = SDK->dwGameBase + 0x928BE2;
		v3 = v2 + 0x8;

		DWORD_PTR keys = SDK->dwGameBase + 0x3F9CF80 + 8 * (((_BYTE)a1 - 0x14) & 0x7F);
		v4 = v2 ^ SDK->RPM<DWORD_PTR>(keys
			+ (((unsigned __int64)(a1 + 0x5465FAAA9F9703ECi64) >> 7) & 7)) ^ (a1 + 0x5465FAAA9F9703ECi64);

		v5 = (v3 - v2 + 7) >> 3;
		v6 = 0i64;
		if (v2 > v3)
			v5 = 0i64;
		if (v5)
		{
			if (v5 >= 4)
			{
				v7 = {};
				v8 = {};
				do
				{
					v6 += 4i64;
					v7 = _mm_xor_si128(v7, _mm_loadu_si128((const __m128i*)v2));
					v9 = _mm_loadu_si128((const __m128i*)(v2 + 16));
					v2 += 0x20i64;
					v8 = _mm_xor_si128(v8, v9);
				} while (v6 < (v5 & 0xFFFFFFFFFFFFFFFCui64));
				v10 = _mm_xor_si128(v7, v8);
				__m128i P1 = _mm_xor_si128(v10, _mm_srli_si128(v10, 8));
				v4 ^= *(DWORD_PTR*)&P1;
			}
			for (; v6 < v5; ++v6)
			{
				v4 ^= SDK->RPM<DWORD_PTR>(v2);
				v2 += 8i64;
			}
		}
		return v4 ^ ~v3 ^ 0x5465FAAA9F9703ECi64;
	}
	inline unsigned __int64 __fastcall CoreHooks::DecryptVis(__int64 a1) //e8 ? ? ? ? 48 89 83 ? ? ? ? 48 8b cb c6 83
	{
		__int64 v1; // rsi
		unsigned __int64 v2; // rbx
		unsigned __int64 v3; // r9
		__int64 v4; // rsi
		unsigned __int64 v5; // rdx
		unsigned __int64 v6; // rcx
		__m128i v7; // xmm1
		__m128i v8; // xmm2
		__m128i v9; // xmm0
		__m128i v10; // xmm1
		v1 = a1;
		v2 = SDK->dwGameBase + 0x91CE32;
		v3 = v2 + 0x8;
		DWORD_PTR VisibleKeyPTR = SDK->dwGameBase + 0x3F9CF80 + 8 * (((_BYTE)v1 - 0x7B) & 0x7F);
		v4 = v2 ^ SDK->RPM<_QWORD>(VisibleKeyPTR + (((unsigned __int64)(v1 - 0x793CDF4D7413767Bi64) >> 7) & 7)) ^ (v1 - 0x793CDF4D7413767Bi64);
		v5 = (v3 - v2 + 7) >> 3;
		v6 = 0i64;
		if (v2 > v3)
			v5 = 0i64;
		if (v5)
		{
			if (v5 >= 4)
			{
				ZeroMemory(&v7, sizeof(v7));
				ZeroMemory(&v8, sizeof(v8));
				do
				{
					v6 += 4i64;
					v7 = _mm_xor_si128(v7, _mm_loadu_si128((const __m128i*)v2));
					v9 = _mm_loadu_si128((const __m128i*)(v2 + 16));
					v2 += 32i64;
					v8 = _mm_xor_si128(v8, v9);
				} while (v6 < (v5 & 0xFFFFFFFFFFFFFFFCui64));
				v10 = _mm_xor_si128(v7, v8);
				auto addr = _mm_xor_si128(v10, _mm_srli_si128(v10, 8));
				v4 ^= *(__int64*)&addr;
			}
			for (; v6 < v5; ++v6)
			{
				v4 ^= SDK->RPM<_QWORD>(v2);
				v2 += 8i64;
			}
		}
		return v4 ^ ~v3 ^ 0x86C320B28BEC8985i64;
	}
	inline uint64_t __fastcall CoreHooks::get_outline_struct(__int64 a1) //40 53 48 83 EC 30 65 48 8B 04 25 ? ? ? ? 48
	{
		__int64 v2; // rcx
		int v4; // er9
		v2 = SDK->RPM<int>(a1 + 0x68);
		if ((_DWORD)v2)
			return  (uint64_t)(0x20 * v2 + SDK->RPM<_QWORD>(a1 + 0x60) - 0x20i64);
		else
			return 0i64;
	}
	inline uint64_t __fastcall CoreHooks::get_color_struct(__int64 a1) //40 57 48 83 EC 20 44 8B 91
	{
		__int64 v2; // rax

		v2 = SDK->RPM<int>(a1 + 0xF8);
		if ((_DWORD)v2)
			return (SDK->RPM<_QWORD>(a1 + 0xF0) + 0x14 * v2 - 0x14);
		else
			return 0i64;
	}
	void Core::Init() {
		__try {
			if (!SDK->Initialize())
				exit(0);
			if (!SDK->GetGlobalKey())
				exit(0);
			g_Config->Init();
			AllThread();
			EntityListThread();
		}
		__except (1) {

		}
	}
}