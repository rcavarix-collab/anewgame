// strtable.h
//
// Every player-facing word (D26; DESIGN.md 4.6.2 "Text for any language"):
// read from assets/text/<language>.txt, never written into code, so the
// game can be translated by adding one file. English (en.txt) is loaded
// first and the chosen language over it, so a missing translation shows
// English rather than nothing; a key missing from both shows as "[key]",
// visible at a glance. tools/check_strings.py fails the build checks on
// any player-facing literal left in code and on keys used but not defined.
//
// File format (UTF-8): one entry per line, `key = text`; `#` starts a
// comment line; keys are lowercase letters, digits, `_` and `.`; `\n` in a
// text is a line break; {0}, {1}... are filled in by StrF, in any order a
// language needs. The key `font` names the installed system font to draw
// the language with (GDI+ doesn't fall back between fonts).
//
// Layer 2 (platform). Cost: one load at start-up; Str is a hash lookup.

#pragma once

#include <cstdint>
#include <filesystem>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <vector>

// Parses one file's text into `out` (later entries replace earlier ones).
// Malformed lines are skipped, each noted in `errors` as "file:line: why".
void ParseStrings(const std::string& utf8, const std::string& fileName,
                  std::unordered_map<std::string, std::string>& out, std::vector<std::string>& errors);

// Loads en.txt, then `language`.txt over it, from `dir`. False when en.txt
// can't be read (every lookup then shows its key). Problems go to `problems`.
bool LoadStrings(const std::filesystem::path& dir, const std::string& language, std::vector<std::string>& problems);
// Replaces the loaded table (tests).
void SetStrings(const std::unordered_map<std::string, std::string>& table);

// The text for `key`, or "[key]".
const std::string& Str(const char* key);
// The text for `key` with {0}, {1}... replaced by `args`.
std::string StrF(const char* key, std::initializer_list<std::string> args);
// Whether `key` is defined.
bool HasStr(const char* key);

// Every code point the loaded texts use, sorted, no repeats (the UI atlas
// bakes these beside ASCII and Latin-1).
std::vector<uint32_t> StringCodepoints();

// UTF-8 helpers. DecodeUtf8 reads one code point at s[i] and advances i;
// a malformed byte reads as U+FFFD and advances one byte.
uint32_t DecodeUtf8(const std::string& s, size_t& i);
void AppendUtf8(std::string& out, uint32_t cp);
std::wstring Utf8ToWide(const std::string& s);   // UTF-16 on Windows
std::string WideToUtf8(const std::wstring& w);
