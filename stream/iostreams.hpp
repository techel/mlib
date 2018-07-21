#pragma once

#include <streambuf>
#include <vector>
#include <algorithm>
#include <functional>
#include <cassert>
#include <cstddef>
#include <cstring>

namespace mlib::stream
{

template<class T>
class IBasicStreambuf : public std::basic_streambuf<T>
{
public:
    virtual size_t readBuffer(T *dst, size_t size)
    {
        assert(false);
        return 0;
    }
	virtual void writeBuffer(const T *src, size_t size)
    {
        assert(false);
    }

private:
	const size_t PutBack;
	std::vector<T> InputBuffer;
	std::vector<T> OutputBuffer;

public:
	IBasicStreambuf(size_t inbuffsz = 256, size_t outbuffsz = 256, size_t putbacksz = 1)
		: PutBack(std::max(putbacksz, size_t(1))), InputBuffer(std::max(inbuffsz, PutBack) + PutBack), OutputBuffer(outbuffsz + 1)
	{
		T *base = &OutputBuffer.front();
		std::basic_streambuf<T>::setp(base, base + OutputBuffer.size() - 1);

		T *end = &InputBuffer.front() + InputBuffer.size();
		std::basic_streambuf<T>::setg(end, end, end);
	}

	typename std::basic_streambuf<T>::int_type overflow(typename std::basic_streambuf<T>::int_type ch)
	{
		if(ch != std::basic_streambuf<T>::traits_type::eof())
		{
			assert(std::less_equal<T*>()(std::basic_streambuf<T>::pptr(), std::basic_streambuf<T>::epptr()));
			*std::basic_streambuf<T>::pptr() = ch;
			std::basic_streambuf<T>::pbump(1);
			sync();
		}
		return ch;
	}

	int sync()
	{
		ptrdiff_t n = std::basic_streambuf<T>::pptr() - std::basic_streambuf<T>::pbase();
		std::basic_streambuf<T>::pbump(-n);
		writeBuffer(std::basic_streambuf<T>::pbase(), static_cast<size_t>(n));
		return 0;
	}

	typename std::basic_streambuf<T>::int_type underflow()
	{
		if(std::basic_streambuf<T>::gptr() < std::basic_streambuf<T>::egptr())
			return std::basic_streambuf<T>::traits_type::to_int_type(*std::basic_streambuf<T>::gptr());

		T *base = &InputBuffer.front();
		T *start = base;

		if(std::basic_streambuf<T>::eback() == base)
		{
			std::memmove(base, std::basic_streambuf<T>::egptr() - PutBack, PutBack);
			start += PutBack;
		}

        auto n = static_cast<std::streamsize>(readBuffer(start, InputBuffer.size() - (start - base)));
		if(n == 0)
			return std::basic_streambuf<T>::traits_type::eof();

		std::basic_streambuf<T>::setg(base, start, start + n);
		return std::basic_streambuf<T>::traits_type::to_int_type(*std::basic_streambuf<T>::gptr());
	}
};

typedef IBasicStreambuf<char> IStreambuf;

}
