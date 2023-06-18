#ifndef __INC_METIN_II_GAME_DRAGON_SOUL_H__
#define __INC_METIN_II_GAME_DRAGON_SOUL_H__

#include "../../common/length.h"

class CHARACTER;
class CItem;

class DragonSoulTable;

class DSManager : public singleton<DSManager>
{
public:
	DSManager();
	~DSManager();
	bool	ReadDragonSoulTableFile(const char * c_pszFileName);

	void	GetDragonSoulInfo(DWORD dwVnum, OUT BYTE& bType, OUT BYTE& bGrade, OUT BYTE& bStep, OUT BYTE& bRefine) const;
	// fixme : titempos��
	WORD	GetBasePosition(const LPITEM pItem) const;
	bool	IsValidCellForThisItem(const LPITEM pItem, const TItemPos& Cell) const;
	int		GetDuration(const LPITEM pItem) const;

	// ��ȥ���� �޾Ƽ� Ư�� ����� �����ϴ� �Լ�
	bool	ExtractDragonHeart(LPCHARACTER ch, LPITEM pItem, LPITEM pExtractor = NULL);

	// Ư�� ��ȥ��(pItem)�� ���â���� ������ ���� ���� ���θ� �����ϰ�,
	// ���н� �λ깰�� �ִ� �Լ�.(�λ깰�� dragon_soul_table.txt�� ����)
	// DestCell�� invalid�� ���� ������ ���� ��, ��ȥ���� �� ������ �ڵ� �߰�.
	// ���� ��, ��ȥ��(pItem)�� delete��.
	// ����������� �ִٸ� ���� ���� Ȯ���� pExtractor->GetValue(0)%��ŭ ������.
	// �λ깰�� ������ �ڵ� �߰�.
	bool	PullOut(LPCHARACTER ch, TItemPos DestCell, IN OUT LPITEM& pItem, LPITEM pExtractor = NULL);

	// ��ȥ�� ���׷��̵� �Լ�
	bool	DoRefineGrade(LPCHARACTER ch, TItemPos (&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE]);
	bool	DoRefineStep(LPCHARACTER ch, TItemPos (&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE]);
	bool	DoRefineStrength(LPCHARACTER ch, TItemPos (&aItemPoses)[DRAGON_SOUL_REFINE_GRID_SIZE]);
#ifdef ENABLE_DS_REFINE_ALL
	void DoRefineAll(LPCHARACTER ch, uint8_t subheader, uint8_t type, uint8_t grade);
#endif

	bool	DragonSoulItemInitialize(LPITEM pItem);

	bool	IsTimeLeftDragonSoul(LPITEM pItem) const;
	int		LeftTime(LPITEM pItem) const;
	bool	ActivateDragonSoul(LPITEM pItem);
	bool	DeactivateDragonSoul(LPITEM pItem, bool bSkipRefreshOwnerActiveState = false);
	bool	IsActiveDragonSoul(LPITEM pItem) const;
#ifdef ENABLE_DS_ENCHANT
	bool	PutAttributes(LPITEM pDS);
#endif
private:
	void	SendRefineResultPacket(LPCHARACTER ch, BYTE bSubHeader, const TItemPos& pos);

	// ĳ������ ��ȥ�� ���� ���캸��, Ȱ��ȭ �� ��ȥ���� ���ٸ�, ĳ������ ��ȥ�� Ȱ�� ���¸� off ��Ű�� �Լ�.
	void	RefreshDragonSoulState(LPCHARACTER ch);

	DWORD	MakeDragonSoulVnum(BYTE bType, BYTE grade, BYTE step, BYTE refine);
	bool	RefreshItemAttributes(LPITEM pItem);
#ifndef ENABLE_DS_ENCHANT
	bool	PutAttributes(LPITEM pDS);
#endif
	DragonSoulTable*	m_pTable;
};

#endif
