#include "pch.h"
#include "CppUnitTest.h"

//#include "..\GoogleTestBench\MyTestClass.h"

#include "MyTestClass.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestNamespace1
{
	TEST_CLASS(TestClass1)
	{
	public:
		
		TEST_METHOD(TestMethod1) {
			Logger::WriteMessage("** TestMethod1 ** **1** Log test case message to output");

			tt::MyTestClass t;
			Assert::AreEqual(t.TrueReturn(), true);
		}

		TEST_METHOD(TestMethod2) {
			Logger::WriteMessage("** TestMethod2 ** **2** Log test case message to output");
			tt::MyTestClass t;
			Assert::AreEqual(t.FalseReturn(), false);
		}


	};
}








namespace TestNamespace2
{
	class sameTest {
	public:
		sameTest() {}
		//string ToString() { return "BLAH"; }
		//virtual override std::string ToString() { retrun "blah"; }


		//static std::wstring ToString() { return L"blah1"; }

		//bool operator == (const sameTest st) { return true; }

		//static std::wstring ToString(const sameTest& t) { return L"fsdfsd"; }
	};


	//template <> inline std::wstring ToString<class sameTest>(const sameTest& x) {
	//	return L"blah1";
	//}
	//template <> inline std::wstring ToString<class sameTest>(const sameTest* y) {
	//	return L"blah2";
	//}
	//template <> inline std::wstring ToString<class sameTest>(sameTest* z) {
	//	return L"blah3";
	//}


	//stemplate <sameTest> static std::wstring ToString(const sameTest& q) { rturn "BLAH1"; }
	//template <class sameTest> static std::wstring ToString(const sameTest& x) { 
	//	return L"blah1"; 
	//}
	//template <class sameTest> static std::wstring ToString(const sameTest* y) { 
	//	return L"blah2"; 
	//}
	//template <class sameTest> static std::wstring ToString(sameTest* z) { 
	//	return L"blah3"; 
	//}


	//template <> 
	//static std::wstring ToString<class sameTest>(const sameTest& x) {
	//	return L"blah1";
	//}
	//template <class sameTest> static std::wstring ToString(const sameTest* y) {
	//	return L"blah2";
	//}
	//template <class sameTest> static std::wstring ToString(sameTest* z) {
	//	return L"blah3";
	//}



	class diffTest {
	public:
		diffTest() {}
	};


	TEST_CLASS(TestClass2)
	{
	public:

		TEST_METHOD(TestMethod21) {
			Logger::WriteMessage("** TestMethod21 ** **1** Log test case message to output");

			tt::MyTestClass t;
			Assert::AreEqual(t.TrueReturn(), true);
		}

		TEST_METHOD(TestMethod22) {
			Logger::WriteMessage("** TestMethod22 ** **2** Log test case message to output");
			tt::MyTestClass t;
			Assert::AreEqual(t.FalseReturn(), false);
		}

		TEST_METHOD(sameTestCase) {
			Logger::WriteMessage("** sameTestCase ** **2** Log test case message to output");

			sameTest t1;
			sameTest t2;
			//wchar_t x[50];


			//__LineInfo li(x,"YY",22);

			Assert::AreSame(t1, t1, L"BLAH ti blah");
			//Assert::AreSame(t1, t1, "Blah", &li);


			Assert::AreNotSame(t1, t2);
			//Assert::AreEqual(t1, t2);
		}

		TEST_METHOD(sameTestCaseWithDiff) {
			Logger::WriteMessage("** sameTestCaseWithDiff ** **2** Log test case message to output");

			sameTest t1;
			sameTest t2;
			//wchar_t x[50];


			//__LineInfo li(x,"YY",22);

			//Assert::AreSame(t1, t1, L"BLAH ti blah");
			//Assert::AreSame(t1, t1, "Blah", &li);


			Assert::AreSame(t1, t2);
			//Assert::AreNotSame(t1, t2);
			//Assert::AreEqual(t1, t2);
		}



	};
}

// Need to follow up. Override of template compare for specific class to run Assert::AreSame 
// Documentation said it was to check if same instance 
// Thought this would be marshling for string comparison. However, the string generated 
// is just for the Expected<str> Actual<str> text for str
namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {

			template <> inline std::wstring ToString<class TestNamespace2::sameTest>(const TestNamespace2::sameTest& x) {
				return L"blah1";
			}
			template <> inline std::wstring ToString<class TestNamespace2::sameTest>(const TestNamespace2::sameTest* y) {
				return L"blah2";
			}
			template <> inline std::wstring ToString<class TestNamespace2::sameTest>(TestNamespace2::sameTest* z) {
				return L"blah3";
			}


			/*			template <class sameTest> static std::wstring ToString(const sameTest& x) {
							return L"blah1";
						}
						template <class sameTest> static std::wstring ToString(const sameTest* y) {
							return L"blah2";
						}
						template <class sameTest> static std::wstring ToString(sameTest* z) {
							return L"blah3";
					}
				*/


		}
	}
}
