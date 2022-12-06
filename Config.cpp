#include "Config.hpp"

namespace JIN
{
	void Config::SaveSetting(std::string Name, float Value)
	{
		WritePrivateProfileString(skCrypt("EXO_Config"), Name.c_str(), std::to_string(Value).c_str(), FileName.c_str());
	}
	float Config::LoadSetting(std::string Name)
	{
		GetPrivateProfileString(skCrypt("EXO_Config"), Name.c_str(), skCrypt("1.0f"), cBuf, MAX_PATH, FileName.c_str());
		const std::string T(cBuf);
		return std::stof(T);
	}
	void Config::Load(std::string Char)
	{
		FileName = FilePath + Char;
		Sleep(100);
		g_Config->g_Draw_Fov = LoadSetting(skCrypt("Draw Fov").decrypt());
		g_Config->g_Draw_Box = LoadSetting(skCrypt("Draw Box").decrypt());
		g_Config->g_Outline = LoadSetting(skCrypt("Draw Outline").decrypt());
		g_Config->g_Draw_Health = LoadSetting(skCrypt("Draw Health").decrypt());
		g_Config->g_Draw_Ult = LoadSetting(skCrypt("Draw Ult").decrypt());
		g_Config->g_Aimbot = LoadSetting(skCrypt("Aimbot").decrypt());
		g_Config->g_Pluse = LoadSetting(skCrypt("Tracer Auto Pluse").decrypt());
		//g_Config->m_ItemESP = LoadSetting(skCrypt("Item").decrypt());
		//g_Config->m_DeathBox = LoadSetting(skCrypt("DeathBox").decrypt());
		//g_Config->g_AutoDuck = LoadSetting(skCrypt("Show Knocked out").decrypt());
		g_Config->g_SkillHelper = LoadSetting(skCrypt("Skill Helper").decrypt());
		g_Config->g_AutoMelee = LoadSetting(skCrypt("Auto Melee").decrypt());
		g_Config->g_Dynamite = LoadSetting(skCrypt("Dynamite Auto Shot").decrypt());
		g_Config->g_FOV = LoadSetting(skCrypt("Field of view").decrypt());
		g_Config->g_acceleration = LoadSetting(skCrypt("Acceleration").decrypt());
		g_Config->g_AimSpeed = LoadSetting(skCrypt("Aiming Speed").decrypt());
		g_Config->g_Bones = LoadSetting(skCrypt("Bone").decrypt());
	}
	void Config::Save(std::string Char)
	{
		FileName = FilePath + Char;
		Sleep(100);
		SaveSetting(skCrypt("Draw Fov").decrypt(), g_Config->g_Draw_Fov);
		SaveSetting(skCrypt("Draw Box").decrypt(), g_Config->g_Draw_Box);
		SaveSetting(skCrypt("Draw Outline").decrypt(), g_Config->g_Outline);
		SaveSetting(skCrypt("Draw Health").decrypt(), g_Config->g_Draw_Health);
		SaveSetting(skCrypt("Draw Ult").decrypt(), g_Config->g_Draw_Ult);
		SaveSetting(skCrypt("Aimbot").decrypt(), g_Config->g_Aimbot);
		SaveSetting(skCrypt("Tracer Auto Pluse").decrypt(), g_Config->g_Pluse);
		SaveSetting(skCrypt("Skill Helper").decrypt(), g_Config->g_SkillHelper);
		SaveSetting(skCrypt("Auto Melee").decrypt(), g_Config->g_AutoMelee);
		SaveSetting(skCrypt("Box").decrypt(), g_Config->g_Dynamite);
		SaveSetting(skCrypt("Field of view").decrypt(), g_Config->g_FOV);
		SaveSetting(skCrypt("Enemies Nearby").decrypt(), g_Config->g_acceleration);
		SaveSetting(skCrypt("Aiming Speed").decrypt(), g_Config->g_AimSpeed);
		SaveSetting(skCrypt("Bone").decrypt(), g_Config->g_Bones);
	}
	void Config::Init()
	{
		FilePath = std::getenv(skCrypt("appdata"));
		_mkdir((FilePath + "\\EXO").c_str());
		FilePath += skCrypt("\\EXO\\");
	}
}