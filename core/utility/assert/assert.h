#pragma once
#include <Config.h>


namespace samurai
{

	namespace assert
	{

		//arguments don't do anything here
		inline void terminate(...);

		void assertFunctionDevelopment(
			const char *expression, 
			const char *file,
			int line, 
			const char *comment = nullptr);

		void assertFunctionProduction
		(
			const char *expression,
			const char *file,
			int line,
			const char *comment = nullptr
		);

		void assertFunctionToLog(
			const char *expression,
			const char *file,
			int line,
			const char *comment = nullptr);

	}

}



#define SAMURAI_ASSERT(expression, comment) (void)(			\
			(!!(expression)) ||									\
			(SAMURAI_INTERNAL_CURRENT_ASSERT_FUNCTION(#expression,	\
				__FILE__, __LINE__, comment), 0)				\
)


#ifdef SAMURAI_DEVELOPMENT

#define SAMURAI_DEVELOPMENT_ONLY_ASSERT(expression, comment) (void)(			\
			(!!(expression)) ||												\
			(samurai::assert::assertFunctionDevelopment(#expression,			\
				__FILE__, __LINE__, comment), 0)							\
)


#endif
