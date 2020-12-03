#include "csvcalc.h"

#include <string>
#include <fstream>
#include <gtest/gtest.h>

static const std::string prefix_test_unix("test/unix/test_");
static const std::string prefix_test_win("test/win/test_");
static const std::string prefix_answer("test/answer/");
static const std::string suffix(".csv");
const char delimiter(',');

class csv_test_fixture : public ::testing::TestWithParam<std::string> {
protected:
    virtual std::string prefix() const = 0;

    csv::table load_test_table() const {
        std::ifstream is(prefix() + GetParam() + suffix);
        return csv::table::read_csv(&is, ',');
    }

    csv::table load_answer_table() const {
        std::ifstream is(prefix_answer + GetParam() + suffix);
        return csv::table::read_csv(&is, ',');
    }

    void match() {
        csv::table test = load_test_table();
        csv::table ans = load_answer_table();

        ASSERT_EQ(test.columns(), ans.columns());
        ASSERT_EQ(test.rows(), ans.rows());

        for(const auto &col : ans.columns()) {
            for(const auto &row : ans.rows()) {
                std::string caddr(col + row);
                const auto& answer_cell = ans.cell_at(caddr);
                const auto& test_cell = test.evaluate_cell_at(caddr);
                ASSERT_EQ(test_cell, answer_cell);
            }
        }
    }
};

class csv_test_fixture_unix : public csv_test_fixture {
protected:
    virtual std::string prefix() const override {
        return prefix_test_unix;
    }
};

class csv_test_fixture_unix_throws : public csv_test_fixture_unix { };

class csv_test_fixture_win : public csv_test_fixture {
protected:
    virtual std::string prefix() const override {
        return prefix_test_win;
    }
};

class csv_test_fixture_win_throws : public csv_test_fixture_win { };

TEST_P(csv_test_fixture_unix, csv_tests) {
    match();
}

TEST_P(csv_test_fixture_unix_throws, csv_test_with_exception) {
    ASSERT_ANY_THROW(match());
}

TEST_P(csv_test_fixture_win, csv_tests) {
    match();
}

TEST_P(csv_test_fixture_win_throws, csv_tests_with_exception) {
    ASSERT_ANY_THROW(match());
}

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_0,
    csv_test_fixture_unix,
    ::testing::Values(
        "g0_basic_01",
        "g0_basic_02",
        "g0_basic_03",
        "g0_basic_04"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_0,
    csv_test_fixture_unix_throws,
    ::testing::Values(
        "g0_syntax_01",
        "g0_wrong_cell_address_01",
        "g0_wrong_cell_address_02",
        "g0_wrong_op_01",
        "g0_wrong_op_02"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_0,
    csv_test_fixture_win,
    ::testing::Values(
        "g0_basic_01",
        "g0_basic_02",
        "g0_basic_03",
        "g0_basic_04"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_0,
    csv_test_fixture_win_throws,
    ::testing::Values(
        "g0_syntax_01",
        "g0_wrong_cell_address_01",
        "g0_wrong_cell_address_02",
        "g0_wrong_op_01",
        "g0_wrong_op_02"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_1,
    csv_test_fixture_unix,
    ::testing::Values(
        "g1_basic_01",
        "g1_basic_02",
        "g1_const_01",
        "g1_const_02"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_1,
    csv_test_fixture_win,
    ::testing::Values(
        "g1_basic_01",
        "g1_basic_02",
        "g1_const_01",
        "g1_const_02"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_2,
    csv_test_fixture_unix,
    ::testing::Values(
        "g2_chaining_01",
        "g2_chaining_02",
        "g2_chaining_03",
        "g2_const_01",
        "g2_const_02"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_2,
    csv_test_fixture_unix_throws,
    ::testing::Values(
        "g2_div_by_zero_01",
        "g2_div_by_zero_02",
        "g2_loop_01",
        "g2_loop_02",
        "g2_syntax_01",
        "g2_syntax_02",
        "g2_syntax_03",
        "g2_syntax_04",
        "g2_syntax_05",
        "g2_syntax_06",
        "g2_syntax_07",
        "g2_syntax_08",
        "g2_syntax_09",
        "g2_syntax_10",
        "g2_syntax_11",
        "g2_syntax_12"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_2,
    csv_test_fixture_win,
    ::testing::Values(
        "g2_chaining_01",
        "g2_chaining_02",
        "g2_chaining_03",
        "g2_const_01",
        "g2_const_02"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_2,
    csv_test_fixture_win_throws,
    ::testing::Values(
        "g2_div_by_zero_01",
        "g2_div_by_zero_02",
        "g2_loop_01",
        "g2_loop_02",
        "g2_syntax_01",
        "g2_syntax_02",
        "g2_syntax_03",
        "g2_syntax_04",
        "g2_syntax_05",
        "g2_syntax_06",
        "g2_syntax_07",
        "g2_syntax_08",
        "g2_syntax_09",
        "g2_syntax_10",
        "g2_syntax_11",
        "g2_syntax_12"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_3,
    csv_test_fixture_unix,
    ::testing::Values(
        "g3_chaining_01",
        "g3_chaining_02",
        "g3_chaining_03",
        "g3_chaining_04",
        "g3_expr_01",
        "g3_expr_02",
        "g3_expr_03",
        "g3_expr_04",
        "g3_expr_05",
        "g3_expr_06",
        "g3_expr_07",
        "g3_expr_literals_01",
        "g3_expr_literals_02",
        "g3_spacing_01",
        "g3_spacing_02",
        "g3_spacing_03",
        "g3_spacing_04"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_3,
    csv_test_fixture_unix_throws,
    ::testing::Values(
        "g3_loop_01",
        "g3_loop_02",
        "g3_div_by_zero_01",
        "g3_expr_08",
        "g3_expr_09",
        "g3_expr_10"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_3,
    csv_test_fixture_win,
    ::testing::Values(
        "g3_chaining_01",
        "g3_chaining_02",
        "g3_chaining_03",
        "g3_chaining_04",
        "g3_expr_01",
        "g3_expr_02",
        "g3_expr_03",
        "g3_expr_04",
        "g3_expr_05",
        "g3_expr_06",
        "g3_expr_07",
        "g3_expr_literals_01",
        "g3_expr_literals_02",
        "g3_spacing_01",
        "g3_spacing_02",
        "g3_spacing_03",
        "g3_spacing_04"));

INSTANTIATE_TEST_CASE_P(
    CSV_Tests_Group_3,
    csv_test_fixture_win_throws,
    ::testing::Values(
        "g3_div_by_zero_01",
        "g3_loop_01",
        "g3_loop_02",
        "g3_expr_08",
        "g3_expr_09",
        "g3_expr_10"));

int
main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
