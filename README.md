# Geo Checker

A desktop utility for inspecting and analyzing `geosite.dat` and `geoip.dat` routing data files used by modern proxy clients — **Xray-core**, **sing-box**, **V2Ray**, **Mihomo**, and others.

![Geo Checker — Light theme](screenshots/app_light.png)

---

## Why this tool exists

Proxy and VPN clients route traffic using two binary data files:

- **`geosite.dat`** — a database of domain names grouped into categories (e.g. `youtube`, `google`, `category-ads`, `geolocation-!cn`). Each entry specifies a rule type: exact domain, subdomain wildcard, full match, or regex.
- **`geoip.dat`** — a database of IP address ranges (CIDR blocks) grouped by country or tag (e.g. `ru`, `cn`, `us`, `private`).

These files are compiled from protobuf and are not human-readable. When you write routing rules like `geosite:youtube` or `geoip:ru`, there's no easy way to know exactly which domains or IPs are covered — until now.

**Geo Checker** lets you open these files and query them instantly, without any command-line tools or scripting.

---

## What you can do

| Action | How |
|---|---|
| Find which categories a domain belongs to | Enter `youtube.com` → see `youtube`, `google` |
| List all domains in a category | Enter `geosite:youtube` → full domain list with rule types |
| List all IP/CIDR ranges in a category | Enter `geoip:ru` → all IP ranges for Russia |
| Resolve a geosite category to real IPs | Enter `geosite:youtube` → DNS-resolve every domain → get IP list |
| Browse all available geosite categories | Click **All geosite categories** → full tag index |
| Browse all available geoip categories | Click **All geoip categories** → full country/tag index |

All operations run asynchronously — the UI stays responsive even on large files with millions of entries.

---

## Features

- **Instant search** — parallel domain lookup across all categories using all CPU cores
- **Full category browser** — list every tag in geosite or geoip with a single click
- **DNS resolve** — resolve an entire geosite category to real IP addresses for firewall rules
- **Three result tabs** — Categories / Domains / IP, each independently scrollable
- **Copy & Save** — copy results to clipboard or export to a text file
- **Dark / Light / System theme** — follows your OS or set manually
- **English and Russian interface** — switchable at runtime without restart
- **Single portable `.exe`** — no installation, no DLLs, no runtime dependencies
- **GUI and CLI** — full-featured desktop app and a command-line tool in one build

---

## Why `geosite:` and `geoip:` beat individual rules

When configuring a proxy or VPN client, you have two approaches to routing traffic:

**Manual rules — listing domains and IPs by hand:**
```
domain:youtube.com
domain:googlevideo.com
domain:ytimg.com
domain:yt3.ggpht.com
ip:142.250.0.0/15
ip:172.217.0.0/16
...
```

**Geo rules — one tag covers everything:**
```
geosite:youtube
geoip:google
```

The difference is dramatic, and it matters in practice:

### Coverage
A single `geosite:youtube` tag covers **hundreds of domains** — not just `youtube.com`, but all CDN hostnames, thumbnail servers, static asset domains, and regional variants. Maintaining this list manually is a constant game of whack-a-mole: services add new subdomains, change CDN providers, and split traffic across new hostnames. Miss one, and your routing silently breaks for that resource.

`geoip:ru` covers **over 100,000 IP/CIDR ranges** for Russia alone. Listing them manually in a config file is simply not feasible.

### Maintenance
Geo data files are maintained by the community and updated continuously. You update the file — your rules stay current automatically. With manual rules, every service change requires you to find and fix the issue yourself.

### Rule matching speed
Proxy clients like Xray-core and sing-box load `geosite.dat` into optimized in-memory structures (domain trees, tries). Matching against `geosite:google` with 3,000 domains is faster than linearly scanning 3,000 individual `domain:` entries, because lookups are O(log n) or O(k) where k is the domain label count.

### Config readability
A routing config with 20 geo rules is readable and auditable. A config with 10,000 individual domain and IP entries is not.

### Where Geo Checker fits in
The problem with geo rules is the opacity — you write `geosite:category-ads` and trust that it covers what you think it covers. Geo Checker removes that opacity:

- **Before writing a rule** — check what's actually in the category
- **Debugging broken routing** — enter the domain, see which tags match, verify your rule applies
- **Choosing between rules** — compare `geosite:google` vs `geosite:googleapis` to avoid over-blocking
- **Building firewall rules** — export IP ranges from `geoip:ru` directly to your firewall config
- **Auditing your setup** — browse all available categories to discover tags you didn't know existed

---

## Download

Go to [**Releases**](https://github.com/Maxim-Root/Geo-Checker/releases) and download `GeoChecker.exe`.

Place `geosite.dat` and `geoip.dat` in the same folder as the executable — the app will detect them automatically on startup. You can also open any file manually via the file picker.

> Common sources for up-to-date data files:
> - [v2fly/domain-list-community](https://github.com/v2fly/domain-list-community/releases) — `geosite.dat`
> - [v2fly/geoip](https://github.com/v2fly/geoip/releases) — `geoip.dat`
> - [Loyalsoldier/v2ray-rules-dat](https://github.com/Loyalsoldier/v2ray-rules-dat/releases) — extended versions of both

---

## Usage examples

**Check if a domain is covered by a rule:**
> Open `geosite.dat` → enter `twitter.com` → click **Find Categories**
> Result: `twitter`, `geolocation-!cn`

**See what `geosite:category-ads` actually blocks:**
> Enter `geosite:category-ads` → click **Domains**
> Result: full list of ad/tracker domains in that category

**Generate a firewall allowlist for Russian IP ranges:**
> Open `geoip.dat` → enter `geoip:ru` → click **IP from geoip**
> Result: all CIDR blocks → click **Save** → export to file

**Explore what categories are available in your data file:**
> Click **All geosite categories** or **All geoip categories**
> Result: complete tag index with count

---

## CLI

```bash
# Find categories for a domain
geochecker-cli search geosite.dat youtube.com

# List all domains in a category
geochecker-cli domains geosite.dat geosite:youtube

# List all IP ranges for a country
geochecker-cli geoip geoip.dat geoip:ru
```

---

## Build from source

### Requirements

- C++17 compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.16+
- Qt 6 (Widgets, Core, Gui, Concurrent) — for GUI build
- Protobuf — auto-downloaded via CMake FetchContent if not found

### Windows — static single exe

```bash
cd cpp
mkdir build-static && cd build-static
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/mingw_64_static" \
  -DGEOCHECKER_FETCH_PROTOBUF=ON
cmake --build . --parallel
```

### Linux

```bash
sudo apt install cmake qt6-base-dev libprotobuf-dev protobuf-compiler build-essential
cd cpp && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

### Docker (CLI only)

```bash
docker build -f cpp/Dockerfile -t geochecker .
docker run --rm geochecker ./build/geochecker-cli search /data/geosite.dat youtube.com
```

---

## Project structure

```
proto/routercommon.proto          V2Ray/Xray protobuf schema
cpp/src/mainwindow.cpp            Qt6 GUI — all UI logic
cpp/src/dat_parser.cpp            Protobuf parser (lazy index + parallel search)
cpp/src/geochecker_types.cpp      RAII data wrappers for GeoSiteData / GeoIPData
cpp/src/main.cpp                  GUI entry point
cpp/src/main_cli.cpp              CLI entry point
cpp/include/dat_parser.hpp        Public API for parsing and querying .dat files
cpp/include/lang_ui_config.hpp    UI layout constants (button widths, spacing)
cpp/resources/                    App icon, Windows .rc, Linux .desktop
```

---

## License

[Apache License 2.0](LICENSE)
