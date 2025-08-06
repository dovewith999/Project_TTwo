#pragma once
#include <memory>
#include <mutex>

/// <summary>
/// 상속받는 클래스에 싱글톤 패턴을 적용시키는 템플릿 클래스
/// 주로 매니저들이 상속받아 사용될 예정
/// unique_ptr과 mutex를 이용해 더욱 안전하다
/// 작성자 : 임희섭
/// 작성일 : 25/08/01
/// </summary>
/// <typeparam name="T"></typeparam>

template<typename T>
class SafeSingleton
{
private:
    static std::unique_ptr<T> m_instance;
    static std::once_flag m_onceFlag;

protected:
    SafeSingleton() = default;
    virtual ~SafeSingleton() = default;

public:
    static T* GetInstance()
    {
        std::call_once(m_onceFlag, []() {
            m_instance = std::make_unique<T>();
            });
        return m_instance.get();
    }

    static void DestroyInstance()
    {
        m_instance.reset();
    }

    // 복사 방지
    SafeSingleton(const SafeSingleton&) = delete;
    SafeSingleton& operator=(const SafeSingleton&) = delete;
    SafeSingleton(SafeSingleton&&) = delete;
    SafeSingleton& operator=(SafeSingleton&&) = delete;
};

// 정적 멤버 정의
template<typename T>
std::unique_ptr<T> SafeSingleton<T>::m_instance = nullptr;

template<typename T>
std::once_flag SafeSingleton<T>::m_onceFlag;