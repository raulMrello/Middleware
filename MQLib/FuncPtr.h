/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef FUNCPTR_H
#define FUNCPTR_H


/* If we had variaditic templates, this wouldn't be a problem, but until C++11 is enabled, we are stuck with multiple classes... */

/** A class for storing and calling a pointer to a static or member function
 */
template <typename R, typename A1>
class FuncPtr1{
public:
    /** Create a FunctionPointer, attaching a static function
     *
     *  @param function The static function to attach (default is none)
     */
	FuncPtr1(R(*function)(A1) = 0) {
        attach(function);
    }

    /** Create a FunctionPointer, attaching a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the member function to attach
     */
    template<typename T>
		FuncPtr1(T *object, R(T::*member)(A1)) {
        attach(object, member);
    }

    /** Attach a static function
     *
     *  @param function The static function to attach (default is none)
     */
	void attach(R(*function)(A1)) {
        _p.function = function;
        _membercaller = 0;
    }

    /** Attach a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the member function to attach
     */
    template<typename T>
		void attach(T *object, R(T::*member)(A1)) {
        _p.object = static_cast<void*>(object);
        *reinterpret_cast<R (T::**)(A1)>(_member) = member;
        _membercaller = &FuncPtr1::membercaller<T>;
    }

    /** Call the attached static or member function
     */
	R call(A1 a) {
        if (_membercaller == 0 && _p.function) {
           return _p.function(a);
        } else if (_membercaller && _p.object) {
           return _membercaller(_p.object, _member, a);
        }
        return (R)0;
    }

    /** Get registered static function
     */
    R(*get_function(A1))() {
        return _membercaller ? (R(*)(A1))0 : (R(*)(A1))_p.function;
    }

#ifdef MBED_OPERATORS
    R operator ()(A1 a) {
        return call(a);
    }
    operator bool(void) const {
        return (_membercaller != NULL ? _p.object : (void*)_p.function) != NULL;
    }
#endif
private:
    template<typename T>
		static R membercaller(void *object, uintptr_t *member, A1 a) {
        T* o = static_cast<T*>(object);
        R (T::**m)(A1) = reinterpret_cast<R (T::**)(A1)>(member);
        return (o->**m)(a);
    }

    union {
        R (*function)(A1); // static function pointer
        void *object;      // object this pointer
    } _p;
    uintptr_t _member[4]; // aligned raw member function pointer storage - converted back by registered _membercaller
    R (*_membercaller)(void*, uintptr_t*, A1); // registered membercaller function to convert back and call _m.member on _object
};

/** A class for storing and calling a pointer to a static or member function (R ()(void))
 */
template <typename R>
class FuncPtr1<R, void>{
public:
    /** Create a FunctionPointer, attaching a static function
     *
     *  @param function The static function to attach (default is none)
     */
	FuncPtr1(R(*function)(void) = 0) {
        attach(function);
    }

    /** Create a FunctionPointer, attaching a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the void member function to attach
     */
    template<typename T>
		FuncPtr1(T *object, R(T::*member)(void)) {
        attach(object, member);
    }

    /** Attach a static function
     *
     *  @param function The void static function to attach (default is none)
     */
	void attach(R(*function)(void)) {
        _p.function = function;
        _membercaller = 0;
    }

    /** Attach a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the void member function to attach
     */
    template<typename T>
		void attach(T *object, R(T::*member)(void)) {
        _p.object = static_cast<void*>(object);
        *reinterpret_cast<R (T::**)(void)>(_member) = member;
        _membercaller = &FuncPtr1::membercaller<T>;
    }

    /** Call the attached static or member function
     */
	R call() {
        if (_membercaller == 0 && _p.function) {
            return _p.function();
        } else if (_membercaller && _p.object) {
            return _membercaller(_p.object, _member);
        }
        return (R)0;
    }

    /** Get registered static function
     */
    R(*get_function())() {
        return _membercaller ? (R(*)())0 : (R(*)())_p.function;
    }

#ifdef MBED_OPERATORS
    R operator ()(void) {
        return call();
    }
    operator bool(void) const {
        return (_membercaller != NULL ? _p.object : (void*)_p.function) != NULL;
    }
#endif

private:
    template<typename T>
		static R membercaller(void *object, uintptr_t *member) {
        T* o = static_cast<T*>(object);
        R (T::**m)(void) = reinterpret_cast<R (T::**)(void)>(member);
        return (o->**m)();
    }

    union {
        R (*function)(void); // static function pointer
        void *object;        // object this pointer
    } _p;
    uintptr_t _member[4]; // aligned raw member function pointer storage - converted back by registered _membercaller
    R (*_membercaller)(void*, uintptr_t*); // registered membercaller function to convert back and call _m.member on _object
};


template <typename R, typename A1, typename A2>
class FuncPtr2 {
public:
	/** Create a FunctionPointer, attaching a static function
		*
		*  @param function The static function to attach (default is none)
		*/
	FuncPtr2(R(*function)(A1, A2) = 0) {
		attach(function);
	}

		/** Create a FunctionPointer, attaching a member function
			*
			*  @param object The object pointer to invoke the member function on (i.e. the this pointer)
			*  @param function The address of the member function to attach
			*/
	template<typename T>
		FuncPtr2(T *object, R(T::*member)(A1, A2)) {
			attach(object, member);
		}

			/** Attach a static function
				*
				*  @param function The static function to attach (default is none)
				*/
	void attach(R(*function)(A1, A2)) {
		_p.function = function;
		_membercaller = 0;
	}

		/** Attach a member function
			*
			*  @param object The object pointer to invoke the member function on (i.e. the this pointer)
			*  @param function The address of the member function to attach
			*/
	template<typename T>
		void attach(T *object, R(T::*member)(A1, A2)) {
			_p.object = static_cast<void*>(object);
			*reinterpret_cast<R(T::**)(A1,A2)>(_member) = member;
			_membercaller = &FuncPtr2::membercaller<T>;
		}

			/** Call the attached static or member function
				*/
	R call(A1 a, A2 b) {
		if (_membercaller == 0 && _p.function) {
			return _p.function(a,b);
		}
		else if (_membercaller && _p.object) {
			return _membercaller(_p.object, _member, a, b);
		}
		return (R)0;
	}

		/** Get registered static function
			*/
	R(*get_function(A1,A2))() {
		return _membercaller ? (R(*)(A1,A2))0 : (R(*)(A1,A2))_p.function;
	}

#ifdef MBED_OPERATORS
	R operator ()(A1 a, A2 b) {
		return call(a,b);
	}
	operator bool(void) const {
		return (_membercaller != NULL ? _p.object : (void*)_p.function) != NULL;
	}
#endif
private:
	template<typename T>
		static R membercaller(void *object, uintptr_t *member, A1 a, A2 b) {
			T* o = static_cast<T*>(object);
			R(T::**m)(A1, A2) = reinterpret_cast<R(T::**)(A1,A2)>(member);
			return (o->**m)(a,b);
		}

	union {
		R(*function)(A1,A2); // static function pointer
		void *object;      // object this pointer
	} _p;
	uintptr_t _member[4]; // aligned raw member function pointer storage - converted back by registered _membercaller
	R(*_membercaller)(void*, uintptr_t*, A1,A2); // registered membercaller function to convert back and call _m.member on _object
};	
	


template <typename R, typename A1, typename A2, typename A3>
class FuncPtr3 {
public:
	/** Create a FunctionPointer, attaching a static function
		*
		*  @param function The static function to attach (default is none)
		*/
	FuncPtr3(R(*function)(A1, A2, A3) = 0) {
		attach(function);
	}

		/** Create a FunctionPointer, attaching a member function
			*
			*  @param object The object pointer to invoke the member function on (i.e. the this pointer)
			*  @param function The address of the member function to attach
			*/
	template<typename T>
		FuncPtr3(T *object, R(T::*member)(A1, A2, A3)) {
			attach(object, member);
		}

			/** Attach a static function
				*
				*  @param function The static function to attach (default is none)
				*/
	void attach(R(*function)(A1, A2, A3)) {
		_p.function = function;
		_membercaller = 0;
	}

		/** Attach a member function
			*
			*  @param object The object pointer to invoke the member function on (i.e. the this pointer)
			*  @param function The address of the member function to attach
			*/
	template<typename T>
		void attach(T *object, R(T::*member)(A1, A2, A3)) {
			_p.object = static_cast<void*>(object);
			*reinterpret_cast<R(T::**)(A1,A2,A3)>(_member) = member;
			_membercaller = &FuncPtr3::membercaller<T>;
		}

			/** Call the attached static or member function
				*/
	R call(A1 a, A2 b, A3 c) {
		if (_membercaller == 0 && _p.function) {
			return _p.function(a,b,c);
		}
		else if (_membercaller && _p.object) {
			return _membercaller(_p.object, _member, a, b, c);
		}
		return (R)0;
	}

		/** Get registered static function
			*/
	R(*get_function(A1,A2,A3))() {
		return _membercaller ? (R(*)(A1,A2,A3))0 : (R(*)(A1,A2,A3))_p.function;
	}

#ifdef MBED_OPERATORS
	R operator ()(A1 a, A2 b, A3 c) {
		return call(a,b,c);
	}
	operator bool(void) const {
		return (_membercaller != NULL ? _p.object : (void*)_p.function) != NULL;
	}
#endif
private:
	template<typename T>
		static R membercaller(void *object, uintptr_t *member, A1 a, A2 b, A3 c) {
			T* o = static_cast<T*>(object);
			R(T::**m)(A1, A2, A3) = reinterpret_cast<R(T::**)(A1,A2,A3)>(member);
			return (o->**m)(a,b,c);
		}

	union {
		R(*function)(A1,A2,A3); // static function pointer
		void *object;      // object this pointer
	} _p;
	uintptr_t _member[4]; // aligned raw member function pointer storage - converted back by registered _membercaller
	R(*_membercaller)(void*, uintptr_t*, A1,A2,A3); // registered membercaller function to convert back and call _m.member on _object
};	




template <typename R, typename A1, typename A2, typename A3, typename A4>
class FuncPtr4 {
public:
	/** Create a FunctionPointer, attaching a static function
		*
		*  @param function The static function to attach (default is none)
		*/
	FuncPtr4(R(*function)(A1, A2, A3, A4) = 0) {
		attach(function);
	}

		/** Create a FunctionPointer, attaching a member function
			*
			*  @param object The object pointer to invoke the member function on (i.e. the this pointer)
			*  @param function The address of the member function to attach
			*/
	template<typename T>
		FuncPtr4(T *object, R(T::*member)(A1, A2, A3, A4)) {
			attach(object, member);
		}

			/** Attach a static function
				*
				*  @param function The static function to attach (default is none)
				*/
	void attach(R(*function)(A1, A2, A3, A4)) {
		_p.function = function;
		_membercaller = 0;
	}

		/** Attach a member function
			*
			*  @param object The object pointer to invoke the member function on (i.e. the this pointer)
			*  @param function The address of the member function to attach
			*/
	template<typename T>
		void attach(T *object, R(T::*member)(A1, A2, A3, A4)) {
			_p.object = static_cast<void*>(object);
			*reinterpret_cast<R(T::**)(A1,A2,A3,A4)>(_member) = member;
			_membercaller = &FuncPtr4::membercaller<T>;
		}

			/** Call the attached static or member function
				*/
	R call(A1 a, A2 b, A3 c, A4 d) {
		if (_membercaller == 0 && _p.function) {
			return _p.function(a,b,c,d);
		}
		else if (_membercaller && _p.object) {
			return _membercaller(_p.object, _member, a, b, c,d);
		}
		return (R)0;
	}

		/** Get registered static function
			*/
	R(*get_function(A1,A2,A3,A4))() {
		return _membercaller ? (R(*)(A1,A2,A3,A4))0 : (R(*)(A1,A2,A3,A4))_p.function;
	}

#ifdef MBED_OPERATORS
	R operator ()(A1 a, A2 b, A3 c, A4 d) {
		return call(a,b,c,d);
	}
	operator bool(void) const {
		return (_membercaller != NULL ? _p.object : (void*)_p.function) != NULL;
	}
#endif
private:
	template<typename T>
		static R membercaller(void *object, uintptr_t *member, A1 a, A2 b, A3 c, A4 d) {
			T* o = static_cast<T*>(object);
			R(T::**m)(A1, A2, A3,A4) = reinterpret_cast<R(T::**)(A1,A2,A3,A4)>(member);
			return (o->**m)(a,b,c,d);
		}

	union {
		R(*function)(A1,A2,A3,A4); // static function pointer
		void *object;      // object this pointer
	} _p;
	uintptr_t _member[4]; // aligned raw member function pointer storage - converted back by registered _membercaller
	R(*_membercaller)(void*, uintptr_t*, A1,A2,A3,A4); // registered membercaller function to convert back and call _m.member on _object
};	



template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
class FuncPtr5 {
public:
	/** Create a FunctionPointer, attaching a static function
		*
		*  @param function The static function to attach (default is none)
		*/
	FuncPtr5(R(*function)(A1, A2, A3, A4, A5) = 0) {
		attach(function);
	}

		/** Create a FunctionPointer, attaching a member function
			*
			*  @param object The object pointer to invoke the member function on (i.e. the this pointer)
			*  @param function The address of the member function to attach
			*/
	template<typename T>
		FuncPtr5(T *object, R(T::*member)(A1, A2, A3, A4, A5)) {
			attach(object, member);
		}

			/** Attach a static function
				*
				*  @param function The static function to attach (default is none)
				*/
	void attach(R(*function)(A1, A2, A3, A4, A5)) {
		_p.function = function;
		_membercaller = 0;
	}

		/** Attach a member function
			*
			*  @param object The object pointer to invoke the member function on (i.e. the this pointer)
			*  @param function The address of the member function to attach
			*/
	template<typename T>
		void attach(T *object, R(T::*member)(A1, A2, A3, A4,A5)) {
			_p.object = static_cast<void*>(object);
			*reinterpret_cast<R(T::**)(A1,A2,A3,A4,A5)>(_member) = member;
			_membercaller = &FuncPtr5::membercaller<T>;
		}

			/** Call the attached static or member function
				*/
	R call(A1 a, A2 b, A3 c, A4 d, A5 e) {
		if (_membercaller == 0 && _p.function) {
			return _p.function(a,b,c,d,e);
		}
		else if (_membercaller && _p.object) {
			return _membercaller(_p.object, _member, a, b, c,d,e);
		}
		return (R)0;
	}

		/** Get registered static function
			*/
	R(*get_function(A1,A2,A3,A4,A5))() {
		return _membercaller ? (R(*)(A1,A2,A3,A4,A5))0 : (R(*)(A1,A2,A3,A4,A5))_p.function;
	}

#ifdef MBED_OPERATORS
	R operator ()(A1 a, A2 b, A3 c, A4 d, A5 e) {
		return call(a,b,c,d,e);
	}
	operator bool(void) const {
		return (_membercaller != NULL ? _p.object : (void*)_p.function) != NULL;
	}
#endif
private:
	template<typename T>
		static R membercaller(void *object, uintptr_t *member, A1 a, A2 b, A3 c, A4 d, A5 e) {
			T* o = static_cast<T*>(object);
			R(T::**m)(A1, A2, A3,A4,A5) = reinterpret_cast<R(T::**)(A1,A2,A3,A4,A5)>(member);
			return (o->**m)(a,b,c,d,e);
		}

	union {
		R(*function)(A1,A2,A3,A4,A5); // static function pointer
		void *object;      // object this pointer
	} _p;
	uintptr_t _member[4]; // aligned raw member function pointer storage - converted back by registered _membercaller
	R(*_membercaller)(void*, uintptr_t*, A1,A2,A3,A4,A5); // registered membercaller function to convert back and call _m.member on _object
};	


#endif
