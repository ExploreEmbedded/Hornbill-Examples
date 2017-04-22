#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsonParser.h"

/**
 * Creates a new parser based over a given  buffer with an array of tokens
 * available.
 */
void jsmnClass::Init(jsmn_parser_t *tokenParser)
{
    tokenParser->pos = 0;
    tokenParser->toknext = 0;
    tokenParser->toksuper = -1;
}


int jsmnClass::equate(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
            strncmp(json + tok->start, s, tok->end - tok->start) == 0)
    {
        return 0;
    }
    return -1;
}


jsmntok_t *jsmnClass::allocToken(jsmn_parser_t *tokenParser, jsmntok_t *tokens, size_t num_tokens)
{
    jsmntok_t *tok;
    if (tokenParser->toknext >= num_tokens) {
        return NULL;
    }
    tok = &tokens[tokenParser->toknext++];
    tok->start = tok->end = -1;
    tok->size = 0;
#ifdef JSMN_PARENT_LINKS
    tok->parent = -1;
#endif
    return tok;
}

/**
 * Fills token type and boundaries.
 */
void jsmnClass::fillToken(jsmntok_t *token, jsmntype_t type, int start, int end)
{
    token->type = type;
    token->start = start;
    token->end = end;
    token->size = 0;
}

/**
 * Fills next available token with JSON primitive.
 */
int jsmnClass::parsePrimitive(jsmn_parser_t *tokenParser, const char *js, size_t len, jsmntok_t *tokens, size_t num_tokens)
{
    jsmntok_t *token;
    int start;

    start = tokenParser->pos;

    for (; tokenParser->pos < len && js[tokenParser->pos] != '\0'; tokenParser->pos++) {
        switch (js[tokenParser->pos]) {
#ifndef JSMN_STRICT
        /* In strict mode primitive must be followed by "," or "}" or "]" */
        case ':':
#endif
        case '\t' : case '\r' : case '\n' : case ' ' :
        case ','  : case ']'  : case '}' :
            goto found;
        }
        if (js[tokenParser->pos] < 32 || js[tokenParser->pos] >= 127) {
            tokenParser->pos = start;
            return JSMN_ERROR_INVAL;
        }
    }
#ifdef JSMN_STRICT
    /* In strict mode primitive must be followed by a comma/object/array */
    tokenParser->pos = start;
    return JSMN_ERROR_PART;
#endif

    found:
    if (tokens == NULL) {
        tokenParser->pos--;
        return 0;
    }
    token = allocToken(tokenParser, tokens, num_tokens);
    if (token == NULL) {
        tokenParser->pos = start;
        return JSMN_ERROR_NOMEM;
    }
    fillToken(token, JSMN_PRIMITIVE, start, tokenParser->pos);
#ifdef JSMN_PARENT_LINKS
    token->parent = tokenParser->toksuper;
#endif
    tokenParser->pos--;
    return 0;
}

/**
 * Fills next token with JSON string.
 */
int jsmnClass::parseString(jsmn_parser_t *tokenParser, const char *js, size_t len, jsmntok_t *tokens, size_t num_tokens)
{
    jsmntok_t *token;

    int start = tokenParser->pos;

    tokenParser->pos++;

    /* Skip starting quote */
    for (; tokenParser->pos < len && js[tokenParser->pos] != '\0'; tokenParser->pos++) {
        char c = js[tokenParser->pos];

        /* Quote: end of string */
        if (c == '\"') {
            if (tokens == NULL) {
                return 0;
            }
            token = allocToken(tokenParser, tokens, num_tokens);
            if (token == NULL) {
                tokenParser->pos = start;
                return JSMN_ERROR_NOMEM;
            }
            fillToken(token, JSMN_STRING, start+1, tokenParser->pos);
#ifdef JSMN_PARENT_LINKS
            token->parent = tokenParser->toksuper;
#endif
            return 0;
        }

        /* Backslash: Quoted symbol expected */
        if (c == '\\' && tokenParser->pos + 1 < len) {
            int i;
            tokenParser->pos++;
            switch (js[tokenParser->pos]) {
            /* Allowed escaped symbols */
            case '\"': case '/' : case '\\' : case 'b' :
            case 'f' : case 'r' : case 'n'  : case 't' :
                break;
                /* Allows escaped symbol \uXXXX */
            case 'u':
                tokenParser->pos++;
                for(i = 0; i < 4 && tokenParser->pos < len && js[tokenParser->pos] != '\0'; i++) {
                    /* If it isn't a hex character we have an error */
                    if(!((js[tokenParser->pos] >= 48 && js[tokenParser->pos] <= 57) || /* 0-9 */
                            (js[tokenParser->pos] >= 65 && js[tokenParser->pos] <= 70) || /* A-F */
                            (js[tokenParser->pos] >= 97 && js[tokenParser->pos] <= 102))) { /* a-f */
                        tokenParser->pos = start;
                        return JSMN_ERROR_INVAL;
                    }
                    tokenParser->pos++;
                }
                tokenParser->pos--;
                break;
                /* Unexpected symbol */
            default:
                tokenParser->pos = start;
                return JSMN_ERROR_INVAL;
            }
        }
    }
    tokenParser->pos = start;
    return JSMN_ERROR_PART;
}


/**
 * Parse JSON string and fill tokens.
 */
int jsmnClass::parse(jsmn_parser_t *tokenParser, const char *js, size_t len, jsmntok_t *tokens, unsigned int num_tokens)
{
    int r;
    int i;
    jsmntok_t *token;
    int count = tokenParser->toknext;

    for (; tokenParser->pos < len && js[tokenParser->pos] != '\0'; tokenParser->pos++) {
        char c;
        jsmntype_t type;

        c = js[tokenParser->pos];

        switch (c) {
        case '{': case '[':
            count++;
            if (tokens == NULL) {
                break;
            }
            token = allocToken(tokenParser, tokens, num_tokens);
            if (token == NULL)
                return JSMN_ERROR_NOMEM;
            if (tokenParser->toksuper != -1) {
                tokens[tokenParser->toksuper].size++;
#ifdef JSMN_PARENT_LINKS
                token->parent = tokenParser->toksuper;
#endif
            }
            token->type = (c == '{' ? JSMN_OBJECT : JSMN_ARRAY);
            token->start = tokenParser->pos;
            tokenParser->toksuper = tokenParser->toknext - 1;
            break;
        case '}': case ']':
            if (tokens == NULL)
                break;
            type = (c == '}' ? JSMN_OBJECT : JSMN_ARRAY);
#ifdef JSMN_PARENT_LINKS
            if (tokenParser->toknext < 1) {
                return JSMN_ERROR_INVAL;
            }
            token = &tokens[tokenParser->toknext - 1];
            for (;;) {
                if (token->start != -1 && token->end == -1) {
                    if (token->type != type) {
                        return JSMN_ERROR_INVAL;
                    }
                    token->end = tokenParser->pos + 1;
                    tokenParser->toksuper = token->parent;
                    break;
                }
                if (token->parent == -1) {
                    if(token->type != type || tokenParser->toksuper == -1) {
                        return JSMN_ERROR_INVAL;
                    }
                    break;
                }
                token = &tokens[token->parent];
            }
#else
            for (i = tokenParser->toknext - 1; i >= 0; i--) {
                token = &tokens[i];
                if (token->start != -1 && token->end == -1) {
                    if (token->type != type) {
                        return JSMN_ERROR_INVAL;
                    }
                    tokenParser->toksuper = -1;
                    token->end = tokenParser->pos + 1;
                    break;
                }
            }
            /* Error if unmatched closing bracket */
            if (i == -1) return JSMN_ERROR_INVAL;
            for (; i >= 0; i--) {
                token = &tokens[i];
                if (token->start != -1 && token->end == -1) {
                    tokenParser->toksuper = i;
                    break;
                }
            }
#endif
            break;
        case '\"':
            r = parseString(tokenParser, js, len, tokens, num_tokens);
            if (r < 0) return r;
            count++;
            if (tokenParser->toksuper != -1 && tokens != NULL)
                tokens[tokenParser->toksuper].size++;
            break;
        case '\t' : case '\r' : case '\n' : case ' ':
            break;
        case ':':
            tokenParser->toksuper = tokenParser->toknext - 1;
            break;
        case ',':
            if (tokens != NULL && tokenParser->toksuper != -1 &&
                    tokens[tokenParser->toksuper].type != JSMN_ARRAY &&
                    tokens[tokenParser->toksuper].type != JSMN_OBJECT) {
#ifdef JSMN_PARENT_LINKS
                tokenParser->toksuper = tokens[tokenParser->toksuper].parent;
#else
                for (i = tokenParser->toknext - 1; i >= 0; i--) {
                    if (tokens[i].type == JSMN_ARRAY || tokens[i].type == JSMN_OBJECT) {
                        if (tokens[i].start != -1 && tokens[i].end == -1) {
                            tokenParser->toksuper = i;
                            break;
                        }
                    }
                }
#endif
            }
            break;
#ifdef JSMN_STRICT
            /* In strict mode primitives are: numbers and booleans */
        case '-': case '0': case '1' : case '2': case '3' : case '4':
        case '5': case '6': case '7' : case '8': case '9':
        case 't': case 'f': case 'n' :
            /* And they must not be keys of the object */
            if (tokens != NULL && tokenParser->toksuper != -1) {
                jsmntok_t *t = &tokens[tokenParser->toksuper];
                if (t->type == JSMN_OBJECT ||
                        (t->type == JSMN_STRING && t->size != 0)) {
                    return JSMN_ERROR_INVAL;
                }
            }
#else
            /* In non-strict mode every unquoted value is a primitive */
        default:
#endif
            r = parsePrimitive(tokenParser, js, len, tokens, num_tokens);
            if (r < 0) return r;
            count++;
            if (tokenParser->toksuper != -1 && tokens != NULL)
                tokens[tokenParser->toksuper].size++;
            break;

#ifdef JSMN_STRICT
            /* Unexpected char in strict mode */
        default:
            return JSMN_ERROR_INVAL;
#endif
        }
    }

    if (tokens != NULL) {
        for (i = tokenParser->toknext - 1; i >= 0; i--) {
            /* Unmatched opened object or array */
            if (tokens[i].start != -1 && tokens[i].end == -1) {
                return JSMN_ERROR_PART;
            }
        }
    }

    return count;
}

jsmnClass jsmn;
