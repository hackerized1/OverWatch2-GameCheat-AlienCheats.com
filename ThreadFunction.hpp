#pragma once
#include "Fucking ImGui/imgui.h"
#include "Fucking ImGui/imgui_impl_dx11.h"
#include "Fucking ImGui/imgui_impl_win32.h"
#include "Fucking ImGui/imgui_internal.h""
#include <d3d9.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
#include <d3d11.h>
#include <chrono>
#include <thread>
#include "ThreadFunction.hpp"
#pragma comment(lib, "d3d11.lib")
#include "includes.hpp"
#include "EntityManager.hpp"
#include "Function.hpp"
#include "SDK.hpp"

#define DIRECTINPUT_VERSION 0x0800
// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace JIN {
	inline void EntityListThread() {
		while (true) {
			ow_entities = get_ow_entities();
			Sleep(1000);
		}
	}
	inline void EntityThread() {
		while (true) {
			if (ow_entities.size() > 0) {
				std::vector<c_entity> tmp_entities{};
				for (int i = 0; i < ow_entities.size(); i++) {
					c_entity entity;
					auto [ComponentParent, LinkParent] = ow_entities[i];
					if (!LinkParent || !ComponentParent)
						continue;
					entity.address = ComponentParent;
					entity.HealthBase = CoreHooks::fnDecryptChildInParent(ComponentParent, TYPE_HEALTH);
					entity.TeamBase = CoreHooks::fnDecryptChildInParent(ComponentParent, TYPE_TEAM);
					entity.VelocityBase = CoreHooks::fnDecryptChildInParent(ComponentParent, TYPE_VELOCITY);
					entity.HeroBase = CoreHooks::fnDecryptChildInParent(LinkParent, TYPE_P_HEROID);
					entity.BoneBase = CoreHooks::fnDecryptChildInParent(ComponentParent, TYPE_BONE);
					entity.OutlineBase = CoreHooks::fnDecryptChildInParent(ComponentParent, TYPE_OUTLINE);
					entity.RotationBase = CoreHooks::fnDecryptChildInParent(ComponentParent, TYPE_ROTATION);
					entity.SkillBase = CoreHooks::fnDecryptChildInParent(ComponentParent, TYPE_SKILL);
					entity.VisBase = CoreHooks::fnDecryptChildInParent(LinkParent, TYPE_P_VISIBILITY);
					entity.AngleBase = CoreHooks::fnDecryptChildInParent(LinkParent, TYPE_PLAYERCONTROLLER);
					health_compo_t health_compo{};
					velocity_compo_t velo_compo{};
					hero_compo_t hero_compo{};
					vis_compo_t vis_compo{};
					if (entity.HealthBase) {
						health_compo = SDK->RPM<health_compo_t>(entity.HealthBase);
						entity.PlayerHealth = health_compo.health + health_compo.armor + health_compo.barrier;
						entity.MinHealth = health_compo.health;
						entity.MaxHealth = health_compo.health_max;
						entity.MinArmorHealth = health_compo.armor;
						entity.MaxArmorHealth = health_compo.armor_max;
						entity.MinBarrierHealth = health_compo.barrier;
						entity.MaxBarrierHealth = health_compo.barrier_max;
						entity.Alive = (entity.PlayerHealth > 0.f) ? true : false;
					}
					if (entity.RotationBase) {
						entity.Rot = SDK->RPM<Vector3>(SDK->RPM<uint64_t>(entity.RotationBase + 0x7C0) + 0x8DC);
					}
					if (entity.VelocityBase) {
						velo_compo = SDK->RPM<velocity_compo_t>(entity.VelocityBase);
						entity.pos = CoreHooks::RootPos(entity.BoneBase);
						entity.velocity = Vector3(velo_compo.velocity.x, velo_compo.velocity.y, velo_compo.velocity.z);
					}
					if (entity.HeroBase) {
						hero_compo = SDK->RPM<hero_compo_t>(entity.HeroBase);
						entity.HeroID = hero_compo.heroid;
						entity.SkinID = hero_compo.skinid;
					}
					if (entity.VisBase) {
						vis_compo = SDK->RPM<vis_compo_t>(entity.VisBase);
						entity.Vis = (CoreHooks::DecryptVis(vis_compo.key1) ^ vis_compo.key2) ? true : false;
					}
					if (entity.TeamBase) {
						entity.Team = ((g_Func->GetTeam(entity.TeamBase) == 0x28 && g_Func->GetTeam(localEntity.TeamBase) == 0x28) || g_Func->GetTeam(entity.TeamBase) != g_Func->GetTeam(localEntity.TeamBase)) ? true : false;
					}
					const auto angle_component = CoreHooks::fnDecryptChildInParent(LinkParent, TYPE_PLAYERCONTROLLER);
					if (angle_component) {
						localEntity = entity;
						localEntity.address = ComponentParent;
						}
					if (g_Config->g_Bones == 0)
					{
						entity.head_pos = g_Func->GetBonePos(entity.BoneBase, entity.VelocityBase, entity.RotationBase, entity.GetSkel()[0]);
					}
					else if (g_Config->g_Bones == 1)
					{
						entity.head_pos = g_Func->GetBonePos(entity.BoneBase, entity.VelocityBase, entity.RotationBase, entity.GetSkel()[2]);
					}
					else if (g_Config->g_Bones == 2) {
						entity.head_pos = g_Func->getNearBone(entity.BoneBase, entity.VelocityBase, entity.RotationBase, entity.GetSkel(), entity.HeroID);
					}
					if (ComponentParent)
						tmp_entities.push_back(entity);
				}
				entities = tmp_entities;
			}
			Sleep(10);
		}
	}
	inline void FunctionThread() {
		__try {
			timeBeginPeriod(1);
			while (true) {
				g_Config->g_AimSpeed = 0.023f;
				bool shooted = false;
				static float origin_sens = 0.f;
				if (SDK->RPM<float>(g_Func->GetSenstivePTR()))
					origin_sens = SDK->RPM<float>(g_Func->GetSenstivePTR());
				else if (origin_sens)
					SDK->WPM<float>(g_Func->GetSenstivePTR(), origin_sens);
				constexpr clock_t accele_tick = 5;
				static clock_t last_accele_update = 0;
				constexpr clock_t ana_casting_time = 300;
				constexpr clock_t melee_casting_time = 500;
				constexpr clock_t road_casting_time = 300;
				constexpr clock_t tracer_casting_time = 150;
				clock_t start_time = clock();
				float acceleration = 0.0f;	
				if (g_Config->g_Aimbot) {
					if (localEntity.PlayerHealth > 0) {
						g_Config->g_Gravity = (localEntity.HeroID == eHero::HERO_HANJO || localEntity.HeroID == eHero::HERO_TORBJORN) ? true : false;
						while (GetAsyncKeyState(g_Config->g_AimKey) || GetAsyncKeyState(g_Config->g_AimKey2))
						{
							if (g_Func->GetReload() == true) {
								if (g_Func->Tracking()) {
									if (!shooted) {
										Vector3 world = g_Func->GetVector3();
										if (world.Size()) {
											Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
											Vector3 angle = g_Func->CalcAngle(g_Func->ViewMatrix().GetCameraVec(), world, g_Func->ViewMatrix().GetCameraVec().DistTo(world));
											Vector3 smoothed = g_Func->smooth_angle(currentAngle, angle, g_Config->g_AimSpeed + acceleration);
											if (g_Config->g_FOV)
											{
												if (currentAngle.DistTo(angle) * (180.f / M_PI) <= 35.f / g_Func->ViewMatrix().GetCameraVec().DistTo(world)) {
													acceleration = 0.f;
													SDK->WPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation, smoothed);
													shooted = false;
												}
												else {
													if (!last_accele_update || (clock() - last_accele_update > accele_tick)) {
														acceleration += g_Config->g_AimSpeed * (g_Config->g_acceleration / 50.f);
														last_accele_update = clock();
													}
													shooted = false;
												}
											}
										}
									}
								}
								else if (g_Func->Predicton()) {
									if (!shooted)
									{
										Vector3 world = g_Func->GetVectorPredit();
										if (world.Size())
										{
											Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
											Vector3 angle = g_Func->CalcAngle(g_Func->ViewMatrix().GetCameraVec(), world, g_Func->ViewMatrix().GetCameraVec().DistTo(world));
											Vector3 smoothed = g_Func->smooth_angle(currentAngle, angle, g_Config->g_AimSpeed);
											if (g_Config->g_FOV)
											{
												if (currentAngle.DistTo(angle) * (180.f / M_PI) <= 35.f / g_Func->ViewMatrix().GetCameraVec().DistTo(world))
												{
													SDK->WPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation, smoothed);
													shooted = false;
												}
												else {
													if (!last_accele_update || (clock() - last_accele_update > accele_tick)) {
														acceleration += g_Config->g_AimSpeed * g_Config->g_acceleration;
														last_accele_update = clock();
													}
													shooted = false;
												}
											}
										}
									}
								}
							}
							if (/*g_Func->GetRecoil() == true && */g_Func->GetReload() == true) {
								if (g_Func->Flicker()) {
									if (!shooted)
									{
										Vector3 world = g_Func->GetVector3();
										if (world.Size())
										{
											Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
											Vector3 angle = g_Func->CalcAngle(g_Func->ViewMatrix().GetCameraVec(), world, g_Func->ViewMatrix().GetCameraVec().DistTo(world));
											Vector3 smoothed = g_Func->smooth_angle(currentAngle, angle, g_Config->g_AimSpeed + acceleration);
											if (g_Config->g_FOV)
											{
												SDK->WPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation, smoothed);
												if (g_Config->g_Trigger)
												{
													SDK->WPM<float>(g_Func->GetSenstivePTR(), 0);
													SDK->WPM<DWORD>(localEntity.AngleBase + offset::Offset_angle_key, 1);
													SDK->WPM<float>(g_Func->GetSenstivePTR(), origin_sens);
													shooted = true;
												}
												else {
													if (!last_accele_update || (clock() - last_accele_update > accele_tick)) {
														acceleration += g_Config->g_AimSpeed * g_Config->g_acceleration;
														last_accele_update = clock();
													}
												}
											}
										}
									}
								}
								else if (g_Func->PredictFlicker()) {
									if (!shooted)
									{
										Vector3 world = g_Func->GetVectorPredit();
										if (world.Size())
										{
											Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
											Vector3 angle = g_Func->CalcAngle(g_Func->ViewMatrix().GetCameraVec(), world, g_Func->ViewMatrix().GetCameraVec().DistTo(world));
											Vector3 smoothed = g_Func->smooth_angle(currentAngle, angle, g_Config->g_AimSpeed + acceleration);
											if (g_Config->g_FOV)
											{
												SDK->WPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation, smoothed);
												if (g_Config->g_Trigger)
												{
													if (localEntity.HeroID == eHero::HERO_HANJO && !g_Func->isSkillAvtivate(localEntity.SkillBase, 0, 0x28E9)) {
														SDK->WPM<float>(g_Func->GetSenstivePTR(), 0);
														g_Func->PressKey(0x1000, 30);
														SDK->WPM<float>(g_Func->GetSenstivePTR(), origin_sens);
														shooted = true;
													}
													else {
														SDK->WPM<float>(g_Func->GetSenstivePTR(), 0);
														SDK->WPM<BYTE>(localEntity.AngleBase + offset::Offset_angle_key, 1);
														SDK->WPM<float>(g_Func->GetSenstivePTR(), origin_sens);
													}
												}
												else {
													if (!last_accele_update || (clock() - last_accele_update > accele_tick)) {
														acceleration += g_Config->g_AimSpeed * g_Config->g_acceleration;
														last_accele_update = clock();
													}
												}
											}
										}
									}
								}
							}
							Sleep(1);
						}
						while (GetAsyncKeyState(VK_LSHIFT)) {
							if (localEntity.HeroID == eHero::HERO_ANA) {
								clock_t cur_time = clock();
								clock_t delta = cur_time - start_time;

								if (delta < ana_casting_time / 2)
								{
									Sleep(1);
								}
								else if (delta < ana_casting_time)
								{
									Vector3 world = g_Func->GetVectorSkill();
									if (world.Size())
									{
										Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
										Vector3 angle = g_Func->CalcAngle(g_Func->ViewMatrix().GetCameraVec(), world, g_Func->ViewMatrix().GetCameraVec().DistTo(world));

										auto distance = static_cast<float>(ana_casting_time - delta);
										auto new_angle = (angle - currentAngle) / distance + currentAngle;

										if (distance < ana_casting_time / 3) SDK->WPM<float>(g_Func->GetSenstivePTR(), 0);
										SDK->WPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation, new_angle);
									}
								}
								else
									SDK->WPM<float>(g_Func->GetSenstivePTR(), origin_sens);
							}
							else if (localEntity.HeroID == eHero::HERO_ROADHOG) {
								clock_t cur_time = clock();
								clock_t delta = cur_time - start_time;

								if (delta < road_casting_time / 2)
								{
									Sleep(1);
								}
								else if (delta < road_casting_time)
								{
									g_Config->g_PreditLevel = 40.f;
									Vector3 world = g_Func->GetVectorSkill();
									if (world.Size() && g_Func->ViewMatrix().GetCameraVec().DistTo(world) <= 20.f)
									{
										Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
										Vector3 angle = g_Func->CalcAngle(g_Func->ViewMatrix().GetCameraVec(), world, g_Func->ViewMatrix().GetCameraVec().DistTo(world));

										auto distance = static_cast<float>(road_casting_time - delta);
										auto new_angle = (angle - currentAngle) / distance + currentAngle;

										if (distance < road_casting_time / 3) SDK->WPM<float>(g_Func->GetSenstivePTR(), 0);
										SDK->WPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation, new_angle);
									}
								}
								else
									SDK->WPM<float>(g_Func->GetSenstivePTR(), origin_sens);
							}
						}
						while (GetAsyncKeyState(0x51)) {
							if (localEntity.HeroID == eHero::HERO_TRACER) {
								clock_t cur_time = clock();
								clock_t delta = cur_time - start_time;

								if (delta < tracer_casting_time / 2)
								{
									Sleep(1);
								}
								else if (delta < tracer_casting_time)
								{
									Vector3 world = g_Func->GetVectorSkill();
									if (world.Size() && g_Func->ViewMatrix().GetCameraVec().DistTo(world) <= 5.f)
									{
										Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
										Vector3 angle = g_Func->CalcAngle(g_Func->ViewMatrix().GetCameraVec(), world, g_Func->ViewMatrix().GetCameraVec().DistTo(world));

										auto distance = static_cast<float>(tracer_casting_time - delta);
										auto new_angle = (angle - currentAngle) / distance + currentAngle;

										if (distance < tracer_casting_time / 3) SDK->WPM<float>(g_Func->GetSenstivePTR(), 0);
										SDK->WPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation, new_angle);
									}
								}
								else
									SDK->WPM<float>(g_Func->GetSenstivePTR(), origin_sens);
							}
						}
						if (localEntity.HeroID == eHero::HERO_MEI) {
							g_Config->g_AimSpeed = 0.0015f;
							float start_ammo = 0.0f;

							while (GetAsyncKeyState(VK_RBUTTON))
							{
								float cur_ammo = g_Func->IsCool(localEntity.SkillBase, 10, 53);
								float 고드름나가신다 = g_Func->IsCool(localEntity.SkillBase, 10, 173);

								if (!고드름나가신다)
								{
									if (start_ammo == 0.0f)
										start_ammo = cur_ammo;

									float 총알차이 = start_ammo - cur_ammo;
									if (총알차이 >= 8.0f)
									{
										Vector3 world = g_Func->GetVectorPredit();

										if (world.Size())
										{
											Vector3 currentAngle = SDK->RPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation);
											Vector3 angle = g_Func->CalcAngle(g_Func->ViewMatrix().GetCameraVec(), world, g_Func->ViewMatrix().GetCameraVec().DistTo(world));
											Vector3 smoothed = g_Func->smooth_angle(currentAngle, angle, g_Config->g_AimSpeed * (1.0f + 총알차이));
											SDK->WPM<float>(g_Func->GetSenstivePTR(), 0);
											if (currentAngle.DistTo(angle) * (180.f / M_PI) <= 10.f / g_Func->ViewMatrix().GetCameraVec().DistTo(world)) {
											}
											else
											{
												SDK->WPM<Vector3>(localEntity.AngleBase + offset::Offset_angle_rotation, smoothed);
												SDK->WPM<float>(g_Func->GetSenstivePTR(), origin_sens);
											}
										}
									}
								}
								else
									start_ammo = 0.0f;
							}
						}
					}
				}
				if (g_Config->g_Outline) {
					if (entities.size() > 0) {
						for (int i = 0; i < entities.size(); i++) {
							if (entities[i].OutlineBase && entities[i].Alive && entities[i].Team) {
								auto outline_struct = CoreHooks::get_outline_struct(entities[i].OutlineBase + 0x20);
								auto xor_key = CoreHooks::decrypt_outline_xor(SDK->RPM<uint64_t>(outline_struct + 0x18));
								SDK->WPM<uint64_t>(outline_struct + 0x10, xor_key ^ 2);

								auto outline_color_struct = CoreHooks::get_color_struct(entities[i].OutlineBase + 0x20);
								SDK->WPM<uint64_t>(outline_color_struct + 0x10, entities[i].Vis ? 0xFF00FF00 : 0xFFFFFFFF);
							}
						}
					}
				}
			}
			timeEndPeriod(1);
		}
		__except (1) {

		}
	}
	inline void esp() {
		__try {
			ImDrawList* Draw = ImGui::GetBackgroundDrawList();
			ImVec2 CrossHair = ImVec2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);
			if (g_Config->g_Draw_Fov) {
				Draw->AddCircle(CrossHair, g_Config->g_FOV, 0xFFFFFFFF, 100);
			}
			if (entities.size() > 0) {
				for (c_entity entity : entities) {
					if (entity.Alive && entity.Team && localEntity.PlayerHealth > 0) {
						espBone a = g_Func->getBoneList(entity.BoneBase, entity.VelocityBase, entity.RotationBase, entity.pos, entity.GetSkel(), entity.HeroID);
						if (!a.boneerror) {
							float size = abs(a.upL.Y - a.downR.Y) / 2.0f;
							if (g_Config->g_Draw_Box) {
								Draw->AddRect(ImVec2(a.upL.X - 3, a.upL.Y - 3), ImVec2(a.downR.X + 3, a.downR.Y + 3), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
							}
							if (g_Config->g_Draw_Health) {
								float hpSize = entity.PlayerHealth / (entity.MaxArmorHealth + entity.MaxBarrierHealth + entity.MaxHealth);
								hpSize = a.downR.Y + 3 - (hpSize * abs(a.upR.Y - a.downR.Y - 6));
								Draw->AddRectFilled(ImVec2(a.upR.X + size * 0.03f, hpSize), ImVec2(a.downR.X + size * 0.15f, a.downR.Y + 3), 0xFF0000FF, 1.5f);
							}
							//if (g_Config->g_Draw_Ult) {
							//	uint64_t SkillBase = SDK->RPM<uint64_t>(SDK->RPM<uint64_t>(entity.SkillBase + 0x2C0) + 0x60);

							//	if (SkillBase)
							//	{
							//		uint64_t UltBase{};
							//		float UltGauge{};
							//		if (SDK->RPM<uint16_t>(SkillBase + 0x40) == 0x1E32)
							//		{
							//			UltBase = SDK->RPM<uint64_t>(SkillBase + 0x48);
							//			UltGauge = SDK->RPM<float>(UltBase + 0x60); // 완료
							//		}
							//		else if (SDK->RPM<uint16_t>(SkillBase + 0x50) == 0x1E32)
							//		{
							//			UltBase = SDK->RPM<uint64_t>(SkillBase + 0x58);
							//			UltGauge = SDK->RPM<float>(UltBase + 0x60); // 완료
							//		}
							//		else if (SDK->RPM<uint16_t>(SkillBase + 0x60) == 0x1E32)
							//		{
							//			UltBase = SDK->RPM<uint64_t>(SkillBase + 0x68);
							//			UltGauge = SDK->RPM<float>(UltBase + 0x60); // 완료
							//		}
							//		float UltSize = UltGauge / 100.f;
							//		UltSize = a.downR.Y + 3 - (UltSize * abs(a.upR.Y - a.downR.Y - 6));
							//		Draw->AddRectFilled(ImVec2(a.upR.X + size * 0.18f, UltSize), ImVec2(a.downR.X + size * 0.30f, a.downR.Y + 3), 0xFF00FFFF, 1.5f);
							//	}
							//}
							if (g_Config->g_Draw_Bone) {
								if (entity.HeroID == eHero::HERO_TRAININGBOT1 || entity.HeroID == eHero::HERO_TRAININGBOT2 || entity.HeroID == eHero::HERO_TRAININGBOT3 || entity.HeroID == eHero::HERO_TRAININGBOT4) {
									Draw->AddLine(ImVec2(a.head.X, a.head.Y), ImVec2(a.neck.X, a.neck.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.neck.X, a.neck.Y), ImVec2(a.body_1.X, a.body_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.body_1.X, a.body_1.Y), ImVec2(a.l_1.X, a.l_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.body_1.X, a.body_1.Y), ImVec2(a.r_1.X, a.r_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
								}
								else {
									Draw->AddLine(ImVec2(a.head.X, a.head.Y), ImVec2(a.neck.X, a.neck.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.neck.X, a.neck.Y), ImVec2(a.body_1.X, a.body_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.body_1.X, a.body_1.Y), ImVec2(a.body_2.X, a.body_2.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.neck.X, a.neck.Y), ImVec2(a.l_1.X, a.l_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.neck.X, a.neck.Y), ImVec2(a.r_1.X, a.r_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.l_1.X, a.l_1.Y), ImVec2(a.l_d_1.X, a.l_d_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.r_1.X, a.r_1.Y), ImVec2(a.r_d_1.X, a.r_d_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.l_d_1.X, a.l_d_1.Y), ImVec2(a.l_d_2.X, a.l_d_2.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.r_d_1.X, a.r_d_1.Y), ImVec2(a.r_d_2.X, a.r_d_2.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.body_2.X, a.body_2.Y), ImVec2(a.l_a_1.X, a.l_a_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.body_2.X, a.body_2.Y), ImVec2(a.r_a_1.X, a.r_a_1.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.l_a_1.X, a.l_a_1.Y), ImVec2(a.l_a_2.X, a.l_a_2.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
									Draw->AddLine(ImVec2(a.r_a_1.X, a.r_a_1.Y), ImVec2(a.r_a_2.X, a.r_a_2.Y), entity.Vis ? 0xFF00FF00 : 0xFFFFFFFF, 1.5f);
								}
							}
						}
					}
				}
			}
		}
		__except (1) {

		}
	}
	// Data
	static ID3D11Device* g_pd3dDevice = NULL;
	static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	static IDXGISwapChain* g_pSwapChain = NULL;
	static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
	// Forward declarations of helper functions
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	bool CreateDeviceD3D(HWND hWnd)
	{
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
			return false;

		CreateRenderTarget();
		return true;
	}
	void CleanupDeviceD3D()
	{
		CleanupRenderTarget();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	}
	void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}
	void CleanupRenderTarget()
	{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	}
	// Win32 message handler
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
	std::chrono::system_clock::time_point a = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point b = std::chrono::system_clock::now();
	void InitUI()
	{
		//timeBeginPeriod(1);
		HWND tWnd = FindWindowA(skCrypt("TankWindowClass"), NULL);
		auto str_ow = skCrypt(u8"fasfasfdsadsa");
		WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, str_ow.decrypt(), NULL };
		RegisterClassEx(&wc);
		HWND hwnd = CreateWindow(wc.lpszClassName, str_ow.decrypt(), WS_POPUP, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
		str_ow.clear();
		if (!CreateDeviceD3D(hwnd))
		{
			CleanupDeviceD3D();
			UnregisterClass(wc.lpszClassName, wc.hInstance);
		}
		MARGINS margins = { -1 };
		DwmExtendFrameIntoClientArea(hwnd, &margins);
		SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE);
		//SetWindowDisplayAffinity(hwnd, WDA_MONITOR);
		ShowWindow(hwnd, SW_SHOWDEFAULT);
		UpdateWindow(hwnd);
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(tWnd);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		int FPS;

		DEVMODE dm{};
		dm.dmSize = sizeof(DEVMODE);

		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
		while (true)
		{
			FPS = dm.dmDisplayFrequency;

			a = std::chrono::system_clock::now();
			std::chrono::duration<double, std::milli> work_time = a - b;
			if (work_time.count() < 1000 / FPS)
			{
				std::chrono::duration<double, std::milli> delta_ms(1000 / FPS - work_time.count());
				auto delta_ms_duration = std::chrono::duration_cast<std::chrono::milliseconds>(delta_ms);
				std::this_thread::sleep_for(std::chrono::milliseconds(delta_ms_duration.count()));
			}

			b = std::chrono::system_clock::now();

			Sleep(2.5);

			std::chrono::duration<double, std::milli> sleep_time = b - a;
			DWORD Style = GetWindowLong(tWnd, GWL_STYLE);
			RECT rect;
			GetWindowRect(tWnd, &rect);

			SetWindowPos(hwnd, HWND_TOPMOST, rect.left, rect.top, rect.right, rect.bottom, SWP_NOZORDER);

			SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

			if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				continue;
			}

			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			esp();
			ImGui::EndFrame();
			ImGui::Render();
			ImVec4 col = ImVec4(0, 0, 0, 0);
			float* a = (float*)&col;
			g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, a);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			g_pSwapChain->Present(1, 0);
		}
		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		::DestroyWindow(hwnd);
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		//timeEndPeriod(1);
	}
	inline void ViewMatrixThread() {
		__try {
			while (true) {
				/*static auto vm_sig = (BYTE*)"\xCD\xCC\xCC\x3D\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01\x01\x00\x00\x00\x01\x01\x00\x01";
				static auto vm_mask = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
				auto vmBase = SDK->FindPatternsExRegs(vm_sig, vm_mask, 0x400000);
				if (vmBase[0] != 0) {
					viewMatrix_ptr = vmBase[0] - 0xB0;
				}*/
				uint64_t vm = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_viewmatrix_base);
				vm = SDK->RPM<uint64_t>(vm + 0x50);
				vm = SDK->RPM<uint64_t>(vm + 0x50);
				vm = SDK->RPM<uint64_t>(vm + 0x28);
				vm = SDK->RPM<uint64_t>(vm + 0x2C0);
				vm = SDK->RPM<uint64_t>(vm + 0x8);
				vm = SDK->RPM<uint64_t>(vm + 0x0);
				vm = vm + 0x590;
				if (vm != 0) {
					viewMatrix_ptr = vm - 0xb0;
					printf("viewMatrix_ptr = %p\n", viewMatrix_ptr);
				}
				Sleep(5);
			}
		}
		__except (1) {

		}
	}
	inline void AllThread() {
		__try {
			_beginthread((_beginthread_proc_type)ViewMatrixThread, 0, 0);
			_beginthread((_beginthread_proc_type)EntityThread, 0, 0);
			_beginthread((_beginthread_proc_type)FunctionThread, 0, 0);
			_beginthread((_beginthread_proc_type)InitUI, 0, 0);
		}
		__except (1) {

		}
	}
}