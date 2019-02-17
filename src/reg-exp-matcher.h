#pragma once
#include "reg-exp-parser.h"

char* regExpMatch(RegExp* regexp, const char* text, Boolean greedy);