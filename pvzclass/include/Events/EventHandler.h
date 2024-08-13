#pragma once
#include "../PVZ.h"
#include "TemplateEvent.h"
#include <vector>
#include <iostream>

class EventHandler
{
private:
	DEBUG_EVENT debugEvent;
	CONTEXT context;
	void failLog(int line, const char* message);
	void getContext();
	void setContext();
	void singleStep();
	void continueDebug(int line);
	void waitDebugInfinity(int line);
	HANDLE getThread(int line);
	void closeThread(HANDLE hThread, int line);
	void resume();

protected:
	std::vector<std::shared_ptr<BaseEvent>> events;

public:
	// ��ʼ����PVZ���̣�����һС�ο���
	void start();

	// �ȴ�����ms��ʱ�䣬�����Ƿ����ϵͳ�ж�
	// ms����Ϊ1��������-1�������޵ȴ�
	// �������ֵΪtrue����Ҫ����handle()��resume()
	bool run(int ms);

	// ֹͣ����PVZ����
	void stop();

	// ����һ���¼���ָ�������Ӧ�����¼���ʵ�����Ͷ���BaseEvent
	void addEvent(std::shared_ptr<BaseEvent> ptr);
};