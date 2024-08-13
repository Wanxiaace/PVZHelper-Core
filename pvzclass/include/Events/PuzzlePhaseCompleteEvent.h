#pragma once
#include "TemplateEvent.h"

// ���չؿ�����¼�
// �����������¼�ʱ�� Miscellaneous��X���꣬Y����
// ����ֵ�����º��Y����
// ����¼�֮��Y����ᴮ���޸�
class PuzzlePhaseCompleteEvent : public TemplateEvent<std::function<
	int(SPT<PVZ::Miscellaneous>, int, int)>>
{
public:
	PuzzlePhaseCompleteEvent();
	void handle(CONTEXT& context) override;
};