#include <stdio.h>
#include <string.h>

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))
#define expr(x) printf(#x"=%.15g\n", (double)(x))


static int FindMatch(const char* str, int pos, int final_pos, int add_ch, int sub_ch)
{
    int direction = pos <= final_pos ? 1 : -1;
    for (int counter = 1; pos != final_pos; pos += direction) {
        int ch = str[pos];
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

int FindMatchingBracket(const char* str, int length, int sel_start, int sel_end, int& res_start, int& res_end)
{
    // 1. Find the matching brackets closest to the selected area.
    // 2. If the matching brackets are already selected, expand the selection to include brackets.
    // 3. If no matching brackets are found, select the whole text.

    const char open_chars[] = "([{'\"";
    const char close_chars[] = ")]}'\"";
    const int types = min(strlen(open_chars), strlen(close_chars));

    int start = 0, end = length, distance = -1;
    for (int i = 0; i < types; i++) {
        int open_ch = open_chars[i];
        int close_ch = close_chars[i];
        int tmp_start = FindMatch(str, sel_start - 1, -1, close_ch, open_ch);
        int tmp_end = FindMatch(str, sel_end, length, open_ch, close_ch);
        if (tmp_start != -1) {
            tmp_start += 1;
        }
        if (min(tmp_start, tmp_end) >= 0 && (distance == -1 || min(sel_start - tmp_start, tmp_end - sel_end) < distance)) {
            start = tmp_start;
            end = tmp_end;
            distance = min(sel_start - tmp_start, tmp_end - sel_end);
        }
    }

    if (start == sel_start && end == sel_end) {
        start--;
        end++;
    }

    res_start = start;
    res_end = end;

    return 1;
}

static int FindMatchingBracketTest()
{
    // const char test[] = "apple(banana[cat]dog)everything";
    const char test[] = "A'quick'bown[fox(jumps(over)the(lazy)dog)]";
    const int length = strlen(test) + 1;  // cursor position can be at the EOF

    for (int pos = 0; pos < length; pos++) {
        int start, end;
        if (FindMatchingBracket(test, length - 1, pos, pos, start, end)) {
            char match[length + 1] = "";
            memcpy(match, test + start, end - start);
            printf("pos = %d, cur = '%c', span = (%d, %d), match = '%s'\n", pos, test[pos], start, end, match);
        } else {
            printf("pos = %d, cur = '%c', \n", pos, test[pos]);
        }
    }

    return 0;
}

#ifndef PLUGINDEFINITION_H
int main() { FindMatchingBracketTest(); }
#endif // PLUGINDEFINITION_H
