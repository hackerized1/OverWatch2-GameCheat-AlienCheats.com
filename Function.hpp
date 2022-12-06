#pragma once
#include "includes.hpp"
#include "SDK.hpp"
#include "EntityManager.hpp"

namespace JIN {
	struct ProjWeapon
	{
		float projectile_speed;
		float projectile_gravity;

		ProjWeapon() : projectile_speed(0), projectile_gravity(0) {}
		ProjWeapon(float a1, float a2) : projectile_speed(a1), projectile_gravity(a2) {}
	};
	class Function {
	private:
		friend struct ProjWeapon;
		__int64 SkillStructCheck(uint64_t a1, uint16_t a2)
		{
			__int64 v2; // r8
			uint64_t v3; // rax
			__int64 v4; // rcx

			v2 = SDK->RPM<uint32_t>(a1 + 0x40);
			if ((int)v2 <= 0)
				return 0i64;
			v3 = SDK->RPM<uint64_t>(a1 + 0x38);
			v4 = 0i64;
			while (SDK->RPM<uint16_t>(v3 + 8) != a2)
			{
				++v4;
				v3 += 0x10i64;
				if (v4 >= v2)
					return 0i64;
			}
			return SDK->RPM<uint64_t>(v3);
		}
		uint64_t FnSkillStruct(__m128* a1, uint16_t* a2)
		{
			__int64 v2; // rbx
			uint16_t* v3;
			__int16 v4; // dx
			__int64 v5; // r9
			__int64 v6; // rax
			unsigned __int16 v7; // dx
			__int64 v8; // r8
			int v9; // eax
			__int64 v10; // rcx
			__int64 v11; // rax
			__int64 v12; // rdi
			int v13; // eax
			int v14; // edx
			__int64 v15; // rax
			__int64 v16; // rcx
			__int64 v17; // rsi
			__int64 v18; // rax
			unsigned __int16 v19; // dx
			unsigned __int16 v20; // ax
			__int64 v21; // rax

			v2 = 0i64;
			v3 = a2;
			if (!a2[1])
				return 0i64;
			v4 = *a2;
			if (!v4)
			{
				v5 = SDK->RPM<uint64_t>(a1->m128_u64[1] + 0x1F0);
				goto LABEL_6;
			}
			v6 = SkillStructCheck(a1->m128_u64[1], v4);
			if (!v6)
				return 0i64;
			v5 = SDK->RPM<uint64_t>(v6 + 0x98);
		LABEL_6:
			v7 = v3[1];
			v8 = 0x20 * ((v3[1] & 0xF) + 1i64);
			v9 = SDK->RPM<uint32_t>(v8 + v5 + 8) - 1;
			if (v9 < 0)
				return 0x0;
			v10 = v9;
			v11 = SDK->RPM<uint64_t>(v8 + v5) + 0x10i64 * v9;
			while (SDK->RPM<uint16_t>(v11) != v7)
			{
				v11 -= 0x10i64;
				if (--v10 < 0)
					return 0x0;
			}
			v12 = SDK->RPM<uint64_t>(v11 + 8);
			if (!v12)
				return 0x0;

			if (*((_DWORD*)v3 + 4) <= 0)
				return v12;
			return v12;
		}
	public:
		inline bool isSkillAvtivate(uint64_t SkillComponent, uint16_t SkillIndex1, uint16_t SkillIndex2)
		{
			__try
			{
				__m128 skillStruct{};
				skillStruct.m128_u64[1] = SkillComponent + 0xD0;
				uint16_t skillId[20] = { SkillIndex1, SkillIndex2 };
				uint64_t skill = FnSkillStruct(&skillStruct, skillId);
				if (!skill)
					return false;
				return SDK->RPM<uint8_t>(skill + 0x48) == 1;
			}
			__except (1) {}
		}
		inline float IsCool(uint64_t SkillComponent, uint16_t SkillIndex1, uint16_t SkillIndex2)
		{
			__try
			{
				__m128 skillStruct{};
				skillStruct.m128_u64[1] = SkillComponent + 0xD0;
				uint16_t skillId[20] = { SkillIndex1, SkillIndex2 };
				uint64_t skill = FnSkillStruct(&skillStruct, skillId);
				if (skill)
				{
					if (SDK->RPM<float>(skill + 0x50) > 0.f)
					{
						return SDK->RPM<float>(skill + 0x60);
					}
				}
			}
			__except (1) {}
		}
		inline bool IsTargetable(uint64_t HeroID, uint64_t SkillComponent)
		{
			//ReinHardt Strike 0x7, 0x2BB
			if (!(HeroID == eHero::HERO_GENJI && isSkillAvtivate(SkillComponent, 0, 0x28E9)) &&
				!(HeroID == eHero::HERO_GENJI && isSkillAvtivate(SkillComponent, 0, 0x28E3)) &&
				!(HeroID == eHero::HERO_SOMBRA && isSkillAvtivate(SkillComponent, 0, 0x7C5)) &&
				!(HeroID == eHero::HERO_MOIRA && isSkillAvtivate(SkillComponent, 0, 0xA0F)) &&
				!(HeroID == eHero::HERO_ZARYA && isSkillAvtivate(SkillComponent, 0, 0x19c8)) &&
				!(HeroID == eHero::HERO_REAPER && isSkillAvtivate(SkillComponent, 0, 0xA0F)) &&
				!(HeroID == eHero::HERO_MEI && isSkillAvtivate(SkillComponent, 0, 0x28E3)) &&
				!(HeroID == eHero::HERO_DOOMFIST && isSkillAvtivate(SkillComponent, 0, 0x254)))
			{
				return true;
			}
			else return false;
		}
		inline void SendReturnInputWithLeft(int dx, int dy, bool r, bool reverse)
		{
			INPUT m_Input[2] = { };
			RtlSecureZeroMemory(m_Input, sizeof(m_Input));

			m_Input[0].type = INPUT_MOUSE;
			m_Input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP; //(!r ? (!reverse ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_LEFTUP) : (!reverse ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_RIGHTUP));
			m_Input[0].mi.dx = dx;
			m_Input[0].mi.dy = dy;
			m_Input[1].type = INPUT_MOUSE;
			m_Input[1].mi.dwFlags = MOUSEEVENTF_MOVE;//| (!r ? (!reverse ? MOUSEEVENTF_LEFTUP : 0) : (!reverse ? MOUSEEVENTF_RIGHTUP : 0));
			m_Input[1].mi.dx = -dx;
			m_Input[1].mi.dy = -dy;
			SendInput(2, m_Input, sizeof(INPUT));

			if (reverse)
			{
				Sleep(30);

				RtlSecureZeroMemory(m_Input, sizeof(m_Input));
				m_Input[0].type = INPUT_MOUSE;
				m_Input[0].mi.dwFlags = !r ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
				SendInput(1, m_Input, sizeof(INPUT));
			}
		}
		inline float GetHanzoCharging(uint64_t SkillBase)
		{
			float ret = 0.f;
			__try
			{
				if (SkillBase)
				{
					uint64_t SkillInstruction = SDK->RPM<uint64_t>(SkillBase + 0x1218);
					uint16_t SkillEnum = SDK->RPM<uint16_t>(SkillInstruction + 0x18);
					uint64_t SkillBase = SDK->RPM<uint64_t>(SkillInstruction + 0x10);
					for (int i = 0; i <= SkillEnum; i++)
					{
						if (SDK->RPM<uint16_t>(SkillBase + (i * 0x80)) == 0xC9)
						{
							ret = SDK->RPM<float>(SkillBase + (i * 0x80) + 0x30);
							break;
						}
					}
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {}
			return ret;
		}
		inline uint64_t GetSenstivePTR()
		{
			uint64_t tmp = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_Sensitivity);
			tmp = SDK->RPM<uint64_t>(tmp + 0x510);
			tmp = SDK->RPM<uint64_t>(tmp + 0x80);
			tmp = SDK->RPM<uint64_t>(tmp + 0x40);
			tmp = SDK->RPM<uint64_t>(tmp + 0x28);
			tmp += offset::Offset_senstive;
			return tmp;
		}
		inline bool GetRecoil()
		{
			__try
			{
				if (localEntity.AngleBase)
				{
					if (SDK->RPM<float>(localEntity.AngleBase + offset::Offset_angle_recoil) <= 0.02f)
						return true;
					else
						return false;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {}
			return false;
		}
		inline bool GetReload()
		{
			__try
			{
				if (localEntity.AngleBase)
				{
					if (SDK->RPM<BYTE>(localEntity.AngleBase + offset::Offset_angle_reload) != 0x4)
						return true;
					else
						return false;
				}
			}
			__except (EXCEPTION_EXECUTE_HANDLER) {}
			return false;
		}
		inline Vector3 CalcAngle(Vector3 MyPos, Vector3 EnPos, float Dis)
		{
			Vector3 Result;

			Result.X = (EnPos.X - MyPos.X) / Dis;
			Result.Y = (EnPos.Y - MyPos.Y) / Dis;
			Result.Z = (EnPos.Z - MyPos.Z) / Dis;

			return Result;
		}
		inline Vector3 smooth_angle(Vector3 local, Vector3 target, float speed)
		{
			float t = 1.0f / local.DistTo(target) * (speed * 0.015f);
			t = min(1.0f, t);
			return Vector3
			{
				(target.X - local.X) * t + local.X,
				(target.Y - local.Y) * t + local.Y,
				(target.Z - local.Z) * t + local.Z
			};
		}
		inline void PressKey(int Key, float duration)
		{
			clock_t previous = clock();
			while (clock() - previous < duration)
			{
				SDK->WPM<DWORD>(localEntity.AngleBase + offset::Offset_angle_key, Key);
			}
		}
		inline Vector3 AngleToMouse(Vector3 Local, Vector3 Target, float InputScale, float Sestive)
		{
			Vector3 Delta = (Target - Local);

			double Div = ((double)0.011519 * (double)Sestive) * (double)InputScale;

			double X = ((double)atan2f(Local.X, Local.Z) - (double)atan2f(Target.X, Target.Z)) / Div;
			double Y = -((double)Target.Y - (double)Local.Y) / Div;

			return { (float)X, (float)Y,  0.0f };
		}
		inline void AimCorrection(Vector3* InVecArg, Vector3 currVelocity, float Distance, float Bulletspeed, float Gravity)
		{		
			if (g_Config->g_Gravity)
			{
				float m_time = powf(Distance / Bulletspeed, 2.f);

				(*InVecArg).X = (*InVecArg).X + ((currVelocity.X) * (Distance / (Bulletspeed)));
				(*InVecArg).Y = (*InVecArg).Y + ((currVelocity.Y) * (Distance / (Bulletspeed)));
				(*InVecArg).Z = (*InVecArg).Z + ((currVelocity.Z) * (Distance / (Bulletspeed)));

				(*InVecArg).Y += (0.5f * 9.8f * m_time);
			}
			else
			{
				(*InVecArg).X = (*InVecArg).X + ((currVelocity.X) * (Distance / (Bulletspeed)));
				(*InVecArg).Y = (*InVecArg).Y + ((currVelocity.Y) * (Distance / (Bulletspeed)));
				(*InVecArg).Z = (*InVecArg).Z + ((currVelocity.Z) * (Distance / (Bulletspeed)));
			}
		}
		inline Vector3 GetVector() {
			int TarGetIndex = -1;
			Vector3 target{};
			Vector2 CrossHair = Vector2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);

			float origin = FLT_MAX;

			if (TarGetIndex == -1)
			{
				if (entities.size() > 0)
				{
					for (int i = 0; i < entities.size(); i++)
					{
						if (entities[i].Alive && entities[i].Team && entities[i].Vis && entities[i].PlayerHealth <= 30.f)
						{
							Vector3 RootPos = entities[i].head_pos;
							Vector2 Vec2 = ViewMatrix().WorldToScreen(RootPos, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
							float CrossDist = CrossHair.Distance(Vec2);

							if (CrossDist < origin)
							{
								target = RootPos;
								origin = CrossDist;
								TarGetIndex = i;
							}
							else
							{
								TarGetIndex = -1;
							}
						}
						else
						{
							TarGetIndex = -1;
						}
					}
				}
			}
			else
			{
				if (entities[TarGetIndex].Alive && entities[TarGetIndex].Team && entities[TarGetIndex].Vis && entities[TarGetIndex].PlayerHealth <= 30.f)
				{
					Vector3 RootPos = entities[TarGetIndex].head_pos;
					Vector2 Vec2 = ViewMatrix().WorldToScreen(RootPos, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
					float CrossDist = CrossHair.Distance(Vec2);

					if (CrossDist < origin)
					{
						target = RootPos;
						origin = CrossDist;
					}
					else
					{
						TarGetIndex = -1;

					}
				}
				else
				{
					TarGetIndex = -1;
				}
			}
			return target;
		}
		inline Vector3 GetVector3()
		{		
			int TarGetIndex = -1;
			Vector3 target{};
			Vector2 CrossHair = Vector2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);

			float origin = FLT_MAX;

			if (TarGetIndex == -1)
			{
				if (entities.size() > 0)
				{
					for (int i = 0; i < entities.size(); i++)
					{
						if (entities[i].Alive && entities[i].Team && entities[i].Vis)
						{
							Vector3 RootPos = entities[i].head_pos;
							Vector2 Vec2 = ViewMatrix().WorldToScreen(RootPos, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
							float CrossDist = CrossHair.Distance(Vec2);

							if (CrossDist < origin && CrossDist <= g_Config->g_FOV)
							{
								target = RootPos;
								origin = CrossDist;
								TarGetIndex = i;
								g_Config->g_Trigger = entities[i].IntersectRayWithAABB(ViewMatrix(), XMFLOAT3(ViewMatrix().GetCameraVec().X, ViewMatrix().GetCameraVec().Y, ViewMatrix().GetCameraVec().Z), XMFLOAT3(localEntity.angle_rotation.X, localEntity.angle_rotation.Y, localEntity.angle_rotation.Z), GetAsyncKeyState(XBUTTON2) ? g_Config->min : g_Config->min2, GetAsyncKeyState(XBUTTON2) ? g_Config->max : g_Config->max2, g_Config->g_TriggerScale / 100.f, XMFLOAT3(RootPos.X, RootPos.Y, RootPos.Z));
							}
							else
							{
								TarGetIndex = -1;
							}
						}
						else
						{
							TarGetIndex = -1;
						}
					}
				}
			}
			else
			{
				if (entities[TarGetIndex].Alive && entities[TarGetIndex].Team && entities[TarGetIndex].Vis)
				{
					Vector3 RootPos = entities[TarGetIndex].head_pos;
					Vector2 Vec2 = ViewMatrix().WorldToScreen(RootPos, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
					float CrossDist = CrossHair.Distance(Vec2);

					if (CrossDist < origin && CrossDist <= g_Config->g_FOV)
					{
						target = RootPos;
						origin = CrossDist;
						g_Config->g_Trigger = entities[TarGetIndex].IntersectRayWithAABB(ViewMatrix(), XMFLOAT3(ViewMatrix().GetCameraVec().X, ViewMatrix().GetCameraVec().Y, ViewMatrix().GetCameraVec().Z), XMFLOAT3(localEntity.angle_rotation.X, localEntity.angle_rotation.Y, localEntity.angle_rotation.Z), GetAsyncKeyState(XBUTTON2) ? g_Config->min : g_Config->min2, GetAsyncKeyState(XBUTTON2) ? g_Config->max : g_Config->max2, g_Config->g_TriggerScale / 100.f, XMFLOAT3(RootPos.X, RootPos.Y, RootPos.Z));
					}
					else
					{
						TarGetIndex = -1;

					}
				}
				else
				{
					TarGetIndex = -1;
				}
			}
			return target;
		}
		inline Vector3 GetVectorPredit() {
			int TarGetIndex = -1;
			Vector3 target = Vector3(0, 0, 0);
			Vector2 CrossHair = Vector2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);

			float origin = FLT_MAX;

			if (TarGetIndex == -1)
			{
				if (entities.size() > 0)
				{
					for (int i = 0; i < entities.size(); i++)
					{
						if (entities[i].Alive && entities[i].Team && entities[i].Vis)
						{
							Vector3 RootPos = entities[i].head_pos;
							Vector3 Veclocity = entities[i].velocity;
							Vector3 world = RootPos;
							AimCorrection(&world, Veclocity, ViewMatrix().GetCameraVec().DistTo(world), GetBulletTrajactory(localEntity.HeroID, SDK->RPM<WORD>(localEntity.AngleBase + offset::Offset_angle_key)).projectile_speed, GetBulletTrajactory(localEntity.HeroID, SDK->RPM<WORD>(localEntity.AngleBase + offset::Offset_angle_key)).projectile_gravity);

							Vector2 Vec2 = ViewMatrix().WorldToScreen(world, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
							float CrossDist = CrossHair.Distance(Vec2);
							if (CrossDist < origin && CrossDist < g_Config->g_FOV)
							{
								target = world;
								origin = CrossDist;
								TarGetIndex = i;
								g_Config->g_Trigger = entities[i].IntersectRayWithAABB(ViewMatrix(), XMFLOAT3(ViewMatrix().GetCameraVec().X, ViewMatrix().GetCameraVec().Y, ViewMatrix().GetCameraVec().Z), XMFLOAT3(localEntity.angle_rotation.X, localEntity.angle_rotation.Y, localEntity.angle_rotation.Z), GetAsyncKeyState(XBUTTON2) ? g_Config->min : g_Config->min2, GetAsyncKeyState(XBUTTON2) ? g_Config->max : g_Config->max2, g_Config->g_TriggerScale / 100.f, XMFLOAT3(world.X, world.Y, world.Z));
							}
							else
							{
								TarGetIndex = -1;
							}
						}
						else
						{
							TarGetIndex = -1;
						}
					}
				}
			}
			else
			{
				if (entities[TarGetIndex].Alive && entities[TarGetIndex].Team && entities[TarGetIndex].Vis)
				{
					Vector3 RootPos = entities[TarGetIndex].head_pos;
					Vector3 world = RootPos;
					Vector3 Veclocity = entities[TarGetIndex].velocity;
					AimCorrection(&world, Veclocity, ViewMatrix().GetCameraVec().DistTo(world), GetBulletTrajactory(localEntity.HeroID, SDK->RPM<WORD>(localEntity.AngleBase + offset::Offset_angle_key)).projectile_speed, GetBulletTrajactory(localEntity.HeroID, SDK->RPM<WORD>(localEntity.AngleBase + offset::Offset_angle_key)).projectile_gravity);

					Vector2 Vec2 = ViewMatrix().WorldToScreen(world, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

					float CrossDist = CrossHair.Distance(Vec2);
					if (CrossDist < origin && CrossDist < g_Config->g_FOV)
					{
						target = world;
						origin = CrossDist;
						g_Config->g_Trigger = entities[TarGetIndex].IntersectRayWithAABB(ViewMatrix(), XMFLOAT3(ViewMatrix().GetCameraVec().X, ViewMatrix().GetCameraVec().Y, ViewMatrix().GetCameraVec().Z), XMFLOAT3(localEntity.angle_rotation.X, localEntity.angle_rotation.Y, localEntity.angle_rotation.Z), GetAsyncKeyState(XBUTTON2) ? g_Config->min : g_Config->min2, GetAsyncKeyState(XBUTTON2) ? g_Config->max : g_Config->max2, g_Config->g_TriggerScale / 100.f, XMFLOAT3(world.X, world.Y, world.Z));
					}
					else
					{
						TarGetIndex = -1;

					}
				}
				else
				{
					TarGetIndex = -1;
				}
			}
			return target;
		}
		inline Vector3 GetVectorSkill() {
			Vector3 target = Vector3(0, 0, 0);
			Vector2 CrossHair = Vector2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);
			float origin = FLT_MAX;
			static int TarGetIndex = -1;
			if (TarGetIndex == -1)
			{
				if (entities.size() > 0)
				{
					for (int i = 0; i < entities.size(); i++)
					{
						if (entities[i].Alive && entities[i].Team && entities[i].Vis)
						{
							Vector3 RootPos = entities[i].head_pos;
							Vector3 Veclocity = entities[i].velocity;
							Vector3 world = RootPos;
							AimCorrection(&world, Veclocity, ViewMatrix().GetCameraVec().DistTo(world), GetBulletTrajactory(localEntity.HeroID, SDK->RPM<WORD>(localEntity.AngleBase + offset::Offset_angle_key)).projectile_speed, GetBulletTrajactory(localEntity.HeroID, SDK->RPM<WORD>(localEntity.AngleBase + offset::Offset_angle_key)).projectile_gravity);

							Vector2 Vec2 = ViewMatrix().WorldToScreen(world, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
							float CrossDist = CrossHair.Distance(Vec2);
							if (CrossDist < origin)
							{
								target = world;
								origin = CrossDist;
								TarGetIndex = i;
							}
							else
							{
								TarGetIndex = -1;
							}
						}
						else
						{
							TarGetIndex = -1;
						}
					}
				}
			}
			else
			{
				if (entities[TarGetIndex].Alive && entities[TarGetIndex].Team && entities[TarGetIndex].Vis)
				{
					Vector3 RootPos = entities[TarGetIndex].head_pos;
					Vector3 world = RootPos;
					Vector3 Veclocity = entities[TarGetIndex].velocity;
					AimCorrection(&world, Veclocity, ViewMatrix().GetCameraVec().DistTo(world), GetBulletTrajactory(localEntity.HeroID, SDK->RPM<WORD>(localEntity.AngleBase + offset::Offset_angle_key)).projectile_speed, GetBulletTrajactory(localEntity.HeroID, SDK->RPM<WORD>(localEntity.AngleBase + offset::Offset_angle_key)).projectile_gravity);

					Vector2 Vec2 = ViewMatrix().WorldToScreen(world, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

					float CrossDist = CrossHair.Distance(Vec2);
					if (CrossDist < origin)
					{
						target = world;
						origin = CrossDist;
					}
					else
					{
						TarGetIndex = -1;

					}
				}
				else
				{
					TarGetIndex = -1;
				}
			}
			return target;
		}
		inline int get_bone_id(uint64_t bonedata, int bone_id) {
			uint32_t* v1 = (uint32_t*)SDK->RPM<uint64_t>(SDK->RPM<uint64_t>(bonedata) + 0x38);
			uint16_t count = SDK->RPM <uint16_t>(SDK->RPM <uint64_t>(bonedata) + 0x66);
			for (int i = 0; i < count; i++) {
				if (SDK->RPM<uint16_t>((uint64_t)(v1 + i)) == bone_id) {
					return i;
				}
			}
			return 0;
		}
		inline bool Tracking() {
			__try {
				if (localEntity.HeroID == eHero::HERO_TRACER || localEntity.HeroID == eHero::HERO_SOLDIER76 || localEntity.HeroID == eHero::HERO_SYMMETRA || localEntity.HeroID == eHero::HERO_DVA || localEntity.HeroID == eHero::HERO_BASTION || localEntity.HeroID == eHero::HERO_BAPTISTE || localEntity.HeroID == eHero::HERO_SOMBRA || localEntity.HeroID == eHero::HERO_WRECKINGBALL || localEntity.HeroID == eHero::HERO_ZARYA)
					return true;
				else
					return false;
				return false;
			}
			__except (1) {

			}
		}
		inline bool Flicker() {
			__try {
				if (localEntity.HeroID == eHero::HERO_MCCREE || localEntity.HeroID == eHero::HERO_ASHE || localEntity.HeroID == eHero::HERO_REAPER || localEntity.HeroID == eHero::HERO_WIDOWMAKER)
					return true;
				else
					return false;
				return false;
			}
			__except (1) {

			}
		}
		inline bool Predicton() {
			__try {
				if (localEntity.HeroID == eHero::HERO_LUCIO || localEntity.HeroID == eHero::HERO_ORISA || localEntity.HeroID == eHero::HERO_MERCY || localEntity.HeroID == eHero::HERO_SIGMA)
					return true;
				else
					return false;
				return false;
			}
			__except (1) {

			}
		}
		inline bool PredictFlicker() {
			__try {
				if (localEntity.HeroID == eHero::HERO_HANJO || localEntity.HeroID == eHero::HERO_GENJI || localEntity.HeroID == eHero::HERO_TORBJORN || localEntity.HeroID == eHero::HERO_ECHO || localEntity.HeroID == eHero::HERO_DOOMFIST || localEntity.HeroID == eHero::HERO_PHARAH || localEntity.HeroID == eHero::HERO_ROADHOG || localEntity.HeroID == eHero::HERO_ZENYATTA)
					return true;
				else
					return false;
				return false;
			}
			__except (1) {

			}
		}
		inline ProjWeapon GetBulletTrajactory(uint64_t Hero, uint32_t Input)
		{
			bool LB = Input & 1;
			bool RB = Input & 2;
			bool Shift = Input & 8;
			bool E = Input & 16;
			bool Q = Input & 32;

			if (Hero == eHero::HERO_HANJO && GetHanzoCharging(localEntity.SkillBase) >= 0.5f) return ProjWeapon(25.f + (110.f - 25.f) * GetHanzoCharging(localEntity.SkillBase), 9.81f);
			else if (Hero == eHero::HERO_MEI && RB) return ProjWeapon(115.f, 0.f);
			else if (Hero == eHero::HERO_MEI && LB) return ProjWeapon(20.f, 0.f);
			else if (Hero == eHero::HERO_GENJI && RB) return ProjWeapon(60.f, 0.f);
			else if (Hero == eHero::HERO_GENJI && LB) return ProjWeapon(60.f, 0.f);
			else if (Hero == eHero::HERO_TRACER && Q) return ProjWeapon(15.f, 9.81f);
			else if (Hero == eHero::HERO_ZENYATTA) return ProjWeapon(80.f, 0.f);
			else if (Hero == eHero::HERO_REINHARDT && E) return ProjWeapon(25.f, 0.f);
			else if (Hero == eHero::HERO_SIGMA && E) return ProjWeapon(37.5f, 9.81f);
			else if (Hero == eHero::HERO_SIGMA && !Shift && !RB) return ProjWeapon(50.f, 0.f);
			else if (Hero == eHero::HERO_ANA && Shift) return ProjWeapon(60.f, 0.f);
			else if (Hero == eHero::HERO_ANA && !RB) return ProjWeapon(125.0, 0);
			else if (Hero == eHero::HERO_ROADHOG && Shift) return ProjWeapon(40.f, 0.f);
			else if (Hero == eHero::HERO_ROADHOG && RB) return ProjWeapon(80.f, 0.f);
			else if (Hero == eHero::HERO_ROADHOG) return ProjWeapon(80.f, 0.f);
			else if (Hero == eHero::HERO_ORISA && LB) return ProjWeapon(90.f, 0.f);
			else if (Hero == eHero::HERO_PHARAH && LB) return ProjWeapon(35.f, 0.f);
			else if (Hero == eHero::HERO_TORBJORN && RB) return ProjWeapon(120.f, 0.f);
			else if (Hero == eHero::HERO_TORBJORN && LB) return ProjWeapon(70.f, 9.81f);
			else if (Hero == eHero::HERO_DOOMFIST && LB) return ProjWeapon(80.f, 0.f);
			else if (Hero == eHero::HERO_DOOMFIST && RB) return ProjWeapon(45.f, 0.f);
			else if (Hero == eHero::HERO_LUCIO) return ProjWeapon(50.f, 0.f);
			else if (Hero == eHero::HERO_MERCY) return ProjWeapon(50.f, 0.f);
			else if (Hero == eHero::HERO_SOLDIER76 && RB) return ProjWeapon(50.f, 0.f);
			else if (Hero == eHero::HERO_BRIGITTE && Shift) return ProjWeapon(80.f, 0.f);
			else if (Hero == eHero::HERO_ECHO && LB) return ProjWeapon(75.f, 0.f);
			else if (Hero == eHero::HERO_ECHO && RB) return ProjWeapon(50.f, 0.f);
			else if (Hero == eHero::HERO_BAPTISTE && RB) return ProjWeapon(60.f, 9.81f);
			else if (Hero == eHero::HERO_MOIRA && LB) return ProjWeapon(30.f, 0.f);
			return ProjWeapon(0.f, 0.f);
		}
		inline BYTE GetTeam(uint64_t TeamBase)
		{
			__try
			{
				BYTE Team = SDK->RPM<BYTE>(TeamBase + 0x5b);
				return Team;
			}
			__except (1)
			{

			}
		}
		inline Matrix ViewMatrix() {
			__try {
				if (SDK->RPM<Matrix>(CoreHooks::GetViewMatrix()).GetCameraVec() != Vector3(0, 0, 0))
					return SDK->RPM<Matrix>(CoreHooks::GetViewMatrix());
				//return CoreHooks::scan_matrix_ex();
			}
			__except (1) {

			}
		}
		inline Vector3 GetBonePos(uint64_t BoneBase, uint64_t VelocityBase, uint64_t RotationBase, int index) {
			__try {
				Vector3 Pos{}, Rot{};
				Pos = CoreHooks::RootPos(BoneBase);
				Rot = SDK->RPM<Vector3>(SDK->RPM<uint64_t>(RotationBase + 0x7C0) + 0x8DC);
				if (Pos != Vector3(0, 0, 0) && Rot != Vector3(0, 0, 0)) {
					uint64_t pBoneData = SDK->RPM<uint64_t>(VelocityBase + 0x820);
					if (pBoneData)
					{
						uint64_t bonesBase = SDK->RPM<uint64_t>(pBoneData + 0x30);
						if (bonesBase)
						{
							DirectX::XMFLOAT3 currentBone = SDK->RPM<DirectX::XMFLOAT3>(bonesBase + (0x30 * get_bone_id(pBoneData, index)) + 0x20);
							DirectX::XMFLOAT3 Result{};
							XMMATRIX rotMatrix = XMMatrixRotationY(Rot.X);
							DirectX::XMStoreFloat3(&Result, XMVector3Transform(XMLoadFloat3(&currentBone), rotMatrix));
							return Vector3(Result.x, Result.y, Result.z) + Pos;
						}
					}
				}
				return Vector3(0, 0, 0);
			}
			__except (1) {

			}
		}
		inline espBone getBoneList(uint64_t BoneBase, uint64_t VelocityBase, uint64_t RotationBase, Vector3 pos, std::array<int, 14> index, uint64_t HeroID) {
			__try {
				espBone a; Vector2 pmin, pmax;
				Vector2 w2s;
				if (pos != Vector3(0, 0 ,0)) {
					if (HeroID == eHero::HERO_TRAININGBOT1 || HeroID == eHero::HERO_TRAININGBOT2 || HeroID == eHero::HERO_TRAININGBOT3 || HeroID == eHero::HERO_TRAININGBOT4) {
						if (ViewMatrix().isWorldToScreen(pos, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), w2s)) {
							pmin = w2s;
							pmax = w2s;
						}
						else { a.boneerror = true; }
						int index[] = { 17,16,3,13,54 };
						for (int i = 0; i < 5; i++) {
							Vector3 bone = GetBonePos(BoneBase, VelocityBase, RotationBase, index[i]);
							Vector2 w2s;
							if (ViewMatrix().isWorldToScreen(bone, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), w2s)) {
								if (i == 0) { a.head = w2s; a.head.Y += 4.f; }
								else if (i == 1) { a.neck = w2s; }
								else if (i == 2) { a.body_1 = w2s; }
								else if (i == 3) { a.l_1 = w2s; }
								else if (i == 4) { a.r_1 = w2s; }
								if (w2s.X < pmin.X) {
									pmin.X = w2s.X;
								}
								if (w2s.Y < pmin.Y) {
									pmin.Y = w2s.Y;
								}
								if (w2s.X > pmax.X) {
									pmax.X = w2s.X;
								}
								if (w2s.Y > pmax.Y) {
									pmax.Y = w2s.Y;
								}
							}
							else { a.boneerror = true; }
						}
						a.upL = pmin;
						a.upR = Vector2(pmax.X, pmin.Y);
						a.downL = Vector2(pmin.X, pmax.Y);
						a.downR = pmax;
						return a;
					}
					else {
						if (ViewMatrix().isWorldToScreen(pos, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), w2s)) {
							pmin = w2s;
							pmax = w2s;
						}
						else { a.boneerror = true; }
						for (int i = 0; i < 14; i++) {
							Vector3 bone = GetBonePos(BoneBase, VelocityBase, RotationBase, index[i]);
							Vector2 w2s;
							if (ViewMatrix().isWorldToScreen(bone, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), w2s)) {
								if (i == 0) { a.head = w2s; }
								else if (i == 1) { a.neck = w2s; }
								else if (i == 2) { a.body_1 = w2s; }
								else if (i == 3) { a.body_2 = w2s; }
								else if (i == 4) { a.l_1 = w2s; }
								else if (i == 5) { a.r_1 = w2s; }
								else if (i == 6) { a.l_d_1 = w2s; }
								else if (i == 7) { a.r_d_1 = w2s; }
								else if (i == 8) { a.l_a_1 = w2s; }
								else if (i == 9) { a.r_a_1 = w2s; }
								else if (i == 10) { a.l_a_2 = w2s; }
								else if (i == 11) { a.r_a_2 = w2s; }
								else if (i == 12) { a.l_d_2 = w2s; }
								else if (i == 13) { a.r_d_2 = w2s; }
								if (w2s.X < pmin.X) {
									pmin.X = w2s.X;
								}
								if (w2s.Y < pmin.Y) {
									pmin.Y = w2s.Y;
								}
								if (w2s.X > pmax.X) {
									pmax.X = w2s.X;
								}
								if (w2s.Y > pmax.Y) {
									pmax.Y = w2s.Y;
								}
							}
							else { a.boneerror = true; }
						}
					}
					a.upL = pmin;
					a.upR = Vector2(pmax.X, pmin.Y);
					a.downL = Vector2(pmin.X, pmax.Y);
					a.downR = pmax;
					return a;
				}
			}
			__except (1) {

			}
		}
		inline Vector3 getNearBone(uint64_t BoneBase, uint64_t VelocityBase, uint64_t RotationBase, std::array<int, 14> index, uint64_t HeroID) {
			Vector3 nearBone; float nearDist = FLT_MAX;
			if (HeroID == eHero::HERO_TRAININGBOT1 || HeroID == eHero::HERO_TRAININGBOT2 || HeroID == eHero::HERO_TRAININGBOT3 || HeroID == eHero::HERO_TRAININGBOT4) {
				int index[5] = { 17,16,3,13,54 };
				for (int i = 0; i < 5; i++) {
					Vector3 bone = GetBonePos(BoneBase, VelocityBase, RotationBase, index[i]);
					Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
					Vector3 angle = CalcAngle(ViewMatrix().GetCameraVec(), bone, ViewMatrix().GetCameraVec().DistTo(bone));
					float dist = angle.DistTo(currentAngle);
					if (dist < nearDist) {
						nearDist = dist; nearBone = bone;
					}
				}
			}
			else {
				for (int i = 0; i < 14; i++) {
					Vector3 bone = GetBonePos(BoneBase, VelocityBase, RotationBase, index[i]);
					Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
					Vector3 angle = CalcAngle(ViewMatrix().GetCameraVec(), bone, ViewMatrix().GetCameraVec().DistTo(bone));
					float dist = angle.DistTo(currentAngle);
					if (dist < nearDist) {
						nearDist = dist; nearBone = bone;
					}
				}
			}
			return nearBone;
		}
	};
	inline auto g_Func = std::make_unique<Function>();
}