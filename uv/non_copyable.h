#ifndef LIBRACPP_NOCOPYABLE_H
#define LIBRACPP_NOCOPYABLE_H

namespace Libra {

	class NonCopyable
	{
	protected:
		NonCopyable() {}
		virtual ~NonCopyable() {}
	private:
		NonCopyable(const NonCopyable&) = delete;
		const NonCopyable operator =(const NonCopyable) = delete;
	};
}



#endif