# C++ RSS Feed Reader

A lightweight C++ web server that fetches, parses, and serves RSS 2.0 / Atom feeds as JSON, with a simple browser front-end. It is also a showcase of core C++ OOP concepts: inheritance (including multiple inheritance), virtual functions, templates, operator overloading, friend functions, custom exceptions, and static members.

By default it loads the Indian Express Ahmedabad city feed:
`https://indianexpress.com/section/city/ahmedabad/feed/`

---

## Features

- Fetch any RSS or Atom feed by URL (via libcurl)
- Parse feeds with pugixml (RSS `<channel>/<item>` and Atom `<feed>/<entry>`)
- REST API returning JSON (nlohmann/json)
- In-memory cache of parsed feeds
- Reader profile with subscription plans (Free / Trial / Premium)
- Thread-safe request handling via a mutex
- Serves a front-end page (`rss.html`) at `/`

## Tech Stack

| Library | Purpose |
|---|---|
| [cpp-httplib](https://github.com/yhirose/cpp-httplib) | HTTP server |
| [nlohmann/json](https://github.com/nlohmann/json) | JSON handling |
| [pugixml](https://pugixml.org/) | XML parsing |
| [libcurl](https://curl.se/libcurl/) | Fetching remote feeds |

Requires a **C++11 or newer** compiler (C++17 recommended).

---

## Installation

### macOS

1. Install Xcode command line tools and Homebrew (skip if already installed):

```bash
xcode-select --install
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

2. Install dependencies:

```bash
brew install curl nlohmann-json pugixml cpp-httplib pkg-config
```

3. Build:

```bash
g++ -std=c++17 main.cpp -o rss_reader \
  $(pkg-config --cflags --libs libcurl pugixml) \
  -I$(brew --prefix)/include -L$(brew --prefix)/lib -pthread
```

> If `pkg-config` can't find a package, use explicit flags instead:
> `-I$(brew --prefix)/include -L$(brew --prefix)/lib -lcurl -lpugixml -pthread`

4. Run:

```bash
./rss_reader
```

---

### Windows (MSYS2 / MinGW-w64, recommended)

1. Install [MSYS2](https://www.msys2.org/), then open the **MSYS2 UCRT64** terminal.

2. Install the toolchain and dependencies:

```bash
pacman -Syu
pacman -S --needed mingw-w64-ucrt-x86_64-gcc \
  mingw-w64-ucrt-x86_64-curl \
  mingw-w64-ucrt-x86_64-nlohmann-json \
  mingw-w64-ucrt-x86_64-pugixml \
  mingw-w64-ucrt-x86_64-cpp-httplib \
  mingw-w64-ucrt-x86_64-pkgconf
```

3. Build:

```bash
g++ -std=c++17 main.cpp -o rss_reader.exe \
  $(pkg-config --cflags --libs libcurl pugixml) \
  -lws2_32 -lcrypt32
```

4. Run:

```bash
./rss_reader.exe
```

### Windows (Visual Studio + vcpkg, alternative)

```powershell
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg install curl nlohmann-json pugixml cpp-httplib --triplet x64-windows
.\vcpkg\vcpkg integrate install
```

Then open a *Developer Command Prompt for VS* and build:

```powershell
cl /std:c++17 /EHsc main.cpp /I vcpkg\installed\x64-windows\include ^
   /link /LIBPATH:vcpkg\installed\x64-windows\lib libcurl.lib pugixml.lib ws2_32.lib
```

> Make sure the DLLs from `vcpkg\installed\x64-windows\bin` are next to the `.exe` or on your `PATH`.

---

### Linux (Debian/Ubuntu, for reference)

```bash
sudo apt update
sudo apt install g++ libcurl4-openssl-dev nlohmann-json3-dev libpugixml-dev libcpp-httplib-dev pkg-config
g++ -std=c++17 main.cpp -o rss_reader $(pkg-config --cflags --libs libcurl pugixml) -pthread
./rss_reader
```

---

## Project Structure

```
.
├── main.cpp      # Server + all classes
├── rss.html      # Front-end page (must be in the working directory)
└── README.md
```

> **Important:** run the executable from the folder containing `rss.html`, otherwise `/` returns a 404.

## Running

```bash
./rss_reader
```

Expected output:

```
=== C++ RSS Feed Reader Server running at http://localhost:3000 ===
```

Open <http://localhost:3000> in your browser.

---

## API Reference

### `GET /`
Serves `rss.html`.

### `GET /api/feed`

| Param | Type | Default | Description |
|---|---|---|---|
| `url` | string | Ahmedabad feed | Feed URL to fetch |
| `limit` | int | 10 | Number of items (clamped to 1–50) |
| `bookmark` | `1` | – | Treat `url` as a Bookmark |

```bash
curl "http://localhost:3000/api/feed?limit=5"
```

Response:

```json
{
  "status": "ok",
  "feed": { "title": "...", "url": "..." },
  "items": [
    { "link": "...", "title": "...", "pubDate": "...", "description": "...", "isRead": false }
  ],
  "totalFeedsCreated": 1
}
```

Errors return `status: "error"` with a `message` (HTTP 400 / 500 / 502).

### `POST /api/reader`
Sets the active reader and subscription.

```bash
curl -X POST http://localhost:3000/api/reader \
  -H "Content-Type: application/json" \
  -d '{"name":"Alice","days":45}'
```

Plan rules: `days >= 30` → Premium, `1–29` → Trial, `0` → Free. Negative values return HTTP 400.

### `GET /api/combine`
Demo endpoint exercising the `%` operator overload on two feeds.

```bash
curl "http://localhost:3000/api/combine?url1=URL_A&url2=URL_B"
```

---

## C++ Concepts Demonstrated

- **Inheritance:** `Feed` → `Article` → `ArticleStatus` → `Reader`
- **Multiple inheritance:** `Reader` inherits from `ArticleStatus` and `Subscription`
- **Polymorphism:** virtual `displayInfo()` / virtual destructor
- **Templates:** `Cache<T>`, `printItem<T>`
- **Operator overloading:** `<<`, `>>`, `%`, and `operator int()`
- **Friend functions:** `checkAccess`, stream operators
- **Custom exceptions:** `FeedException`, `InvalidSubscriptionException`
- **Static members:** `Feed::totalFeedsCreated`
- **RAII:** `Cleanup` scope guard
- **Constructor overloading:** `Feed`, `Subscription`, `Reader`

## Troubleshooting

- **`httplib.h: No such file`**: install `cpp-httplib` or download `httplib.h` from its GitHub repo into the project folder.
- **Linker errors for curl/pugixml**: check `-lcurl -lpugixml` and library paths.
- **Windows linker errors about sockets**: add `-lws2_32 -lcrypt32`.
- **`rss.html not found`**: run the binary from the directory containing `rss.html`.
- **HTTPS fetch failures**: make sure libcurl was built with SSL support and CA certificates are available.
- **Port 3000 in use**: change the port in `svr.listen("0.0.0.0", 3000)`.

## Known Limitations

- Cache is in-memory only and never expires
- A single global mutex serializes all requests
- `stoi` on an invalid `limit` value will throw (returned as HTTP 500)
- `/api/feed` accepts any URL, so don't expose the server publicly without validation (SSRF risk)

## License

Add your preferred license here (e.g. MIT).
