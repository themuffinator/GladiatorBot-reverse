//
// Name:                match_reply.c
// Function:    match templates with reply fallbacks for unit tests
//
//=========================================================================

#include "game.h"
#include "syn.h"
#include "match.h"

CONTEXT_NEARBYITEM
{
	[("Rocket Launcher", 1), ("Quad Damage", 1), ("rocket", 1)]
}

MTCONTEXT_ENTERGAME
{
	NEARBYITEM, " acquired" = (MSG_ENTERGAME, 0);
	"generic greeting" = (MSG_ENTERGAME, 0);
}

["fallback"] = 2
{
	"fallback reply";
}
