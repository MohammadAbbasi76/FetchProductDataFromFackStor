FetchProductDataFromFackStor

Small C++ application that fetches product data from a fake store API, parses and
extracts product information (id, title, price, category), writes a CSV, and compresses
the results into a zip file that **I do as interview test**.

This README explains how to build and run the project, which dependencies are required,
how to use the main components from code, and common troubleshooting steps when
installing or linking dependencies ("dependency rejections").

## Features
- HTTP GET using libcurl
- JSON parsing using nlohmann::json
- Extract product fields: id, title, price, category
- Save to CSV (`data/products.csv`) with proper CSV escaping
- Compress `data/` into `data.zip` using libzip
- CMake-based build with `CMakePresets.json` for convenient configure/build presets

## Repository layout (important files)
- `App/main.cpp` — example program that ties components together
- `App/modules/ReceiveData.*` — fetches HTTP JSON from the API
- `App/modules/DataProcessing.*` — parses JSON into `Product` structs
- `App/modules/FileOpration.*` — writes `products.csv` and helper file I/O
- `App/modules/Compression.*` — compresses the `data/` folder into a zip
- `CMakeLists.txt`, `App/CMakeLists.txt`, `App/modules/CMakeLists.txt` — build scripts
- `CMakePresets.json` — ready-made configure/build presets (Ninja)

## Dependencies
This project targets Linux (tested on Ubuntu). Required packages and libraries:

- libcurl (development headers) — for HTTP requests
- libzip (development headers) — for creating zip archives
- nlohmann::json (header-only) — JSON parsing
- A C++17-capable compiler and CMake 3.19+ (presets file uses 3.19 features)

Install on Debian/Ubuntu (recommended):

```bash
sudo apt update
sudo apt install -y build-essential cmake ninja-build libcurl4-openssl-dev libzip-dev libnlohmann-json-dev
```

Notes:
- `libnlohmann-json-dev` installs the header-only nlohmann/json package system-wide. If you prefer, you can add it as a submodule or fetch it with `conan`/`vcpkg`.
- If your distribution doesn't provide `libzip` or `libcurl` packages, install from source or use your package manager equivalent.

## Common dependency rejections (errors) and fixes

- libcurl header not found / linker errors (undefined reference to curl_easy_init, ...):
	- Ensure `libcurl4-openssl-dev` (or distro equivalent) is installed.
	- Ensure `find_package(CURL REQUIRED)` in CMake finds the correct package. Run `cmake --preset debug` and check output for `CURL_FOUND`.

- libzip header not found or link failures (zip_open, zip_file_add):
	- Install `libzip-dev` (Debian/Ubuntu) or build libzip from source.
	- On some systems the library name differs — check `pkg-config --libs libzip`.

- nlohmann/json compile errors:
	- Install `libnlohmann-json-dev` or add the header to your include path.

- SSL/TLS or certificate verification failures when contacting API:
	- Make sure system time is correct (wrong time can break certificate validation).
	- Install CA certificates bundle (e.g. `ca-certificates` on Debian/Ubuntu).

- Permission errors creating `data/` or `data/products.csv`:
	- Ensure the running user has write permission in the project folder or choose a writable base directory.

- CMake presets not available / older CMake:
	- Presets in this repo expect CMake 3.19+. If your system has older CMake, either upgrade CMake or run configure manually:

```bash
mkdir -p build && cd build
cmake -G Ninja ..
cmake --build .
```

## Build using CMakePresets (recommended)

From repository root:

```bash
# Configure (Debug)
cmake --preset debug

# Build using the preset
cmake --build --preset build
```

For release builds:

```bash
cmake --preset release
cmake --build --preset build-release
```

If you don't use presets, you can use the manual commands shown in the previous section.

## Code usage / example (how to use library classes)

This project is small and exposes four main components. The following shows the typical flow (also implemented in `App/main.cpp`):

- `ReceiveData` — fetch JSON string from API
- `DataProcessing` — parse JSON string into `std::vector<Product>`
- `FileOpration` — write `products.csv` into a directory
- `Compression` — zip the directory into `data.zip`

Example (conceptual):

```cpp
#include "modules/ReceiveData.hpp"
#include "modules/DataProcessing.hpp"
#include "modules/FileOpration.hpp"
#include "modules/Compression.hpp"

int main() {
		// Fetch
		ReceiveData receiver("https://fakestoreapi.com/products");
		std::string json = receiver.SendRquestAndHandleIt();

		// Parse
		DataProcessing parser(json);
		std::vector<Product> products = parser.parseJsonData();

		// Save to CSV
		FileOpration fileOp("data");
		fileOp.ModifyCSV(products);

		// Compress the resulting folder
		Compression compressor("data", "data.zip");
		compressor.CompressFolder();
}
```

`Product` struct fields:

- `int id`
- `std::string title`
- `double price`
- `std::string category`

## Running the example program

After a successful build (see Build section), run the executable produced in `build/App/AppLayer`:

```bash
./build/App/AppLayer
```

This will:
- fetch products from the Fake Store API
- write `data/products.csv`
- create `data.zip` containing the `data/` folder

## Troubleshooting and next steps

- If the program fails at runtime with exceptions thrown from network or JSON parsing, the error message will be printed. Common fixes:
	- Network errors: check connectivity and proxy settings
	- JSON parse errors: check the API response content (e.g., rate limit pages or HTML errors)

- Improvements you might add:
	- Unit tests for `DataProcessing::parseJsonData()` (happy path + malformed JSON)
	- CLI flags to choose API endpoint, output folder, or skip compression
	- Configure logging instead of printing directly to stdout/stderr

If you want, I can also:
- Add a small test harness for `DataProcessing` and `FileOpration`.
- Add a `conanfile.txt` or `vcpkg.json` to make dependency installation reproducible.

---

If anything in this README needs to be adjusted (different package names, alternate OS instructions, or extra usage examples), tell me which OS and package manager you prefer and I'll update it.