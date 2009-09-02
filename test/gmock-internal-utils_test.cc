// Copyright 2007, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: wan@google.com (Zhanyong Wan)

// Google Mock - a framework for writing C++ mock classes.
//
// This file tests the internal utilities.

#include <gmock/internal/gmock-internal-utils.h>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <gmock/gmock.h>
#include <gmock/internal/gmock-port.h>
#include <gtest/gtest.h>
#include <gtest/gtest-spi.h>

namespace testing {
namespace internal {

namespace {

using ::std::tr1::tuple;

TEST(ConvertIdentifierNameToWordsTest, WorksWhenNameContainsNoWord) {
  EXPECT_EQ("", ConvertIdentifierNameToWords(""));
  EXPECT_EQ("", ConvertIdentifierNameToWords("_"));
  EXPECT_EQ("", ConvertIdentifierNameToWords("__"));
}

TEST(ConvertIdentifierNameToWordsTest, WorksWhenNameContainsDigits) {
  EXPECT_EQ("1", ConvertIdentifierNameToWords("_1"));
  EXPECT_EQ("2", ConvertIdentifierNameToWords("2_"));
  EXPECT_EQ("34", ConvertIdentifierNameToWords("_34_"));
  EXPECT_EQ("34 56", ConvertIdentifierNameToWords("_34_56"));
}

TEST(ConvertIdentifierNameToWordsTest, WorksWhenNameContainsCamelCaseWords) {
  EXPECT_EQ("a big word", ConvertIdentifierNameToWords("ABigWord"));
  EXPECT_EQ("foo bar", ConvertIdentifierNameToWords("FooBar"));
  EXPECT_EQ("foo", ConvertIdentifierNameToWords("Foo_"));
  EXPECT_EQ("foo bar", ConvertIdentifierNameToWords("_Foo_Bar_"));
  EXPECT_EQ("foo and bar", ConvertIdentifierNameToWords("_Foo__And_Bar"));
}

TEST(ConvertIdentifierNameToWordsTest, WorksWhenNameContains_SeparatedWords) {
  EXPECT_EQ("foo bar", ConvertIdentifierNameToWords("foo_bar"));
  EXPECT_EQ("foo", ConvertIdentifierNameToWords("_foo_"));
  EXPECT_EQ("foo bar", ConvertIdentifierNameToWords("_foo_bar_"));
  EXPECT_EQ("foo and bar", ConvertIdentifierNameToWords("_foo__and_bar"));
}

TEST(ConvertIdentifierNameToWordsTest, WorksWhenNameIsMixture) {
  EXPECT_EQ("foo bar 123", ConvertIdentifierNameToWords("Foo_bar123"));
  EXPECT_EQ("chapter 11 section 1",
            ConvertIdentifierNameToWords("_Chapter11Section_1_"));
}

// Tests that CompileAssertTypesEqual compiles when the type arguments are
// equal.
TEST(CompileAssertTypesEqual, CompilesWhenTypesAreEqual) {
  CompileAssertTypesEqual<void, void>();
  CompileAssertTypesEqual<int*, int*>();
}

// Tests that RemoveReference does not affect non-reference types.
TEST(RemoveReferenceTest, DoesNotAffectNonReferenceType) {
  CompileAssertTypesEqual<int, RemoveReference<int>::type>();
  CompileAssertTypesEqual<const char, RemoveReference<const char>::type>();
}

// Tests that RemoveReference removes reference from reference types.
TEST(RemoveReferenceTest, RemovesReference) {
  CompileAssertTypesEqual<int, RemoveReference<int&>::type>();
  CompileAssertTypesEqual<const char, RemoveReference<const char&>::type>();
}

// Tests GMOCK_REMOVE_REFERENCE_.

template <typename T1, typename T2>
void TestGMockRemoveReference() {
  CompileAssertTypesEqual<T1, GMOCK_REMOVE_REFERENCE_(T2)>();
}

TEST(RemoveReferenceTest, MacroVersion) {
  TestGMockRemoveReference<int, int>();
  TestGMockRemoveReference<const char, const char&>();
}


// Tests that RemoveConst does not affect non-const types.
TEST(RemoveConstTest, DoesNotAffectNonConstType) {
  CompileAssertTypesEqual<int, RemoveConst<int>::type>();
  CompileAssertTypesEqual<char&, RemoveConst<char&>::type>();
}

// Tests that RemoveConst removes const from const types.
TEST(RemoveConstTest, RemovesConst) {
  CompileAssertTypesEqual<int, RemoveConst<const int>::type>();
}

// Tests GMOCK_REMOVE_CONST_.

template <typename T1, typename T2>
void TestGMockRemoveConst() {
  CompileAssertTypesEqual<T1, GMOCK_REMOVE_CONST_(T2)>();
}

TEST(RemoveConstTest, MacroVersion) {
  TestGMockRemoveConst<int, int>();
  TestGMockRemoveConst<double&, double&>();
  TestGMockRemoveConst<char, const char>();
}

// Tests that AddReference does not affect reference types.
TEST(AddReferenceTest, DoesNotAffectReferenceType) {
  CompileAssertTypesEqual<int&, AddReference<int&>::type>();
  CompileAssertTypesEqual<const char&, AddReference<const char&>::type>();
}

// Tests that AddReference adds reference to non-reference types.
TEST(AddReferenceTest, AddsReference) {
  CompileAssertTypesEqual<int&, AddReference<int>::type>();
  CompileAssertTypesEqual<const char&, AddReference<const char>::type>();
}

// Tests GMOCK_ADD_REFERENCE_.

template <typename T1, typename T2>
void TestGMockAddReference() {
  CompileAssertTypesEqual<T1, GMOCK_ADD_REFERENCE_(T2)>();
}

TEST(AddReferenceTest, MacroVersion) {
  TestGMockAddReference<int&, int>();
  TestGMockAddReference<const char&, const char&>();
}

// Tests GMOCK_REFERENCE_TO_CONST_.

template <typename T1, typename T2>
void TestGMockReferenceToConst() {
  CompileAssertTypesEqual<T1, GMOCK_REFERENCE_TO_CONST_(T2)>();
}

TEST(GMockReferenceToConstTest, Works) {
  TestGMockReferenceToConst<const char&, char>();
  TestGMockReferenceToConst<const int&, const int>();
  TestGMockReferenceToConst<const double&, double>();
  TestGMockReferenceToConst<const string&, const string&>();
}

TEST(PointeeOfTest, WorksForSmartPointers) {
  CompileAssertTypesEqual<const char,
      PointeeOf<internal::linked_ptr<const char> >::type>();
}

TEST(PointeeOfTest, WorksForRawPointers) {
  CompileAssertTypesEqual<int, PointeeOf<int*>::type>();
  CompileAssertTypesEqual<const char, PointeeOf<const char*>::type>();
  CompileAssertTypesEqual<void, PointeeOf<void*>::type>();
}

TEST(GetRawPointerTest, WorksForSmartPointers) {
  const char* const raw_p4 = new const char('a');  // NOLINT
  const internal::linked_ptr<const char> p4(raw_p4);
  EXPECT_EQ(raw_p4, GetRawPointer(p4));
}

TEST(GetRawPointerTest, WorksForRawPointers) {
  int* p = NULL;
  EXPECT_EQ(NULL, GetRawPointer(p));
  int n = 1;
  EXPECT_EQ(&n, GetRawPointer(&n));
}

class Base {};
class Derived : public Base {};

// Tests that ImplicitlyConvertible<T1, T2>::value is a compile-time constant.
TEST(ImplicitlyConvertibleTest, ValueIsCompileTimeConstant) {
  GMOCK_COMPILE_ASSERT_((ImplicitlyConvertible<int, int>::value), const_true);
  GMOCK_COMPILE_ASSERT_((!ImplicitlyConvertible<void*, int*>::value),
                        const_false);
}

// Tests that ImplicitlyConvertible<T1, T2>::value is true when T1 can
// be implicitly converted to T2.
TEST(ImplicitlyConvertibleTest, ValueIsTrueWhenConvertible) {
  EXPECT_TRUE((ImplicitlyConvertible<int, double>::value));
  EXPECT_TRUE((ImplicitlyConvertible<double, int>::value));
  EXPECT_TRUE((ImplicitlyConvertible<int*, void*>::value));
  EXPECT_TRUE((ImplicitlyConvertible<int*, const int*>::value));
  EXPECT_TRUE((ImplicitlyConvertible<Derived&, const Base&>::value));
  EXPECT_TRUE((ImplicitlyConvertible<const Base, Base>::value));
}

// Tests that ImplicitlyConvertible<T1, T2>::value is false when T1
// cannot be implicitly converted to T2.
TEST(ImplicitlyConvertibleTest, ValueIsFalseWhenNotConvertible) {
  EXPECT_FALSE((ImplicitlyConvertible<double, int*>::value));
  EXPECT_FALSE((ImplicitlyConvertible<void*, int*>::value));
  EXPECT_FALSE((ImplicitlyConvertible<const int*, int*>::value));
  EXPECT_FALSE((ImplicitlyConvertible<Base&, Derived&>::value));
}

// Tests that IsAProtocolMessage<T>::value is a compile-time constant.
TEST(IsAProtocolMessageTest, ValueIsCompileTimeConstant) {
  GMOCK_COMPILE_ASSERT_(IsAProtocolMessage<ProtocolMessage>::value, const_true);
  GMOCK_COMPILE_ASSERT_(!IsAProtocolMessage<int>::value, const_false);
}

// Tests that IsAProtocolMessage<T>::value is true when T is
// ProtocolMessage or a sub-class of it.
TEST(IsAProtocolMessageTest, ValueIsTrueWhenTypeIsAProtocolMessage) {
  EXPECT_TRUE(IsAProtocolMessage<ProtocolMessage>::value);
#if GMOCK_HAS_PROTOBUF_
  EXPECT_TRUE(IsAProtocolMessage<const TestMessage>::value);
#endif  // GMOCK_HAS_PROTOBUF_
}

// Tests that IsAProtocolMessage<T>::value is false when T is neither
// ProtocolMessage nor a sub-class of it.
TEST(IsAProtocolMessageTest, ValueIsFalseWhenTypeIsNotAProtocolMessage) {
  EXPECT_FALSE(IsAProtocolMessage<int>::value);
  EXPECT_FALSE(IsAProtocolMessage<const Base>::value);
}

// Tests IsContainerTest.

class NonContainer {};

TEST(IsContainerTestTest, WorksForNonContainer) {
  EXPECT_EQ(sizeof(IsNotContainer), sizeof(IsContainerTest<int>(0)));
  EXPECT_EQ(sizeof(IsNotContainer), sizeof(IsContainerTest<char[5]>(0)));
  EXPECT_EQ(sizeof(IsNotContainer), sizeof(IsContainerTest<NonContainer>(0)));
}

TEST(IsContainerTestTest, WorksForContainer) {
  EXPECT_EQ(sizeof(IsContainer), sizeof(IsContainerTest<std::vector<bool> >(0)));
  EXPECT_EQ(sizeof(IsContainer), sizeof(IsContainerTest<std::map<int, double> >(0)));
}

// Tests the TupleMatches() template function.

TEST(TupleMatchesTest, WorksForSize0) {
  tuple<> matchers;
  tuple<> values;

  EXPECT_TRUE(TupleMatches(matchers, values));
}

TEST(TupleMatchesTest, WorksForSize1) {
  tuple<Matcher<int> > matchers(Eq(1));
  tuple<int> values1(1),
      values2(2);

  EXPECT_TRUE(TupleMatches(matchers, values1));
  EXPECT_FALSE(TupleMatches(matchers, values2));
}

TEST(TupleMatchesTest, WorksForSize2) {
  tuple<Matcher<int>, Matcher<char> > matchers(Eq(1), Eq('a'));
  tuple<int, char> values1(1, 'a'),
      values2(1, 'b'),
      values3(2, 'a'),
      values4(2, 'b');

  EXPECT_TRUE(TupleMatches(matchers, values1));
  EXPECT_FALSE(TupleMatches(matchers, values2));
  EXPECT_FALSE(TupleMatches(matchers, values3));
  EXPECT_FALSE(TupleMatches(matchers, values4));
}

TEST(TupleMatchesTest, WorksForSize5) {
  tuple<Matcher<int>, Matcher<char>, Matcher<bool>, Matcher<long>,  // NOLINT
      Matcher<string> >
      matchers(Eq(1), Eq('a'), Eq(true), Eq(2L), Eq("hi"));
  tuple<int, char, bool, long, string>  // NOLINT
      values1(1, 'a', true, 2L, "hi"),
      values2(1, 'a', true, 2L, "hello"),
      values3(2, 'a', true, 2L, "hi");

  EXPECT_TRUE(TupleMatches(matchers, values1));
  EXPECT_FALSE(TupleMatches(matchers, values2));
  EXPECT_FALSE(TupleMatches(matchers, values3));
}

// Tests that Assert(true, ...) succeeds.
TEST(AssertTest, SucceedsOnTrue) {
  Assert(true, __FILE__, __LINE__, "This should succeed.");
  Assert(true, __FILE__, __LINE__);  // This should succeed too.
}

#if GTEST_HAS_DEATH_TEST

// Tests that Assert(false, ...) generates a fatal failure.
TEST(AssertTest, FailsFatallyOnFalse) {
  EXPECT_DEATH({  // NOLINT
    Assert(false, __FILE__, __LINE__, "This should fail.");
  }, "");

  EXPECT_DEATH({  // NOLINT
    Assert(false, __FILE__, __LINE__);
  }, "");
}

#endif  // GTEST_HAS_DEATH_TEST

// Tests that Expect(true, ...) succeeds.
TEST(ExpectTest, SucceedsOnTrue) {
  Expect(true, __FILE__, __LINE__, "This should succeed.");
  Expect(true, __FILE__, __LINE__);  // This should succeed too.
}

// Tests that Expect(false, ...) generates a non-fatal failure.
TEST(ExpectTest, FailsNonfatallyOnFalse) {
  EXPECT_NONFATAL_FAILURE({  // NOLINT
    Expect(false, __FILE__, __LINE__, "This should fail.");
  }, "This should fail");

  EXPECT_NONFATAL_FAILURE({  // NOLINT
    Expect(false, __FILE__, __LINE__);
  }, "Expectation failed");
}

// TODO(wan@google.com): find a way to re-enable these tests.
#if 0

// Tests the Log() function.

// Verifies that Log() behaves correctly for the given verbosity level
// and log severity.
void TestLogWithSeverity(const string& verbosity, LogSeverity severity,
                         bool should_print) {
  const string old_flag = GMOCK_FLAG(verbose);
  GMOCK_FLAG(verbose) = verbosity;
  CaptureTestStdout();
  Log(severity, "Test log.\n", 0);
  if (should_print) {
    EXPECT_PRED2(RE::FullMatch,
                 GetCapturedTestStdout(),
                 severity == WARNING ?
                 "\nGMOCK WARNING:\nTest log\\.\nStack trace:\n[\\s\\S]*" :
                 "\nTest log\\.\nStack trace:\n[\\s\\S]*");
  } else {
    EXPECT_EQ("", GetCapturedTestStdout());
  }
  GMOCK_FLAG(verbose) = old_flag;
}

// Tests that when the stack_frames_to_skip parameter is negative,
// Log() doesn't include the stack trace in the output.
TEST(LogTest, NoStackTraceWhenStackFramesToSkipIsNegative) {
  GMOCK_FLAG(verbose) = kInfoVerbosity;
  CaptureTestStdout();
  Log(INFO, "Test log.\n", -1);
  EXPECT_EQ("\nTest log.\n", GetCapturedTestStdout());
}

// Tests that in opt mode, a positive stack_frames_to_skip argument is
// treated as 0.
TEST(LogTest, NoSkippingStackFrameInOptMode) {
  CaptureTestStdout();
  Log(WARNING, "Test log.\n", 100);
  const string log = GetCapturedTestStdout();
#ifdef NDEBUG
  // In opt mode, no stack frame should be skipped.
  EXPECT_THAT(log, ContainsRegex("\nGMOCK WARNING:\n"
                                 "Test log\\.\n"
                                 "Stack trace:\n"
                                 ".+"));
#else
  // In dbg mode, the stack frames should be skipped.
  EXPECT_EQ("\nGMOCK WARNING:\n"
            "Test log.\n"
            "Stack trace:\n", log);
#endif  // NDEBUG
}

// Tests that all logs are printed when the value of the
// --gmock_verbose flag is "info".
TEST(LogTest, AllLogsArePrintedWhenVerbosityIsInfo) {
  TestLogWithSeverity(kInfoVerbosity, INFO, true);
  TestLogWithSeverity(kInfoVerbosity, WARNING, true);
}

// Tests that only warnings are printed when the value of the
// --gmock_verbose flag is "warning".
TEST(LogTest, OnlyWarningsArePrintedWhenVerbosityIsWarning) {
  TestLogWithSeverity(kWarningVerbosity, INFO, false);
  TestLogWithSeverity(kWarningVerbosity, WARNING, true);
}

// Tests that no logs are printed when the value of the
// --gmock_verbose flag is "error".
TEST(LogTest, NoLogsArePrintedWhenVerbosityIsError) {
  TestLogWithSeverity(kErrorVerbosity, INFO, false);
  TestLogWithSeverity(kErrorVerbosity, WARNING, false);
}

// Tests that only warnings are printed when the value of the
// --gmock_verbose flag is invalid.
TEST(LogTest, OnlyWarningsArePrintedWhenVerbosityIsInvalid) {
  TestLogWithSeverity("invalid", INFO, false);
  TestLogWithSeverity("invalid", WARNING, true);
}

#endif  // 0

TEST(TypeTraitsTest, true_type) {
  EXPECT_TRUE(true_type::value);
}

TEST(TypeTraitsTest, false_type) {
  EXPECT_FALSE(false_type::value);
}

TEST(TypeTraitsTest, is_reference) {
  EXPECT_FALSE(is_reference<int>::value);
  EXPECT_FALSE(is_reference<char*>::value);
  EXPECT_TRUE(is_reference<const int&>::value);
}

TEST(TypeTraitsTest, is_pointer) {
  EXPECT_FALSE(is_pointer<int>::value);
  EXPECT_FALSE(is_pointer<char&>::value);
  EXPECT_TRUE(is_pointer<const int*>::value);
}

TEST(TypeTraitsTest, type_equals) {
  EXPECT_FALSE((type_equals<int, const int>::value));
  EXPECT_FALSE((type_equals<int, int&>::value));
  EXPECT_FALSE((type_equals<int, double>::value));
  EXPECT_TRUE((type_equals<char, char>::value));
}

TEST(TypeTraitsTest, remove_reference) {
  EXPECT_TRUE((type_equals<char, remove_reference<char&>::type>::value));
  EXPECT_TRUE((type_equals<const int,
               remove_reference<const int&>::type>::value));
  EXPECT_TRUE((type_equals<int, remove_reference<int>::type>::value));
  EXPECT_TRUE((type_equals<double*, remove_reference<double*>::type>::value));
}

// TODO(wan@google.com): find a way to re-enable these tests.
#if 0

// Verifies that Log() behaves correctly for the given verbosity level
// and log severity.
string GrabOutput(void(*logger)(), const char* verbosity) {
  const string saved_flag = GMOCK_FLAG(verbose);
  GMOCK_FLAG(verbose) = verbosity;
  CaptureTestStdout();
  logger();
  GMOCK_FLAG(verbose) = saved_flag;
  return GetCapturedTestStdout();
}

class DummyMock {
 public:
  MOCK_METHOD0(TestMethod, void());
  MOCK_METHOD1(TestMethodArg, void(int dummy));
};

void ExpectCallLogger() {
  DummyMock mock;
  EXPECT_CALL(mock, TestMethod());
  mock.TestMethod();
};

// Verifies that EXPECT_CALL logs if the --gmock_verbose flag is set to "info".
TEST(ExpectCallTest, LogsWhenVerbosityIsInfo) {
  EXPECT_THAT(GrabOutput(ExpectCallLogger, kInfoVerbosity),
              HasSubstr("EXPECT_CALL(mock, TestMethod())"));
}

// Verifies that EXPECT_CALL doesn't log
// if the --gmock_verbose flag is set to "warning".
TEST(ExpectCallTest, DoesNotLogWhenVerbosityIsWarning) {
  EXPECT_EQ("", GrabOutput(ExpectCallLogger, kWarningVerbosity));
}

// Verifies that EXPECT_CALL doesn't log
// if the --gmock_verbose flag is set to "error".
TEST(ExpectCallTest,  DoesNotLogWhenVerbosityIsError) {
  EXPECT_EQ("", GrabOutput(ExpectCallLogger, kErrorVerbosity));
}

void OnCallLogger() {
  DummyMock mock;
  ON_CALL(mock, TestMethod());
};

// Verifies that ON_CALL logs if the --gmock_verbose flag is set to "info".
TEST(OnCallTest, LogsWhenVerbosityIsInfo) {
  EXPECT_THAT(GrabOutput(OnCallLogger, kInfoVerbosity),
              HasSubstr("ON_CALL(mock, TestMethod())"));
}

// Verifies that ON_CALL doesn't log
// if the --gmock_verbose flag is set to "warning".
TEST(OnCallTest, DoesNotLogWhenVerbosityIsWarning) {
  EXPECT_EQ("", GrabOutput(OnCallLogger, kWarningVerbosity));
}

// Verifies that ON_CALL doesn't log if
// the --gmock_verbose flag is set to "error".
TEST(OnCallTest, DoesNotLogWhenVerbosityIsError) {
  EXPECT_EQ("", GrabOutput(OnCallLogger, kErrorVerbosity));
}

void OnCallAnyArgumentLogger() {
  DummyMock mock;
  ON_CALL(mock, TestMethodArg(_));
}

// Verifies that ON_CALL prints provided _ argument.
TEST(OnCallTest, LogsAnythingArgument) {
  EXPECT_THAT(GrabOutput(OnCallAnyArgumentLogger, kInfoVerbosity),
              HasSubstr("ON_CALL(mock, TestMethodArg(_)"));
}

#endif  // 0

}  // namespace
}  // namespace internal
}  // namespace testing
