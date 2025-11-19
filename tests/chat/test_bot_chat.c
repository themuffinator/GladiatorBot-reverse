#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "botlib/ai_chat/ai_chat.h"
#include "botlib/common/l_log.h"
#include "botlib/precomp/l_precomp.h"
#include "botlib/precomp/l_script.h"

extern void BotLib_TestResetLastMessage(void);
extern const char *BotLib_TestGetLastMessage(void);
extern int BotLib_TestGetLastMessageType(void);
extern void BotLib_TestSetLibVar(const char *var_name, float value);
extern void BotLib_TestResetLibVars(void);

/*
=============
configure_chat_libvars

Resets and sets the requested mocked libvar values.
=============
*/
static void configure_chat_libvars(float fastchat_value, float nochat_value) {
	BotLib_TestResetLibVars();
	BotLib_TestSetLibVar("fastchat", fastchat_value);
	BotLib_TestSetLibVar("nochat", nochat_value);
}

/*
=============
drain_console

Clears queued console messages for deterministic checks.
=============
*/
static void drain_console(bot_chatstate_t *chat) {
	int type = 0;
	char buffer[256];
	while (BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer))) {
		(void)type;
	}
}

/*
=============
test_enter_chat_uses_unit_test_template
=============
*/
static void test_enter_chat_uses_unit_test_template(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/unit_test_chat.c", "unit_enter_valid"));

	drain_console(chat);
	BotChat_SetContextCooldown(chat, 2, 0.0);
	BotEnterChat(chat, 0, 0);

	int type = 0;
	char buffer[256];
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 2);
	assert(strcmp(buffer, "{NETNAME} triggered the deterministic join message") == 0);

	BotFreeChatState(chat);
}

/*
=============
test_enter_chat_construct_message_failure_respects_cooldown_reset
=============
*/
static void test_enter_chat_construct_message_failure_respects_cooldown_reset(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/unit_test_chat.c", "unit_enter_invalid"));

	drain_console(chat);
	BotChat_SetContextCooldown(chat, 2, 1.0);
	BotChat_SetTime(chat, 10.0);
	BotLib_TestResetLastMessage();
	BotEnterChat(chat, 0, 0);
	assert(BotNumConsoleMessages(chat) == 0);
	assert(BotLib_TestGetLastMessageType() == PRT_ERROR);
	assert(strstr(BotLib_TestGetLastMessage(), "too long") != NULL);

	BotEnterChat(chat, 0, 0);
	int type = 0;
	char buffer[256];
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 2);
	assert(strstr(buffer, "blocked by cooldown") != NULL);

	drain_console(chat);
	BotChat_SetTime(chat, 12.0);
	BotEnterChat(chat, 0, 0);
	assert(BotNumConsoleMessages(chat) == 0);

	BotFreeChatState(chat);
}

/*
=============
test_reply_chat_death_context
=============
*/
static void test_reply_chat_death_context(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/rchat.c", "reply"));

	drain_console(chat);
	assert(BotReplyChat(chat, "unit-test", 1));

	int type = 0;
	char buffer[256];
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 1);
	assert(BotChat_HasReplyTemplate(chat, 1, buffer));

	BotFreeChatState(chat);
}

/*
=============
test_reply_chat_falls_back_to_reply_table
=============
*/
static void test_reply_chat_falls_back_to_reply_table(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/rchat.c", "reply"));

	drain_console(chat);
	assert(BotReplyChat(chat, "abnormal", 5));

	int type = 0;
	char buffer[256];
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 5);
	assert(BotChat_HasReplyTemplate(chat, 5, buffer));

	BotFreeChatState(chat);
}

/*
=============
test_enter_chat_enqueues_message
=============
*/
static void test_enter_chat_enqueues_message(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/match.c", "match"));

	drain_console(chat);
	BotChat_SetContextCooldown(chat, 2, 0.0);
	BotEnterChat(chat, 0, 0);

	int type = 0;
	char buffer[256];
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 2);
	assert(strcmp(buffer, "{NETNAME} entered the game") == 0);

	BotFreeChatState(chat);
}

/*
=============
test_enter_chat_cooldown_blocks_repeated_messages
=============
*/
static void test_enter_chat_cooldown_blocks_repeated_messages(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/match.c", "match"));

	drain_console(chat);
	BotChat_SetContextCooldown(chat, 2, 5.0);
	BotChat_SetTime(chat, 1.0);
	BotEnterChat(chat, 0, 0);

	int type = 0;
	char buffer[256];
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 2);
	assert(strcmp(buffer, "{NETNAME} entered the game") == 0);

	BotChat_SetTime(chat, 2.0);
	BotEnterChat(chat, 0, 0);

	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 2);
	assert(strcmp(buffer,
			"context 2 blocked by cooldown (4.00s remaining)\n") == 0);

	BotFreeChatState(chat);
}


/*
=============
test_reply_chat_logs_missing_contexts
=============
*/
static void test_reply_chat_logs_missing_contexts(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/rchat.c", "reply"));

	drain_console(chat);
	BotLib_TestResetLastMessage();

	assert(BotReplyChat(chat, "unit-test", 1));
	assert(!BotReplyChat(chat, "unit-test", 9999));
	assert(strcmp(BotLib_TestGetLastMessage(), "no rchats\n") == 0);
	assert(BotLib_TestGetLastMessageType() == PRT_MESSAGE);
	assert(BotNumConsoleMessages(chat) == 2);

	int type = 0;
	char buffer[256];
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 1);
	assert(BotChat_HasReplyTemplate(chat, 1, buffer));

	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == PRT_MESSAGE);
	assert(strcmp(buffer, "no rchats\n") == 0);
	assert(!BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));

	BotFreeChatState(chat);
}

/*
=============
test_reply_chat_construct_message_paths
=============
*/
static void test_reply_chat_construct_message_paths(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/unit_test_chat.c", "unit_reply"));

	drain_console(chat);
	BotChat_SetContextCooldown(chat, 9100, 1.0);
	BotChat_SetTime(chat, 1.0);
	assert(BotReplyChat(chat, "unit-test", 9100));

	int type = 0;
	char buffer[256];
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 9100);
	assert(strcmp(buffer, "Unit test reply constructed successfully") == 0);

	assert(!BotReplyChat(chat, "unit-test", 9100));
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 9100);
	assert(strstr(buffer, "blocked by cooldown") != NULL);

	drain_console(chat);
	BotChat_SetTime(chat, 3.0);
	assert(BotReplyChat(chat, "unit-test", 9100));
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 9100);
	assert(strcmp(buffer, "Unit test reply constructed successfully") == 0);

	drain_console(chat);
	BotChat_SetContextCooldown(chat, 9101, 0.5);
	BotChat_SetTime(chat, 4.0);
	BotLib_TestResetLastMessage();
	assert(!BotReplyChat(chat, "unit-test", 9101));
	assert(BotNumConsoleMessages(chat) == 0);
	assert(BotLib_TestGetLastMessageType() == PRT_ERROR);
	assert(strstr(BotLib_TestGetLastMessage(), "too long") != NULL);

	assert(!BotReplyChat(chat, "unit-test", 9101));
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == 9101);
	assert(strstr(buffer, "blocked by cooldown") != NULL);

	drain_console(chat);
	BotChat_SetTime(chat, 5.0);
	assert(!BotReplyChat(chat, "unit-test", 9101));
	assert(BotNumConsoleMessages(chat) == 0);

	BotFreeChatState(chat);
}

/*
=============
test_synonym_lookup_contains_nearbyitem_entries
=============
*/
static void test_synonym_lookup_contains_nearbyitem_entries(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/rchat.c", "reply"));

	assert(BotChat_HasSynonymPhrase(chat, "CONTEXT_NEARBYITEM", "Quad Damage"));
	assert(BotChat_HasSynonymPhrase(chat, "CONTEXT_NEARBYITEM",
				"Rocket Launcher"));

	BotFreeChatState(chat);
}

/*
=============
test_known_template_is_registered
=============
*/
static void test_known_template_is_registered(void) {
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/rchat.c", "reply"));

	assert(BotChat_HasReplyTemplate(chat, 1, "{VICTIM} commits suicide"));

	BotFreeChatState(chat);
}

/*
=============
test_include_path_too_long_is_rejected
=============
*/
static void test_include_path_too_long_is_rejected(void) {
	const size_t segment_length = 256;
	const size_t segment_count = 5;
	const size_t fragment_length =
		segment_count * segment_length + (segment_count - 1);

	char include_fragment[fragment_length + 1];
	size_t offset = 0;
	for (size_t segment = 0; segment < segment_count; ++segment) {
		if (segment > 0) {
			include_fragment[offset++] = '/';
		}
		memset(include_fragment + offset, (int)('a' + (int)segment),
			   segment_length);
		offset += segment_length;
	}
	include_fragment[offset] = '\0';

	char script[sizeof(include_fragment) + 16];
	int written =
		snprintf(script, sizeof(script), "#include <%s>\n", include_fragment);
	assert(written > 0);
	assert((size_t)written < sizeof(script));

	pc_source_t *source =
		PC_LoadSourceMemory("unit-test", script, (size_t)written);
	assert(source != NULL);

	pc_token_t token;
	assert(!PC_ReadToken(source, &token));

	const pc_diagnostic_t *diagnostic = PC_GetDiagnostics(source);
	bool found_error = false;
	while (diagnostic != NULL) {
		if (diagnostic->level == PC_ERROR_LEVEL_ERROR &&
			diagnostic->message != NULL &&
			strstr(diagnostic->message, "path too long") != NULL) {
			found_error = true;
			break;
		}
		diagnostic = diagnostic->next;
	}
	assert(found_error);

	PC_FreeSource(source);
}

/*
=============
test_botloadchatfile_fastchat_nochat_combinations

Ensures the mocked libvars gate chat loading and diagnostics correctly.
=============
*/
static void test_botloadchatfile_fastchat_nochat_combinations(void) {
	const char *expected_message =
		"couldn't load chat reply from " BOT_ASSET_ROOT "/rchat.c\n";
	bot_chatstate_t *chat = BotAllocChatState();
	assert(chat != NULL);

	configure_chat_libvars(0.0f, 0.0f);
	assert(BotLoadChatFile(chat, BOT_ASSET_ROOT "/rchat.c", "reply"));
	drain_console(chat);

	configure_chat_libvars(0.0f, 1.0f);
	BotLib_TestResetLastMessage();
	assert(!BotLoadChatFile(chat, BOT_ASSET_ROOT "/rchat.c", "reply"));
	assert(BotLib_TestGetLastMessageType() == PRT_FATAL);
	assert(strcmp(BotLib_TestGetLastMessage(), expected_message) == 0);
	assert(BotNumConsoleMessages(chat) == 0);

	configure_chat_libvars(1.0f, 1.0f);
	BotLib_TestResetLastMessage();
	drain_console(chat);
	assert(!BotLoadChatFile(chat, BOT_ASSET_ROOT "/rchat.c", "reply"));
	assert(BotLib_TestGetLastMessageType() == PRT_FATAL);
	assert(strcmp(BotLib_TestGetLastMessage(), expected_message) == 0);
	assert(BotNumConsoleMessages(chat) == 1);

	int type = 0;
	char buffer[256];
	assert(BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));
	assert(type == PRT_FATAL);
	assert(strcmp(buffer, expected_message) == 0);
	assert(!BotNextConsoleMessage(chat, &type, buffer, sizeof(buffer)));

	configure_chat_libvars(0.0f, 0.0f);
	BotFreeChatState(chat);
}

/*
=============
main
=============
*/
int main(void) {
	configure_chat_libvars(0.0f, 0.0f);
	test_include_path_too_long_is_rejected();
	test_enter_chat_uses_unit_test_template();
	test_enter_chat_construct_message_failure_respects_cooldown_reset();
	test_reply_chat_death_context();
	test_reply_chat_falls_back_to_reply_table();
	test_reply_chat_construct_message_paths();
	test_synonym_lookup_contains_nearbyitem_entries();
	test_known_template_is_registered();
	test_include_path_too_long_is_rejected();
	test_enter_chat_enqueues_message();
	test_enter_chat_cooldown_blocks_repeated_messages();
	test_reply_chat_logs_missing_contexts();
	test_botloadchatfile_fastchat_nochat_combinations();

	printf("bot_chat_tests: all checks passed\n");
	return 0;
}
