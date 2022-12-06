#pragma once
#include "includes.hpp"
#include "SDK.hpp"
#include "Core.hpp"
namespace JIN {
	struct health_compo_t {
		union {
			OFF_MEMB(float, health, 0xE0);
			OFF_MEMB(float, health_max, 0xDC);
			OFF_MEMB(float, armor, 0x220);
			OFF_MEMB(float, armor_max, 0x21C);
			OFF_MEMB(float, barrier, 0x360);
			OFF_MEMB(float, barrier_max, 0x35C);
		};
	};
	struct obj_compo_t {
		union {
			OFF_MEMB(XMFLOAT3, obj_pos, 0xE0);
		};
	};
	struct velocity_compo_t {
		union {
			OFF_MEMB(XMFLOAT3, velocity, 0x70);
			OFF_MEMB(XMFLOAT3, location, 0x1d0);
			OFF_MEMB(uint64_t, bonedata, 0x820);
		};
	};
	struct hero_compo_t {
		union {
			OFF_MEMB(uint64_t, heroid, 0xE8);
			OFF_MEMB(uint64_t, skinid, 0xF0);
		};
	};
	struct vis_compo_t {
		union {
			OFF_MEMB(uint64_t, key1, 0xA0);
			OFF_MEMB(uint64_t, key2, 0x98);
		};
	};
	class c_entity
	{
	public:
		DWORD_PTR address;
		DWORD_PTR LinkBase;
		DWORD_PTR HealthBase;
		DWORD_PTR TeamBase;
		DWORD_PTR VelocityBase;
		DWORD_PTR HeroBase;
		DWORD_PTR BoneBase;
		DWORD_PTR OutlineBase;
		DWORD_PTR SkillBase;
		DWORD_PTR RotationBase;
		DWORD_PTR VisBase;
		DWORD_PTR AngleBase;
		DWORD_PTR ObjectBase;
		DWORD_PTR HeroID;
		DWORD_PTR SkinID;
		int head_index = 0;
		uint32_t PlayerID;
		uint16_t Dva;

		float PlayerHealth = 0.f;
		float MinHealth = 0.f;
		float MaxHealth = 0.f;
		float MinArmorHealth = 0.f;
		float MaxArmorHealth = 0.f;
		float MinBarrierHealth = 0.f;
		float MaxBarrierHealth = 0.f;
		float ULT = 0.f;
		espBone BoneList;
		bool Alive;
		bool Vis;
		bool Team;
		bool Trg;

		c_entity() : address(0) {};
		c_entity(uint64_t _UniqueID) : address(address) {};
		__forceinline bool operator==(const c_entity& entity) const
		{
			return (address == entity.address);
		}
		__forceinline bool operator!=(const c_entity& entity) const
		{
			return (address != entity.address);
		}

		Vector3 head_pos, EnemyAngle, angle_rotation, velocity, Rot, pos, obj_pos;

		inline bool IntersectRayWithAABB(Matrix viewMatrix, XMFLOAT3 origin, XMFLOAT3 direction, XMFLOAT3 min, XMFLOAT3 max, float scale, XMFLOAT3 basePos)
		{
			XMStoreFloat3(&min, XMLoadFloat3(&min) * scale);
			XMStoreFloat3(&max, XMLoadFloat3(&max) * scale);

			XMFLOAT3 points[] = {
				XMFLOAT3(min.x, min.y, min.z),
				XMFLOAT3(min.x, max.y, min.z),
				XMFLOAT3(max.x, max.y, min.z),
				XMFLOAT3(max.x, min.y, min.z),
				XMFLOAT3(max.x, max.y, max.z),
				XMFLOAT3(min.x, max.y, max.z),
				XMFLOAT3(min.x, min.y, max.z),
				XMFLOAT3(max.x, min.y, max.z)
			};

			XMFLOAT2 pointsTransformed[8];
			RECT minmaxTransformed{ 518523, 518523, 0, 0 };
			for (int i = 0; i < 8; i++)
			{
				XMMATRIX rotMatrix = XMMatrixRotationY(-atan2(Rot.Z, Rot.X));
				XMStoreFloat3(&points[i], XMVector3Transform(XMLoadFloat3(&points[i]), rotMatrix));
				XMStoreFloat3(&points[i], XMLoadFloat3(&points[i]) + XMLoadFloat3(&basePos));
				auto pointss = Vector3(points[i].x, points[i].y, points[i].z);
				pointsTransformed[i] = XMFLOAT2(viewMatrix.WorldToScreen(pointss, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)).X, viewMatrix.WorldToScreen(pointss, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)).Y);
				minmaxTransformed.left = fmin(minmaxTransformed.left, pointsTransformed[i].x);
				minmaxTransformed.top = fmin(minmaxTransformed.top, pointsTransformed[i].y);
				minmaxTransformed.right = fmax(minmaxTransformed.right, pointsTransformed[i].x);
				minmaxTransformed.bottom = fmax(minmaxTransformed.bottom, pointsTransformed[i].y);
			}

			RECT rcTemp{}, crossHair{ GetSystemMetrics(SM_CXSCREEN) / 2 - 1,  GetSystemMetrics(SM_CYSCREEN) / 2 - 1,  GetSystemMetrics(SM_CXSCREEN) / 2 + 1,  GetSystemMetrics(SM_CYSCREEN) / 2 + 1 };
			return IntersectRect(&rcTemp, &minmaxTransformed, &crossHair);
		}
		inline std::array<int, 14> GetSkel()
		{
			switch (HeroID)
			{
			case eHero::HERO_ANA:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_ASHE:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_BAPTISTE:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_BASTION:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, 85, 95, 89, 99, BONE_L_HAND, 156};
			case eHero::HERO_BRIGITTE:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_DOOMFIST:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_DVA:
				if (SDK->RPM<uint16_t>(LinkBase + 0xD0) != SDK->RPM<uint16_t>(LinkBase + 0xD8))
					return std::array<int, 14>{BONE_HEAD, BONE_NECK, 4, BONE_BODY_BOT, 80, 53, 27, 57, 85, 95, 89, 99, 153, 154};
				else
					return std::array<int, 14>{BONE_HEAD, 16, 81, 82, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_ECHO:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_GENJI:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_HANJO:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_JUNKRAT:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_LUCIO:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_MCCREE:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_MEI:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, 56, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, 70};
			case eHero::HERO_MERCY:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_MOIRA:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_ORISA:
				return std::array<int, 14>{17, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, 56, 85, 95, 92, 102, BONE_L_HAND, 58};
			case eHero::HERO_PHARAH:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_REAPER:
				return std::array<int, 14>{17, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_REINHARDT:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_ROADHOG:
				return std::array<int, 14>{17, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_SIGMA:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_SOLDIER76:
				return std::array<int, 14>{17, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_SOMBRA:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_SYMMETRA:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_TORBJORN:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, 28, BONE_R_HAND};
			case eHero::HERO_TRACER:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_WIDOWMAKER:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_WINSTON:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_WRECKINGBALL:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_ZARYA:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			case eHero::HERO_ZENYATTA:
				return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
			}
			return std::array<int, 14>{BONE_HEAD, BONE_NECK, BONE_BODY, BONE_BODY_BOT, BONE_L_SHOULDER, BONE_R_SHOULDER, BONE_L_ELBOW, BONE_R_ELBOW, BONE_L_ANKLE, BONE_R_ANKLE, BONE_L_SHANK, BONE_R_SHANK, BONE_L_HAND, BONE_R_HAND};
		}
	};
	inline auto verify_entity = [&](DWORD_PTR entity, char verify_pattern[]) -> bool {
		const size_t pattern_size = sizeof(verify_pattern);
		char current_pattern[pattern_size];
		SDK->read_buf(entity, current_pattern, pattern_size);
		for (int i = 0; i < pattern_size; i++) {
			if (current_pattern[i] != verify_pattern[i] && verify_pattern[i] != 0) {
				return false;
			}
		}
		return true;
	};

	__forceinline std::vector<std::pair<DWORD_PTR, DWORD_PTR>> get_ow_entities()
	{
		std::vector<std::pair<DWORD_PTR, DWORD_PTR>> result{};

		struct Entity {
			uintptr_t entity;
			uintptr_t pad;
		};

		uintptr_t entity_list = SDK->RPM<uintptr_t>(SDK->dwGameBase + offset::Address_ComponentList);

		MEMORY_BASIC_INFORMATION mbi{};
		VirtualQueryEx(SDK->hProcess, (LPCVOID)entity_list, &mbi, sizeof(mbi));

		SIZE_T entity_list_size = mbi.RegionSize, count = 0, count2 = 0;
		Entity* raw_list = new Entity[entity_list_size];

		for (size_t i = entity_list_size; i > 0; i -= 1024)
		{
			if (ReadProcessMemory(SDK->hProcess, reinterpret_cast<PVOID>(entity_list), raw_list, i * sizeof(Entity), nullptr))
			{
				entity_list_size = i;
				break;
			}
		}

		for (size_t i = 0; i < entity_list_size; ++i)
		{
			uintptr_t cur_entity = raw_list[i].entity;
			if (!cur_entity)
				continue;

			uintptr_t common_linker = CoreHooks::fnDecryptChildInParent(cur_entity, TYPE_LINK);
			if (!common_linker)
				continue;

			auto _get_entity_by_unique_id = [&raw_list, &entity_list_size](uint32_t unique_id) -> uintptr_t
			{
				for (size_t x = 0; x < entity_list_size; ++x)
				{
					uintptr_t cur_entity = raw_list[x].entity;
					if (!cur_entity)
						continue;

					if (SDK->RPM<uint32_t>(cur_entity + 0x110) == unique_id)
						return cur_entity;
				}

				return 0;
			};

			uintptr_t common = _get_entity_by_unique_id(SDK->RPM<uint32_t>(common_linker + 0xD0)), dummy;
			if (!ReadProcessMemory(SDK->hProcess, LPVOID(common), &dummy, 8, nullptr))
				continue;

			result.emplace_back(common, cur_entity);
		}
		return result;
	}

	inline std::vector<std::pair<DWORD_PTR, DWORD_PTR>>ow_entities{};
	inline std::vector<c_entity> entities{};
	inline c_entity localEntity{};
	inline uintptr_t viewMatrix_ptr{};
}