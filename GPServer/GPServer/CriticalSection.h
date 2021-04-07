#pragma once

#include <synchapi.h>

/*
* CRITICAL_SECTION 구조체를 좀 더 쉽고 안전하게 쓰기 위한 래퍼 클래스.
* CriticalSection - CRITICAL_SECTION 구조체의 RAII 오브젝트 패턴 래퍼. 카피 사고 방지.
* CSLock - CriticalSection의 동작 자동화.
* TODO: 클래스 이름들이 일반적인 이름이라 namespace도 고려해야 한다.
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