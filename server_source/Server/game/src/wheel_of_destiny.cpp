#include "stdafx.h"
#include "wheel_of_destiny.h"
#include "char.h"

#if defined(ENABLE_CHRISTMAS_WHEEL_OF_DESTINY)
static constexpr auto WheelItemMax = 16;

static constexpr std::tuple<DWORD, std::uint8_t, std::uint8_t> m_Data[WheelItemMax] =
{
	{78108, 1, 0},
	{78109, 1, 0},
	{78205, 1, 0},
	{78301, 1, 0},
	{70402, 1, 0},
	{78205, 1, 0},
	{78302, 1, 0},
	{50259, 1, 0},
	{78205, 1, 0},
	{78303, 1, 0},
	{89001, 1, 0},
	{78205, 1, 0},
	{78304, 1, 0},
	//{78306, 1, 0}, 30% vs natale
	{164406, 1, 0},
	{78205, 1, 0},
	{78305, 1, 0},
};

CWheelDestiny::CWheelDestiny(LPCHARACTER m_ch) : ch(m_ch), gift_vnum(0), gift_count(1), turn_count(0)
{
	for (auto i = 0; i < WheelItemMax; i++)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_WHEEL_ICON %lu %d %d", std::get<0>(m_Data[i]), std::get<1>(m_Data[i]), i);
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_WHEEL_OPEN");
}

CWheelDestiny::~CWheelDestiny()
{
	if (GetGiftVnum())
	{
		sys_err("<CWheelDestiny> player(%s) didn't get his gift(vnum: %lu(%d.x))!", ch->GetName(), GetGiftVnum(), GetGiftCount());
	}
}

template <typename T> std::string NumberToMoneyString(T val)
{
	constexpr int comma = 3;
	auto str = std::to_string(val);
	auto pos = static_cast<int>(str.length()) - comma;

	while (pos > 0) {
		str.insert(pos, ".");
		pos -= comma;
	}

	return str;
}

void CWheelDestiny::TurnWheel()
{
	if (GetGiftVnum()) {
#ifdef TEXTS_IMPROVEMENT
		ch->ChatPacketNew(CHAT_TYPE_INFO, 1307, "");
#endif
		return;
	}

	if (ch->CountSpecifyItem(78205) < 25) {
		return;
	}

	auto Rand = PickAGift();
	if (Rand == -1)
	{
		sys_err("CWheelDestiny::TurnWheel() Error Pick Gift (%s)", ch->GetName());
		return;
	}

	ch->RemoveSpecifyItem(78205, 25);

	//vnum, count, spin count, pos
	ch->ChatPacket(CHAT_TYPE_COMMAND, "BINARY_WHEEL_TURN %lu %d %d %d", GetGiftVnum(), GetGiftCount(), number(1, 8), Rand);

	turn_count++;
}

std::uint8_t CWheelDestiny::GetChance() const
{
#undef max
	const auto TurnCount = GetTurnCount();
	if (TurnCount >= 10 && TurnCount < 25)
	{
		return 25;
	}

	if (TurnCount >= 25 && TurnCount < 40)
	{
		return 50;
	}

	if (TurnCount >= 40)
	{
		return std::numeric_limits<std::uint8_t>::max();
	}
	
	return 0;
}

int CWheelDestiny::PickAGift()
{
	const auto Chance = GetChance();

	while (true) {
		const auto rand_pos = number(0, WheelItemMax - 1);
		const auto& [item, count, m_chance] = m_Data[rand_pos];
		if (Chance >= m_chance)
		{
			SetGift(item, count);
			return rand_pos;
		}
	}

	return -1;
}

void CWheelDestiny::SetGift(const DWORD vnum, const std::uint8_t count)
{
	gift_vnum = vnum;
	gift_count = count;
}

void CWheelDestiny::GiveMyGift()
{
	const auto GiftVnum = GetGiftVnum();
	if (GiftVnum) {
		ch->AutoGiveItem(GiftVnum, GetGiftCount());
		SetGift(0, 1);
	}
	else
	{
		sys_err("Dude, where is the gift_vnum? <player: %s>", ch->GetName());
	}
}

DWORD CWheelDestiny::GetGiftVnum() const
{
	return gift_vnum;
}

std::uint8_t CWheelDestiny::GetGiftCount() const
{
	return gift_count; 
}

std::uint16_t CWheelDestiny::GetTurnCount() const
{
	return turn_count; 
}
#endif
