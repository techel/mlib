#pragma once

#include <cstddef>

namespace mlib::stream
{

//
// sink interface
//

template<class T>
struct IBasicSink
{
	virtual ~IBasicSink() = default;
	virtual void writeSink(const T *source, size_t len) = 0;
};

using ISink = IBasicSink<char>;

//
// source interface
//

template<class T>
struct IBasicSource
{
	virtual ~IBasicSource() = default;
	virtual size_t readSource(T *dest, size_t len) = 0;
};

using ISource = IBasicSource<char>;

}
