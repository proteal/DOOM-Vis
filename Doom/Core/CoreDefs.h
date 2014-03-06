//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

#define DECL_PIMPL(className) \
class className \
{ \
public: \
class Impl; \
private: \
	unique_ptr<Impl> impl; \
public: \
	explicit className(unique_ptr<Impl>& _impl) \
		:impl(move(_impl)) \
	{ } \
	Impl& GetImpl() { assert(impl); return *impl; }
