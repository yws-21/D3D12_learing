#pragma once

#include <type_traits>//引入做类型检查

template<typename CT, typename = std::enable_if_t<std::is_base_of_v<IUnknown, CT>>>
/*
template<typename CT>是一个模版，可以用它来包装任何类型的 COM 接口
（std::enable_if_t<std::is_base_of_v<IUnknown, CT>>）：
强制规定：类型 CT 必须继承自 IUnknown（IUnknown 是所有 COM 接口的基类）。
如果传了一个普通的类进去，编译器会直接报错，防止用错地方。
*/
class ComPointer
{
public:
	//生命周期管理（构造器们）主要是初始化
	ComPointer() = default;	// 1、默认构造函数=空指针

	ComPointer(CT* pointer) // 2、用最原始的定义指针法构造 
	{
		SetPointerAndAddRef(pointer); //  set以及给引用计数+1
	}

	ComPointer(const ComPointer<CT>& other) // 3、 拷贝构造函数
	{
		SetPointerAndAddRef(other.m_pointer); //给引用原对象的引用计数+1
	}
	ComPointer(ComPointer<CT>&& other) noexcept // 4、 移动构造函数
	{
		m_pointer = other.m_pointer;
		other.m_pointer = nullptr; 
	}

	~ComPointer()
	{
		ClearPointer(); //析构
	}

	//赋值运算符重载 （后续的赋值、改变
	ComPointer<CT>& operator=(const ComPointer<CT>& other)//拷贝赋值运算符
	{
		ClearPointer();//释放手里的
		SetPointerAndAddRef(other.m_pointer);//引用other且+1
		return *this;
	}
	ComPointer<CT>& operator=(ComPointer<CT>&& other)//移动赋值运算符
	{
		ClearPointer();
		m_pointer = other.m_pointer;
		other.m_pointer = nullptr;
		return *this;
	}
	ComPointer<CT>& operator=(CT* other)// 直接用原生的指针来赋值
	{
		ClearPointer();
		SetPointerAndAddRef(other);
		return *this;
	}

	ULONG Release() //手动释放指针
	{
		return ClearPointer();
	}

	CT* GetRef()
	{
		if (m_pointer) //如果指针非空
		{
			m_pointer->AddRef(); //加一个引用计数
			return m_pointer; //把指针给你(用完要自己release)
		}
		return nullptr; //这没东西
	}
	CT* Get()
	{
		return m_pointer;  //全给你
	}

	// 查询接口：类似于面向对象里的向下/向上转型，询问这个对象是否支持另一种 COM 接口
	template<typename T>
	bool QueryInterface(ComPointer<T>& other, HRESULT* errorCode = nullptr)
	{
		if (m_pointer)
		{
			HRESULT result = m_pointer->QueryInterface(IID_PPV_ARGS(&other));
			//  IID_PPV_ARGS核心功能是将 接口的 GUID（IID） 和 接口指针的地址 打包成一个参数，传递给需要这两个参数的 COM 函数。
			if (errorCode) *errorCode = result;
			return result == S_OK;
		}
		return false;
	}
	//关系运算符重载
	bool operator==(const ComPointer<CT>& other)
	{
		return m_pointer == other.m_pointer;
	}
	bool operator==(const CT* other)
	{
		return m_pointer == other;
	}

	CT* operator->()
	{
		return m_pointer;
	}
	CT** operator&()
	{
		return &m_pointer;
	}

	operator bool()
	{
		return m_pointer != nullptr;
	}
	operator CT* ()
	{
		return m_pointer;
	}

private:
	// 内部清理函数：如果当前有指针，就调用它的 Release() 并设为 nullptr
	ULONG ClearPointer()
	{
		ULONG newRef = 0;
		if (m_pointer)
		{
			newRef = m_pointer->Release();
			m_pointer = nullptr;
		}

		return newRef;
	}

	// 内部绑定函数：把新指针存起来，如果不为空就执行 AddRef()
	void SetPointerAndAddRef(CT* pointer)
	{
		m_pointer = pointer;
		if (m_pointer)
		{
			m_pointer->AddRef();
		}
	}

private:
	CT* m_pointer = nullptr;
}; ​


​