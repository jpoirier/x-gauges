#ifndef DELEGATE_H
#define DELEGATE_H

/***************************************************************************
 *                                                                         *
 *   Copyright (C) 2006 by Kanardia d.o.o.                                 *
 *   Writen by:                                                            *
 *      Rok Markovic [rok.markovic@kanardia.eu]                            *
 *                                                                         *
 *   Status: Confidential                                                  *
 *                                                                         *
 *   Licence: All rights reserved                                          *
 *                                                                         *
 ***************************************************************************/


/**
 * \author Rok Markovic
 * \date   July 2006
 *
 * @brief Realization of callback methods in c++.
 *
 * This is very fast dynamic callback implementation in c++.
 
	// Instance
	Delegate<ret_val, arg1> D =
		Delegate<ret_val, arg1>::FromMethod<Class, &Class::Method>(&ClassObject);
	
	// Call
	r = D(arg1);
 */

// --------------------------------------------------------------------------

template <typename R, typename AT1>
class Delegate
{
public:
	Delegate() : m_pObject(0), m_pStub(0){}

	template <class T, R (T::*TMethod)(AT1)> 
	static Delegate FromMethod(T* pObject)
	{
		return FromStub(pObject, &MethodStub<T, TMethod>); 
	}

	template <class T, R (T::*TMethod)(AT1) const>
	static Delegate FromConstMethod(T const* pObject)
	{
		return FromStub(const_cast<T*>(pObject), &ConstMethodStub<T, TMethod>); 
	}

	R operator()(AT1 a1) const
	{
		return (*m_pStub)(m_pObject, a1); 
	}
	
	bool IsNull() const
	{ return m_pObject == 0; }


private:
	typedef R (*stub_type)(void* pObject, AT1);

	static Delegate FromStub(void* pObject, stub_type pStub)
	{
		Delegate d;
		d.m_pObject = pObject;
		d.m_pStub   = pStub;
		return d;
	}

	template <class T, R (T::*TMethod)(AT1)>
	static R MethodStub(void* pObject, AT1 a1)
	{
		T* p = static_cast<T*>(pObject);
		return (p->*TMethod)(a1);
	}

	template <class T, R (T::*TMethod)(AT1) const>
	static R ConstMethodStub(void* pObject, AT1 a1)
	{
		T const* p = static_cast<T*>(pObject);
		return (p->*TMethod)(a1);
	}
	
private:
	void* m_pObject;
	stub_type m_pStub;
};

// --------------------------------------------------------------------------

template <typename R>
class Delegate0
{
public: 
	Delegate0() : m_pObject(0), m_pStub(0) {}

	template <class T, R (T::*TMethod)()>
	static Delegate0 FromMethod(T* pObject)
	{
		return FromStub(pObject, &MethodStub<T, TMethod>); 
	}

	template <class T, R (T::*TMethod)() const>
	static Delegate0 FromConstMethod(T const* pObject)
	{
		return FromStub(const_cast<T*>(pObject), &ConstMethodStub<T, TMethod>); 
	}

	R operator()() const 
	{
		return (*m_pStub)(m_pObject); 
	}
	
	bool IsNull() const
	{ return m_pObject == 0; }

private:
	typedef R (*stub_type)(void* pObject);

	static Delegate0 FromStub(void* pObject, stub_type pStub)
	{
		Delegate0 d;
		d.m_pObject = pObject;
		d.m_pStub = pStub;
		return d;
	}

	template <class T, R (T::*TMethod)()>
	static R MethodStub(void* pObject)
	{
		T* p = static_cast<T*>(pObject);
		return (p->*TMethod)();
	}

	template <class T, R (T::*TMethod)() const>
	static R ConstMethodStub(void* pObject)
	{
		T const* p = static_cast<T*>(pObject);
		return (p->*TMethod)();
	}

private:
	void* m_pObject;
	stub_type m_pStub;
};

// --------------------------------------------------------------------------

#endif
