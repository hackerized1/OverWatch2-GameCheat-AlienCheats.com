#pragma once
#include "includes.hpp"

namespace JIN {
	class Config {
	public:
		void Save(std::string Char);
		void Load(std::string Char);
		void Init();
	public:
		std::atomic_bool g_Opened = true;
		bool g_Pluse = false;
		bool g_AnlgeLock = false;
		bool g_AutoDuck = false;
		bool g_Aimbot = true;
		bool g_IsLogin = false;
		bool g_Outline = true;
		bool g_Draw_Fov = true;
		bool g_Draw_Box = true;
		bool g_Draw_Health = true;
		bool g_Draw_Ult = true;
		bool g_Draw_Bone = true;
		bool g_SkillHelper = true;
		bool g_AutoMelee = true;
		bool g_Dynamite = true;
		bool g_Gravity = false;
		bool g_Trigger;

		float g_FOV = 250.0f;
		float g_DynamiteFov = 50.f;
		float g_acceleration = 0.093f;
		float g_AimSpeed = 0.023f; //0.023f
		float g_PreditLevel = 110.f;
		float g_TriggerScale = 120.5f;

		XMFLOAT3 min{ -0.1f, -0.05f, -0.1f }, max{ 0.1f, 0.13f, 0.1f };
		XMFLOAT3 min2{ -0.16f, -0.3f, -0.16f }, max2{ 0.15f, 0.2f, 0.15f };

		int g_AimKey = VK_XBUTTON1;
		int g_AimKey2 = VK_XBUTTON2;
		int g_Helper_Key = VK_MBUTTON;
		int g_Bones = 0;
		const char* g_Bone_str[3] = { skCrypt("Head"), skCrypt(u8"Body") , skCrypt(u8"CloseBone") };
	private:
		void SaveSetting(std::string Name, float Value);
		float LoadSetting(std::string Name);

		std::string FilePath;
		std::string FileName;
		char* cBuf = (char*)malloc(sizeof(char) * 256);
	};
	inline auto g_Config = std::make_unique<Config>();
}