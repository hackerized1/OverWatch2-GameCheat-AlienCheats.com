#pragma once
#include "includes.hpp"

namespace JIN {
    namespace offset {
		/* Address */
		constexpr auto Address_viewmatrix_base = 0x0457D340; //75 ? 48 89 1d ? ? ? ? eb ? 48 8b 05 ? ? ? ? 48 89 58 ? 48 8b 05
		constexpr auto Address_ComponentList = 0x3ED6770; //48 8B 15 ? ? ? ? 33 DB 48 85 D2 74 ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 89 1D ? ? ? ? 48 8B 15 ? ? ? ?
		constexpr auto Address_Sensitivity = 0x3EF4A70; //48 83 ec ? 48 8b 15 ? ? ? ? 48 85 d2 74 ? 48 8d 0d ? ? ? ? e8 ? ? ? ? 48 c7 05 ? ? ? ? ? ? ? ? 48 c7 05 ? ? ? ? ? ? ? ? 48 c7 05 ? ? ? ? ? ? ? ? 48 83 c4 ? c3 3f

		/* Xor */
		constexpr uint64_t root_pos_xor_key[8] = { //48 8D 54 24 ? 49 8D 8E ? ? 00 00 E8 ? ? ? ? 48 8D 54 24
	   0x0D12B5FD63CA8CFA7i64, //r12
	   0x1377842459E9A488i64,  //rbx  
	   0x6E2710685DE081CBi64,  //rdi
	   0x2A97E5699D41DA8Ei64,  //rsi 
	   0x0A644C0E8ED5D1C4Ei64, //rbp
	   0x83313BE6B79C41B1i64,  //r14
	   0xAB47BB4B57BBE122i64,  //r15
	   0x0D9A1BCECFB25F1FCi64  //r11   
		};
		/* Offsets */
		constexpr auto Offset_senstive = 0x7C0;

		/* Angle Offsets */
		constexpr auto Offset_angle_genji_status = 0x247;
		constexpr auto Offset_angle_rotation = 0x12A0;
		constexpr auto Offset_angle_key = Offset_angle_rotation - 0x5C;
		constexpr auto Offset_angle_recoil = 0x1774;
		constexpr auto Offset_angle_reload = 0x297;

        /* Pattern */
        inline char entity_pattern[] = { 0x1A, 0x00, 0x40, 0x00, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00 };
        inline char link_entity_pattern[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00 };
        inline char ashe_dynamite_pattern[] = { 0x1A, 0x0, 0x40, 0x17, 0x0, 0x80, 0xE, 0x0, 0x0, 0x10 };
    }
}