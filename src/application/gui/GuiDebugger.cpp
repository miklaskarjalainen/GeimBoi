#include "GuiDebugger.hpp"
extern "C" {
#include "gbCore.h"
}

#include <imgui.h>

GeimBoi::GuiDebugger::GuiDebugger(gb_emu_t* emu) : m_Emulator(emu) {}

GeimBoi::GuiDebugger::~GuiDebugger() {}

void GeimBoi::GuiDebugger::draw()
{
	draw_cart();
	draw_cpu();
	draw_ppu();
}

void GeimBoi::GuiDebugger::draw_cpu()
{
	ImGui::Begin("CPU State");

	ImGui::SeparatorText("Registers");
	ImGui::Text("AF 0x%04X", GB_REG_AF(m_Emulator->cpu.regs));
	ImGui::SameLine();
	ImGui::Text("BC 0x%04X", GB_REG_BC(m_Emulator->cpu.regs));
	ImGui::Text("DE 0x%04X", GB_REG_DE(m_Emulator->cpu.regs));
	ImGui::SameLine();
	ImGui::Text("HL 0x%04X", GB_REG_HL(m_Emulator->cpu.regs));
	ImGui::Text(
		"SP 0x%04X [0x%04X]",
		GB_REG_SP(m_Emulator->cpu.regs),
		gb_mmu_read_u16(&m_Emulator->mmu, GB_REG_SP(m_Emulator->cpu.regs))
	);
	ImGui::Text(
		"PC 0x%04X [0x%04X]",
		GB_REG_PC(m_Emulator->cpu.regs),
		gb_mmu_read_u16(&m_Emulator->mmu, GB_REG_PC(m_Emulator->cpu.regs))
	);

	ImGui::Text(
		"FLAGS %c %c %c %c",
		GB_IS_BIT(GB_REG_F(m_Emulator->cpu.regs), GB_FLAG_ZERO_BIT) ? 'Z' : '-',
		GB_IS_BIT(GB_REG_F(m_Emulator->cpu.regs), GB_FLAG_SUBS_BIT) ? 'N' : '-',
		GB_IS_BIT(GB_REG_F(m_Emulator->cpu.regs), GB_FLAG_HALF_BIT) ? 'H' : '-',
		GB_IS_BIT(GB_REG_F(m_Emulator->cpu.regs), GB_FLAG_CARR_BIT) ? 'C' : '-'
	);

	ImGui::Text(
		"InterruptEnable: %s",
		m_Emulator->cpu.interrupt_enable ? "true" : "false"
	);
	ImGui::Text("Is halted: %s", m_Emulator->cpu.is_halted ? "true" : "false");
	ImGui::End();
}

void GeimBoi::GuiDebugger::draw_cart()
{
	ImGui::Begin("Cartridge");
	ImGui::Text("Mapper: %u", gb_cart_mapper_type(&m_Emulator->cart));
	ImGui::Text("Rom banks: %u", m_Emulator->cart.rom_banks);
	ImGui::Text("Ram banks: %u", m_Emulator->cart.ram_banks);
	ImGui::Text(
		"MBC1 mode: %u", m_Emulator->cart.mapper_data.mbc1.banking_mode
	);
	ImGui::Text(
		"MBC1 bank (low): %u", m_Emulator->cart.mapper_data.mbc1.rom_bank_low
	);
	ImGui::Text(
		"MBC1 bank (high): %u", m_Emulator->cart.mapper_data.mbc1.rom_bank_high
	);
	ImGui::End();
}

void GeimBoi::GuiDebugger::draw_ppu()
{
	ImGui::Begin("PPU");
	ImGui::Text("T-Cycles 0x%04X", m_Emulator->ppu.t_cycles);
	ImGui::Text("Enabled: %X", GB_IS_BIT(m_Emulator->ppu.lcdc, 7));
	ImGui::Text("Mode %u", gb_mmu_read_u8(&m_Emulator->mmu, 0xFF41) & 0x3);
	ImGui::End();
}
