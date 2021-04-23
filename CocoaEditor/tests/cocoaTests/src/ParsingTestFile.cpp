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
		void Start();
		void Update(MyData& myData, TransformData& transformData, Rigidbody2D& rigidbody2D);
	};
}