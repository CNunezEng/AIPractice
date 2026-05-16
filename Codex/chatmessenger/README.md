# Codex Chat Messenger

Purpose: a small UDP chat messenger for comparing AI coding agents in the `AIPractice` project.

## Environment

- C++17
- Windows
- MinGW g++
- Winsock2

## Build

From this folder:

```bat
build.bat
```

Or:

```bat
mingw32-make
```

## Run

Open two terminals from this folder.

Terminal 1:

```bat
build\Client.exe 127.0.0.1:5000 127.0.0.1:5001
```

Terminal 2:

```bat
build\Server.exe 127.0.0.1:5001 127.0.0.1:5000
```

Type a message and press Enter. Incoming messages print immediately. Type `/quit` to exit.

## Tests

Tests use Catch2 from `thirdPartyIncludes/Catch2`. Runtime parser cases live in `tests/test_cases.ini`, so new input and expected-result rows can be added without changing test source.

```bat
build.bat test
```

The current test suite includes one intentional failing edge case:

- `parseEndpoint_localhost_edge_case_should_fail`

It expects `localhost:5000` to parse as valid, while the current implementation only accepts dotted IPv4 addresses.

## Git Notes

Generated build outputs belong in `build/` and are ignored by this project's `.gitignore`. Commit source, tests, documentation, and the vendored Catch2 amalgamated files plus license.

## Design Notes

- Transport uses UDP because the requested command line supplies both source and destination endpoints.
- `IUdpTransport` is an explicit interface injected into `ChatApp`.
- `UdpSocket` uses the Adapter pattern to wrap Winsock socket calls.
- Parsing and validation are separated from console and socket I/O.
- Errors are explicit and point to the invalid argument when possible.

## Improvement Suggestions

- Add optional message timestamps.
- Add a TCP mode if guaranteed delivery becomes important.
- Add scripted integration tests that start both peers and exchange messages.
- Add Doxygen generation in CI when a shared repository workflow is chosen.
