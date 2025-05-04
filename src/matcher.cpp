// MIT License
// Copyright (c) 2024-2025 Shixian Li (znsoooo) <lsx7@sina.com>


#define _UNICODE  // enable unicode

#include <tchar.h>
#include <locale.h>
#include <tuple>
#include <vector>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define expr(x) printf(#x"=%.15g\n", (double)(x))

typedef std::vector<std::vector<int>> Vec2d;

static const TCHAR text_brackets[] = _TEXT("()[]{}<>''\"\"``（）［］｛｝《》「」『』【】〖〗‘’“”");
static const TCHAR text_inline[] = _TEXT("'\"<");
static const TCHAR text_eol[] = _TEXT("\r\n");


static bool CountChar(const TCHAR ch, const TCHAR add_ch, const TCHAR sub_ch, const bool in_line, int& counter, bool& checked, bool& any)
{
    if (checked) {
        return false;
    } else {
        any = true;
    }
    if (in_line && _tcschr(text_eol, ch)) {
        checked = true;
        return false;
    }
    if (ch == sub_ch) {
        counter--;
    } else if (ch == add_ch) {
        counter++;
    }
    if (counter == -1) {
        checked = true;
        return true;
    }
    return false;
}

static auto FindMatchingBracket(const TCHAR* str, const int length, const int sel_start, const int sel_end)
{
    // find the matching brackets closest to the selected area.
    // if no found, select the whole text.
    // left position is closer.

    const int count = _tcsclen(text_brackets) & ~1;

    bool in_line[count] = {0};
    int  counter[count] = {0};
    bool checked[count] = {0};
    bool any = true;

    for (int i = 0; i < count; i += 2) {
        in_line[i] = in_line[i + 1] = _tcschr(text_inline, text_brackets[i]);
    }

    for (int idx_left = sel_start - 1, idx_right = sel_end; any && idx_left >= 0 && idx_right < length; idx_left--, idx_right++) {
        any = false;
        if (idx_left >= 0) {
            const TCHAR ch_left = str[idx_left];
            for (int idx_br = 0; idx_br < count; idx_br += 2) {
                const TCHAR open_ch = text_brackets[idx_br];
                const TCHAR close_ch = text_brackets[idx_br + 1];
                if (CountChar(ch_left, close_ch, open_ch, in_line[idx_br], counter[idx_br], checked[idx_br], any)) {
                    int counter2 = 0;
                    bool checked2 = false, any2 = false;
                    for (int idx_right2 = sel_end; !checked2 && idx_right2 < length; idx_right2++) {
                        const TCHAR ch_right2 = str[idx_right2];
                        if (CountChar(ch_right2, open_ch, close_ch, in_line[idx_br], counter2, checked2, any2)) {
                            return std::make_tuple(true, idx_left + 1, idx_right2);
                        }
                    }
                }
            }
        }
        if (idx_right < length) {
            const TCHAR ch_right = str[idx_right];
            for (int idx_br = 1; idx_br < count; idx_br += 2) {
                const TCHAR open_ch = text_brackets[idx_br - 1];
                const TCHAR close_ch = text_brackets[idx_br];
                if (CountChar(ch_right, open_ch, close_ch, in_line[idx_br], counter[idx_br], checked[idx_br], any)) {
                    int counter2 = 0;
                    bool checked2 = false, any2 = false;
                    for (int idx_left2 = sel_start - 1; !checked2 && idx_left2 >= 0; idx_left2--) {
                        const TCHAR ch_left2 = str[idx_left2];
                        if (CountChar(ch_left2, close_ch, open_ch, in_line[idx_br], counter2, checked2, any2)) {
                            return std::make_tuple(true, idx_left2 + 1, idx_right);
                        }
                    }
                }
            }
        }
    }

    return std::make_tuple(false, 0, length);
}

auto FindMatchingBrackets(const TCHAR* str, const int length, const Vec2d sels)
{
    // if any selection changed, only change the changed selections.
    // if not any selection changed, expand to include brackets.

    int changed = 0;
    Vec2d results;

    for (int i = 0; i < sels.size(); i++) {
        auto [found, start, end] = FindMatchingBracket(str, length, sels[i][0], sels[i][1]);
        results.push_back({start, end});
        changed += start != sels[i][0] || end != sels[i][1];
    }

    for (int i = 0; i < sels.size(); i++) {
        results[i][0] = changed ? results[i][0] : max(0, results[i][0] - 1);
        results[i][1] = changed ? results[i][1] : min(length, results[i][1] + 1);
    }

    return std::make_tuple(changed, results);
}


int main()  // unit test
{
    // del a.exe & cls & g++ -std=c++17 matcher.cpp & a

    setlocale(LC_ALL, "");

    const TCHAR test[] = _TEXT("A'quick'bown[fox(jumps(over)the(lazy)dog)]（你好（世界））");
    const int length = _tcsclen(test) + 1;  // cursor position can be at the EOF

    expr(sizeof(TCHAR));

    for (int pos = 0; pos < length; pos++) {
        auto [found, start, end] = FindMatchingBracket(test, length - 1, pos, pos);
        if (found) {
            TCHAR match[length + 1] = {0};
            _tcsncpy(match, test + start, end - start);
            _tprintf(_TEXT("pos = %d, cur = '%c', span = (%d, %d), match = '%s'\n"), pos, test[pos], start, end, match);
        } else {
            _tprintf(_TEXT("pos = %d, cur = '%c', \n"), pos, test[pos]);
        }
    }

    return 0;
}
