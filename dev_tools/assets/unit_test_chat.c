//===========================================================================
//
// Name:                unit_test_chat.c
// Function:    deterministic templates for unit tests
//
//===========================================================================

#include "game.h"
#include "match.h"

MTCONTEXT_ENTERGAME
{
"This message is intentionally oversized to trip BotConstructChatMessage validation ",
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" = (MSG_ENTERGAME, 0);
NETNAME, " triggered the deterministic join message" = (MSG_ENTERGAME, 0);
}

["unit-test-reply-valid"] = 9100
{
"Unit test reply constructed successfully";
}

["unit-test-reply-invalid"] = 9101
{
"Reply text padded to exceed BotConstructChatMessage limits ",
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
}

["unit-test-random-valid"] = 9200
{
"Random string placeholder: \\rrandom_misc\\.";
}

["unit-test-random-invalid"] = 9201
{
"Random string placeholder: \\runit_test_missing\\.";
}
