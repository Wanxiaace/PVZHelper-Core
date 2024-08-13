#pragma once
#include "../PVZ.h"
#include <iostream>
#include <vector>

class BaseEvent
{
public:
	DWORD address;
	BYTE raw;
	// ��ʼ��������¼�������Ҫ�û�����
	void start();
	// �����¼�������Ҫ�û�����
	virtual void handle(CONTEXT& context);
	// �����¼�������Ҫ�û�����
	void end();
	BaseEvent();
};