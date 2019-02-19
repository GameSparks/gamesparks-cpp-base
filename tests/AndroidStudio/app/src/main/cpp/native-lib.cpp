#include <jni.h>
#include <string>
#include <gtest/gtest.h>
#include <mutex>
#include <queue>
#include <thread>

enum TextColour {
    COLOR_WHITE = 0,
    COLOR_GREEN = 1,
    COLOR_YELLOW = 2,
    COLOR_RED = 3,
    COLOR_GREY = 4,
};

struct LogEntry
{
    LogEntry(TextColour textColour, const std::string& msg) : textColour(textColour), msg(msg) {}

    TextColour textColour = COLOR_WHITE;
    std::string msg;
};

typedef std::queue<LogEntry> LogEntries;
static LogEntries logEntries;
static std::mutex LogEntriesMutex;

static void pushLogEntry(const LogEntry& le)
{
    std::lock_guard<std::mutex> lock(LogEntriesMutex);
    logEntries.push(le);
}

static bool popLogEntry(LogEntry& le)
{
    std::lock_guard<std::mutex> lock(LogEntriesMutex);
    if(logEntries.empty())
        return false;
    le = logEntries.front();
    logEntries.pop();
    return true;
}

static LogEntry le(COLOR_WHITE, "");


extern "C"
JNIEXPORT jstring JNICALL
Java_com_gamesparks_GameSparksTests_MainActivity_getNextLogEntry(
        JNIEnv *env,
        jobject /* this */) {

    if(popLogEntry(le))
    {
        return env->NewStringUTF(le.msg.c_str());
    }
    else
    {
        return env->NewStringUTF("");
    }
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_gamesparks_GameSparksTests_MainActivity_getLastLogEntryColor(
        JNIEnv *env,
        jobject /* this */) {

    return jint(le.textColour);
}


using namespace testing;

static const char kUniversalFilter[] = "*";
static const char kTypeParamLabel[] = "TypeParam";
static const char kValueParamLabel[] = "GetParam()";


class PrettyUnitTestResultPrinter : public TestEventListener {

    static void ColoredPrintf(TextColour colour, const char *fmt, ...) {
        static std::vector<char> buffer(1024, '\0');

        va_list myargs;

        /* Initialise the va_list variable with the ... after fmt */

        va_start(myargs, fmt);

        /* Forward the '...' to vprintf */
        int count = vsnprintf(buffer.data(), buffer.size(), fmt, myargs);

        /* Clean up the va_list */
        va_end(myargs);

        if(count > 0)
        {
            std::string str( buffer.begin(), buffer.begin() + count );
            pushLogEntry({colour, str});
        }
        else
        {
            pushLogEntry({colour, "vsnprintf error"});
        }
    }

    // Formats a countable noun.  Depending on its quantity, either the
    // singular form or the plural form is used. e.g.
    //
    // FormatCountableNoun(1, "formula", "formuli") returns "1 formula".
    // FormatCountableNoun(5, "book", "books") returns "5 books".
    static std::string FormatCountableNoun(int count,
                                           const char *singular_form,
                                           const char *plural_form) {
        return internal::StreamableToString(count) + " " +
               (count == 1 ? singular_form : plural_form);
    }

    // Formats the count of tests.
    static std::string FormatTestCount(int test_count) {
        return FormatCountableNoun(test_count, "test", "tests");
    }

    // Formats the count of test cases.
    static std::string FormatTestCaseCount(int test_case_count) {
        return FormatCountableNoun(test_case_count, "test case", "test cases");
    }

    static void PrintFullTestCommentIfPresent(const TestInfo& test_info) {
        const char* const type_param = test_info.type_param();
        const char* const value_param = test_info.value_param();

        if (type_param != NULL || value_param != NULL) {
            ColoredPrintf(COLOR_WHITE, ", where ");
            if (type_param != NULL) {
                ColoredPrintf(COLOR_WHITE, "%s = %s", kTypeParamLabel, type_param);
                if (value_param != NULL)
                    ColoredPrintf(COLOR_WHITE, " and ");
            }
            if (value_param != NULL) {
                ColoredPrintf(COLOR_WHITE, "%s = %s", kValueParamLabel, value_param);
            }
        }
    }

    // Converts a TestPartResult::Type enum to human-friendly string
// representation.  Both kNonFatalFailure and kFatalFailure are translated
// to "Failure", as the user usually doesn't care about the difference
// between the two when viewing the test result.
    static const char *TestPartResultTypeToString(TestPartResult::Type type) {
        switch (type) {
            case TestPartResult::kSuccess:
                return "Success";

            case TestPartResult::kNonFatalFailure:
            case TestPartResult::kFatalFailure:
#ifdef _MSC_VER
                return "error: ";
#else
                return "Failure\n";
#endif
            default:
                return "Unknown result type";
        }
    }

    // Prints a TestPartResult to an std::string.
    static std::string PrintTestPartResultToString(
            const TestPartResult &test_part_result) {
        return (Message()
                << internal::FormatFileLocation(test_part_result.file_name(),
                                                test_part_result.line_number())
                << " " << TestPartResultTypeToString(test_part_result.type())
                << test_part_result.message()).GetString();
    }

    // Prints a TestPartResult.
    static void PrintTestPartResult(const TestPartResult &test_part_result) {
        const std::string &result =
                PrintTestPartResultToString(test_part_result);
        ColoredPrintf(COLOR_WHITE, "%s\n", result.c_str());
        // If the test program runs in Visual Studio or a debugger, the
        // following statements add the test part result message to the Output
        // window such that the user can double-click on it to jump to the
        // corresponding source code location; otherwise they do nothing.
#if GTEST_OS_WINDOWS && !GTEST_OS_WINDOWS_MOBILE
        // We don't call OutputDebugString*() on Windows Mobile, as printing
  // to stdout is done by OutputDebugString() there already - we don't
  // want the same message printed twice.
  ::OutputDebugStringA(result.c_str());
  ::OutputDebugStringA("\n");
#endif
    }

    bool CStringEquals(const char *lhs, const char *rhs) {
        if (lhs == NULL) return rhs == NULL;

        if (rhs == NULL) return false;

        return strcmp(lhs, rhs) == 0;
    }

public:
    PrettyUnitTestResultPrinter() {}

    static void PrintTestName(const char *test_case, const char *test) {
        ColoredPrintf(COLOR_WHITE, "%s.%s", test_case, test);
    }

    // The following methods override what's in the TestEventListener class.
    virtual void OnTestProgramStart(const UnitTest & /*unit_test*/) {}

    virtual void OnTestIterationStart(const UnitTest &unit_test, int iteration);

    virtual void OnEnvironmentsSetUpStart(const UnitTest &unit_test);

    virtual void OnEnvironmentsSetUpEnd(const UnitTest & /*unit_test*/) {}

    virtual void OnTestCaseStart(const TestCase &test_case);

    virtual void OnTestStart(const TestInfo &test_info);

    virtual void OnTestPartResult(const TestPartResult &result);

    virtual void OnTestEnd(const TestInfo &test_info);

    virtual void OnTestCaseEnd(const TestCase &test_case);

    virtual void OnEnvironmentsTearDownStart(const UnitTest &unit_test);

    virtual void OnEnvironmentsTearDownEnd(const UnitTest & /*unit_test*/) {}

    virtual void OnTestIterationEnd(const UnitTest &unit_test, int iteration);

    virtual void OnTestProgramEnd(const UnitTest & /*unit_test*/) {}

private:
    static void PrintFailedTests(const UnitTest &unit_test);
};

// Fired before each iteration of tests starts.
void PrettyUnitTestResultPrinter::OnTestIterationStart(
        const UnitTest &unit_test, int iteration) {
    if (GTEST_FLAG(repeat) != 1)
        ColoredPrintf(COLOR_WHITE, "\nRepeating all tests (iteration %d) . . .\n\n", iteration + 1);

    const char *const filter = GTEST_FLAG(filter).c_str();

    // Prints the filter if it's not *.  This reminds the user that some
    // tests may be skipped.
    if (!CStringEquals(filter, kUniversalFilter)) {
        ColoredPrintf(COLOR_YELLOW,
                      "Note: %s filter = %s\n", GTEST_NAME_, filter);
    }

    //if (internal::ShouldShard(kTestTotalShards, kTestShardIndex, false)) {
    //    const Int32 shard_index = Int32FromEnvOrDie(kTestShardIndex, -1);
    //    ColoredPrintf(COLOR_YELLOW,
    //                  "Note: This is test shard %d of %s.\n",
    //                  static_cast<int>(shard_index) + 1,
    //                  internal::posix::GetEnv(kTestTotalShards));
    //}

    if (GTEST_FLAG(shuffle)) {
        ColoredPrintf(COLOR_YELLOW,
                      "Note: Randomizing tests' orders with a seed of %d .\n",
                      unit_test.random_seed());
    }

    ColoredPrintf(COLOR_GREEN, "[==========] ");
    ColoredPrintf(COLOR_WHITE, "Running %s from %s.\n",
           FormatTestCount(unit_test.test_to_run_count()).c_str(),
           FormatTestCaseCount(unit_test.test_case_to_run_count()).c_str());
}

void PrettyUnitTestResultPrinter::OnEnvironmentsSetUpStart(
        const UnitTest & /*unit_test*/) {
    ColoredPrintf(COLOR_GREEN, "[----------] ");
    ColoredPrintf(COLOR_WHITE, "Global test environment set-up.\n");
}

void PrettyUnitTestResultPrinter::OnTestCaseStart(const TestCase &test_case) {
    const std::string counts =
            FormatCountableNoun(test_case.test_to_run_count(), "test", "tests");
    ColoredPrintf(COLOR_GREEN, "[----------] ");
    ColoredPrintf(COLOR_WHITE, "%s from %s", counts.c_str(), test_case.name());
    if (test_case.type_param() == NULL) {
        ColoredPrintf(COLOR_WHITE, "\n");
    } else {
        ColoredPrintf(COLOR_WHITE, ", where %s = %s\n", kTypeParamLabel, test_case.type_param());
    }
}

void PrettyUnitTestResultPrinter::OnTestStart(const TestInfo &test_info) {
    ColoredPrintf(COLOR_GREEN, "[ RUN      ] ");
    PrintTestName(test_info.test_case_name(), test_info.name());
    ColoredPrintf(COLOR_WHITE, "\n");
}

// Called after an assertion failure.
void PrettyUnitTestResultPrinter::OnTestPartResult(
        const TestPartResult &result) {
    // If the test part succeeded, we don't need to do anything.
    if (result.type() == TestPartResult::kSuccess)
        return;

    // Print failure message from the assertion (e.g. expected this and got that).
    PrintTestPartResult(result);
}

void PrettyUnitTestResultPrinter::OnTestEnd(const TestInfo &test_info) {
    if (test_info.result()->Passed()) {
        ColoredPrintf(COLOR_GREEN, "[       OK ] ");
    } else {
        ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
    }
    PrintTestName(test_info.test_case_name(), test_info.name());
    if (test_info.result()->Failed())
        PrintFullTestCommentIfPresent(test_info);

    if (GTEST_FLAG(print_time)) {
        ColoredPrintf(COLOR_WHITE, " (%s ms)\n", internal::StreamableToString(
                test_info.result()->elapsed_time()).c_str());
    } else {
        ColoredPrintf(COLOR_WHITE, "\n");
    }
}

void PrettyUnitTestResultPrinter::OnTestCaseEnd(const TestCase &test_case) {
    if (!GTEST_FLAG(print_time)) return;

    const std::string counts =
            FormatCountableNoun(test_case.test_to_run_count(), "test", "tests");
    ColoredPrintf(COLOR_GREEN, "[----------] ");
    ColoredPrintf(COLOR_WHITE, "%s from %s (%s ms total)\n\n",
           counts.c_str(), test_case.name(),
           internal::StreamableToString(test_case.elapsed_time()).c_str());
}

void PrettyUnitTestResultPrinter::OnEnvironmentsTearDownStart(
        const UnitTest & /*unit_test*/) {
    ColoredPrintf(COLOR_GREEN, "[----------] ");
    ColoredPrintf(COLOR_WHITE, "Global test environment tear-down\n");
}

// Internal helper for printing the list of failed tests.
void PrettyUnitTestResultPrinter::PrintFailedTests(const UnitTest &unit_test) {
    const int failed_test_count = unit_test.failed_test_count();
    if (failed_test_count == 0) {
        return;
    }

    for (int i = 0; i < unit_test.total_test_case_count(); ++i) {
        const TestCase &test_case = *unit_test.GetTestCase(i);
        if (!test_case.should_run() || (test_case.failed_test_count() == 0)) {
            continue;
        }
        for (int j = 0; j < test_case.total_test_count(); ++j) {
            const TestInfo &test_info = *test_case.GetTestInfo(j);
            if (!test_info.should_run() || test_info.result()->Passed()) {
                continue;
            }
            ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
            ColoredPrintf(COLOR_WHITE, "%s.%s", test_case.name(), test_info.name());
            PrintFullTestCommentIfPresent(test_info);
            ColoredPrintf(COLOR_WHITE, "\n");
        }
    }
}

void PrettyUnitTestResultPrinter::OnTestIterationEnd(const UnitTest &unit_test,
                                                     int /*iteration*/) {
    ColoredPrintf(COLOR_GREEN, "[==========] ");
    ColoredPrintf(COLOR_WHITE, "%s from %s ran.",
           FormatTestCount(unit_test.test_to_run_count()).c_str(),
           FormatTestCaseCount(unit_test.test_case_to_run_count()).c_str());
    if (GTEST_FLAG(print_time)) {
        ColoredPrintf(COLOR_WHITE, " (%s ms total)",
               internal::StreamableToString(unit_test.elapsed_time()).c_str());
    }
    ColoredPrintf(COLOR_WHITE, "\n");
    ColoredPrintf(COLOR_GREEN, "[  PASSED  ] ");
    ColoredPrintf(COLOR_WHITE, "%s.\n", FormatTestCount(unit_test.successful_test_count()).c_str());

    int num_failures = unit_test.failed_test_count();
    if (!unit_test.Passed()) {
        const int failed_test_count = unit_test.failed_test_count();
        ColoredPrintf(COLOR_RED, "[  FAILED  ] ");
        ColoredPrintf(COLOR_WHITE, "%s, listed below:\n", FormatTestCount(failed_test_count).c_str());
        PrintFailedTests(unit_test);
        ColoredPrintf(COLOR_WHITE, "\n%2d FAILED %s\n", num_failures,
               num_failures == 1 ? "TEST" : "TESTS");
    }

    int num_disabled = unit_test.reportable_disabled_test_count();
    if (num_disabled && !GTEST_FLAG(also_run_disabled_tests)) {
        if (!num_failures) {
            ColoredPrintf(COLOR_WHITE, "\n");  // Add a spacer if no FAILURE banner is displayed.
        }
        ColoredPrintf(COLOR_YELLOW,
                      "  YOU HAVE %d DISABLED %s\n\n",
                      num_disabled,
                      num_disabled == 1 ? "TEST" : "TESTS");
    }
    // Ensure that Google Test output is printed before, e.g., heapchecker output.
}


#include <ostream>

template<typename TChar2, typename TTraits2>
struct MessageOutputer;

template<>
struct MessageOutputer<char, std::char_traits<char>> {
    template<typename TIterator>
    void operator()(TIterator begin, TIterator end) const {
        std::string s(begin, end);

        pushLogEntry({COLOR_GREY, s});
    }
};

template<>
struct MessageOutputer<wchar_t, std::char_traits<wchar_t>> {
    template<typename TIterator>
    void operator()(TIterator begin, TIterator end) const {
        std::wstring s(begin, end);
        //pushLogEntry({COLOR_GREY, s});
    }
};

/// \brief This class is a derivate of basic_stringbuf which will output all the written data using the OutputDebugString function
template<typename TChar, typename TTraits = std::char_traits<TChar>>
class OutputDebugStringBuf : public std::basic_streambuf<TChar, TTraits> {
public:
    explicit OutputDebugStringBuf() : _buffer(256) {
        std::basic_streambuf<TChar, TTraits>::setg(nullptr, nullptr, nullptr);
        std::basic_streambuf<TChar, TTraits>::setp(_buffer.data(), _buffer.data() + _buffer.size());
    }

    ~OutputDebugStringBuf() {
    }

    static_assert(std::is_same<TChar, char>::value || std::is_same<TChar, wchar_t>::value, "OutputDebugStringBuf only supports char and wchar_t types");

    int sync() try {
        MessageOutputer<TChar, TTraits>()(std::basic_streambuf<TChar, TTraits>::pbase(), std::basic_streambuf<TChar, TTraits>::pptr());
        std::basic_streambuf<TChar, TTraits>::setp(_buffer.data(), _buffer.data() + _buffer.size());
        return 0;
    }
    catch (...) {
        return -1;
    }

    typename TTraits::int_type overflow(typename TTraits::int_type c = TTraits::eof()) {
        auto syncRet = sync();
        if (c != TTraits::eof()) {
            _buffer[0] = c;
            std::basic_streambuf<TChar, TTraits>::setp(_buffer.data() + 1, _buffer.data() + _buffer.size());
        }
        return syncRet == -1 ? TTraits::eof() : 0;
    }


private:
    std::vector<TChar>      _buffer;
};



extern "C"
JNIEXPORT void JNICALL
Java_com_gamesparks_GameSparksTests_MainActivity_startTestSuite(
        JNIEnv *env,
        jobject /* this */) {

    std::thread([]() {
        // install log redirection
        static OutputDebugStringBuf<char> charDebugOutput;
        std::cerr.rdbuf(&charDebugOutput);
        std::clog.rdbuf(&charDebugOutput);
        std::cout.rdbuf(&charDebugOutput);

        // here you can modify the command line arguments passed to gtest

        const char *argv[] = {
            "GameSparksTests",
            //"--gtest_filter=DeviceStats.*"
        };
        int argc = sizeof(argv) / sizeof(argv[0]);

        setenv("GAMESPARKS_API_KEY", "exampleKey01", 1);
        setenv("GAMESPARKS_API_SECRET", "exampleSecret0123456789012345678", 1);

        setenv("RT_SESSION_0", "gst-men-rt03|5100|examplePlayerId|exampleTokenString0W+Q8nUP7RKwaUHNLuvhTuLSsjzBcikxJN8xtfSjA5pcPFygiU5jES2k6ncMy/Hd15Ml1t0UcUoO69qih6Hu2O7d7rEtDW8fybuaq07zhFA/+v0cxIJp6uso5txDA+CN9gKRF3V3Ykgbp+DRsty7LUoMZwya0zPXR5PWY9b4PBVJzK14wlQgdfnx/d1s5vASL/+lK5uuGlnxrBh9rKscSjuLws8uoviyr8CrmVuXVaUZ2PK/9gkc7iw5TusO14vY24nGdtsfwCNa9DLFCW+qqBZFR1wbiC7it2nihcxgEXilb8NCW2t0uTAZZDnYdl3CA==", 1);
        setenv("RT_SESSION_1", "gst-men-rt03|5100|examplePlayerId|exampleTokenString1W+Q8nUP7RKwaUHNLuvhTuLSsjzBcikxJN8xtfSjA5pcPFygiU5jES2k6ncMy/Hd15Ml1t0UcUoO69qih6Hu2O7d7rEtDW8fybuaq07zhFAAgHDnQ2S6repGK/TKy1t5dgKRF3V3Ykgbp+DRsty7LUoMZwya0zPXR5PWY9b4PBVJzK14wlQgdfnx/d1s5vASL/+lK5uuGlnxrBh9rKscSjuLws8uoviyr8CrmVuXVaUZ2PK/9gkc7iw5TusO14vY24nGdtsfwCNa9DLFCW+qqBZFR1wbiC7it2nihcxgEXifqpsE6raa9yBq2fOiwk52w==", 1);

        testing::InitGoogleTest(&argc, (char**)argv);
        ::testing::TestEventListeners &listeners =
                ::testing::UnitTest::GetInstance()->listeners();
        // Adds a listener to the end.  Google Test takes the ownership.
        listeners.Append(new PrettyUnitTestResultPrinter());
        auto result = RUN_ALL_TESTS();

        //return result;
    }).detach();
}


