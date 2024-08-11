#include <math.h>
#include <stdio.h>
#include <string.h>


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

int FindMatchingBracket(const char* str, int pos, int* ptr_start, int* ptr_end)
{
    const char open_chars[] = "([{'\"";
    const char close_chars[] = ")]}'\"";
    const int types = fmin(strlen(open_chars), strlen(close_chars));
    const int length = strlen(str);

    int start = -1, end = -1;
    for (int i = 0; i < types; i++) {
        int open_ch = open_chars[i];
        int close_ch = close_chars[i];
        int temp_start = FindMatch(str, pos - 1, -1, close_ch, open_ch);
        int temp_end = FindMatch(str, pos, length, open_ch, close_ch);
        if (start == -1 || 
            temp_start != -1 && temp_end != -1 &&
            temp_end - temp_start < end - start
        ) {
            start = temp_start;
            end = temp_end;
        }
    }

    *ptr_start = start + 1;
    *ptr_end = end;
    return start != -1 && end != -1;
}

static int FindMatchingBracketTest()
{
    // const char test[] = "apple(banana[cat]dog)everything";
    const char test[] = "A'quick'bown[fox(jumps(over)the(lazy)dog)]";
    const int length = strlen(test) + 1;  // cursor position can be at the EOF

    for (int pos = 0; pos < length; pos++) {
        int start, end;
        if (FindMatchingBracket(test, pos, &start, &end)) {
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
