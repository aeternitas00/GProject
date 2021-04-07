#pragma once

#include <synchapi.h>

/*
* CRITICAL_SECTION ����ü�� �� �� ���� �����ϰ� ���� ���� ���� Ŭ����.
* CriticalSection - CRITICAL_SECTION ����ü�� RAII ������Ʈ ���� ����. ī�� ��� ����.
* CSLock - CriticalSection�� ���� �ڵ�ȭ.
* TODO: Ŭ���� �̸����� �Ϲ����� �̸��̶� namespace�� ����ؾ� �Ѵ�.
*/

class NonCopyable
{
protected:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator= (const NonCopyable&) = delete;

public:
	NonCopyable() = default;
	~NonCopyable() = default;
};

class CriticalSection : NonCopyable
{
public:
	CriticalSection()
	{
		::InitializeCriticalSection(&m_rep);
	}
	~CriticalSection()
	{
		::DeleteCriticalSection(&m_rep);
	}

	void Enter()
	{
		::EnterCriticalSection(&m_rep);
	}
	void Leave()
	{
		::LeaveCriticalSection(&m_rep);
	}

private:
	CRITICAL_SECTION m_rep;
};

class CSLock : NonCopyable
{
public:
	CSLock(CriticalSection& a_section)
		: m_section(a_section) {
		m_section.Enter();
	}
	~CSLock()
	{
		m_section.Leave();
	}

private:
	CriticalSection& m_section;
};