#include <stdio.h>
#include <string.h>

int FindMatchingBracket(const char* str, int pos, int& start, int& end)
{
    const char brackets1[] = "([{\'\"";
    const char brackets2[] = ")]}\'\"";
    const int length = strlen(str);

    char* found1 = 0;
    char* found2 = 0;
    start = end = pos;

    // Find the nearest bracket
    for (start = pos - 1, end = pos; 0 <= start && end < length; start--, end++) {
        if ((found1 = strchr(brackets1, str[start])) || (found2 = strchr(brackets2, str[end]))) {
            break;
        }
    }

    // Find the matching bracket
    int counter = 1;
    if (found1) {
        for (int pair = brackets2[found1-brackets1]; end < length; end++) {
            if (str[end] == pair) {
                counter--;
            } else if (str[end] == str[start]) {
                counter++;
            }
            if (counter == 0) {
                start++;
                return 1;
            }
        }
    } else if (found2) {
        int counter = 1;
        for (int pair = brackets1[found2-brackets2]; 0 <= start; start--) {
            if (str[start] == pair) {
                counter--;
            } else if (str[start] == str[end]) {
                counter++;
            }
            if (counter == 0) {
                start++;
                return 1;
            }
        }
    }

    return 0;
}

int FindMatchingBracketTest()
{
    // const char test[] = "apple(banana[cat]dog)everything";
    const char test[] = "A'quick'bown[fox(jumps(over)the(lazy)dog)]";
    const int length = strlen(test);

    for (int pos = 0; pos < length; pos++) {
        int start, end;
        if (FindMatchingBracket(test, pos, start, end)) {
            char match[length+1] = "";
            memcpy(match, test + start, end - start);
            printf("pos = %d, span = (%d, %d), cur = '%c', match = '%s'\n", pos, start, end, test[pos], match);
        } else {
            printf("pos = %d\n", pos);
        }
    }

    return 0;
}

#ifndef PLUGINDEFINITION_H
int main() { FindMatchingBracketTest(); }
#endif //PLUGINDEFINITION_H
