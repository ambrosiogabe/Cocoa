#include "blah.h"
#include <blah2.h>

#define something 2
#undef something

namespace Cocoa
{
	struct MyData
	{
		float a;
		int* b;
		SomeTemplate<Foo, Bar> templateMagic;
	};

	USYSTEM(MyData, const TransformData, Camera, Rigidbody2D);
	namespace MyDataSystem
	{
		void Start()
		{
			if (true)
			{
				int a = 5;
			}
			else
			{
				int b = 10;
			}
		}
		void Update(MyData& myData, TransformData& transformData, Rigidbody2D& rigidbody2D);
	};

	class SomeRandomClass
	{
		float someMemberVars;
		const int& anotherMemberVar;
	};

	struct LastStructure
	{
		union
		{
			int withAUnion;
			float blah;
		};
	};
}