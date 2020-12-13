using System.Collections.Generic;

namespace Reflection
{
	public class UClass
	{
		public string ClassName { get; set; }

		public List<UVariable> Variables { get; set; }

		public string FullPathToFile { get; set; }
	}
}
