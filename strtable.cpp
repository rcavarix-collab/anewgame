// strtable.cpp -- see strtable.h (D26). Layer 2.

#include "strtable.h"
#include <algorithm>
#include <fstream>
#include <set>
#include <sstream>

namespace {
std::unordered_map<std::string, std::string> g_table;

bool KeyChar(char c) { return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '.'; }

std::string Trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r')) a++;
    while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\r')) b--;
    return s.substr(a, b - a);
}
} // namespace

void ParseStrings(const std::string& utf8, const std::string& fileName,
                  std::unordered_map<std::string, std::string>& out, std::vector<std::string>& errors) {
    std::istringstream in(utf8);
    std::string line;
    int n = 0;
    while (std::getline(in, line)) {
        n++;
        if (n == 1 && line.size() >= 3 && (unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
            line = line.substr(3); // a byte-order mark, as some editors write
        std::string t = Trim(line);
        if (t.empty() || t[0] == '#') continue;
        size_t eq = t.find('=');
        std::string key = eq == std::string::npos ? "" : Trim(t.substr(0, eq));
        bool ok = !key.empty();
        for (char c : key) ok = ok && KeyChar(c);
        if (!ok) { errors.push_back(fileName + ":" + std::to_string(n) + ": expected `key = text`"); continue; }
        std::string raw = Trim(t.substr(eq + 1)), text;
        for (size_t i = 0; i < raw.size(); i++) {
            if (raw[i] == '\\' && i + 1 < raw.size() && raw[i + 1] == 'n') { text.push_back('\n'); i++; }
            else text.push_back(raw[i]);
        }
        out[key] = text;
    }
}

bool LoadStrings(const std::filesystem::path& dir, const std::string& language, std::vector<std::string>& problems) {
    auto read = [&](const std::string& lang, bool required) {
        std::filesystem::path f = dir / (lang + ".txt");
        std::ifstream in(f, std::ios::binary);
        if (!in) {
            if (required) problems.push_back(f.string() + ": can't be read");
            return false;
        }
        std::stringstream ss; ss << in.rdbuf();
        ParseStrings(ss.str(), lang + ".txt", g_table, problems);
        return true;
    };
    g_table.clear();
    bool ok = read("en", true);
    if (!language.empty() && language != "en") read(language, true);
    return ok;
}

void SetStrings(const std::unordered_map<std::string, std::string>& table) { g_table = table; }

const std::string& Str(const char* key) {
    auto it = g_table.find(key);
    if (it != g_table.end()) return it->second;
    // Missing: remember "[key]" so the reference stays valid.
    static std::unordered_map<std::string, std::string> missing;
    std::string& m = missing[key];
    if (m.empty()) m = std::string("[") + key + "]";
    return m;
}

bool HasStr(const char* key) { return g_table.count(key) != 0; }

std::string StrF(const char* key, std::initializer_list<std::string> args) {
    const std::string& f = Str(key);
    std::vector<std::string> a(args);
    std::string out;
    for (size_t i = 0; i < f.size(); i++) {
        if (f[i] == '{') {
            size_t j = i + 1; int n = 0; bool digit = false;
            while (j < f.size() && f[j] >= '0' && f[j] <= '9') { n = n * 10 + (f[j] - '0'); j++; digit = true; }
            if (digit && j < f.size() && f[j] == '}' && n < (int)a.size()) { out += a[n]; i = j; continue; }
        }
        out.push_back(f[i]);
    }
    return out;
}

std::vector<uint32_t> StringCodepoints() {
    std::set<uint32_t> cps;
    for (const auto& kv : g_table) {
        if (kv.first == "font") continue;
        for (size_t i = 0; i < kv.second.size();) cps.insert(DecodeUtf8(kv.second, i));
    }
    return std::vector<uint32_t>(cps.begin(), cps.end());
}

uint32_t DecodeUtf8(const std::string& s, size_t& i) {
    unsigned char c = (unsigned char)s[i];
    int len = c < 0x80 ? 1 : (c >> 5) == 6 ? 2 : (c >> 4) == 14 ? 3 : (c >> 3) == 30 ? 4 : 0;
    if (len == 0 || i + len > s.size()) { i++; return 0xFFFD; }
    uint32_t cp = len == 1 ? c : len == 2 ? (c & 0x1F) : len == 3 ? (c & 0x0F) : (c & 0x07);
    for (int k = 1; k < len; k++) {
        unsigned char d = (unsigned char)s[i + k];
        if ((d >> 6) != 2) { i++; return 0xFFFD; }
        cp = (cp << 6) | (d & 0x3F);
    }
    i += len;
    return cp;
}

void AppendUtf8(std::string& out, uint32_t cp) {
    if (cp < 0x80) out.push_back((char)cp);
    else if (cp < 0x800) { out.push_back((char)(0xC0 | (cp >> 6))); out.push_back((char)(0x80 | (cp & 0x3F))); }
    else if (cp < 0x10000) { out.push_back((char)(0xE0 | (cp >> 12))); out.push_back((char)(0x80 | ((cp >> 6) & 0x3F))); out.push_back((char)(0x80 | (cp & 0x3F))); }
    else { out.push_back((char)(0xF0 | (cp >> 18))); out.push_back((char)(0x80 | ((cp >> 12) & 0x3F))); out.push_back((char)(0x80 | ((cp >> 6) & 0x3F))); out.push_back((char)(0x80 | (cp & 0x3F))); }
}

std::wstring Utf8ToWide(const std::string& s) {
    std::wstring w;
    for (size_t i = 0; i < s.size();) {
        uint32_t cp = DecodeUtf8(s, i);
        if (cp >= 0x10000) { cp -= 0x10000; w.push_back((wchar_t)(0xD800 + (cp >> 10))); w.push_back((wchar_t)(0xDC00 + (cp & 0x3FF))); }
        else w.push_back((wchar_t)cp);
    }
    return w;
}

std::string WideToUtf8(const std::wstring& w) {
    std::string s;
    for (size_t i = 0; i < w.size(); i++) {
        uint32_t cp = (uint32_t)w[i];
        if (cp >= 0xD800 && cp < 0xDC00 && i + 1 < w.size()) { cp = 0x10000 + ((cp - 0xD800) << 10) + ((uint32_t)w[i + 1] - 0xDC00); i++; }
        AppendUtf8(s, cp);
    }
    return s;
}
