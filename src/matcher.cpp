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


static int FindMatch(const TCHAR* str, int pos, const int final_pos, const int add_ch, const int sub_ch, const bool in_line)
{
    const int add = pos <= final_pos ? 1 : -1;
    for (int counter = 1; pos != final_pos; pos += add) {
        const int ch = str[pos];
        if (in_line && _tcschr(text_eol, ch)) {
            return -1;
        }
        if (ch == sub_ch) {
            counter--;
        } else if (ch == add_ch) {
            counter++;
        }
        if (counter == 0) {
            return pos;
        }
    }
    return -1;
}

static auto FindMatchingBracket(const TCHAR* str, const int length, const int sel_start, const int sel_end)
{
    // find the matching brackets closest to the selected area.
    // if no found, select the whole text.

    const int count = _tcsclen(text_brackets) / 2;

    bool found = false;
    int start = 0, end = length, distance = (length + 1) * 2;
    for (int i = 0; i < count; i++) {
        const int open_ch = text_brackets[i * 2];
        const int close_ch = text_brackets[i * 2 + 1];
        const bool in_line = _tcschr(text_inline, open_ch);
        int tmp_start = FindMatch(str, sel_start - 1, -1, close_ch, open_ch, in_line) + 1;
        int tmp_end = FindMatch(str, sel_end, length, open_ch, close_ch, in_line);
        int tmp_distance = min((sel_start - tmp_start) * 2, (tmp_end - sel_end) * 2 + 1);  // left char is more closer
        if (tmp_start != 0 && tmp_end != -1 && tmp_distance < distance) {
            found = true;
            start = tmp_start;
            end = tmp_end;
            distance = tmp_distance;
        }
    }

    return std::make_tuple(found, max(0, start), min(length, end));
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
